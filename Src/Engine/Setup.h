#pragma once

#include <cassert>

// Version
#define AWAY_VERSION                "1.0"
#define AWAY_VERSION_MAJOR            1
#define AWAY_VERSION_MINOR            0
#define AWAY_VERSION_PATCH            0

// Platform
#define AWAY_PLATFORM_UNKNOWN        0x00000000
#define AWAY_PLATFORM_WINDOWS        0x00010000
#define AWAY_PLATFORM_LINUX          0x00020000
#define AWAY_PLATFORM_OSX            0x00040000
#define AWAY_PLATFORM_IOS            0x00080000
#define AWAY_PLATFORM_ANDROID        0x00100000
#define AWAY_PLATFORM_HTML5          0x02000000

#ifdef AWAY_FORCE_PLATFORM_UNKNOWN
# define AWAY_PLATFORM AWAY_PLATFORM_UNKNOWN
#elif defined(__APPLE__)
# include "TargetConditionals.h"
# if TARGET_OS_IPHONE
#  define AWAY_PLATFORM AWAY_PLATFORM_IOS
# elif TARGET_OS_MAC
#  define AWAY_PLATFORM AWAY_PLATFORM_OSX
# endif
#elif defined(_WIN32)
# define AWAY_PLATFORM AWAY_PLATFORM_WINDOWS
#elif defined(__ANDROID__)
# define AWAY_PLATFORM AWAY_PLATFORM_ANDROID
#elif defined(__linux)
# define AWAY_PLATFORM AWAY_PLATFORM_LINUX
#elif defined(EMSCRIPTEN)
# define AWAY_PLATFORM AWAY_PLATFORM_HTML5
#else
# define AWAY_PLATFORM AWAY_PLATFORM_UNKNOWN
#endif

#if !AWAY_PLATFORM
# warning "Your platform cannot be detected, it may be unsupported by AWAY."
#endif

// Compiler
#define AWAY_COMPILER_UNKNOWN        0x00000000

// Intel
#define AWAY_COMPILER_INTEL          0x00100000
#define AWAY_COMPILER_INTEL9         0x00100010
#define AWAY_COMPILER_INTEL10_0      0x00100020
#define AWAY_COMPILER_INTEL10_1      0x00100030
#define AWAY_COMPILER_INTEL11_0      0x00100040
#define AWAY_COMPILER_INTEL11_1      0x00100050
#define AWAY_COMPILER_INTEL12_0      0x00100060
#define AWAY_COMPILER_INTEL12_1      0x00100070
#define AWAY_COMPILER_INTEL13_0      0x00100080

// Visual C++ defines
#define AWAY_COMPILER_VC             0x01000000
#define AWAY_COMPILER_VC8            0x01000070
#define AWAY_COMPILER_VC9            0x01000080
#define AWAY_COMPILER_VC10           0x01000090
#define AWAY_COMPILER_VC11           0x010000A0
#define AWAY_COMPILER_VC12           0x010000B0

// GCC defines
#define AWAY_COMPILER_GCC            0x02000000
#define AWAY_COMPILER_GCC34          0x02000050
#define AWAY_COMPILER_GCC35          0x02000060
#define AWAY_COMPILER_GCC40          0x02000070
#define AWAY_COMPILER_GCC41          0x02000080
#define AWAY_COMPILER_GCC42          0x02000090
#define AWAY_COMPILER_GCC43          0x020000A0
#define AWAY_COMPILER_GCC44          0x020000B0
#define AWAY_COMPILER_GCC45          0x020000C0
#define AWAY_COMPILER_GCC46          0x020000D0
#define AWAY_COMPILER_GCC47          0x020000E0
#define AWAY_COMPILER_GCC48          0x020000F0
#define AWAY_COMPILER_GCC49          0x02000100

// Borland C++
#define AWAY_COMPILER_BC             0x04000000

// CodeWarrior
#define AWAY_COMPILER_CODEWARRIOR    0x08000000

