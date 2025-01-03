#include "TrayIcon.h"

HINSTANCE hInst;
NOTIFYICONDATA nid;

void CreateTrayIcon(HWND hwnd) {
    ZeroMemory(&nid, sizeof(nid));
    nid.cbSize = sizeof(nid);
    nid.hWnd = hwnd;
    nid.uID = 1001;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_USER + 1;
    nid.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_APPLICATION));
    wcsncpy_s(nid.szTip, L"My App", ARRAYSIZE(nid.szTip));
    Shell_NotifyIcon(NIM_ADD, &nid);
}
