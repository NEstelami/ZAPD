#ifndef CRASH_HANDLER_H
#define CRASH_HANDLER_H
#if __has_include(<unistd.h>)
#define HAS_POSIX 1
#else
#define HAS_POSIX 0
#endif


#include <array>
#include <cstdio>
#include <cstdlib>
#include <ctime>

#if HAS_POSIX == 1
#include <csignal>
#include <cxxabi.h>  // for __cxa_demangle
#include <dlfcn.h>   // for dladdr
#include <execinfo.h>
#include <unistd.h>
void ErrorHandler(int sig);
#elif defined(_MSC_VER)
#include <Windows.h>
#include <DbgHelp.h>

#include <inttypes.h>
LONG seh_filter(_EXCEPTION_POINTERS* ex);

#pragma comment(lib, "Dbghelp.lib")
#endif

#endif
