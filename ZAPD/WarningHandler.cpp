#include "WarningHandler.h"

#include <cassert>
#include <unordered_map>

#include "Globals.h"
#include "Utils/StringHelper.h"

static std::unordered_map<std::string, WarningType> sWarningsStringToTypeMap = {
    {"deprecated", WarningType::Deprecated},
};
static std::unordered_map<WarningType, std::string> sWarningsTypeToStringMap = {
    {WarningType::Deprecated, "deprecated"},
};

std::array<bool, static_cast<size_t>(WarningType::Max)> WarningHandler::enabledWarnings = {false};

bool WarningHandler::Werror = false;

void WarningHandler::Init(int argc, char* argv[]) {
    // Werror = false;
    // for (size_t i = 0; i < enabledWarnings.size(); i++) {
    //     enabledWarnings[i] = false;
    // }

    enabledWarnings[static_cast<size_t>(WarningType::Always)] = true;

    for (int i = 1; i < argc; i++) {

        // If it doesn't starts with "-W" skip it.
        if (argv[i][0] != '-' || argv[i][1] != 'W' || argv[i][1] == '\0') {
            continue;
        }

        // Skip "-W"
        std::string_view currentArgv = &argv[i][2];

        if (currentArgv == "error") {
            Werror = true;
        } else if (currentArgv == "everything") {
            for (size_t i = 0; i < enabledWarnings.size(); i++) {
                enabledWarnings[i] = true;
            }
        } else {
            auto warning_type = sWarningsStringToTypeMap.find(std::string(currentArgv));
            if (warning_type != sWarningsStringToTypeMap.end()) {
                size_t index = static_cast<size_t>(warning_type->second);
                enabledWarnings[index] = true;
            }
            else {
                HANDLE_WARNING(WarningType::Always, StringHelper::Sprintf("Unknown warning flag '%s'", argv[i]));
            }
        }
    }
}

void WarningHandler::Error(const char* filename, int32_t line, const char* function, const std::string& msg) {
    // if (something) {
        fprintf(stderr, "%s:%i: in function %s:\n", filename, line, function);
    // }

    std::string errorMsg = VT_FGCOL(RED) "Error" VT_RST ": ";
    //errorMsg += BOLD;
    errorMsg += msg;
    errorMsg += VT_RST;

    throw std::runtime_error(errorMsg);
}

void WarningHandler::Warning(const char* filename, int32_t line, const char* function, WarningType warnType, const std::string& msg) {
    assert(static_cast<size_t>(warnType) >= 0 && warnType < WarningType::Max);
    if (!enabledWarnings.at(static_cast<size_t>(WarningType::Everything)) && !enabledWarnings.at(static_cast<size_t>(warnType))) {
        return;
    }
    
    if (Werror) {
        WarningHandler::Error(filename, line, function, msg);
        return;
    }

    // Move to common function?
    // if (something) {
        fprintf(stderr, "%s:%i: in function '%s':\n", filename, line, function);
    // }

    // TODO: bold
    fprintf(stderr, VT_FGCOL(PURPLE) "Warning" VT_RST ": ");
    // TODO: bold
    fprintf(stderr, "%s" VT_RST "\n", msg.c_str());
}

void WarningHandler::Warning_Resource(const char* filename, int32_t line, const char* function, WarningType warnType, ZFile *parent, uint32_t offset, const std::string& msg) {
    std::string warningMsg = StringHelper::Sprintf("When processing file %s: in input binary file %s, offset 0x%06X:\n", Globals::Instance->inputPath.c_str(), parent->GetName().c_str(), offset);
    warningMsg += msg;

    WarningHandler::Warning(filename, line, function, warnType, warningMsg);
}
