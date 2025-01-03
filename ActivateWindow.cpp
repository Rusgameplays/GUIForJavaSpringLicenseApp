
#include <string>
#include <windows.h>
#include <iphlpapi.h>
#include <sstream>
#include <codecvt>
#include "PipeUtils.h"

#pragma comment(lib, "iphlpapi.lib")

LRESULT CALLBACK ActivateWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

std::string wstringToString(const std::wstring& wstr) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.to_bytes(wstr);
}

std::wstring getMACAddress() {
    IP_ADAPTER_INFO adapterInfo[16];
    DWORD bufferSize = sizeof(adapterInfo);

    if (GetAdaptersInfo(adapterInfo, &bufferSize) != ERROR_SUCCESS) {
        return L"00:00:00:00:00:00";
    }

    PIP_ADAPTER_INFO adapter = adapterInfo;
    std::wstring macAddress;

    for (int i = 0; i < adapter->AddressLength; ++i) {
        if (i != 0) {
            macAddress += L":";
        }

        wchar_t byteStr[3];
        swprintf(byteStr, sizeof(byteStr) / sizeof(wchar_t), L"%02X", adapter->Address[i]);
        macAddress += byteStr;
    }

    return macAddress;
}

LRESULT CALLBACK ActivateWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static HWND hwndKey, hwndName, hwndUserId, hwndActivateButton;
    static HFONT hFont;
    static HBRUSH hBrush;
    static std::wstring key, name, userId, mac;

    switch (msg) {
        case WM_CREATE: {
            RECT rect;
            GetClientRect(hwnd, &rect);
            int windowWidth = rect.right - rect.left;

            // Размеры элементов
            int fieldWidth = 200;
            int fieldHeight = 25;
            int buttonWidth = 150;
            int buttonHeight = 30;

            int labelX = 20;
            int fieldX = 180;

            int keyY = 30;
            int nameY = 70;
            int userIdY = 110;
            int buttonY = 150;

            CreateWindow(L"STATIC", L"Enter license key:", WS_VISIBLE | WS_CHILD, labelX, keyY, 150, fieldHeight, hwnd, NULL, NULL, NULL);
            hwndKey = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, fieldX, keyY, fieldWidth, fieldHeight, hwnd, NULL, NULL, NULL);

            CreateWindow(L"STATIC", L"Enter device name:", WS_VISIBLE | WS_CHILD, labelX, nameY, 150, fieldHeight, hwnd, NULL, NULL, NULL);
            hwndName = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, fieldX, nameY, fieldWidth, fieldHeight, hwnd, NULL, NULL, NULL);

            CreateWindow(L"STATIC", L"Enter user ID:", WS_VISIBLE | WS_CHILD, labelX, userIdY, 150, fieldHeight, hwnd, NULL, NULL, NULL);
            hwndUserId = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, fieldX, userIdY, fieldWidth, fieldHeight, hwnd, NULL, NULL, NULL);

            hwndActivateButton = CreateWindow(L"BUTTON", L"Activate License", WS_VISIBLE | WS_CHILD | BS_FLAT,
                (windowWidth - buttonWidth) / 2, buttonY, buttonWidth, buttonHeight, hwnd, (HMENU)1, NULL, NULL);

            hFont = CreateFont(14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                DEFAULT_PITCH, L"Arial");

            SendMessage(hwndKey, WM_SETFONT, WPARAM(hFont), TRUE);
            SendMessage(hwndName, WM_SETFONT, WPARAM(hFont), TRUE);
            SendMessage(hwndUserId, WM_SETFONT, WPARAM(hFont), TRUE);
            SendMessage(hwndActivateButton, WM_SETFONT, WPARAM(hFont), TRUE);

            hBrush = CreateSolidBrush(RGB(240, 248, 255));
            SetClassLongPtr(hwnd, GCLP_HBRBACKGROUND, (LONG_PTR)hBrush);

            break;
        }

        case WM_COMMAND: {
            switch (LOWORD(wParam)) {
                case 1: {
                    wchar_t wbuffer[256];

                    GetWindowText(hwndKey, wbuffer, sizeof(wbuffer) / sizeof(wchar_t));
                    key = wbuffer;

                    GetWindowText(hwndName, wbuffer, sizeof(wbuffer) / sizeof(wchar_t));
                    name = wbuffer;

                    GetWindowText(hwndUserId, wbuffer, sizeof(wbuffer) / sizeof(wchar_t));
                    userId = wbuffer;

                    if (key.empty() || name.empty() || userId.empty()) {
                        MessageBox(hwnd, L"Please fill all fields before submitting.", L"Error", MB_OK | MB_ICONERROR);
                        return 0;
                    }

                    mac = getMACAddress();

                    if (mac == L"00:00:00:00:00:00") {
                        MessageBox(hwnd, L"Failed to retrieve MAC address. Please try again.", L"Error", MB_OK | MB_ICONERROR);
                        return 0;
                    }

                    std::wstring wcommand = L"activate_license " + key + L" " + name + L" " + mac + L" " + userId;

                    std::string command = wstringToString(wcommand);

                    sendCommandToPipe(hPipe, command.c_str());

                    char buffer[1024];
                    DWORD bytesRead;
                    if (ReadFile(hPipe, buffer, sizeof(buffer) - 1, &bytesRead, NULL) && bytesRead > 0) {
                        buffer[bytesRead] = '\0';
                        std::string serverResponse(buffer);

                        if (serverResponse.find("{\"serverDate") == 0) {
                            MessageBox(hwnd, std::wstring(serverResponse.begin(), serverResponse.end()).c_str(), L"Activation Successful", MB_OK | MB_ICONINFORMATION);
                        } else {
                            MessageBox(hwnd, L"Activation Failed", L"Error", MB_OK | MB_ICONERROR);
                        }
                    } else {
                        MessageBox(hwnd, L"Failed to read response from server.", L"Error", MB_OK | MB_ICONERROR);
                    }

                    break;
                }
            }
            break;
        }

        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

void ShowActivateWindow(HWND hwnd) {
    WNDCLASS wc = {};
    wc.lpfnWndProc = ActivateWindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = L"ActivateWindowClass";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    if (!RegisterClass(&wc)) {
        MessageBox(NULL, L"Window class registration failed!", L"Error", MB_OK | MB_ICONERROR);
        return;
    }

    HWND hwndActivate = CreateWindowEx(
        0, L"ActivateWindowClass", L"Activate Panel", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 300, hwnd, NULL, wc.hInstance, NULL
    );

    if (!hwndActivate) {
        MessageBox(NULL, L"Failed to create Activate window.", L"Error", MB_OK | MB_ICONERROR);
    }
}
