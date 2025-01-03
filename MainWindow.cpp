#include "LoginWindow.h"
#include "RegistrationWindow.h"
#include "windows.h"
#include "TrayIcon.h"

#define ID_TRAY_APP_ICON 1001
#define WM_TRAY_ICON (WM_USER + 1)
#define ID_TRAY_MENU_OPEN 1002
#define ID_TRAY_MENU_EXIT 1003

extern HWND hwndMain;

LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    HBRUSH hBrush;
    HFONT hFont;

    switch (msg) {
        case WM_CREATE:
        {
            RECT rect;
            GetClientRect(hwnd, &rect);
            int windowWidth = rect.right - rect.left;
            int windowHeight = rect.bottom - rect.top;

            int buttonWidth = 100;
            int buttonHeight = 40;

            int loginButtonX = (windowWidth - buttonWidth) / 2;
            int loginButtonY = (windowHeight - buttonHeight) / 2 - 30;

            int registerButtonX = (windowWidth - buttonWidth) / 2;
            int registerButtonY = (windowHeight - buttonHeight) / 2 + 30;

            CreateWindow(L"BUTTON", L"Login", WS_VISIBLE | WS_CHILD | BS_FLAT,
                loginButtonX, loginButtonY, buttonWidth, buttonHeight, hwnd, (HMENU)1, NULL, NULL);
            CreateWindow(L"BUTTON", L"Register", WS_VISIBLE | WS_CHILD | BS_FLAT,
                registerButtonX, registerButtonY, buttonWidth, buttonHeight, hwnd, (HMENU)2, NULL, NULL);

            hBrush = CreateSolidBrush(RGB(240, 248, 255));
            SetClassLongPtr(hwnd, GCLP_HBRBACKGROUND, (LONG_PTR)hBrush);

            hFont = CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                DEFAULT_PITCH, L"Arial");
            SendMessage(GetDlgItem(hwnd, 1), WM_SETFONT, WPARAM(hFont), TRUE);
            SendMessage(GetDlgItem(hwnd, 2), WM_SETFONT, WPARAM(hFont), TRUE);

            CreateTrayIcon(hwnd);
        }
        break;

        case WM_COMMAND:
            if (LOWORD(wParam) == 1) {
                ShowLoginWindow(hwnd);
            } else if (LOWORD(wParam) == 2) {
                ShowRegistrationWindow(hwnd);
            } else if (LOWORD(wParam) == ID_TRAY_MENU_EXIT) {

                Shell_NotifyIcon(NIM_DELETE, &nid);
                PostQuitMessage(0);
            } else if (LOWORD(wParam) == ID_TRAY_MENU_OPEN) {

                ShowWindow(hwnd, SW_SHOWNORMAL);
                SetForegroundWindow(hwnd);
            }
            break;

        case WM_CLOSE:
            ShowWindow(hwnd, SW_HIDE);
            break;

        case WM_TRAY_ICON:
            if (lParam == WM_LBUTTONUP) {
                ShowWindow(hwnd, SW_SHOWNORMAL);
                SetForegroundWindow(hwnd);
            } else if (lParam == WM_RBUTTONUP) {
                HMENU hMenu = CreatePopupMenu();
                AppendMenu(hMenu, MF_STRING, ID_TRAY_MENU_OPEN, L"Open Program");
                AppendMenu(hMenu, MF_STRING, ID_TRAY_MENU_EXIT, L"Exit");
                AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);

                POINT pt;
                GetCursorPos(&pt);
                SetForegroundWindow(hwnd);
                TrackPopupMenu(hMenu, TPM_BOTTOMALIGN | TPM_LEFTBUTTON, pt.x, pt.y, 0, hwnd, NULL);
                DestroyMenu(hMenu);
            }
            break;

        case WM_DESTROY:
            Shell_NotifyIcon(NIM_DELETE, &nid);
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

void RegisterMainWindow(WNDCLASSEX& wc) {
    wc.lpfnWndProc = MainWindowProc;
    wc.lpszClassName = L"MainWindowClass";
    wc.hInstance = GetModuleHandle(NULL);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpszMenuName = NULL;
    wc.cbSize = sizeof(WNDCLASSEX);
    RegisterClassEx(&wc);
}
