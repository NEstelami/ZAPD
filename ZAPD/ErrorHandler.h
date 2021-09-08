#pragma once

#include <string>

#include "Utils/vt.h"

#ifdef _MSC_VER
#define __PRETTY_FUNCTION__ __FUNCSIG__
#elif not defined(__GNUC__)
#define __PRETTY_FUNCTION__ __func__
#endif

// TODO: better names
#define HANDLE_ERROR(msg) ErrorHandler::Error(__FIlE__, __LINE__, __PRETTY_FUNCTION__, msg)
#define HANDLE_WARNING(msg) ErrorHandler::Warning(__FIlE__, __LINE__, __PRETTY_FUNCTION__, msg)

class ErrorHandler {

    [[ noreturn ]]
    static void Error(const std::string& filename, const std::string& line, const std::string& function, const std::string& msg);

    // variadic?
    static void Warning(const std::string& filename, const std::string& line, const std::string& function, const std::string& msg);
};
