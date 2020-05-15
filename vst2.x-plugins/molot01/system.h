#ifndef _system_h_
#define _system_h_

#ifdef WIN32

# include <windows.h>    // CRITICAL_SECTION
# include <crtdbg.h>
# include <malloc.h>     // alloca

#else /* !WIN32 */

# include <pthread.h>

# define _ASSERT(x)

#endif /* WIN32 */

#include <locale.h>     // setlocale
#include <stdarg.h>     // va_list
#include <stdio.h>      // snprintf
#include <string.h>     // memset
#include <stdio.h>      // sprintf
#include <stdlib.h>     // rand

#define _USE_MATH_DEFINES
#include <math.h>

#ifdef TARGET_API_MAC_CARBON
#define PTHREAD_MUTEX_TIMED_NP      PTHREAD_MUTEX_NORMAL
#define PTHREAD_MUTEX_RECURSIVE_NP  PTHREAD_MUTEX_RECURSIVE
#define PTHREAD_MUTEX_ERRORCHECK_NP PTHREAD_MUTEX_ERRORCHECK
#define PTHREAD_MUTEX_DEFAULT       PTHREAD_MUTEX_NORMAL
#endif /* TARGET_API_MAC_CARBON */

#ifdef WIN32_DISABLED

// ANSI C vsnprintf() must return total number of chars not number of chars written
static __inline int ansi_vsnprintf(char *buffer, size_t count, const char *fmt, va_list ap)
{
    int rc;
    
    if (count > 0) {
        rc = _vsnprintf(buffer, count - 1, fmt, ap);
        buffer[count - 1] = '\0';
    } else
        rc = -1;

    if (rc < 0) {
        // >= XP only
        rc = _vscprintf(fmt, ap);
    }

    return rc;
}

#define vsnprintf   ansi_vsnprintf

static __inline int snprintf(char *buffer, size_t count, const char *fmt, ...)
{
    int rc;
    va_list ap;
    va_start(ap, fmt);
    
    rc = vsnprintf(buffer, count, fmt, ap);
    
    va_end(ap);
    return rc;
}

#endif /* WIN32 */

#endif
