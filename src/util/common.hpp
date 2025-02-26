#pragma once

#define TOSTRINGIMPL(x) #x
#define TOSTRING(x) TOSTRINGIMPL(x)

#ifdef __FILE_NAME__
#define FILE_NAME __FILE_NAME__
#else
#define FILE_NAME __FILE__
#endif

#define RUNTIME_ASSERT(e, m) \
    do { \
        if (!(e)) { \
            printf("[" FILE_NAME ":" TOSTRING(__LINE__) "] ASSERT: " #e ", MSG: %s\n", m); \
            abort(); \
        } \
    } while (0)
