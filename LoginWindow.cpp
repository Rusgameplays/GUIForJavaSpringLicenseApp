#include "LoginWindow.h"
#include "ActivateWindow.h"
#include "PipeUtils.h"

void ShowLoginWindow(HWND hwnd) {
    CreateWindowEx(0, L"LoginWindowClass", L"Login", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                   CW_USEDEFAULT, CW_USEDEFAULT, 400, 200, hwnd, NULL, GetModuleHandle(NULL), NULL);
}

LRESULT CALLBACK LoginWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static HWND hEmail, hPassword, hLoginButton;
    static HFONT hFont;
    static HBRUSH hBrush;

    switch (msg) {
        case WM_CREATE:
        {

            RECT rect;
            GetClientRect(hwnd, &rect);
            int windowWidth = rect.right - rect.left;

            int fieldWidth = 200;
            int fieldHeight = 25;
            int buttonWidth = 100;
            int buttonHeight = 30;

            int emailX = 120;
            int emailY = 10;
            int passX = 120;
            int passY = 40;

            int buttonX = (windowWidth - buttonWidth) / 2;
            int buttonY = 70;

            CreateWindow(L"STATIC", L"Email: ", WS_VISIBLE | WS_CHILD, 10, emailY, 100, fieldHeight, hwnd, NULL, NULL, NULL);
            hEmail = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, emailX, emailY, fieldWidth, fieldHeight, hwnd, NULL, NULL, NULL);

            CreateWindow(L"STATIC", L"Password: ", WS_VISIBLE | WS_CHILD, 10, passY, 100, fieldHeight, hwnd, NULL, NULL, NULL);
            hPassword = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_PASSWORD, passX, passY, fieldWidth, fieldHeight, hwnd, NULL, NULL, NULL);

            hLoginButton = CreateWindow(L"BUTTON", L"Login", WS_VISIBLE | WS_CHILD | BS_FLAT,
                buttonX, buttonY, buttonWidth, buttonHeight, hwnd, (HMENU)1, NULL, NULL);

            hFont = CreateFont(14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                DEFAULT_PITCH, L"Arial");

            SendMessage(hEmail, WM_SETFONT, WPARAM(hFont), TRUE);
            SendMessage(hPassword, WM_SETFONT, WPARAM(hFont), TRUE);
            SendMessage(hLoginButton, WM_SETFONT, WPARAM(hFont), TRUE);


            hBrush = CreateSolidBrush(RGB(240, 248, 255));
            SetClassLongPtr(hwnd, GCLP_HBRBACKGROUND, (LONG_PTR)hBrush);

            break;
        }

        case WM_COMMAND:
            if (LOWORD(wParam) == 1) {
                int emailLength = GetWindowTextLength(hEmail) + 1;
                int passLength = GetWindowTextLength(hPassword) + 1;
                wchar_t* emailBuffer = new wchar_t[emailLength];
                wchar_t* passBuffer = new wchar_t[passLength];

                GetWindowText(hEmail, emailBuffer, emailLength);
                GetWindowText(hPassword, passBuffer, passLength);

                std::wstring email = emailBuffer;
                std::wstring password = passBuffer;

                std::string command = "login " + std::string(email.begin(), email.end()) + " " +
                                      std::string(password.begin(), password.end());
                sendCommandToPipe(hPipe, command);
                std::string response = getResponseFromPipe(hPipe);

                if (response.find("Bearer") != std::string::npos && response.length() > 7) {
                    MessageBox(hwnd, L"Login Successful!", L"Info", MB_OK);
                    ShowActivateWindow(hwnd);
                } else {
                    MessageBox(hwnd, L"Login Failed!", L"Error", MB_OK);
                }

                delete[] emailBuffer;
                delete[] passBuffer;
            }
            break;

        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}
