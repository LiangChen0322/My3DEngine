// My3dEngine.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <Windows.h>
#include <iostream>
#include <fstream>

#include "My3dEngine.h"

#define MAX_LOADSTRING 100

const int scWidth  = 900;
const int scHeight = 600;
//char * disBuffer;

//HBITMAP hBitmap = NULL;
HDC         scDC;        // the DC to hold our image
HBITMAP     scDisplay;       // the actual bitmap which contains the image (will be put in the DC)
HBITMAP     scDisplayOld;    // the DC's old bitmap (for cleanup)
//BITMAP		scBitmap = { 0, 900, 600, 2, 4, 1};
BYTE        *scBits;
COLORREF *disBuffer;

HINSTANCE hInst;								// ��ǰʵ��
TCHAR szTitle[MAX_LOADSTRING];					// �������ı�
TCHAR szWindowClass[MAX_LOADSTRING];			// ����������


// �˴���ģ���а����ĺ�����ǰ������: 
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

 	// TODO:  �ڴ˷��ô��롣
	MSG msg;
	HACCEL hAccelTable;

	// ��ʼ��ȫ���ַ���
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_MY3DENGINE, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// ִ��Ӧ�ó����ʼ��: 
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MY3DENGINE));

	// ����Ϣѭ��: 
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  ����:  MyRegisterClass()
//
//  Ŀ��:  ע�ᴰ���ࡣ
//
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

void loadImage(HWND hWnd)
{
	disBuffer = (COLORREF*)malloc(scWidth * scHeight * sizeof(COLORREF));
	memset((void *)disBuffer, 0x33, scHeight * scWidth * sizeof(COLORREF));
	COLORREF color = RGB(0, 255, 0);
	for (int i = 0; i < scHeight; i += scWidth + 1) {
		disBuffer[i] = color;
	}
}

//
//   ����:  InitInstance(HINSTANCE, int)
//
//   Ŀ��:  ����ʵ�����������������
//
//   ע��: 
//
//        �ڴ˺����У�������ȫ�ֱ����б���ʵ�������
//        ��������ʾ�����򴰿ڡ�
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // ��ʵ������洢��ȫ�ֱ�����

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd) {
      return FALSE;
   }

   SetWindowPos(hWnd, HWND_TOP, 400, 300, scWidth, scHeight, 0);
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   SetTimer(hWnd, 0, 1000, NULL);
   //loadImage();

   return TRUE;
}

void MyPaint(HWND hWnd)
{

	PAINTSTRUCT 	ps;
	HDC 			hdc;
	
	hdc = BeginPaint(hWnd, &ps);
	//hdc = GetDC(hWnd);
	// Creating temp bitmap
	HBITMAP bitmap = CreateBitmap(scWidth,  scHeight, 1, 8*4, (void *) disBuffer);
	if (bitmap == NULL)
		std::cout << "test";

	// Temp HDC to copy picture
	HDC src = CreateCompatibleDC(hdc); // hdc - Device context for window, I've got earlier with GetDC(hWnd) or GetDC(NULL);
	SelectObject(src, bitmap); // Inserting picture into our temp HDC
	// Copy image from temp HDC to window
	BitBlt(hdc, // Destination
	       0,  // x and
	       0,  // y - upper-left corner of place, where we'd like to copy
	       scWidth, // width of the region
	       scHeight, // height
	       src, // source
	       0,   // x and
	       0,   // y of upper left corner  of part of the source, from where we'd like to copy
	       SRCCOPY); // Defined DWORD to juct copy pixels. Watch more on msdn;

	DeleteDC(src); // Deleting temp HDC
	EndPaint(hWnd, &ps);
}

//
//  ����:  WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  Ŀ��:    ���������ڵ���Ϣ��
//
//  WM_COMMAND	- ����Ӧ�ó���˵�
//  WM_PAINT	- ����������
//  WM_DESTROY	- �����˳���Ϣ������
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message) {
	case WM_TIMER:
		//MyPaint(hWnd);
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// �����˵�ѡ��: 
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
	case WM_CREATE:
		loadImage(hWnd);
		break;
	case WM_PAINT:
		MyPaint(hWnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_KEYDOWN: {
		wchar_t a[5] = { 0 };
		a[0] = wParam;
		MessageBoxW(hWnd, L"", a, 0);
		break;
	}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// �����ڡ������Ϣ�������
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
