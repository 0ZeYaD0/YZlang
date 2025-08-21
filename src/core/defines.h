#ifndef DEFINES_H
#define DEFINES_H

// yzlang specific
#define VERSION_MAJOR 0
#define VERSION_MINOR 1
#define VERSION_PATCH 1

#define YZDEBUG true

// unsigned int types
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

// signed int types
typedef signed char i8;
typedef signed short i16;
typedef signed int i32;
typedef signed long long i64;

// floating point types
typedef float f32;
typedef double f64;

// for bit fields (accessing bit no. x)
#define BIT(x) (1 << x)

// define static assertions
#if defined(__clang__) || defined(__gcc__) || defined(__GNUC__)
    #define ST_ASSERT _Static_assert
#else
    #define ST_ASSERT static_assert
#endif

// // static assertions for type sizes
// ST_ASSERT(sizeof(u8) == 1, "expected u8 to be 1 byte.");
// ST_ASSERT(sizeof(u16) == 2, "expected u16 to be 2 bytes.");
// ST_ASSERT(sizeof(u32) == 4, "expected u32 to be 4 bytes.");
// ST_ASSERT(sizeof(u64) == 8, "expected u64 to be 8 bytes.");

// ST_ASSERT(sizeof(i8) == 1, "expected i8 to be 1 byte.");
// ST_ASSERT(sizeof(i16) == 2, "expected i16 to be 2 bytes.");
// ST_ASSERT(sizeof(i32) == 4, "expected i32 to be 4 bytes.");
// ST_ASSERT(sizeof(i64) == 8, "expected i64 to be 8 bytes.");

// ST_ASSERT(sizeof(f32) == 4, "expected f32 to be 4 bytes.");
// ST_ASSERT(sizeof(f64) == 8, "expected f64 to be 8 bytes.");

// platform detection

// windows
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
    #define IPLATFORM_WINDOWS 1
    #ifndef _WIN64
        #error "64-bit is required on windows."
    #endif
// linux
#elif defined(__linux__) || defined(__gnu_linux__)
    #define IPLATFORM_LINUX 1
#elif defined(__unix__)
    #define IPLATFORM_UNIX 1
#elif defined(__POSIX__)
    #define IPLATFORM_POSIX 1
// catch any other unsupported OS
#else
    #error "platform is not supported."
#endif

#ifndef EXIT_FAILURE
    #define EXIT_FAILURE 1
#endif

#endif // DEFINES_H