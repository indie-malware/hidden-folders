#include <windows.h>

int main() {
    char tempPath[MAX_PATH];
    char folderPath[MAX_PATH];

    GetTempPathA(MAX_PATH, tempPath);
    lstrcatA(tempPath, "Hidden"); // Change the folder name to what you want
    lstrcpyA(folderPath, tempPath);

    CreateDirectoryA(folderPath, NULL);
    SetFileAttributesA(folderPath, FILE_ATTRIBUTE_HIDDEN);

    return 0;
}
