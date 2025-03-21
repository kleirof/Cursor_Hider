#pragma once

#include "resource.h"
#include "Defined.h"
#include <windows.h>
#include <iostream>
#include <vector>

class CursorHider
{
private:
	struct WindowInfo
	{
		HWND hwnd = NULL;
		std::string windowTitle;
		std::string processName;
	};

	std::vector<WindowInfo> windowList;
	HWND selectedWindow = NULL;

	HHOOK hMouseHook = NULL;
	HCURSOR hCursor = LoadCursor(GetModuleHandle(NULL), MAKEINTRESOURCE(IDC_CURSOR1));
	bool isHiden = false;
	bool isEnabled = false;

	int cursorTypes[17] = {
			OCR_NORMAL, OCR_IBEAM, OCR_WAIT, OCR_CROSS, OCR_UP, OCR_SIZE, OCR_ICON,
			OCR_SIZENWSE, OCR_SIZENESW, OCR_SIZEWE, OCR_SIZENS, OCR_SIZEALL,
			OCR_ICOCUR, OCR_NO, OCR_APPSTARTING, OCR_HAND, OCR_HELP
	};

	std::vector<HCURSOR> createdCursors;

	std::atomic<bool> cleaned {false};

	CursorHider();

	~CursorHider();

	CursorHider(const CursorHider&) = delete;

	CursorHider& operator=(const CursorHider&) = delete;

	void HideCursor();

	void RestoreCursor();

	std::string ToLower(const std::string& str);

	static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);

	BOOL CALLBACK InstanceEnumWindowsProc(HWND hwnd, LPARAM lParam);

	bool SearchWindows();

	static LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam);

	LRESULT CALLBACK InstanceMouseProc(int nCode, WPARAM wParam, LPARAM lParam);

	bool RegisterHotKey();

	bool SelectWindow();

	void ProcessMessages();

	void OnExit();

	static BOOL WINAPI ConsoleHandler(DWORD signal);

public:
	static CursorHider& GetInstance();

	int Execute();
};

