#include "ErrorHandler.h"

#include "Globals.h"


void ErrorHandler::Error(const std::string& filename, const std::string& line, const std::string& function, const std::string& msg) {
    // if (something) {
        fprintf(stderr, "%s:%s: in function %s:\n", filename.c_str(), line.c_str(), function.c_str());
    // }

    throw std::runtime_error("ERROR:\n" + msg);
}

void ErrorHandler::Warning(const std::string& filename, const std::string& line, const std::string& function, const std::string& msg) {
    // if (Globals::Instance.Werror) {
    //     ErrorHandler::Error(filename, line, function);
    //     return;
    // }

    // Move to common function?
    // if (something) {
        fprintf(stderr, "%s:%s: in function %s:\n", filename.c_str(), line.c_str(), function.c_str());
    // }

    fprintf(stderr, "WARNING:\n%s\n", msg.c_str());
}