// Clang
#define AWAY_COMPILER_CLANG          0x20000000
#define AWAY_COMPILER_CLANG26        0x20000010
#define AWAY_COMPILER_CLANG27        0x20000020
#define AWAY_COMPILER_CLANG28        0x20000030
#define AWAY_COMPILER_CLANG29        0x20000040
#define AWAY_COMPILER_CLANG30        0x20000050
#define AWAY_COMPILER_CLANG31        0x20000060
#define AWAY_COMPILER_CLANG32        0x20000070
#define AWAY_COMPILER_CLANG33        0x20000080
#define AWAY_COMPILER_CLANG40        0x20000090
#define AWAY_COMPILER_CLANG41        0x200000A0
#define AWAY_COMPILER_CLANG42        0x200000B0
#define AWAY_COMPILER_CLANG43        0x200000C0
#define AWAY_COMPILER_CLANG50        0x200000D0
#define AWAY_COMPILER_CLANG60        0x200000E0

// LLVM GCC
#define AWAY_COMPILER_LLVM_GCC       0x40000000

// Build model
#define AWAY_MODEL_32                0x00000010
#define AWAY_MODEL_64                0x00000020

// Force generic C++ compiler
#ifdef AWAY_FORCE_COMPILER_UNKNOWN
# define AWAY_COMPILER AWAY_COMPILER_UNKNOWN
#elif defined(__INTEL_COMPILER)
# if __INTEL_COMPILER == 900
#  define AWAY_COMPILER AWAY_COMPILER_INTEL9
# elif __INTEL_COMPILER == 1000
#  define AWAY_COMPILER AWAY_COMPILER_INTEL10_0
# elif __INTEL_COMPILER == 1010
#  define AWAY_COMPILER AWAY_COMPILER_INTEL10_1
# elif __INTEL_COMPILER == 1100
#  define AWAY_COMPILER AWAY_COMPILER_INTEL11_0
# elif __INTEL_COMPILER == 1110
#  define AWAY_COMPILER AWAY_COMPILER_INTEL11_1
# elif __INTEL_COMPILER == 1200
#  define AWAY_COMPILER AWAY_COMPILER_INTEL12_0
# elif __INTEL_COMPILER == 1210
#  define AWAY_COMPILER AWAY_COMPILER_INTEL12_1
# elif __INTEL_COMPILER >= 1300
#  define AWAY_COMPILER AWAY_COMPILER_INTEL13_0
# else
#  define AWAY_COMPILER AWAY_COMPILER_INTEL
# endif

// Visual C++
#elif defined(_MSC_VER)
# if _MSC_VER < 1700
#  error "Away requires Visual C++ 2012 or higher"
# elif _MSC_VER == 1700
#  define AWAY_COMPILER AWAY_COMPILER_VC11 // 2012
# elif _MSC_VER >= 1800
#  define AWAY_COMPILER AWAY_COMPILER_VC12 // 2013
# elif _MSC_VER >= 1900
#  define AWAY_COMPILER AWAY_COMPILER_VC13 // 2014
# else//_MSC_VER
#  define AWAY_COMPILER AWAY_COMPILER_VC
# endif//_MSC_VER

// Clang
#elif defined(__clang__)
# if (__clang_major__ <= 2) || ((__clang_major__ == 3) && (__clang_minor__ < 2))
#  error "AWAY requires Clang 3.2 or higher"
# elif(__clang_major__ == 3) && (__clang_minor__ == 2)
#  define AWAY_COMPILER AWAY_COMPILER_CLANG32
# elif(__clang_major__ == 3) && (__clang_minor__ == 3)
#  define AWAY_COMPILER AWAY_COMPILER_CLANG33
# elif(__clang_major__ == 4) && (__clang_minor__ == 0)
#  define AWAY_COMPILER AWAY_COMPILER_CLANG40
# elif(__clang_major__ == 4) && (__clang_minor__ == 1)
#  define AWAY_COMPILER AWAY_COMPILER_CLANG41
# elif(__clang_major__ == 4) && (__clang_minor__ == 2)
#  define AWAY_COMPILER AWAY_COMPILER_CLANG42
# elif(__clang_major__ == 4) && (__clang_minor__ >= 3)
#  define AWAY_COMPILER AWAY_COMPILER_CLANG43
# elif(__clang_major__ == 5)
#  define AWAY_COMPILER AWAY_COMPILER_CLANG50
# elif(__clang_major__ == 6)
#  define AWAY_COMPILER AWAY_COMPILER_CLANG60
# else
#  define AWAY_COMPILER AWAY_COMPILER_CLANG
# endif

