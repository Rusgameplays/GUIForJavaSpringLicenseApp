#include <windows.h>
#include "MainWindow.h"
#include "LoginWindow.h"
#include "RegistrationWindow.h"
#include "ActivateWindow.h"
#include "PipeUtils.h"
#include "TrayIcon.h"
#define ID_TRAY_APP_ICON 1001
#define WM_TRAY_ICON (WM_USER + 1)

HWND hwndMain;
HANDLE hPipe = INVALID_HANDLE_VALUE;

int main() {
    WNDCLASSEX wc = {sizeof(WNDCLASSEX)};
    wc.hInstance = GetModuleHandle(NULL);

    wc.lpfnWndProc = MainWindowProc;
    wc.lpszClassName = L"MainWindowClass";
    RegisterClassEx(&wc);

    wc.lpfnWndProc = LoginWindowProc;
    wc.lpszClassName = L"LoginWindowClass";
    RegisterClassEx(&wc);

    wc.lpfnWndProc = RegistrationWindowProc;
    wc.lpszClassName = L"RegistrationWindowClass";
    RegisterClassEx(&wc);

    wc.lpfnWndProc = ActivateWindowProc;
    wc.lpszClassName = L"AdminWindowClass";
    RegisterClassEx(&wc);

    hwndMain = CreateWindowEx(0, L"MainWindowClass", L"Main Window", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                               CW_USEDEFAULT, CW_USEDEFAULT, 300, 200, NULL, NULL, hInst, NULL);

    CreateTrayIcon(hwndMain);

    hPipe = CreateFile(L"\\\\.\\pipe\\MySimpleServicePipe", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (hPipe == INVALID_HANDLE_VALUE) {
        MessageBox(NULL, L"Failed to connect to pipe!", L"Error", MB_OK | MB_ICONERROR);
        return 1;
    }

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
