#pragma once

#define __CHINESE

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

#define _NORMAL "\033[0m"
#define _YELLOW "\033[93m"
#define _RED "\033[91m"
#define _GREEN "\033[92m"

#define HOTKEY_TOGGLE_CURSOR 1

#ifdef __CHINESE

#define ENTER_KEYWORD (_YELLOW "�����������ؼ����Բ�����Ҫ���ع��Ĵ���: " _NORMAL)
#define NO_WINDOW (_RED "û���ҵ��κδ��ڣ�" _NORMAL)
#define NO_WINDOW_FOUND_1 (_RED "û���ҵ��������������Ĵ��� \"" _NORMAL)
#define NO_WINDOW_FOUND_2 (_RED "\"��" _NORMAL)
#define SEARCH_RESULT (_YELLOW  "���������" _NORMAL)
#define WINDOW_HANDLE (_YELLOW  "���ھ��: " _NORMAL)
#define TITLE (_YELLOW  "����: " _NORMAL)
#define PROCESS (_YELLOW  "����: " _NORMAL)
#define ENTER_NUMBER  (_YELLOW "�������������ع��Ĵ��ڱ�ţ�" _NORMAL)
#define HIDEN_HANDLE (_YELLOW  "�����ع��Ĵ��ھ����" _NORMAL)
#define HOTKEY_HINT (_GREEN  "��ݼ� Ctrl + Alt + H ��������/����������깦�ܡ�" _NORMAL)
#define INVALID_SELECTION (_RED "��Ч��ѡ��" _NORMAL)
#define FAIL_REGISTER_HOTKEY (_RED "ע���ݼ�ʧ�ܣ�" _NORMAL)
#define FAIL_REGISTER_HOOK (_RED "������깳��ʧ�ܣ�" _NORMAL)

#else

#define ENTER_KEYWORD (_YELLOW "Please enter search keywords to find the window where you need to hide the cursor: " _NORMAL)
#define NO_WINDOW (_RED "No windows found!" _NORMAL)
#define NO_WINDOW_FOUND_1 ( _RED "No windows found that meet the search criteria '" _NORMAL)
#define NO_WINDOW_FOUND_2 ( _RED "'." _NORMAL)
#define SEARCH_RESULT (_YELLOW "Search result: " _NORMAL)
#define WINDOW_HANDLE (_YELLOW "window handle: " _NORMAL)
#define TITLE (_YELLOW "title: " _NORMAL)
#define PROCESS (_YELLOW "process: " _NORMAL)
#define ENTER_NUMBER (_YELLOW "Please enter the window number where you want to hide the cursor: " _NORMAL)
#define HIDEN_HANDLE (_YELLOW "You've hiden the cursor of window handle of: " _NORMAL)
#define HOTKEY_HINT (_GREEN "The hotkey Ctrl + Alt + H can enable/disable the hiding cursor function." _NORMAL)
#define INVALID_SELECTION (_RED "Invalid selection!" _NORMAL)
#define FAIL_REGISTER_HOTKEY (_RED "Failed to register the hotkey!" _NORMAL)
#define FAIL_REGISTER_HOOK (_RED "Failed to set mouse hook!" _NORMAL)

#endif