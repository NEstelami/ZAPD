#include "WarningHandler.h"

#include <cassert>
#include <unordered_map>
#include <vector>

#include "Globals.h"
#include "Utils/StringHelper.h"

// If a warning isn't in this list, it would not be possible to enable/disable it
static std::unordered_map<std::string, WarningType> sWarningsStringToTypeMap = {
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
static std::unordered_map<WarningType, const char*> sWarningsTypeToStringMap = {
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
static std::vector<WarningType> sWarningsEnabledByDefault = {
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

    for (const auto& warnType: sWarningsEnabledByDefault) {
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
            auto warningTypeIter = sWarningsStringToTypeMap.find(std::string(currentArgv));
            if (warningTypeIter != sWarningsStringToTypeMap.end()) {
                size_t index = static_cast<size_t>(warningTypeIter->second);
                enabledWarnings[index] = warningTypeOn;
            }
            else {
                HANDLE_WARNING(WarningType::Always, StringHelper::Sprintf("Unknown warning flag '%s'", argv[i]), "");
            }
        }
    }
}

void WarningHandler::Error(const char* filename, int32_t line, const char* function, const std::string& header, const std::string& body) {
    // if (something) {
        fprintf(stderr, "%s:%i: in function %s:\n", filename, line, function);
    // }

    std::string errorMsg = VT_ERR "error: " VT_RST;
    errorMsg += VT_HILITE;
    errorMsg += header;
    errorMsg += VT_RST;
    if (body != "") {
        errorMsg += "\n" WARN_INDT;
        errorMsg += body;
    }

    // Which one is better??
    // Does one mean we don't get the backtrace?
    //throw std::runtime_error(errorMsg);
    fprintf(stderr, "%s\n", errorMsg.c_str());
    exit(EXIT_FAILURE);
}

void WarningHandler::Warning(const char* filename, int32_t line, const char* function, WarningType warnType, const std::string& header, const std::string& body) {
    assert(static_cast<size_t>(warnType) >= 0 && warnType < WarningType::Max);
    if (!enabledWarnings.at(static_cast<size_t>(WarningType::Everything)) && !enabledWarnings.at(static_cast<size_t>(warnType))) {
        return;
    }

    std::string headerMsg = header;
    auto warningNameIter = sWarningsTypeToStringMap.find(warnType);
    if (warningNameIter != sWarningsTypeToStringMap.end()) {
        headerMsg += StringHelper::Sprintf(" [-W%s]", warningNameIter->second);
    }

    if (Werror) {
        WarningHandler::Error(filename, line, function, headerMsg, body);
        return;
    }

    // Move to common function?
    // if (something) {
        fprintf(stderr, "%s:%i: in function '%s':\n", filename, line, function);
    // }

    fprintf(stderr, VT_WARN "warning" VT_RST ": ");
    fprintf(stderr, VT_HILITE "%s" VT_RST "\n", headerMsg.c_str());
    if (body != "") {
        fprintf(stderr, "\t %s\n",  body.c_str());
    }
}

void WarningHandler::Warning_Resource(const char* filename, int32_t line, const char* function, WarningType warnType, ZFile *parent, uint32_t offset, const std::string& header, const std::string& body) {
    assert(parent != nullptr);
    std::string warningMsg = body;
    //warningMsg += StringHelper::Sprintf("\nWhen processing file %s: in input binary file %s, offset 0x%06X\n", Globals::Instance->inputPath.c_str(), parent->GetName().c_str(), offset);
    fprintf(stderr, "\nWhen processing file %s: in input binary file %s, offset 0x%06X: ", Globals::Instance->inputPath.c_str(), parent->GetName().c_str(), offset);

    WarningHandler::Warning(filename, line, function, warnType, header, warningMsg);
}

void WarningHandler::Warning_Build(const char* filename, int32_t line, const char* function, WarningType warnType, const std::string& header, const std::string& body) {
    std::string warningMsg = body;
    //warningMsg += StringHelper::Sprintf("\nWhen processing binary file %s: ", Globals::Instance->inputPath.c_str(), parent->GetName().c_str(), offset);
    fprintf(stderr, "\nWhen processing binary file %s: ", Globals::Instance->inputPath.c_str());

    WarningHandler::Warning(filename, line, function, warnType, header, warningMsg);
}
