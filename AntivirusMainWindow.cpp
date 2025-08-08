#include "AntivirusMainWindow.h"
#include <shlobj.h>
#include <sstream>
#include <string>
#include <vector>

#include "PipeUtils.h"

const wchar_t* ANTIVIRUS_WINDOW_CLASS = L"AntivirusMainWindowClass";

#define ID_BUTTON_SCAN_FOLDER 1001
#define ID_BUTTON_LOAD_SIGNATURES 1002
#define ID_BUTTON_SCAN_FILE 1003
#define ID_BUTTON_SCAN_DRIVE 1004
#define ID_BUTTON_SCAN_ALL_DRIVES 1005



std::wstring BrowseForFile(HWND hwnd) {
    wchar_t filePath[MAX_PATH] = {0};

    OPENFILENAME ofn = {0};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFilter = L"All Files\0*.*\0";
    ofn.lpstrFile = filePath;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
    ofn.lpstrTitle = L"Select File to Scan";

    if (GetOpenFileName(&ofn)) {
        return filePath;
    }
    return L"";
}

std::wstring BrowseForFolder(HWND hwnd) {
    BROWSEINFO bi = {0};
    bi.lpszTitle = L"Select Folder to Scan";
    bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
    bi.hwndOwner = hwnd;

    LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
    if (pidl != nullptr) {
        wchar_t path[MAX_PATH];
        if (SHGetPathFromIDList(pidl, path)) {
            CoTaskMemFree(pidl);
            return path;
        }
        CoTaskMemFree(pidl);
    }
    return L"";
}


std::wstring ConvertToWideString(const std::string& str) {
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), NULL, 0);
    std::wstring wstr(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), &wstr[0], size_needed);
    return wstr;
}
LRESULT CALLBACK AntivirusMainWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: {
            HFONT hFont = CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                     DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                                     DEFAULT_PITCH, L"Segoe UI");

            CreateWindow(L"BUTTON", L"SCAN FOlder or Drive",
                         WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                         50, 50, 200, 40,
                         hwnd, (HMENU)ID_BUTTON_SCAN_FOLDER, NULL, NULL);

            CreateWindow(L"BUTTON", L"DOWNLOAD",
                         WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                         50, 110, 200, 40,
                         hwnd, (HMENU)ID_BUTTON_LOAD_SIGNATURES, NULL, NULL);

            CreateWindow(L"BUTTON", L"SCAN FILE",
             WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
             50, 170, 200, 40,
             hwnd, (HMENU)ID_BUTTON_SCAN_FILE, NULL, NULL);

            CreateWindow(L"BUTTON", L"SCAN ALL DRIVES",
             WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
             50, 230, 200, 40,
             hwnd, (HMENU)ID_BUTTON_SCAN_ALL_DRIVES, NULL, NULL);


            SendMessage(GetDlgItem(hwnd, ID_BUTTON_SCAN_ALL_DRIVES), WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(GetDlgItem(hwnd, ID_BUTTON_SCAN_FOLDER), WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(GetDlgItem(hwnd, ID_BUTTON_LOAD_SIGNATURES), WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(GetDlgItem(hwnd, ID_BUTTON_SCAN_FILE), WM_SETFONT, (WPARAM)hFont, TRUE);

            break;
        }

        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case ID_BUTTON_SCAN_FOLDER:
                {
                    std::wstring folder = BrowseForFolder(hwnd);
                    if (folder.empty()) {
                        break;
                    }

                    if (hPipe != INVALID_HANDLE_VALUE) {
                        std::string command = "scan " + std::string(folder.begin(), folder.end());
                        if (sendCommandToPipe(hPipe, command)) {
                            std::string response = getResponseFromPipe(hPipe);
                            std::wstring wideResponse = ConvertToWideString(response);
                            MessageBox(hwnd, wideResponse.c_str(), L"Scan Result", MB_OK | MB_ICONINFORMATION);
                        } else {
                            MessageBox(hwnd, L"Error sending command to pipe.", L"Error", MB_OK | MB_ICONERROR);
                        }
                    } else {
                        MessageBox(hwnd, L"Failed to connect to the pipe.", L"Error", MB_OK | MB_ICONERROR);
                    }
                }
                break;


                case ID_BUTTON_LOAD_SIGNATURES:
                    {

                    if (hPipe != INVALID_HANDLE_VALUE) {

                        std::string command = "load_signatures";
                        if (sendCommandToPipe(hPipe, command)) {
                            std::string response = getResponseFromPipe(hPipe);
                            std::wstring wideResponse = ConvertToWideString(response);
                            MessageBox(hwnd, wideResponse.c_str(), L"Signature Download Result", MB_OK | MB_ICONINFORMATION);
                        } else {
                            MessageBox(hwnd, L"Error sending command to pipe.", L"Error", MB_OK | MB_ICONERROR);
                        }

                    } else {
                        MessageBox(hwnd, L"Failed to connect to the pipe.", L"Error", MB_OK | MB_ICONERROR);
                    }

                    }
                    break;
                case ID_BUTTON_SCAN_FILE:
                {
                    std::wstring filePath = BrowseForFile(hwnd);
                    if (filePath.empty()) {
                        break;
                    }

                    if (hPipe != INVALID_HANDLE_VALUE) {
                        std::string command = "file " + std::string(filePath.begin(), filePath.end());
                        if (sendCommandToPipe(hPipe, command)) {
                            std::string response = getResponseFromPipe(hPipe);
                            std::wstring wideResponse = ConvertToWideString(response);
                            MessageBox(hwnd, wideResponse.c_str(), L"Scan File Result", MB_OK | MB_ICONINFORMATION);
                        } else {
                            MessageBox(hwnd, L"Error sending command to pipe.", L"Error", MB_OK | MB_ICONERROR);
                        }
                    } else {
                        MessageBox(hwnd, L"Failed to connect to the pipe.", L"Error", MB_OK | MB_ICONERROR);
                    }
                }
                break;
                case ID_BUTTON_SCAN_ALL_DRIVES:
                {
                    if (hPipe != INVALID_HANDLE_VALUE) {
                        std::string command = "drives";
                        if (sendCommandToPipe(hPipe, command)) {
                            std::string response = getResponseFromPipe(hPipe);
                            std::wstring wideResponse = ConvertToWideString(response);
                            MessageBox(hwnd, wideResponse.c_str(), L"Scan All Drives Result", MB_OK | MB_ICONINFORMATION);
                        } else {
                            MessageBox(hwnd, L"Error sending command to pipe.", L"Error", MB_OK | MB_ICONERROR);
                        }
                    } else {
                        MessageBox(hwnd, L"Failed to connect to the pipe.", L"Error", MB_OK | MB_ICONERROR);
                    }
                }
                break;
            }
            break;
        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;
        case WM_DESTROY:
            break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

void RegisterAntivirusMainWindowClass(HINSTANCE hInstance) {
    WNDCLASS wc = {};
    wc.lpfnWndProc = AntivirusMainWindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = ANTIVIRUS_WINDOW_CLASS;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    RegisterClass(&wc);
}

void ShowAntivirusMainWindow() {
    CreateWindowEx(0, ANTIVIRUS_WINDOW_CLASS, L"Antivirus Dashboard", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                   CW_USEDEFAULT, CW_USEDEFAULT, 600, 400, NULL, NULL, GetModuleHandle(NULL), NULL);
}

