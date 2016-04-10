#ifndef PLATFORM_H
#define PLATFORM_H

// This method borrowed from Polycode (which I've used before).
// If you need to do something that's platform dependent, #include
// this header and use:
// #if PLATFORM == PLATFORM_WINDOWS
// ...
// #endif

#define PLATFORM_WINDOWS  1
#define PLATFORM_MAC      2
#define PLATFORM_UNIX     3

#ifdef _WINDOWS
    #define PLATFORM PLATFORM_WINDOWS
#elif defined(__APPLE__) && defined(__MACH__)
    #define PLATFORM PLATFORM_MAC
#else
    #define PLATFORM PLATFORM_UNIX
#endif

// http://stackoverflow.com/questions/1505582/determining-32-vs-64-bit-in-c
#if _WIN32 || _WIN64
#if _WIN64
#define ENVIRONMENT64
#else
#define ENVIRONMENT32
#endif
#endif

// Check GCC
#if __GNUC__
#if __x86_64__ || __ppc64__
#define ENVIRONMENT64
#else
#define ENVIRONMENT32
#endif
#endif

#endif    // PLATFORM_H