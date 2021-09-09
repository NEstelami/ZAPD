#pragma once

#include <array>
#include <string>
#include <string_view>

#include "ZFile.h"
#include "Utils/vt.h"

#ifdef _MSC_VER
#define __PRETTY_FUNCTION__ __FUNCSIG__
#elif not defined(__GNUC__)
#define __PRETTY_FUNCTION__ __func__
#endif

// TODO: better names
#define HANDLE_ERROR(msg) ErrorHandler::Error(__FILE__, __LINE__, __PRETTY_FUNCTION__, msg)
#define HANDLE_WARNING(warningType, msg) ErrorHandler::Warning(__FILE__, __LINE__, __PRETTY_FUNCTION__, warningType, msg)

enum class WarningType {
    Everything,
    Always, // Warnings of this type is always printed
    Deprecated,

    Max,
};

class ErrorHandler {
public:
    static std::array<bool, static_cast<size_t>(WarningType::Max)> enabledWarnings;

    static void Init(int argc, char* argv[]);

    [[ noreturn ]]
    static void Error(const char* filename, int32_t line, const char* function, const std::string& msg);
    //[[ noreturn ]]
    //static void Error_Resource(const std::string& filename, int32_t line, const char* function, const std::string& msg);

    // variadic?
    static void Warning(const char* filename, int32_t line, const char* function, WarningType warnType, const std::string& msg);
    static void Warning_Resource(const char* filename, int32_t line, const char* function, WarningType warnType, ZFile *parent, uint32_t offset, const std::string& msg);


protected:
    static bool Werror;
};
