#include "resource.h"
#include "CursorHider.h"
#include <iomanip>
#include <algorithm>
#include <string>
#include <psapi.h>
#include <tchar.h>

CursorHider::CursorHider()
{
	SetConsoleCtrlHandler(ConsoleHandler, TRUE);
}

CursorHider::~CursorHider()
{
	OnExit();
}

void CursorHider::HideCursor()
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

void CursorHider::RestoreCursor()
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

std::string CursorHider::ToLower(const std::string& str) {
	std::string lowerStr = str;
	std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(),
		[](unsigned char c) { return std::tolower(c); });
	return lowerStr;
}

BOOL CALLBACK CursorHider::EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	return GetInstance().InstanceEnumWindowsProc(hwnd, lParam);
}

BOOL CALLBACK CursorHider::InstanceEnumWindowsProc(HWND hwnd, LPARAM lParam)
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

bool CursorHider::SearchWindows()
{
	std::cout << ENTER_KEYWORD;
	std::string searchTerm;
	std::cin.clear();
	std::getline(std::cin, searchTerm);

	EnumWindows(EnumWindowsProc, 0);

	if (windowList.empty())
	{
		std::cout << NO_WINDOW << std::endl;
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
		std::cout << NO_WINDOW_FOUND_1 << searchTerm << NO_WINDOW_FOUND_2 << std::endl;
		return false;
	}

	std::cout << SEARCH_RESULT << std::endl;
	for (size_t i = 0; i < searchResults.size(); ++i)
	{
		std::cout << i + 1 << ". " << WINDOW_HANDLE << searchResults[i].hwnd
			<< " - " << TITLE << searchResults[i].windowTitle
			<< " - " << PROCESS << searchResults[i].processName << std::endl;
	}

	std::cout << ENTER_NUMBER;
	size_t selection;
	std::cin >> selection;

	if (selection > 0 && selection <= searchResults.size())
	{
		selectedWindow = searchResults[selection - 1].hwnd;
		std::cout << HIDEN_HANDLE << selectedWindow
			<< " - " << TITLE << searchResults[selection - 1].windowTitle
			<< " - " << PROCESS << searchResults[selection - 1].processName << std::endl;
		std::cout << HOTKEY_HINT << std::endl;
		isEnabled = true;
	}
	else
	{
		std::cout << INVALID_SELECTION << std::endl;
		return false;
	}
	return true;
}

LRESULT CALLBACK CursorHider::MouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	return GetInstance().InstanceMouseProc(nCode, wParam, lParam);
}

LRESULT CALLBACK CursorHider::InstanceMouseProc(int nCode, WPARAM wParam, LPARAM lParam)
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

bool CursorHider::RegisterHotKey()
{
	if (!::RegisterHotKey(NULL, HOTKEY_TOGGLE_CURSOR, MOD_CONTROL | MOD_ALT, 0x48))
	{
		std::cout << FAIL_REGISTER_HOTKEY << std::endl;
		return false;
	}
	return true;
}

bool CursorHider::SelectWindow()
{
	if (!SearchWindows())
	{
		system("pause");
		return false;
	}

	hMouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseProc, NULL, 0);
	if (hMouseHook == NULL)
	{
		std::cout << FAIL_REGISTER_HOOK << std::endl;
		system("pause");
		return false;
	}
	return true;
}

void CursorHider::ProcessMessages()
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

void CursorHider::OnExit()
{
	if (cleaned)
	{
		return;
	}

	if (hMouseHook)
	{
		UnhookWindowsHookEx(hMouseHook);
	}
	RestoreCursor();
	UnregisterHotKey(NULL, HOTKEY_TOGGLE_CURSOR);
	cleaned = true;
}

BOOL WINAPI CursorHider::ConsoleHandler(DWORD signal) 
{
	switch (signal) {
	case CTRL_C_EVENT:
	case CTRL_CLOSE_EVENT:
	case CTRL_BREAK_EVENT:
	case CTRL_LOGOFF_EVENT:
	case CTRL_SHUTDOWN_EVENT:
		GetInstance().OnExit();
		return FALSE;
	default:
		return FALSE;
	}
}

CursorHider& CursorHider::GetInstance() 
{
	static CursorHider instance;
	return instance;
}

int CursorHider::Execute()
{
	system("cls");

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
