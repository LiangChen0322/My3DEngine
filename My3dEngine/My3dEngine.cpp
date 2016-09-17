// My3dEngine.cpp
//

#include "stdafx.h"
#include <Windows.h>
#include <iostream>
#include <fstream>
#include "World.h"

#include "My3dEngine.h"

#define MAX_LOADSTRING 100

const int scWidth  = 900;
const int scHeight = 600;

g3::World world(scWidth, scHeight);

HINSTANCE hInst;
TCHAR szTitle[MAX_LOADSTRING];
TCHAR szWindowClass[MAX_LOADSTRING];

ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	MSG msg;
	HACCEL hAccelTable;

	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_MY3DENGINE, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	if (!InitInstance (hInstance, nCmdShow)) {
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MY3DENGINE));

	while (GetMessage(&msg, NULL, 0, 0)) {
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MY3DENGINE));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_MY3DENGINE);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	hInst = hInstance;
	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
						CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	if (!hWnd) {
		return FALSE;
	}

	SetWindowPos(hWnd, HWND_TOP, 400, 300, scWidth, scHeight, 0);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	SetTimer(hWnd, 0, 50, NULL);

	return TRUE;
}

/* draw and is play the screen */
void MyPaint(HWND hWnd)
{
	PAINTSTRUCT    ps;
	HDC            hdc;
	HBITMAP        disBitmap;
	COLORREF      *disBuffer;
	
	hdc = BeginPaint(hWnd, &ps);
	HDC hdcMem = CreateCompatibleDC(hdc);

	disBuffer = world.getBuffer();
	disBitmap = CreateBitmap(scWidth,  scHeight, 1, 8*4, (void *)disBuffer);
	if (disBitmap == NULL)
		return;

	SelectObject(hdcMem, disBitmap);
	BitBlt(hdc, 0, 0, scWidth, scHeight, hdcMem, 0, 0, SRCCOPY);

	DeleteDC(hdcMem); // Deleting temp HDC
	EndPaint(hWnd, &ps);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;

	switch (message) {
	case WM_TIMER:
		world.freshFrame();
		InvalidateRect(hWnd, NULL, TRUE);
		//std::cout << "Timer trigger" << std::endl;
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		switch (wmId) {
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	// case WM_CREATE:
	// 	break;
	case WM_ERASEBKGND:
		return TRUE;
	case WM_PAINT:
		MyPaint(hWnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_KEYDOWN: {
		// wchar_t a[5] = { 0 };
		// a[0] = wParam;
		// MessageBoxW(hWnd, L"", a, 0);
		world.key_press((char)wParam);
		break;
	}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
