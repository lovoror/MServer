#ifndef __GOLBAL_H__
#define __GOLBAL_H__

#include <cstdio>      /* c compatible,like printf */
#include <cstdlib>     /* c lib like malloc */
#include <iostream>    /* c++ base support */
#include <ctime>       /* time function */
#include <cstdarg>     /* va_start va_end ... */
#include <unistd.h>    /* POSIX api */
#include <sys/types.h> /* linux sys types like pid_t */
#include <cstring>     /* memset memcpy */
#include <cerrno>      /* errno */
#include <limits.h>    /* PATH_MAX */

#include "../config.h" /* config paramter */

/* assert with error msg,third parted code not include this file will work fine
   with old assert.
*/
#include "types.h"     /* base data type */
#include "assert.h"
#include "clog.h"      /* log function */

/* Qt style unused paramter handler */
#define UNUSED(x) (void)x

/* after c++0x,static define in glibc/assert/assert.h */
#if __cplusplus < 201103L    /* -std=gnu99 */
    #define static_assert(x,msg) \
        typedef char __STATIC_ASSERT__##__LINE__[(x) ? 1 : -1]
#endif

/* 分支预测，汇编优化。逻辑上不会改变cond的值 */
#define expect_false(cond) __builtin_expect (!!(cond),0)
#define expect_true(cond)  __builtin_expect (!!(cond),1)

#ifdef _PRINTF_
    #define PRINTF(...) cprintf_log( "CP",__VA_ARGS__ )
#else
    #define PRINTF(...)
#endif

#ifdef _ERROR_
    #define ERROR(...) cerror_log( "CE",__VA_ARGS__ )
#else
    #define ERROR(...)
#endif

/* terminated without destroying any object and without calling any of the 
 * functions passed to atexit or at_quick_exit
 */
#define FATAL(...)    do{ERROR(__VA_ARGS__);::abort();}while(0)

extern void __log_assert_fail (const char *__assertion, const char *__file,
           unsigned int __line, const char *__function);

/* This prints an "log assertion failed" message and return,not abort.  */
#define log_assert(why,expr,...)                         \
    do{                                                  \
        if ( !(expr) )                                   \
        {                                                \
            __log_assert_fail (__STRING((why,expr)),     \
                __FILE__, __LINE__, __ASSERT_FUNCTION);  \
            return __VA_ARGS__;                          \
        }                                                \
    }while (0)

#define array_resize(type,base,cur,cnt,init)        \
    if ( (cnt) > (cur) )                            \
    {                                               \
        uint32 size = cur > 0 ? cur : 16;           \
        while ( size < (uint32)cnt )                \
        {                                           \
            size *= 2;                              \
        }                                           \
        type *tmp = new type[size];                 \
        init( tmp,sizeof(type)*size );              \
        if ( cur > 0)                               \
            memcpy( tmp,base,sizeof(type)*cur );    \
        delete []base;                              \
        base = tmp;                                 \
        cur = size;                                 \
    }

#define EMPTY(base,size)
#define array_zero(base,size)    \
    memset ((void *)(base), 0, size)

/* will be called while process exit */
extern void onexit();
/* will be called while allocate memory failed with new */
extern void new_fail();
extern void signal_block();

#endif  /* __GOLBAL_H__ */
