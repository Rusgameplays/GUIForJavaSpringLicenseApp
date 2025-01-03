#include "RegistrationWindow.h"
#include "PipeUtils.h"

void ShowRegistrationWindow(HWND hwnd) {
    CreateWindowEx(0, L"RegistrationWindowClass", L"Register", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                   CW_USEDEFAULT, CW_USEDEFAULT, 400, 250, hwnd, NULL, GetModuleHandle(NULL), NULL);
}

LRESULT CALLBACK RegistrationWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static HWND hEmail, hName, hPassword, hRegisterButton;
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
            int nameX = 120;
            int nameY = 40;
            int passX = 120;
            int passY = 70;

            int buttonX = (windowWidth - buttonWidth) / 2;
            int buttonY = 100;

            CreateWindow(L"STATIC", L"Email: ", WS_VISIBLE | WS_CHILD, 10, emailY, 100, fieldHeight, hwnd, NULL, NULL, NULL);
            hEmail = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, emailX, emailY, fieldWidth, fieldHeight, hwnd, NULL, NULL, NULL);

            CreateWindow(L"STATIC", L"Name: ", WS_VISIBLE | WS_CHILD, 10, nameY, 100, fieldHeight, hwnd, NULL, NULL, NULL);
            hName = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, nameX, nameY, fieldWidth, fieldHeight, hwnd, NULL, NULL, NULL);

            CreateWindow(L"STATIC", L"Password: ", WS_VISIBLE | WS_CHILD, 10, passY, 100, fieldHeight, hwnd, NULL, NULL, NULL);
            hPassword = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_PASSWORD, passX, passY, fieldWidth, fieldHeight, hwnd, NULL, NULL, NULL);

            hRegisterButton = CreateWindow(L"BUTTON", L"Register", WS_VISIBLE | WS_CHILD | BS_FLAT,
                buttonX, buttonY, buttonWidth, buttonHeight, hwnd, (HMENU)1, NULL, NULL);

            hFont = CreateFont(14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                DEFAULT_PITCH, L"Arial");

            SendMessage(hEmail, WM_SETFONT, WPARAM(hFont), TRUE);
            SendMessage(hName, WM_SETFONT, WPARAM(hFont), TRUE);
            SendMessage(hPassword, WM_SETFONT, WPARAM(hFont), TRUE);
            SendMessage(hRegisterButton, WM_SETFONT, WPARAM(hFont), TRUE);

            hBrush = CreateSolidBrush(RGB(240, 248, 255));
            SetClassLongPtr(hwnd, GCLP_HBRBACKGROUND, (LONG_PTR)hBrush);

            break;
        }

        case WM_COMMAND:
            if (LOWORD(wParam) == 1) {
                int emailLength = GetWindowTextLength(hEmail) + 1;
                int nameLength = GetWindowTextLength(hName) + 1;
                int passLength = GetWindowTextLength(hPassword) + 1;
                wchar_t* emailBuffer = new wchar_t[emailLength];
                wchar_t* nameBuffer = new wchar_t[nameLength];
                wchar_t* passBuffer = new wchar_t[passLength];

                GetWindowText(hEmail, emailBuffer, emailLength);
                GetWindowText(hName, nameBuffer, nameLength);
                GetWindowText(hPassword, passBuffer, passLength);

                std::wstring email = emailBuffer;
                std::wstring name = nameBuffer;
                std::wstring password = passBuffer;

                std::string command = "register " + std::string(email.begin(), email.end()) + " " +
                                      std::string(name.begin(), name.end()) + " " +
                                      std::string(password.begin(), password.end());

                sendCommandToPipe(hPipe, command);
                std::string response = getResponseFromPipe(hPipe);

                delete[] emailBuffer;
                delete[] nameBuffer;
                delete[] passBuffer;

                DestroyWindow(hwnd);
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
