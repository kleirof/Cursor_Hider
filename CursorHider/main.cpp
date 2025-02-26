#include <windows.h>
#include <iostream>
#include <vector>
#include <string>
#include <psapi.h>
#include <tchar.h>
#include <algorithm>
#include "resource.h"

#define OCR_NORMAL          32512
#define OCR_IBEAM           32513
#define OCR_WAIT            32514
#define OCR_CROSS           32515
#define OCR_UP              32516
#define OCR_SIZE            32640
#define OCR_ICON            32641
#define OCR_SIZENWSE        32642
#define OCR_SIZENESW        32643
#define OCR_SIZEWE          32644
#define OCR_SIZENS          32645
#define OCR_SIZEALL         32646
#define OCR_ICOCUR          32647
#define OCR_NO              32648
#define OCR_APPSTARTING     32650
#define OCR_HAND            32649
#define OCR_HELP            32651

#define HOTKEY_TOGGLE_CURSOR 1

struct WindowInfo
{
    HWND hwnd = NULL;
    std::string windowTitle;
    std::string processName;
};

std::vector<WindowInfo> windowList;
HWND selectedWindow = NULL;

HHOOK hMouseHook;
HCURSOR hCursor = LoadCursor(GetModuleHandle(NULL), MAKEINTRESOURCE(IDC_CURSOR1));
bool isHiden = false;
bool isEnabled = false;

int cursorTypes[] = {
        OCR_NORMAL, OCR_IBEAM, OCR_WAIT, OCR_CROSS, OCR_UP, OCR_SIZE, OCR_ICON,
        OCR_SIZENWSE, OCR_SIZENESW, OCR_SIZEWE, OCR_SIZENS, OCR_SIZEALL,
        OCR_ICOCUR, OCR_NO, OCR_APPSTARTING, OCR_HAND, OCR_HELP
};

std::vector<HCURSOR> createdCursors;

void HideCursor()
{
    if (!isHiden)
    {
        isHiden = true;

        for (int cursorType : cursorTypes)
        {
            HCURSOR newCursor = CopyCursor(hCursor);
            createdCursors.push_back(newCursor);
            SetSystemCursor(newCursor, cursorType);
        }
    }
}

void RestoreCursor()
{
    if (isHiden)
    {
        isHiden = false;

        for (HCURSOR cursor : createdCursors)
        {
            DestroyCursor(cursor);
        }

        SystemParametersInfo(SPI_SETCURSORS, 0, NULL, SPIF_UPDATEINIFILE | SPIF_SENDCHANGE);
    }
}

std::string ToLower(const std::string& str) {
    std::string lowerStr = str;
    std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(),
        [](unsigned char c) { return std::tolower(c); });
    return lowerStr;
}

void OnExit()
{
    if (hMouseHook)
    {
        UnhookWindowsHookEx(hMouseHook);
    }
    RestoreCursor();
    UnregisterHotKey(NULL, HOTKEY_TOGGLE_CURSOR);
}

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
    char windowTitle[256];
    if (GetWindowTextA(hwnd, windowTitle, sizeof(windowTitle)))
    {
        DWORD processId;
        GetWindowThreadProcessId(hwnd, &processId);

        char processName[MAX_PATH] = "<unknown>";
        HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId);
        if (hProcess)
        {
            DWORD size = MAX_PATH;
            QueryFullProcessImageNameA(hProcess, 0, processName, &size);

            CloseHandle(hProcess);
        }

        WindowInfo window;
        window.hwnd = hwnd;
        window.windowTitle = windowTitle;
        window.processName = processName;
        windowList.push_back(window);
    }
    return TRUE;
}

bool SearchWindows()
{
    std::cout << "请输入搜索关键字以查找需要隐藏光标的窗口: ";
    std::string searchTerm;
    std::cin.clear();
    std::getline(std::cin, searchTerm);

    EnumWindows(EnumWindowsProc, 0);

    if (windowList.empty())
    {
        std::cout << "没有找到任何窗口!" << std::endl;
        return false;
    }

    std::vector<WindowInfo> searchResults;
    for (const auto& window : windowList)
    {
        if (ToLower(window.windowTitle).find(ToLower(searchTerm)) != std::string::npos)
        {
            searchResults.push_back(window);
        }
    }

    if (searchResults.empty())
    {
        std::cout << "没有找到符合搜索条件的窗口 \"" << searchTerm << "\"." << std::endl;
        return false;
    }

    std::cout << "搜索结果：" << std::endl;
    for (size_t i = 0; i < searchResults.size(); ++i)
    {
        std::cout << i + 1 << ". 窗口句柄: " << searchResults[i].hwnd
            << " - 标题: " << searchResults[i].windowTitle
            << " - 进程: " << searchResults[i].processName << std::endl;
    }

    std::cout << "请输入你想隐藏光标的窗口编号: ";
    size_t selection;
    std::cin >> selection;

    if (selection > 0 && selection <= searchResults.size())
    {
        selectedWindow = searchResults[selection - 1].hwnd;
        std::cout << "你隐藏鼠标的窗口句柄: " << selectedWindow
            << " - 标题: " << searchResults[selection - 1].windowTitle
            << " - 进程: " << searchResults[selection - 1].processName << std::endl;
        std::cout << "快捷键 Ctrl + Alt + H 可以启用/禁用隐藏鼠标功能。" << std::endl;
        isEnabled = true;
    }
    else
    {
        std::cout << "无效的选择!" << std::endl;
        return false;
    }
    return true;
}

LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode == HC_ACTION)
    {
        MSLLHOOKSTRUCT* pMouse = (MSLLHOOKSTRUCT*)lParam;
        POINT pt = pMouse->pt;

        if (selectedWindow != NULL && isEnabled)
        {
            HWND hwndUnderCursor = WindowFromPoint(pt);

            if (hwndUnderCursor == selectedWindow || IsChild(selectedWindow, hwndUnderCursor))
            {
                HideCursor();
            }
            else
            {
                RestoreCursor();
            }
        }
    }
    return CallNextHookEx(hMouseHook, nCode, wParam, lParam);
}

bool RegisterHotKey()
{
    if (!RegisterHotKey(NULL, HOTKEY_TOGGLE_CURSOR, MOD_CONTROL | MOD_ALT, 0x48))
    {
        std::cout << "注册快捷键失败!" << std::endl;
        return false;
    }
    return true;
}

bool SelectWindow()
{
    if (!SearchWindows())
    {
        system("pause");
        return false;
    }

    hMouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseProc, NULL, 0);
    if (hMouseHook == NULL)
    {
        std::cout << "设置鼠标钩子失败!" << std::endl;
        system("pause");
        return false;
    }
    return true;
}

void ProcessMessages()
{
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        if (msg.message == WM_HOTKEY)
        {
            if (msg.wParam == HOTKEY_TOGGLE_CURSOR)
            {
                isEnabled = !isEnabled;

                POINT pt;
                GetCursorPos(&pt);

                if (selectedWindow != NULL)
                {
                    HWND hwndUnderCursor = WindowFromPoint(pt);

                    if (hwndUnderCursor == selectedWindow || IsChild(selectedWindow, hwndUnderCursor))
                    {
                        if (!isEnabled)
                        {
                            RestoreCursor();
                        }
                        else
                        {
                            HideCursor();
                        }
                    }
                }
            }
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

int main()
{
    atexit(OnExit);

    if (!RegisterHotKey())
    {
        return -1;
    }

    if (!SelectWindow())
    {
        return -2;
    }

    ProcessMessages();

    return 0;
}
