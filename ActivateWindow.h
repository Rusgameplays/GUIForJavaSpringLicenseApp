
#ifndef ACTIVATEWINDOW_H
#define ACTIVATEWINDOW_H

#include <windows.h>

LRESULT CALLBACK ActivateWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void ShowActivateWindow(HWND hwndParent = NULL);

#endif //ACTIVATEWINDOW_H
