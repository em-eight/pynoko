#pragma once

#include <cstdio>
#include <cstdlib>

#define TOSTRINGIMPL(x) #x
#define TOSTRING(x) TOSTRINGIMPL(x)

#ifdef __FILE_NAME__
#define FILE_NAME __FILE_NAME__
#else
#define FILE_NAME __FILE__
#endif

#define STATIC_ASSERT(e) static_assert(e, "[" FILE_NAME ":" TOSTRING(__LINE__) "] ASSERT: " #e)
#define RUNTIME_ASSERT(e, m) \
    do { \
        if (!(e)) { \
            printf("[" FILE_NAME ":" TOSTRING(__LINE__) "] ASSERT: " #e ", MSG: %s\n", m); \
            abort(); \
        } \
    } while (0)

#define PANIC(m, ...) \
    do { \
        printf("[" FILE_NAME ":" TOSTRING(__LINE__) "] PANIC: " m "\n", ##__VA_ARGS__); \
        abort(); \
    } while (0)

#ifndef NDEBUG
#define DEBUG(m, ...) \
    do { \
        printf("[" FILE_NAME ":" TOSTRING(__LINE__) "] DEBUG: " m "\n", ##__VA_ARGS__); \
    } while (0)
#else
#define DEBUG(m, ...)
#endif
