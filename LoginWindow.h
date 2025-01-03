#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <windows.h>

LRESULT CALLBACK LoginWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void ShowLoginWindow(HWND hwnd);

#endif // LOGINWINDOW_H
