#ifndef PIPEUTILS_H
#define PIPEUTILS_H

#include <string>
#include <windows.h>

bool sendCommandToPipe(HANDLE hPipe, const std::string& command);
std::string getResponseFromPipe(HANDLE hPipe);
extern HANDLE hPipe;

#endif // PIPEUTILS_H
