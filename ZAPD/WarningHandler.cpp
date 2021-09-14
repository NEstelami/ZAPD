#include "WarningHandler.h"

#include <cassert>

#include "Globals.h"
#include "Utils/StringHelper.h"

// If a warning isn't in this list, it would not be possible to enable/disable it
std::unordered_map<std::string, WarningType> WarningHandler::warningsStringToTypeMap = {
    {"deprecated", WarningType::Deprecated},
    {"unaccounted", WarningType::Unaccounted},
    {"missing-offsets", WarningType::MissingOffsets},
    {"intersection", WarningType::Intersection},
    {"missing-attribute", WarningType::MissingAttribute},
    {"invalid-attribute-value", WarningType::InvalidAttributeValue},
    {"unknown-attribute", WarningType::UnknownAttribute},
    {"invalid-xml", WarningType::InvalidXML},
    {"invalid-jpeg", WarningType::InvalidJPEG},
    {"missing-segment", WarningType::MissingSegment},
    {"not-implemented", WarningType::NotImplemented},
};
std::unordered_map<WarningType, const char*> WarningHandler::warningsTypeToStringMap = {
    {WarningType::Deprecated, "deprecated"},
    {WarningType::Unaccounted, "unaccounted"},
    {WarningType::MissingOffsets, "missing-offsets"},
    {WarningType::Intersection, "intersection"},
    {WarningType::MissingAttribute, "missing-attribute"},
    {WarningType::InvalidAttributeValue, "invalid-attribute-value"},
    {WarningType::UnknownAttribute, "unknown-attribute"},
    {WarningType::InvalidXML, "invalid-xml"},
    {WarningType::InvalidJPEG, "invalid-jpeg"},
    {WarningType::MissingSegment, "missing-segment"},
    {WarningType::NotImplemented, "not-implemented"},
};
std::unordered_set<WarningType> WarningHandler::warningsEnabledByDefault = {
    WarningType::Always,
    WarningType::Intersection,
#ifdef DEPRECATION_ON
    WarningType::Deprecated,
#endif
    //WarningType::MissingOffsets,
    WarningType::MissingAttribute,
    WarningType::InvalidAttributeValue,
    WarningType::UnknownAttribute,
    WarningType::InvalidXML,
    WarningType::InvalidJPEG,
    WarningType::NotImplemented,
};

std::array<bool, static_cast<size_t>(WarningType::Max)> WarningHandler::enabledWarnings = {false};

bool WarningHandler::Werror = false;

void WarningHandler::Init(int argc, char* argv[]) {
    // Werror = false;
    // for (size_t i = 0; i < enabledWarnings.size(); i++) {
    //     enabledWarnings[i] = false;
    // }

    for (const auto& warnType: warningsEnabledByDefault) {
        enabledWarnings[static_cast<size_t>(warnType)] = true;
    }

    for (int i = 1; i < argc; i++) {

        // If it doesn't start with "-W" skip it.
        if (argv[i][0] != '-' || argv[i][1] != 'W' || argv[i][2] == '\0') {
            continue;
        }

        bool warningTypeOn = true; // ?
        size_t startingIndex = 2;

        // "-Wno-"
        if (argv[i][2] == 'n' && argv[i][3] == 'o' && argv[i][4] == '-' && argv[i][5] != '\0') {
            warningTypeOn = false;
            startingIndex = 5;
        }

        // Skip "-W" or "-Wno-"
        std::string_view currentArgv = &argv[i][startingIndex];

        if (currentArgv == "error") {
            Werror = warningTypeOn;
        } else if (currentArgv == "everything") {
            for (size_t i = 0; i < enabledWarnings.size(); i++) {
                enabledWarnings[i] = warningTypeOn;
            }
        } else {
            auto warningTypeIter = warningsStringToTypeMap.find(std::string(currentArgv));
            if (warningTypeIter != warningsStringToTypeMap.end()) {
                size_t index = static_cast<size_t>(warningTypeIter->second);
                enabledWarnings[index] = warningTypeOn;
            }
            else {
                HANDLE_WARNING(WarningType::Always, StringHelper::Sprintf("Unknown warning flag '%s'", argv[i]), "");
            }
        }
    }
}

