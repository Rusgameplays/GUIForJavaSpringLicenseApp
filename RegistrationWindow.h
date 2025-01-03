#ifndef REGISTRATIONWINDOW_H
#define REGISTRATIONWINDOW_H

#include <windows.h>

LRESULT CALLBACK RegistrationWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void ShowRegistrationWindow(HWND hwnd);

#endif // REGISTRATIONWINDOW_H
