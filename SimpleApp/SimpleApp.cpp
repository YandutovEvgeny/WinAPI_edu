#pragma comment lib comctl32
#include<Windows.h>
#include<CommCtrl.h>

#define IDC_MAIN_EDIT		101
#define IDC_MAIN_TOOL		1001
#define ID_FILE_NEW			1002
#define ID_FILE_OPEN		1003
#define ID_FILE_SAVE		1004
#define IDC_MAIN_STATUS		2001

CONST CHAR g_szCLASS_NAME[] = "theForgers Tutorial Application";

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL LoadTextFileToEdit(HWND hEdit, LPCTSTR pszFileName);
BOOL SaveTextFileFromEdit(HWND hEdit, LPCTSTR pszFileName);

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

	OPENFILENAME ofn;
	CHAR szFileName[MAX_PATH] = "";

	ZeroMemory(&ofn, sizeof(ofn));

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files(*.*)\0*.*\0";
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	ofn.lpstrDefExt = "txt";

	if (GetOpenFileName(&ofn))
	{
		SaveTextFileFromEdit(hwnd, szFileName);		//?
		LoadTextFileToEdit(hwnd, szFileName);		//?
	}

	return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
	{
		//Statusbar
		HWND hStatus;
		hStatus = CreateWindowEx
		(
			NULL, STATUSCLASSNAME, NULL, WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP,
			0, 0, 0, 0, hwnd, (HMENU)IDC_MAIN_STATUS, DrvGetModuleHandle(NULL),
			NULL
		);

		int statwidths[] = { 100, -1 };
		SendMessage(hStatus, SB_SETPARTS, sizeof(statwidths) / sizeof(int), (LPARAM)statwidths);
		SendMessage(hStatus, SB_SETTEXT, 0, (LPARAM)"Привет :)");

		//Toolbar
		HWND hTool;
		hTool = CreateWindowEx
		(
			NULL, TOOLBARCLASSNAME, NULL, WS_CHILD | WS_VISIBLE, 0, 0, 0, 0,
			hwnd, (HMENU)IDC_MAIN_TOOL, GetModuleHandle(NULL), NULL
		);
		//Это сообщение необходимо для того, чтобы система могла определить, какую версию библиотеки common controls вы используете
		SendMessage(hTool, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);

		TBBUTTON tbb[3];	//tbb - tool bar button
		TBADDBITMAP tbab;	//tbab - tool bar add bitmap

		tbab.hInst = HINST_COMMCTRL;
		tbab.nID = IDB_STD_SMALL_COLOR;
		SendMessage(hTool, TB_ADDBITMAP, 0, (LPARAM)&tbab);

		ZeroMemory(tbb, sizeof(tbb));
		tbb[0].iBitmap = STD_FILENEW;
		tbb[0].fsState = TBSTATE_ENABLED;
		tbb[0].fsStyle = TBSTYLE_BUTTON;
		tbb[0].idCommand = ID_FILE_NEW;

		tbb[1].iBitmap = STD_FILEOPEN;
		tbb[1].fsState = TBSTATE_ENABLED;
		tbb[1].fsStyle = TBSTYLE_BUTTON;
		tbb[1].idCommand = ID_FILE_OPEN;

		tbb[2].iBitmap = STD_FILESAVE;
		tbb[2].fsState = TBSTATE_ENABLED;
		tbb[2].fsStyle = TBSTYLE_BUTTON;
		tbb[2].idCommand = ID_FILE_SAVE;

		SendMessage(hTool, TB_ADDBUTTONS, sizeof(tbb) / sizeof(TBBUTTON), (LPARAM)&tbb);
		
		//Edit
		HFONT hfDefault;
		HWND hEdit;

		hEdit = CreateWindowEx		//Создание Edit-окна
		(
			WS_EX_CLIENTEDGE, "EDIT", "",
			WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
			0, 0, 100, 100, hwnd, (HMENU)IDC_MAIN_EDIT, NULL, NULL
		);
		if (hEdit == NULL)MessageBox(hwnd, "Could not create edit box!", "Error", MB_OK | MB_ICONERROR);

		hfDefault = (HFONT)GetStockObject(SYSTEM_FONT);
		SendMessage(hEdit, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));
	}
		break;
	case WM_SIZE:
	{
		HWND hTool;
		RECT rcTool;
		int iToolHeight;	//Высота панели инструментов

		HWND hStatus;
		RECT rcStatus;
		int iStatusHeight;

		HWND hEdit;
		RECT rcClient;
		int iEditHeight;

		//Размер панели инструментов и получаем высоту
		hTool = GetDlgItem(hwnd, IDC_MAIN_TOOL);
		SendMessage(hTool, TB_AUTOSIZE, 0, 0);

		GetWindowRect(hTool, &rcTool);
		iToolHeight = rcTool.bottom - rcTool.top;

		//Размер строки состояния и получаем высоту
		hStatus = GetDlgItem(hwnd, IDC_MAIN_STATUS);
		SendMessage(hStatus, WM_SIZE, 0, 0);
			
		GetWindowRect(hStatus, &rcStatus);
		iStatusHeight = rcStatus.bottom - rcStatus.top;

		//Расчитываем оставшуюся высоту
		GetClientRect(hwnd, &rcClient);	  //Получаем размер клиентской области

		iEditHeight = rcClient.bottom - iToolHeight - iStatusHeight;

		hEdit = GetDlgItem(hwnd, IDC_MAIN_EDIT);	//Получаем дескриптор Edit-окна
		SetWindowPos(hEdit, NULL, 0, 0, rcClient.right, rcClient.bottom, SWP_NOZORDER);		//Устанавливаем размер Edit-окна
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

BOOL LoadTextFileToEdit(HWND hEdit, LPCTSTR pszFileName)
{
	HANDLE HFile;
	BOOL bSuccess = FALSE;

	HFile = CreateFile(pszFileName, GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, 0, NULL);
	if (HFile != INVALID_HANDLE_VALUE)
	{
		DWORD dwFileSize;

		dwFileSize = GetFileSize(HFile, NULL);
		if (dwFileSize != 0xFFFFFFFF)
		{
			LPSTR pszFileText;

			pszFileText = (LPSTR)GlobalAlloc(GPTR, dwFileSize + 1);
			if (pszFileText != NULL)
			{
				DWORD dwRead;

				if (ReadFile(HFile, pszFileText, dwFileSize, &dwRead, NULL))
				{
					pszFileText[dwFileSize] = 0; // Добавить нулевой терминатор
					if (SetWindowText(hEdit, pszFileText))
						bSuccess = TRUE; // Это сработало!
				}
				GlobalFree(pszFileText);
			}
		}
		CloseHandle(HFile);
	}
	return bSuccess;
}

BOOL SaveTextFileFromEdit(HWND hEdit, LPCTSTR pszFileName)
{
	HANDLE HFile;
	BOOL bSuccess = FALSE;

	HFile = CreateFile(pszFileName, GENERIC_WRITE, 0, NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (HFile != INVALID_HANDLE_VALUE)
	{
		DWORD dwTextLength;

		dwTextLength = GetWindowTextLength(hEdit);
		// Не нужно беспокоиться, если нет текста.
		if (dwTextLength > 0)
		{
			LPSTR pszText;
			DWORD dwBufferSize = dwTextLength + 1;

			pszText = (LPSTR)GlobalAlloc(GPTR, dwBufferSize);
			if (pszText != NULL)
			{
				if (GetWindowText(hEdit, pszText, dwBufferSize))
				{
					DWORD dwWritten;

					if (WriteFile(HFile, pszText, dwTextLength, &dwWritten, NULL))
						bSuccess = TRUE;
				}
				GlobalFree(pszText);
			}
		}
		CloseHandle(HFile);
	}
	return bSuccess;
}