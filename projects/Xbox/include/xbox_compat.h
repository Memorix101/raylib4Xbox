// xbox_compat.h — MSVC CRT compatibility shims for Xbox (nxdk)
// Included implicitly via -include in CFLAGS so third-party headers
// (stb_image.h, cgltf.h, tinyobj_loader_c.h) get these definitions.
#ifndef XBOX_COMPAT_H
#define XBOX_COMPAT_H

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>

// _ftelli64: 64-bit ftell — Xbox uses 32-bit FATX so ftell is sufficient
#ifndef _ftelli64
#define _ftelli64(f) ((long long)ftell(f))
#endif

// _fseeki64: 64-bit fseek counterpart
#ifndef _fseeki64
#define _fseeki64(f, off, whence) fseek((f), (long)(off), (whence))
#endif

// fopen_s: safe fopen (returns errno_t, takes FILE**)
#ifndef fopen_s
static inline int fopen_s(FILE **pfile, const char *filename, const char *mode)
{
    if (!pfile) return EINVAL;
    *pfile = fopen(filename, mode);
    return (*pfile == NULL) ? errno : 0;
}
#endif

// sscanf_s: redirect to plain sscanf (no buffer-size params in our usage)
#ifndef sscanf_s
#define sscanf_s sscanf
#endif

// sprintf_s: redirect to snprintf
#ifndef sprintf_s
#define sprintf_s(buf, size, ...) snprintf(buf, size, __VA_ARGS__)
#endif

// _countof: array element count
#ifndef _countof
#define _countof(arr) (sizeof(arr) / sizeof((arr)[0]))
#endif

// _stricmp / _strnicmp: case-insensitive string compare
// strcasecmp / strncasecmp: POSIX equivalents
#ifndef _stricmp
#define _stricmp(a,b)    _strnicmp_impl((a),(b),(size_t)-1)
#define _strnicmp(a,b,n) _strnicmp_impl((a),(b),(n))
static inline int _strnicmp_impl(const char *a, const char *b, size_t n)
{
    for (size_t i = 0; i < n && (a[i] || b[i]); i++) {
        int ca = (unsigned char)a[i], cb = (unsigned char)b[i];
        if (ca >= 'A' && ca <= 'Z') ca += 32;
        if (cb >= 'A' && cb <= 'Z') cb += 32;
        if (ca != cb) return ca - cb;
    }
    return 0;
}
#endif
#ifndef strcasecmp
#define strcasecmp(a,b)    _stricmp(a,b)
#define strncasecmp(a,b,n) _strnicmp(a,b,n)
#endif

// _wfopen_s: wide-char fopen — nxdk has no wchar file I/O; stub to always fail
#ifndef _wfopen_s
#include <wchar.h>
static inline int _wfopen_s(FILE **f, const wchar_t *path, const wchar_t *mode)
{
    (void)path; (void)mode;
    if (f) *f = NULL;
    return 1;
}
#endif

// __int64: 64-bit integer type (already typedef'd by nxdk via xboxkrnl)
// but stb_image may use it conditionally — ensure it's present
#ifndef __int64
#define __int64 long long
#endif

// String-to-float conversions.
// WARNING: nxdk's strtod/strtof/atof are NOT real implementations - libxboxrt's
// strtod/strtof are assert(0) stubs and pdclib only declares atof. Anything that
// parses floating-point text through them (e.g. cgltf parsing glTF JSON numbers)
// crashes with a Fatal System Error. We provide a self-contained parser here and
// route atof/strtod/strtof to it so those code paths work on Xbox.
#include <stdlib.h>

static inline double xbox_strtod(const char *s, char **endptr)
{
    const char *p = s;
    double result = 0.0;
    int sign = 1;

    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r' || *p == '\f' || *p == '\v') p++;

    if (*p == '+') p++;
    else if (*p == '-') { sign = -1; p++; }

    while (*p >= '0' && *p <= '9')
    {
        result = result*10.0 + (double)(*p - '0');
        p++;
    }

    if (*p == '.')
    {
        double scale = 0.1;
        p++;
        while (*p >= '0' && *p <= '9')
        {
            result += (double)(*p - '0')*scale;
            scale *= 0.1;
            p++;
        }
    }

    result *= sign;

    if (*p == 'e' || *p == 'E')
    {
        int expSign = 1;
        int exp = 0;
        double pow10 = 1.0;
        p++;
        if (*p == '+') p++;
        else if (*p == '-') { expSign = -1; p++; }
        while (*p >= '0' && *p <= '9')
        {
            exp = exp*10 + (*p - '0');
            p++;
        }
        while (exp-- > 0) pow10 *= 10.0;
        if (expSign > 0) result *= pow10;
        else result /= pow10;
    }

    if (endptr) *endptr = (char *)p;
    return result;
}

#ifndef atof
#define atof(s)       xbox_strtod((s), NULL)
#endif
#ifndef strtod
#define strtod(s, e)  xbox_strtod((s), (e))
#endif
#ifndef strtof
#define strtof(s, e)  ((float)xbox_strtod((s), (e)))
#endif

#endif // XBOX_COMPAT_H
