#include "PipeUtils.h"

bool sendCommandToPipe(HANDLE hPipe, const std::string& command) {
    DWORD bytesWritten;
    if (!WriteFile(hPipe, command.c_str(), command.size(), &bytesWritten, NULL)) {
        return false;
    }
    return true;
}

std::string getResponseFromPipe(HANDLE hPipe) {
    DWORD bytesRead;
    char buffer[4196];
    std::string response = "";

    if (ReadFile(hPipe, buffer, sizeof(buffer) - 1, &bytesRead, NULL)) {
        buffer[bytesRead] = '\0';
        response = std::string(buffer);
    }
    return response;
}
