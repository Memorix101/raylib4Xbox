// Stub sys/stat.h for Xbox (nxdk)
// stat() and GetFileModTime() will return -1/-1 — filesystem introspection
// is limited on Xbox; actual file I/O uses nxdk's FATX layer.
#ifndef XBOX_SYS_STAT_H
#define XBOX_SYS_STAT_H

#include <stdint.h>

typedef long time_t;

struct stat {
    uint32_t st_mode;
    time_t   st_mtime;
    uint32_t st_size;
};

#define S_IFMT   0170000
#define S_IFREG  0100000
#define S_IFDIR  0040000

#define S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
#define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)

static inline int stat(const char *path, struct stat *buf)
{
    (void)path;
    if (buf) { buf->st_mode = 0; buf->st_mtime = 0; buf->st_size = 0; }
    return -1;
}

#endif
