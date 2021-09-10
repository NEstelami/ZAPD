#pragma once

#include <array>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>

#include "ZFile.h"
#include "Utils/vt.h"

#ifdef _MSC_VER
#define __PRETTY_FUNCTION__ __FUNCSIG__
#elif not defined(__GNUC__)
#define __PRETTY_FUNCTION__ __func__
#endif

// Macros for displaying warnings/errors
#define VT_HILITE VT_BFGCOL(WHITE)
#define VT_WARN VT_BFGCOL(PURPLE)
#define VT_ERR VT_BFGCOL(RED)

#define HILITE(string) (VT_HILITE string VT_RST)
#define WARN_FMT(string) (VT_WARN string VT_RST)
#define ERR_FMT(string) (VT_WARN string VT_RST)

// Maybe make WARN_LF instead
#define WARN_INDT "\t"
#define HANG_INDT "\t\t"

// TODO: better names
#define HANDLE_ERROR(header, body) WarningHandler::Error_Plain(__FILE__, __LINE__, __PRETTY_FUNCTION__, header, body)
#define HANDLE_WARNING(warningType, header, body) WarningHandler::Warning_Plain(__FILE__, __LINE__, __PRETTY_FUNCTION__, warningType, header, body)
#define HANDLE_WARNING_RESOURCE(warningType, parent, resource, offset, header, body) WarningHandler::Warning_Resource(__FILE__, __LINE__, __PRETTY_FUNCTION__, warningType, parent, resource, offset, header, body)
#define HANDLE_WARNING_BUILD(warningType, header, body) WarningHandler::Warning_Build(__FILE__, __LINE__, __PRETTY_FUNCTION__, warningType, header, body)

enum class WarningType {
    Everything,
    Always, // Warnings of this type are always printed, cannot be disabled.
    Deprecated,
    Unaccounted,
    MissingOffsets,
    Intersection,
    MissingAttribute, // MissingXmlAttributes?
    InvalidAttributeValue,
    UnknownAttribute,
    InvalidXML,
    InvalidJPEG,
    MissingSegment,
    NotImplemented,

    Max,
};

class WarningHandler {
public:
    static std::unordered_map<std::string, WarningType> warningsStringToTypeMap;
    static std::unordered_map<WarningType, const char*> warningsTypeToStringMap;
    static std::unordered_set<WarningType> warningsEnabledByDefault;

    static std::array<bool, static_cast<size_t>(WarningType::Max)> enabledWarnings;

    static void Init(int argc, char* argv[]);

    static bool IsWarningEnabled(WarningType warnType);
    
    static void FunctionPreamble(const char* filename, int32_t line, const char* function);
    static void ProcessedFilePreamble();
    static void ExtractedFilePreamble(ZFile *parent, ZResource *res, uint32_t offset);
    static std::string ConstructMessage(std::string message, const std::string& header, const std::string& body);


    [[ noreturn ]]
    static void PrintErrorAndThrow(const std::string& header, const std::string& body);
    static void PrintWarningBody(const std::string& header, const std::string& body);

    static void Error_Plain(const char* filename, int32_t line, const char* function, const std::string& header, const std::string& body);
    //static void Error_Resource(const std::string& filename, int32_t line, const char* function, const std::string& header, const std::string& body);


    static void WarningTypeAndChooseEscalate(WarningType warnType, const std::string& header, const std::string& body);

    static void Warning_Plain(const char* filename, int32_t line, const char* function, WarningType warnType, const std::string& header, const std::string& body);
    static void Warning_Resource(const char* filename, int32_t line, const char* function, WarningType warnType, ZFile *parent, ZResource* res, uint32_t offset, const std::string& header, const std::string& body);
    static void Warning_Build(const char* filename, int32_t line, const char* function, WarningType warnType, const std::string& header, const std::string& body);


    static void PrintHelp();

protected:
    static bool Werror;
};
