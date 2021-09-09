#include "ErrorHandler.h"

#include "Globals.h"
#include "Utils/StringHelper.h"


void ErrorHandler::Error(const char* filename, int32_t line, const char* function, const std::string& msg) {
    // if (something) {
        fprintf(stderr, "%s:%i: in function %s:\n", filename, line, function);
    // }

    std::string errorMsg = VT_FGCOL(RED) "Error" VT_RST ":\n";
    //errorMsg += BOLD;
    errorMsg += msg;
    errorMsg += VT_RST;

    throw std::runtime_error(errorMsg);
}

void ErrorHandler::Warning(const char* filename, int32_t line, const char* function, const std::string& msg) {
    // if (Globals::Instance.Werror) {
    //     ErrorHandler::Error(filename, line, function);
    //     return;
    // }

    // Move to common function?
    // if (something) {
        fprintf(stderr, "%s:%i: in function %s:\n", filename, line, function);
    // }

    // TODO: bold
    fprintf(stderr, VT_FGCOL(PURPLE) "Warning" VT_RST ":\n");
    // TODO: bold
    fprintf(stderr, "%s" VT_RST "\n", msg.c_str());
}

void ErrorHandler::Warning_Resource(const char* filename, int32_t line, const char* function, ZFile *parent, uint32_t offset, const std::string& msg) {
    std::string warningMsg = StringHelper::Sprintf("When processing file %s: in input binary file %s, offset 0x%06X:\n", Globals::Instance->inputPath.c_str(), parent->GetName().c_str(), offset);
    warningMsg += msg;

    ErrorHandler::Warning(filename, line, function, warningMsg);
}