// G++
#elif defined(__GNUC__) || defined(__MINGW32__)
# if (__GNUC__ <= 3) || ((__GNUC__ == 4) && (__GNUC_MINOR__ < 8))
#  error "AWAY requires GCC 4.8 or higher"
# elif (__GNUC__ == 4) && (__GNUC_MINOR__ == 8)
#  define AWAY_COMPILER (AWAY_COMPILER_GCC48)
# elif (__GNUC__ == 4) && (__GNUC_MINOR__ >= 9)
#  define AWAY_COMPILER (AWAY_COMPILER_GCC49)
# elif (__GNUC__ > 4 )
#  define AWAY_COMPILER (AWAY_COMPILER_GCC49)
# else
#  define AWAY_COMPILER (AWAY_COMPILER_GCC)
# endif

// Borland C++
#elif defined(_BORLANDC_)
# define AWAY_COMPILER AWAY_COMPILER_BC

// Codewarrior
#elif defined(__MWERKS__)
# define AWAY_COMPILER AWAY_COMPILER_CODEWARRIOR

#else
# define AWAY_COMPILER AWAY_COMPILER_UNKNOWN
#endif

#if AWAY_COMPILER == AWAY_COMPILER_UNKNOWN
# warning "Your compiler cannot be detected, it may be unsupported by AWAY."
#endif

// Build arch
#define AWAY_ARCH_32                 0x00000010
#define AWAY_ARCH_64                 0x00000020

#if (defined(__arch64__) || defined(__LP64__) || defined(_M_X64) || defined(__ppc64__) || defined(__x86_64__))
# define AWAY_ARCH    AWAY_ARCH_64
#elif(defined(__i386__) || defined(__ppc__))
# define AWAY_ARCH    AWAY_ARCH_32
#else
# define AWAY_ARCH    AWAY_ARCH_32
#endif

// Device
#define AWAY_DEVICE_UNKNOWN          0x00000000
#define AWAY_DEVICE_NATIVE           0x00000001
#define AWAY_DEVICE_BROWSER          0x00000002
#define AWAY_DEVICE_SIMULATOR        0x00000004
#define AWAY_DEVICE_SERVER           0x00000008

#if AWAY_PLATFORM & AWAY_PLATFORM_HTML5
# define AWAY_DEVICE AWAY_DEVICE_BROWSER
#elif AWAY_PLATFORM & AWAY_PLATFORM_IOS
# if TARGET_IPHONE_SIMULATOR
#  define AWAY_DEVICE AWAY_DEVICE_SIMULATOR
# else
#  define AWAY_DEVICE AWAY_DEVICE_NATIVE
# endif
#elif AWAY_PLATFORM & AWAY_PLATFORM_ANDROID
# define AWAY_DEVICE AWAY_DEVICE_NATIVE
#elif AWAY_PLATFORM & AWAY_PLATFORM_WINDOWS
# define AWAY_DEVICE AWAY_DEVICE_NATIVE
#elif AWAY_PLATFORM & AWAY_PLATFORM_OSX
# define AWAY_DEVICE AWAY_DEVICE_NATIVE
#elif AWAY_PLATFORM & AWAY_PLATFORM_LINUX
# define AWAY_DEVICE AWAY_DEVICE_NATIVE
#else
# define AWAY_DEVICE AWAY_DEVICE_UNKNOWN
#endif
