#ifndef TRAYICON_H
#define TRAYICON_H

#include <windows.h>

void CreateTrayIcon(HWND hwnd);
extern HINSTANCE hInst;
extern HWND hwndMain;
extern NOTIFYICONDATA nid;

#endif // TRAYICON_H
