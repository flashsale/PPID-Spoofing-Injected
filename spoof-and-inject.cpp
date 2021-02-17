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




int main() {

    STARTUPINFOEXA si;
    PROCESS_INFORMATION pi;
    SIZE_T st;
    HANDLE ph;

    FreeConsole();

    ph = OpenProcess(PROCESS_ALL_ACCESS, false, getProcessID());

    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi));

    InitializeProcThreadAttributeList(NULL, 1, 0, &st);
    si.lpAttributeList = (LPPROC_THREAD_ATTRIBUTE_LIST)HeapAlloc(GetProcessHeap(), 0, st);
    InitializeProcThreadAttributeList(si.lpAttributeList, 1, 0, &st);
    UpdateProcThreadAttribute(si.lpAttributeList, 0, PROC_THREAD_ATTRIBUTE_PARENT_PROCESS, &ph, sizeof(ph), NULL, NULL);

    si.StartupInfo.cb = sizeof(STARTUPINFOEXA);

    CreateProcessA("c:\\Windows\\System32\\notepad.exe", NULL, NULL, NULL, TRUE, EXTENDED_STARTUPINFO_PRESENT, NULL, NULL, reinterpret_cast<LPSTARTUPINFOA>(&si), &pi);

    WaitForSingleObject(pi.hProcess, INFINITE);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}