bool WarningHandler::IsWarningEnabled(WarningType warnType) {
    assert(static_cast<size_t>(warnType) >= 0 && warnType < WarningType::Max);

    if (enabledWarnings.at(static_cast<size_t>(WarningType::Everything))) {
        return true;
    }
    if (enabledWarnings.at(static_cast<size_t>(warnType))) {
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
void WarningHandler::ExtractedFilePreamble(ZFile *parent, ZResource* res, uint32_t offset) {
    fprintf(stderr, "in input binary file %s, ", parent->GetName().c_str());
    if (res != nullptr) {
        fprintf(stderr, "resource '%s' at ", res->GetName().c_str());
    }
    fprintf(stderr, "offset 0x%06X: ", offset); 
}

/**
 * Construct the rest of the message, after warning:/error:
 */
std::string WarningHandler::ConstructMessage(std::string message, const std::string& header, const std::string& body) {
    message.reserve(message.size() + header.size() + body.size() + 10 * (sizeof(HANG_INDT) - 1));
    message += StringHelper::Sprintf(HILITE("%s"), header.c_str());
    message += "\n";

    if (std::string_view(body) == "") {
        return message;
    }

    message += HANG_INDT;
    for (const char* ptr = body.c_str(); *ptr != '\0'; ptr++) {
        message += *ptr;
        if (*ptr == '\n') {
            message += HANG_INDT;
        }
    }

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

void WarningHandler::Error_Plain(const char* filename, int32_t line, const char* function, const std::string& header, const std::string& body) {
    FunctionPreamble(filename, line, function);

    WarningHandler::PrintErrorAndThrow(header, body);
}

void WarningHandler::WarningTypeAndChooseEscalate(WarningType warnType, const std::string& header, const std::string& body) {

    std::string headerMsg = header;
    auto warningNameIter = warningsTypeToStringMap.find(warnType);
    if (warningNameIter != warningsTypeToStringMap.end()) {
        headerMsg += StringHelper::Sprintf(" [-W%s]", warningNameIter->second);
    }

    if (Werror) {
        WarningHandler::PrintErrorAndThrow(headerMsg, body);
    } else {
        WarningHandler::PrintWarningBody(headerMsg, body);

    }
}


/* Warning types, to be used via the macros */

void WarningHandler::Warning_Plain(const char* filename, int32_t line, const char* function, WarningType warnType, const std::string& header, const std::string& body) {
    if (!IsWarningEnabled(warnType)) {
        return;
    }

    FunctionPreamble(filename, line, function);

    WarningHandler::WarningTypeAndChooseEscalate(warnType, header, body);
}

void WarningHandler::Warning_Resource(const char* filename, int32_t line, const char* function, WarningType warnType, ZFile *parent, ZResource* res, uint32_t offset, const std::string& header, const std::string& body) {
    assert(parent != nullptr);

    if (!IsWarningEnabled(warnType)) {
        return;
    }

    FunctionPreamble(filename, line, function);
    ProcessedFilePreamble();
    ExtractedFilePreamble(parent, res, offset);

    WarningHandler::WarningTypeAndChooseEscalate(warnType, header, body);
}

void WarningHandler::Warning_Build(const char* filename, int32_t line, const char* function, WarningType warnType, const std::string& header, const std::string& body) {
    if (!IsWarningEnabled(warnType)) {
        return;
    }

    FunctionPreamble(filename, line, function);
    ProcessedFilePreamble();

    WarningHandler::WarningTypeAndChooseEscalate(warnType, header, body);
}

void WarningHandler::PrintHelp() {
    printf("\nExisting warnings:\n");
    for (const auto& iter: warningsTypeToStringMap) {
        const char* enabledMsg = "";
        if (warningsEnabledByDefault.find(iter.first) != warningsEnabledByDefault.end()) {
            enabledMsg = "(enabled)";
        }

        printf("\t -W%-25s %s\n", iter.second, enabledMsg);
    }

    // TODO: mention -Weverything and -Werror

    printf("\n");
    printf("Warnings can be disabled with `-Wno-` instead of `-W`\n");
}
