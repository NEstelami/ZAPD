#pragma once

#include <array>
#include <string>

#include "ZFile.h"
#include "Utils/vt.h"

#ifdef _MSC_VER
#define __PRETTY_FUNCTION__ __FUNCSIG__
#elif not defined(__GNUC__)
#define __PRETTY_FUNCTION__ __func__
#endif

// TODO: better names
#define HANDLE_ERROR(msg) ErrorHandler::Error(__FIlE__, __LINE__, __PRETTY_FUNCTION__, msg)
#define HANDLE_WARNING(msg) ErrorHandler::Warning(__FIlE__, __LINE__, __PRETTY_FUNCTION__, msg)

enum class WarningType {
    Everything,

    Max,
};

class ErrorHandler {
public:
    static std::array<bool, static_cast<size_t>(WarningType::Max)> enabledWarnings;

    [[ noreturn ]]
    static void Error(const char* filename, int32_t line, const char* function, const std::string& msg);
    //[[ noreturn ]]
    //static void Error_Resource(const std::string& filename, int32_t line, const char* function, const std::string& msg);

    // variadic?
    static void Warning(const char* filename, int32_t line, const char* function, const std::string& msg);
    static void Warning_Resource(const char* filename, int32_t line, const char* function, ZFile *parent, uint32_t offset, const std::string& msg);
};
