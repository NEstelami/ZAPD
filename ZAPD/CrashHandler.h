#include <array>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <ctime>

#if defined(__linux__)
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
