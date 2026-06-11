// Stub io.h for Xbox (nxdk)
// _access() is used by raylib's FileExists() — on Xbox we return -1 (not found)
// since nxdk doesn't expose a direct POSIX access() equivalent.
// Use nxdk's file I/O (fopen) for actual file access checks.
#ifndef XBOX_IO_H
#define XBOX_IO_H

static inline int _access(const char *filename, int mode)
{
    (void)filename;
    (void)mode;
    return -1;
}

#endif
