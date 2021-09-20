/**
 * ZAPD Warning- and Error-handling system (?)
 *
 * This provides a common standard way to write ZAPD warnings/errors, which should be used for all
 * such. It will pretty-print them in a uniform way, with styles defined in the header.
 *
 * Warnings/errors should be constructed using the macros given in the header; there are now plenty
 * of examples in the codebase of how to do this. Their purposes are noted above each category in
 * the header. Each warning has a type, one of the ones in warningStringToInitMap, or
 * WarningType::Always, which is used for warnings that cannot be disabled and do not display a
 * type.
 *
 * Currently there are three levels of alert a warning can have:
 * - Off (does not display anything)
 * - Warn (print a warning but continue processing)
 * - Err (behave like an error, i.e. print and throw an exception to crash ZAPD when occurs)
 *
 * Flag use:
 * - -Wno-foo disables warnings of type foo
 * - -Wfoo enables warnings of type foo
 * - -Werror=foo escalates foo to behave like an error
 * - -Weverything enables all warnings
 * - -Werror escalates all warnings to errors
 *
 * Errors do not have types, and will always throw an exception; they cannot be disabled.
 */
#include "WarningHandler.h"

#include <cassert>
#include "Globals.h"
#include "Utils/StringHelper.h"

typedef struct
{
	WarningType type;
	WarningLevel defaultLevel;
	std::string description;
} WarningInfoInit;

typedef struct
{
	WarningLevel level;
	std::string name;
	std::string description;
} WarningInfo;

/**
 * Master list of all default warning types and features
 *
 * To add a warning type, fill in a new row of this map. Think carefully about what its default
 * level should be, and try and make the description both brief and informative: it is used in the
 * help message, so again, think about what the end user needs to know.
 */
// clang-format off
static const std::unordered_map<std::string, WarningInfoInit> warningStringToInitMap = {
    {"deprecated",              {WarningType::Deprecated,               
#ifdef DEPRECATION_ON
    WarningLevel::Warn,
#else
    WarningLevel::Off,
#endif
    "Deprecated features"}},
    {"unaccounted",             {WarningType::Unaccounted,           WarningLevel::Off,  "Large blocks of unaccounted"}},
    {"missing-offsets",         {WarningType::MissingOffsets,        WarningLevel::Warn, "Offset attribute missing in XML tag"}},
    {"intersection",            {WarningType::Intersection,          WarningLevel::Warn, "Two assets intersect"}},
    {"missing-attribute",       {WarningType::MissingAttribute,      WarningLevel::Warn, "Required attribute missing in XML tag"}},
    {"invalid-attribute-value", {WarningType::InvalidAttributeValue, WarningLevel::Warn, "Attribute declared in XML is wrong"}},
    {"unknown-attribute",       {WarningType::UnknownAttribute,      WarningLevel::Warn, "Unknown attribute in XML entry tag"}},
    {"invalid-xml",             {WarningType::InvalidXML,            WarningLevel::Warn, "XML has syntax errors"}},
    {"invalid-jpeg",            {WarningType::InvalidJPEG,           WarningLevel::Warn, "JPEG file does not conform to the game's format requirements"}},
    {"invalid-png",             {WarningType::InvalidPNG,            WarningLevel::Warn, "Issues arising when processing PNG data"}},
    {"invalid-extracted-data",  {WarningType::InvalidExtractedData,  WarningLevel::Warn, "Extracted data does not have correct form"}},
    {"missing-segment",         {WarningType::MissingSegment,        WarningLevel::Warn, "Segment not given in File tag in XML"}},
    {"hardcoded-pointer",       {WarningType::HardcodedPointer,      WarningLevel::Warn, "ZAPD lacks the info to make a symbol, so must output a hardcoded pointer"}},
    {"not-implemented",         {WarningType::NotImplemented,        WarningLevel::Warn, "ZAPD does not currently support this feature"}},
};

/**
 * Map constructed at runtime to contain the warning features as set by the user using -W flags.
 */
static std::unordered_map<WarningType, WarningInfo> warningTypeToInfoMap;

void WarningHandler::ConstructTypeToInfoMap() {
    for (auto& entry : warningStringToInitMap) {
        warningTypeToInfoMap[entry.second.type] = {entry.second.defaultLevel, entry.first, entry.second.description};
    }
    warningTypeToInfoMap[WarningType::Always] = {WarningLevel::Warn, "always", "you shouldn't be reading this"};
    assert(warningTypeToInfoMap.size() == static_cast<size_t>(WarningType::Max));
}

bool WarningHandler::Werror = false;

/**
 * Initialises the main warning type map and reads flags passed to set warning level.
 */
