// dirent_xbox.h — minimal POSIX dirent for Xbox (nxdk)
// Wraps nxdk's Win32 FindFirstFileA / FindNextFileA / FindClose.
// Provides opendir() / readdir() / closedir() as used by raylib's directory scanning.
#ifndef DIRENT_XBOX_H
#define DIRENT_XBOX_H

#include <windows.h>    // nxdk: FindFirstFileA, FindNextFileA, FindClose, WIN32_FIND_DATAA
#include <string.h>
#include <stdlib.h>

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

struct dirent {
    char d_name[MAX_PATH];
};

typedef struct {
    HANDLE          handle;
    WIN32_FIND_DATAA findData;
    struct dirent   entry;
    int             first;          // 1 = FindFirstFileA result not yet returned
} DIR;

static inline DIR *opendir(const char *path)
{
    char pattern[MAX_PATH];
    // Append \* to the path to list all entries
    int len = (int)strlen(path);
    if (len + 3 >= MAX_PATH) return NULL;
    memcpy(pattern, path, len);
    if (len > 0 && path[len - 1] != '\\' && path[len - 1] != '/')
        pattern[len++] = '\\';
    pattern[len++] = '*';
    pattern[len]   = '\0';

    DIR *dir = (DIR *)malloc(sizeof(DIR));
    if (!dir) return NULL;

    dir->handle = FindFirstFileA(pattern, &dir->findData);
    if (dir->handle == INVALID_HANDLE_VALUE)
    {
        free(dir);
        return NULL;
    }
    dir->first = 1;
    return dir;
}

static inline struct dirent *readdir(DIR *dir)
{
    if (!dir || dir->handle == INVALID_HANDLE_VALUE) return NULL;

    if (dir->first)
    {
        dir->first = 0;
    }
    else
    {
        if (!FindNextFileA(dir->handle, &dir->findData)) return NULL;
    }

    strncpy(dir->entry.d_name, dir->findData.cFileName, MAX_PATH - 1);
    dir->entry.d_name[MAX_PATH - 1] = '\0';
    return &dir->entry;
}

static inline int closedir(DIR *dir)
{
    if (!dir) return -1;
    if (dir->handle != INVALID_HANDLE_VALUE) FindClose(dir->handle);
    free(dir);
    return 0;
}

#endif // DIRENT_XBOX_H
