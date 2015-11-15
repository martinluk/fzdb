#ifndef PLATFORM_H
#define PLATFORM_H

// This method borrowed from Polycode (which I've used before).
// If you need to do something that's platform dependent, use:
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

#endif	// PLATFORM_H