void WarningHandler::Init(int argc, char* argv[]) {
    ConstructTypeToInfoMap();

    for (int i = 1; i < argc; i++) {

        // If it doesn't start with "-W" skip it.
        if (argv[i][0] != '-' || argv[i][1] != 'W' || argv[i][2] == '\0') {
            continue;
        }

        WarningLevel warningTypeOn = WarningLevel::Warn;
        size_t startingIndex = 2;

        // "-Wno-"
        if (argv[i][2] == 'n' && argv[i][3] == 'o' && argv[i][4] == '-' && argv[i][5] != '\0') {
            warningTypeOn = WarningLevel::Off;
            startingIndex = 5;
        }

        // Read starting after the "-W" or "-Wno-"
        std::string_view currentArgv = &argv[i][startingIndex];

        if (currentArgv == "error") {
            Werror = warningTypeOn != WarningLevel::Off;
        } else if (currentArgv == "everything") {
            for (auto& it: warningTypeToInfoMap) {
                if (it.second.level <= WarningLevel::Warn) {
                    it.second.level = warningTypeOn;
                }
            }
        } else {
            if (currentArgv.rfind("error=", 0) == 0) {
                // Read starting after the "error=" part
                currentArgv = &argv[i][startingIndex + 6];
                warningTypeOn = warningTypeOn != WarningLevel::Off ? WarningLevel::Err : WarningLevel::Warn;
            }

            auto it = warningStringToInitMap.find(std::string(currentArgv));
            if (it != warningStringToInitMap.end()) {
                warningTypeToInfoMap[it->second.type].level = warningTypeOn;
            }
            else {
                HANDLE_WARNING(WarningType::Always, StringHelper::Sprintf("Unknown warning flag '%s'", argv[i]), "");
            }
        }
    }
}

bool WarningHandler::IsWarningEnabled(WarningType warnType) {
    assert(static_cast<size_t>(warnType) >= 0 && warnType < WarningType::Max);

    if (Globals::Instance->verbosity >= VerbosityLevel::VERBOSITY_DEBUG) {
        printf("IsWarningEnabled:\n");
        auto& thingy = warningTypeToInfoMap.at(warnType);
        printf("\t%s: %i\n", thingy.name.c_str(),  static_cast<int32_t>(thingy.level));
    }

    if (warnType == WarningType::Always) {
        return true;
    }
    if (warningTypeToInfoMap.at(warnType).level != WarningLevel::Off) {
        return true;
    }
    return false;
}

bool WarningHandler::WasElevatedToError(WarningType warnType) {
    assert(static_cast<size_t>(warnType) >= 0 && warnType < WarningType::Max);

    if (!IsWarningEnabled(warnType)) {
        return false;
    }

    if (Werror) {
        return true;
    }

    if (warningTypeToInfoMap.at(warnType).level == WarningLevel::Err) {
        return true;
    }

    return false;
}

/**
 * Print file/line/function info for debugging
 */
void WarningHandler::FunctionPreamble(const char* filename, int32_t line, const char* function) {
    // if (Globals::Instance->???) {
        fprintf(stderr, "%s:%i: in function %s:\n", filename, line, function);
    // }
}

/**
 *  Print the information about the file(s) being processed (XML for extraction, png etc. for building)
 */
void WarningHandler::ProcessedFilePreamble() {
    if (Globals::Instance->inputPath != "") {
        fprintf(stderr, "When processing file %s: ", Globals::Instance->inputPath.c_str());
    }
}

/**
 *  Print information about the binary file being extracted
 */
void WarningHandler::ExtractedFilePreamble(const ZFile *parent, const ZResource* res, const uint32_t offset) {
    fprintf(stderr, "in input binary file %s, ", parent->GetName().c_str());
    if (res != nullptr) {
        fprintf(stderr, "resource '%s' at ", res->GetName().c_str());
    }
    fprintf(stderr, "offset 0x%06X: \n\t", offset); 
}

/**
 * Construct the rest of the message, after warning:/error. The message is filled in one character at a time, with indents added after newlines
 */
std::string WarningHandler::ConstructMessage(std::string message, const std::string& header, const std::string& body) {
    message.reserve(message.size() + header.size() + body.size() + 10 * (sizeof(HANG_INDT) - 1));
    message += StringHelper::Sprintf(HILITE("%s"), header.c_str());
    message += "\n";

    if (body == "") {
        return message;
    }

    message += HANG_INDT;
    for (const char* ptr = body.c_str(); *ptr != '\0'; ptr++) {
        message += *ptr;
        if (*ptr == '\n') {
            message += HANG_INDT;
        }
    }
    message += "\n";

    return message;
}

void WarningHandler::PrintErrorAndThrow(const std::string& header, const std::string& body) {
    std::string errorMsg = ERR_FMT("error: ");
    throw std::runtime_error(ConstructMessage(errorMsg, header, body));
}

void WarningHandler::PrintWarningBody(const std::string& header, const std::string& body) {
    std::string errorMsg = WARN_FMT("warning: ");
    fprintf(stderr, "%s", ConstructMessage(errorMsg, header, body).c_str());
}

/* Error types, to be used via the macros */

void WarningHandler::ErrorType(WarningType warnType, const std::string& header, const std::string& body) {
    std::string headerMsg = header;

    for (const auto& iter: warningStringToInitMap) {
        if (iter.second.type == warnType) {
            headerMsg += StringHelper::Sprintf(" [%s]", iter.first.c_str());
        }
    }

    PrintErrorAndThrow(headerMsg, body);
}

