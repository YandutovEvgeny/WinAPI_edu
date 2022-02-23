#include<Windows.h>
#include "resource.h"

#define ID_FILE_EXIT		9001
#define ID_STUFF_GO			9002
#define ID_TASKS_FIRST		9003
#define ID_TASKS_SECOND		9004

CONST CHAR g_szCLASS_NAME[] = "The title of my window";

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	//wc.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDI_APPLICATION);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszClassName = g_szCLASS_NAME;
	wc.lpszMenuName = NULL;

	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL, "Class registration failed!", "Error", MB_OK | MB_ICONERROR);
		return 0;
	}

	HWND hwnd = CreateWindowEx
	(
		WS_EX_WINDOWEDGE, g_szCLASS_NAME, g_szCLASS_NAME,
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		480, 240, NULL, NULL, hInstance, NULL
	);

	if (hwnd == NULL)
	{
		MessageBox(NULL, "Window creation failed!", "Error", MB_OK | MB_ICONERROR);
		return 0;
	}
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
	{
		HMENU hMenu, hSubMenu; 
		HICON hIcon, hIconSm;
		
		hMenu = CreateMenu();

		hSubMenu = CreatePopupMenu();
		AppendMenu(hSubMenu, MF_STRING, ID_FILE_EXIT, "E&xit");
		AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, "&File");

		hSubMenu = CreatePopupMenu();
		AppendMenu(hSubMenu, MF_STRING, ID_STUFF_GO, "&Go");
		AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, "&Stuff");

		hSubMenu = CreatePopupMenu();
		AppendMenu(hSubMenu, MF_STRING | MF_CHECKED, ID_TASKS_FIRST, "&First");
		AppendMenu(hSubMenu, MF_STRING | MF_GRAYED, ID_TASKS_SECOND, "&Second");
		AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, "&Tasks");

		SetMenu(hwnd, hMenu);
		
		hIcon = (HICON)LoadImage(NULL, "Coding.ico", IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
		if (hIcon)SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
		else MessageBox(hwnd, "Could not load large icon!", "Error", MB_OK | MB_ICONERROR);

		hIconSm = (HICON)LoadImage(NULL, "Coding.ico", IMAGE_ICON, 16, 16, LR_LOADFROMFILE);
		if (hIconSm)SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIconSm);
		else MessageBox(hwnd, "Could not load small icon!", "Error", MB_OK | MB_ICONERROR);
	}
	break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_FILE_EXIT: PostQuitMessage(0); break;
		case ID_STUFF_GO: MessageBox(hwnd, "Dont touch me now!", "Critical Error", MB_OK | MB_TOPMOST | MB_ICONERROR); break;
		}
		break;
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default: return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	return 0;
}