void WarningHandler::Error_Plain(const char* filename, int32_t line, const char* function, WarningType warnType, const std::string& header, const std::string& body) {
    FunctionPreamble(filename, line, function);

    ErrorType(warnType, header, body);
}

void WarningHandler::Error_Resource(const char* filename, int32_t line, const char* function, WarningType warnType, const ZFile *parent, const ZResource* res, const uint32_t offset, const std::string& header, const std::string& body) {
    assert(parent != nullptr);

    FunctionPreamble(filename, line, function);
    ProcessedFilePreamble();
    ExtractedFilePreamble(parent, res, offset);

    ErrorType(warnType, header, body);
}


void WarningHandler::WarningTypeAndChooseEscalate(WarningType warnType, const std::string& header, const std::string& body) {
    std::string headerMsg = header;

    for (const auto& iter: warningStringToInitMap) {
        if (iter.second.type == warnType) {
            headerMsg += StringHelper::Sprintf(" [-W%s]", iter.first.c_str());
        }
    }

    if (WasElevatedToError(warnType)) {
        PrintErrorAndThrow(headerMsg, body);
    } else {
        PrintWarningBody(headerMsg, body);
    }
}


/* Warning types, to be used via the macros */

void WarningHandler::Warning_Plain(const char* filename, int32_t line, const char* function, WarningType warnType, const std::string& header, const std::string& body) {
    if (!IsWarningEnabled(warnType)) {
        return;
    }

    FunctionPreamble(filename, line, function);

    WarningTypeAndChooseEscalate(warnType, header, body);
}

void WarningHandler::Warning_Resource(const char* filename, int32_t line, const char* function, WarningType warnType, const ZFile *parent, const ZResource* res, const uint32_t offset, const std::string& header, const std::string& body) {
    assert(parent != nullptr);

    if (!IsWarningEnabled(warnType)) {
        return;
    }

    FunctionPreamble(filename, line, function);
    ProcessedFilePreamble();
    ExtractedFilePreamble(parent, res, offset);

    WarningTypeAndChooseEscalate(warnType, header, body);
}

void WarningHandler::Warning_Build(const char* filename, int32_t line, const char* function, WarningType warnType, const std::string& header, const std::string& body) {
    if (!IsWarningEnabled(warnType)) {
        return;
    }

    FunctionPreamble(filename, line, function);
    ProcessedFilePreamble();

    WarningTypeAndChooseEscalate(warnType, header, body);
}


/* Help-related functions */

/**
 * Print each warning name, default status, and description using the init map
 */
void WarningHandler::PrintHelp() {
    uint columnWidth = 25;
    std::string dt;

    printf("\nWarning types ( * means enabled by default)\n");
    for (auto& entry : warningStringToInitMap) {
        if (entry.second.defaultLevel <= WarningLevel::Warn) {
            dt = "-W";
            dt += entry.first;
            if (entry.second.defaultLevel == WarningLevel::Warn) {
                dt += " *";
            }
            printf(HELP_DT_INDT "%-*s", columnWidth, dt.c_str());

            if (dt.length() + 2 > columnWidth) {
                printf("\n" HELP_DT_INDT "%-*s", columnWidth, "");
            }
            printf("%s\n", entry.second.description.c_str());
        }
    }

    printf("\nDefault errors\n");
    for (auto& entry : warningStringToInitMap) {
        if (entry.second.defaultLevel > WarningLevel::Warn) {
            dt = "-W";
            dt += entry.first;
            printf(HELP_DT_INDT "%-*s", columnWidth, dt.c_str());

            if (dt.length() + 2 > columnWidth) {
                printf("\n" HELP_DT_INDT "%*s", columnWidth, "");
            }
            printf("%s\n", entry.second.description.c_str());
        }
    }

    printf("\n");
    printf("Other\n" HELP_DT_INDT "-Weverything will enable all existing warnings.\n" HELP_DT_INDT "-Werror will promote all warnings to errors.\n");

    printf("\n");
    printf("Warnings can be disabled using -Wno-... instead of -W...; -Weverything will override any -Wno-... flags passed before it.\n");
}

/**
 * Print which warnings are currently enabled
 */
void WarningHandler::PrintWarningsDebugInfo()
{
    std::string dt;

    printf("Warnings status:\n");
    for (auto& it: warningTypeToInfoMap) {
        dt = it.second.name;
        dt += ": ";

        printf(HELP_DT_INDT "%-25s", dt.c_str());
        switch (it.second.level)
        {
        case WarningLevel::Off:
            printf(VT_FGCOL(LIGHTGRAY) "Off" VT_RST);
            break;
        case WarningLevel::Warn:
            printf(VT_FGCOL(YELLOW) "Warn" VT_RST);
            break;
        case WarningLevel::Err:
            printf(VT_FGCOL(RED) "Err" VT_RST);
            break;

        }
        printf("\n");
    }
    printf("\n");
}
