// Paint_Programing.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Paint_Programing.h"
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include "Windowsx.h"
#include "Windows.h"
#include "DrawObj.h"
#include "Listener.h"
#include "WM_Command.h"
#include "Globals.h"

using namespace std;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// Globals Verieties。	
	Globals::var().hInst = hInstance;
	Listener::WinProcMsgListener().hInst = hInstance;

	// Icon program
	LoadStringW(hInstance, IDS_APP_TITLE, Globals::var().szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_PAINT_PROGRAMING, Globals::var().szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	//map each message I need to seperate functions
	Listener::WinProcMsgListener().AddEvent(WM_COMMAND, WM_CommandEvent);
	Listener::WinProcMsgListener().AddEvent(WM_CREATE, WM_CreateEvent);
	Listener::WinProcMsgListener().AddEvent(WM_MOUSEMOVE, WM_MouseMoveEvent);
	Listener::WinProcMsgListener().AddEvent(WM_LBUTTONDOWN, WM_LButtonDownEvent);
	Listener::WinProcMsgListener().AddEvent(WM_LBUTTONUP, WM_LButtonUpEvent);
	Listener::WinProcMsgListener().AddEvent(WM_KEYDOWN, WM_KeyDownEvent);
	Listener::WinProcMsgListener().AddEvent(WM_PAINT, WM_PaintEvent);
	Listener::WinProcMsgListener().AddEvent(WM_SIZE, WM_SizeEvent);
	Listener::WinProcMsgListener().AddEvent(WM_HSCROLL, WM_HScrollEvent);
	Listener::WinProcMsgListener().AddEvent(WM_VSCROLL, WM_VScrollEvent);
	Listener::WinProcMsgListener().AddEvent(WM_CLOSE, WM_CloseEvent);
	Listener::WinProcMsgListener().AddEvent(WM_DESTROY, WM_DestroyEvent);
	Listener::WinProcMsgListener().AddEvent(WM_GETMINMAXINFO, WM_GetMinMaxInfo);
	Listener::WinProcMsgListener().AddEvent(WM_SETCURSOR, WM_SetCursorEvent);

	// Resize Program
	if (!InitInstance(hInstance, SW_MAXIMIZE))
	{
		return FALSE;
	}

	//create my child window
	WNDCLASSEXW childClass;
	childClass.cbSize = sizeof(WNDCLASSEX);
	childClass.style = CS_HREDRAW | CS_VREDRAW;
	childClass.lpfnWndProc = ChildWndProc;
	childClass.cbClsExtra = 0;
	childClass.cbWndExtra = 0;
	childClass.hInstance = hInstance;
	childClass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PAINT_PROGRAMING));
	childClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	childClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	childClass.lpszMenuName = L"Child";
	childClass.lpszClassName = Globals::var().szChildClass;
	childClass.hIconSm = LoadIcon(childClass.hInstance, MAKEINTRESOURCE(IDI_SMALL));
	RegisterClassEx(&childClass);
	// Tool box
	Globals::var().myChildWindow = CreateWindow(Globals::var().szChildClass, L"Tool Box", WS_CAPTION | WS_VISIBLE | WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 0, 0, 77, 320, Globals::var().hWndFather, (HMENU)103, Globals::var().hInst, NULL);

	//create my tool buttons
	Globals::var().myButton[0] = CreateWindow(L"BUTTON", L"B1", WS_VISIBLE | WS_CHILD | BS_BITMAP, 5, 5, 50, 50, Globals::var().myChildWindow, (HMENU)120, Globals::var().hInst, NULL);
	Globals::var().myButton[1] = CreateWindow(L"BUTTON", L"B2", WS_VISIBLE | WS_CHILD | BS_BITMAP, 5, 60, 50, 50, Globals::var().myChildWindow, (HMENU)121, Globals::var().hInst, NULL);
	Globals::var().myButton[2] = CreateWindow(L"BUTTON", L"B3", WS_VISIBLE | WS_CHILD | BS_BITMAP, 5, 115, 50, 50, Globals::var().myChildWindow, (HMENU)122, Globals::var().hInst, NULL);
	Globals::var().myButton[3] = CreateWindow(L"BUTTON", L"B4", WS_VISIBLE | WS_CHILD | BS_BITMAP, 5, 170, 50, 50, Globals::var().myChildWindow, (HMENU)123, Globals::var().hInst, NULL);
	Globals::var().myButton[4] = CreateWindow(L"BUTTON", L"B5", WS_VISIBLE | WS_CHILD | BS_BITMAP, 5, 225, 50, 50, Globals::var().myChildWindow, (HMENU)124, Globals::var().hInst, NULL);

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PAINT_PROGRAMING));

	MSG msg;

	// 
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}



//
//   MyRegisterClass()
//
//  
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PAINT_PROGRAMING));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_PAINT_PROGRAMING);
	wcex.lpszClassName = Globals::var().szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}
//
//   InitInstance()
//
//  
//

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	Globals::var().hInst = hInstance; 

	Globals::var().hWndFather = CreateWindowW(Globals::var().szWindowClass, Globals::var().szTitle, WS_OVERLAPPEDWINDOW | WS_VSCROLL | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	if (!Globals::var().hWndFather)
	{
		return FALSE;
	}

	ShowWindow(Globals::var().hWndFather, nCmdShow);
	UpdateWindow(Globals::var().hWndFather);

	return TRUE;
}

//
//  WinMain: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  
//
//  WM_COMMAND 
//  WM_PAINT   
//  WM_DESTROY 
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return Listener::WinProcMsgListener().Trig(message, Parameter(hWnd, message, wParam, lParam));
}


//the callback class for child window
LRESULT CALLBACK ChildWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	int wmId;
	static HBITMAP bmpIcon1, bmpIcon2, bmpIcon3, bmpIcon4, bmpIcon5;   //a bitmap icon for button

	switch (message)
	{
	case WM_CREATE:
		bmpIcon1 = (HBITMAP)LoadImage(NULL, L"line.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		bmpIcon2 = (HBITMAP)LoadImage(NULL, L"rect.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		bmpIcon3 = (HBITMAP)LoadImage(NULL, L"circle.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		bmpIcon4 = (HBITMAP)LoadImage(NULL, L"text.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		bmpIcon5 = (HBITMAP)LoadImage(NULL, L"select.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		break;
	case WM_SIZE:
		//InvalidateRect(hWnd, NULL, TRUE);
		break;
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		// Event
		SendMessage(Globals::var().hWndFather, WM_COMMAND, wmId, 0);
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		//image for child window buttons
		SendMessage(Globals::var().myButton[0], BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)bmpIcon1);
		SendMessage(Globals::var().myButton[1], BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)bmpIcon2);
		SendMessage(Globals::var().myButton[2], BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)bmpIcon3);
		SendMessage(Globals::var().myButton[3], BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)bmpIcon4);
		SendMessage(Globals::var().myButton[4], BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)bmpIcon5);
		SendMessage(Globals::var().myButton[Globals::var().currentDrawMode], BM_SETSTATE, BN_PUSHED, 0);

		if (!bmpIcon1)
			MessageBox(hWnd, 0, TEXT("NO IMAGE"), MB_OK); // Infor

		EndPaint(hWnd, &ps);
		ReleaseDC(hWnd, hdc);
		break;
	case WM_DESTROY:
		DeleteObject(bmpIcon1);
		DeleteObject(bmpIcon2);
		DeleteObject(bmpIcon3);
		DeleteObject(bmpIcon4);
		DeleteObject(bmpIcon5);
		PostQuitMessage(0);
		break;
	case WM_WINDOWPOSCHANGING:  //passed when dragging
	{
		WINDOWPOS *pos = (WINDOWPOS *)lParam;
		RECT rectFather;
		GetWindowRect(Globals::var().hWndFather, &rectFather);

		if (pos->x < 0)
			pos->x = 0;

		if (pos->y < 0)
			pos->y = 0;

		if (pos->y > rectFather.bottom - rectFather.top - 397)
			pos->y = rectFather.bottom - rectFather.top - 397;

		if (pos->x > rectFather.right - rectFather.left - 110)
			pos->x = rectFather.right - rectFather.left - 110;
		break;
	}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return NULL;
}

//scroll the window if x and y focus are out of border. X, Y focus is mouse position on screen
void AutoScroll(HWND hwnd, int Xfocus, int Yfocus, int xCurrentScroll, int yCurrentScroll, RECT windowRect)
{
	RECT rect;
	GetWindowRect(hwnd, &rect);
	if (Xfocus > (rect.right - rect.left - 29) && xCurrentScroll < 2000)
	{
		WPARAM wParam;
		if (Globals::var().currentDrawMode == DrawTexts)
			wParam = MAKEWPARAM(SB_THUMBTRACK, xCurrentScroll + (Xfocus - windowRect.right) + 1);
		else
			wParam = MAKEWPARAM(SB_THUMBTRACK, xCurrentScroll + (Xfocus - windowRect.right));
		SendMessage(hwnd, WM_HSCROLL, wParam, NULL);
	}
	else if (xCurrentScroll > 0 && Xfocus <= 0)
	{
		WPARAM wParam;
		if (Globals::var().currentDrawMode == DrawTexts)
			wParam = MAKEWPARAM(SB_THUMBTRACK, (xCurrentScroll + Xfocus - 8) < 0 ? 0 : xCurrentScroll + Xfocus - 8);  //留個空位給新輸入文字
		else
			wParam = MAKEWPARAM(SB_THUMBTRACK, (xCurrentScroll + Xfocus) <= 5 ? 0 : xCurrentScroll + Xfocus - 5);
		SendMessage(hwnd, WM_HSCROLL, wParam, NULL);
	}

	if (Yfocus > windowRect.bottom && yCurrentScroll < 2000)
	{
		WPARAM wParam = MAKEWPARAM(SB_THUMBTRACK, yCurrentScroll + (Yfocus - windowRect.bottom));
		SendMessage(hwnd, WM_VSCROLL, wParam, NULL);
	}
	else if (yCurrentScroll > 0 && Yfocus < 0)
	{
		WPARAM wParam;
		if (Globals::var().currentDrawMode == DrawTexts)
			wParam = MAKEWPARAM(SB_THUMBTRACK, (yCurrentScroll + Yfocus - 10) < 0 ? 0 : yCurrentScroll + Yfocus - 10);
		else
			wParam = MAKEWPARAM(SB_THUMBTRACK, (yCurrentScroll + Yfocus) < 0 ? 0 : yCurrentScroll + Yfocus);
		SendMessage(hwnd, WM_VSCROLL, wParam, NULL);
	}
}

//for scrolling while moving object.
//scroll the window if x and y focus are out of border. X, Y focus is mouse position on screen
void AutoScrollObject(HWND hwnd, const DrawObj* obj, int xCurrentScroll, int yCurrentScroll, RECT windowRect)
{
	//RECT rect;
	//GetWindowRect(hwnd, &rect);

	int top = (obj->ptBeg.y < obj->ptEnd.y ? obj->ptBeg.y : obj->ptEnd.y);
	int left = (obj->ptBeg.x < obj->ptEnd.x ? obj->ptBeg.x : obj->ptEnd.x);
	int bottom = (obj->ptBeg.y > obj->ptEnd.y ? obj->ptBeg.y : obj->ptEnd.y);
	int right = (obj->ptBeg.x > obj->ptEnd.x ? obj->ptBeg.x : obj->ptEnd.x);
	Globals::var().autoScrolling = true;

	POINT p;  //p is mouse position
	GetCursorPos(&p);
	ScreenToClient(hwnd, &p);

	// x direction
	if (right - xCurrentScroll > windowRect.right && xCurrentScroll < 2000 && p.x > windowRect.right / 2)
	{
		WPARAM wParam = MAKEWPARAM(SB_THUMBTRACK, right - windowRect.right);
		SendMessage(hwnd, WM_HSCROLL, wParam, NULL);
	}
	else if (xCurrentScroll > 0 && left - xCurrentScroll < 0 && p.x < windowRect.right / 2)
	{
		WPARAM wParam = MAKEWPARAM(SB_THUMBTRACK, left < 0 ? 0 : left);
		SendMessage(hwnd, WM_HSCROLL, wParam, NULL);
	}

	// y direction
	if (bottom - yCurrentScroll > windowRect.bottom && yCurrentScroll < 2000 && p.y > windowRect.bottom / 2) //滑鼠要接近底部
	{
		WPARAM wParam = MAKEWPARAM(SB_THUMBTRACK, bottom - windowRect.bottom);
		SendMessage(hwnd, WM_VSCROLL, wParam, NULL);
	}
	else if (yCurrentScroll > 0 && top - yCurrentScroll < 0 && p.y < windowRect.bottom / 2)
	{
		WPARAM wParam = MAKEWPARAM(SB_THUMBTRACK, top < 0 ? 0 : top);
		SendMessage(hwnd, WM_VSCROLL, wParam, NULL);
	}

	Globals::var().autoScrolling = false;
}

//for scrolling while resizing object.
//scrolling is effective while mouse and object border is same opsition.
void AutoScrollObjectResize(HWND hwnd, const DrawObj* obj, int xCurrentScroll, int yCurrentScroll, RECT windowRect)
{
	int top = (obj->ptBeg.y < obj->ptEnd.y ? obj->ptBeg.y : obj->ptEnd.y);
	int left = (obj->ptBeg.x < obj->ptEnd.x ? obj->ptBeg.x : obj->ptEnd.x);
	int bottom = (obj->ptBeg.y > obj->ptEnd.y ? obj->ptBeg.y : obj->ptEnd.y);
	int right = (obj->ptBeg.x > obj->ptEnd.x ? obj->ptBeg.x : obj->ptEnd.x);
	Globals::var().autoScrolling = true;

	POINT p;  //p is mouse position
	GetCursorPos(&p);
	ScreenToClient(hwnd, &p);

	// x direction
	if (right - xCurrentScroll > windowRect.right && xCurrentScroll < 2000 && abs(p.x - right + xCurrentScroll) < 30)
	{
		WPARAM wParam = MAKEWPARAM(SB_THUMBTRACK, right - windowRect.right);
		SendMessage(hwnd, WM_HSCROLL, wParam, NULL);
	}
	else if (xCurrentScroll > 0 && left - xCurrentScroll < 0 && abs(p.x - left + xCurrentScroll) < 30)
	{
		WPARAM wParam = MAKEWPARAM(SB_THUMBTRACK, left - 1 < 0 ? 0 : left - 1);
		SendMessage(hwnd, WM_HSCROLL, wParam, NULL);
	}
	else if (right - xCurrentScroll < 0 && xCurrentScroll > 0 && abs(p.x - right + xCurrentScroll) < 30)
	{
		WPARAM wParam = MAKEWPARAM(SB_THUMBTRACK, right - windowRect.right);
		SendMessage(hwnd, WM_HSCROLL, wParam, NULL);
	}
	else if (xCurrentScroll < 2000 && left - xCurrentScroll > windowRect.right && abs(p.x - left + xCurrentScroll) < 30)
	{
		WPARAM wParam = MAKEWPARAM(SB_THUMBTRACK, left - 1 < 0 ? 0 : left - 1);
		SendMessage(hwnd, WM_HSCROLL, wParam, NULL);
	}

	// y direction
	if (bottom - yCurrentScroll > windowRect.bottom && yCurrentScroll < 2000 && abs(p.y - bottom + yCurrentScroll) < 30) //滑鼠要接近底部
	{
		WPARAM wParam = MAKEWPARAM(SB_THUMBTRACK, bottom - windowRect.bottom);
		SendMessage(hwnd, WM_VSCROLL, wParam, NULL);
	}
	else if (yCurrentScroll > 0 && top - yCurrentScroll < 0 && abs(p.y - top + yCurrentScroll) < 30)
	{
		WPARAM 	wParam = MAKEWPARAM(SB_THUMBTRACK, top < 0 ? 0 : top);
		SendMessage(hwnd, WM_VSCROLL, wParam, NULL);
	}
	else if (bottom - yCurrentScroll < 0 && yCurrentScroll > 0 && abs(p.y - bottom + yCurrentScroll) < 30)  //mouse is on bottom line
	{
		WPARAM wParam = MAKEWPARAM(SB_THUMBTRACK, bottom - 1 < 0 ? 0 : bottom - 1);
		SendMessage(hwnd, WM_VSCROLL, wParam, NULL);
	}
	else if (top - yCurrentScroll > windowRect.bottom && yCurrentScroll < 2000 && abs(p.y - top + yCurrentScroll) < 30)  //mouse is on top line
	{
		WPARAM wParam = MAKEWPARAM(SB_THUMBTRACK, top - windowRect.bottom);
		SendMessage(hwnd, WM_VSCROLL, wParam, NULL);
	}
}

//for scrolling while drawing object.
//we only compare ptEnd when is drawing the object on ptEnd
void AutoScrollObjectWhenDrawing(HWND hwnd, const DrawObj* obj, int xCurrentScroll, int yCurrentScroll, RECT windowRect)
{
	Globals::var().autoScrolling = true;

	if (obj->ptEnd.x - xCurrentScroll > windowRect.right && xCurrentScroll < 2000)
	{
		WPARAM wParam = MAKEWPARAM(SB_THUMBTRACK, obj->ptEnd.x - windowRect.right);
		SendMessage(hwnd, WM_HSCROLL, wParam, NULL);
	}
	else if (xCurrentScroll > 0 && obj->ptEnd.x - xCurrentScroll < 0)
	{
		WPARAM wParam = MAKEWPARAM(SB_THUMBTRACK, obj->ptEnd.x < 0 ? 0 : obj->ptEnd.x);
		SendMessage(hwnd, WM_HSCROLL, wParam, NULL);
	}

	if (obj->ptEnd.y - yCurrentScroll > windowRect.bottom && yCurrentScroll < 2000)
	{
		WPARAM wParam = MAKEWPARAM(SB_THUMBTRACK, obj->ptEnd.y - windowRect.bottom);
		SendMessage(hwnd, WM_VSCROLL, wParam, NULL);
	}
	else if (yCurrentScroll > 0 && obj->ptEnd.y - yCurrentScroll < 0)
	{
		WPARAM wParam = MAKEWPARAM(SB_THUMBTRACK, obj->ptEnd.y < 0 ? 0 : obj->ptEnd.y);
		SendMessage(hwnd, WM_VSCROLL, wParam, NULL);
	}
	Globals::var().autoScrolling = false;
}

RECT GetLocalCoordinates(HWND hWnd)
{
	RECT Rect;
	GetWindowRect(hWnd, &Rect);
	MapWindowPoints(HWND_DESKTOP, GetParent(hWnd), (LPPOINT)&Rect, 2);
	return Rect;
}

void CleanObjects(HWND hWnd)
{
	for (auto& it : Globals::var().drawObjList)  //delete each pointer. 
		delete(it);
	Globals::var().drawObjList.clear();  //clear() does not delete memory! WTF! (or use smart pointers) (line 170)
	Globals::var().selectedObjectPtr = nullptr;
	Globals::var().hasSelected = false;
	Globals::var().pastebinObjectPtr = nullptr;
	InvalidateRect(hWnd, NULL, TRUE);
}

void SetTitle(string name, HWND hWnd)
{
	string s = "1412552 - Basic Paint Program - [" + name + "]";
	wstring stemp = wstring(s.begin(), s.end());
	LPCWSTR sw = stemp.c_str();
	SetWindowText(hWnd, sw);
}

void ChangeToolsSelectionState(int position, HMENU hMenu)
{
	if (Globals::var().selectedObjectPtr != nullptr && position != 4)  //de-select the current object
		Globals::var().selectedObjectPtr = nullptr;
	HMENU hMenu2 = GetSubMenu(hMenu, 2);   //hMenu2 = Edit
	for (int i = 0; i < 5; i++)
	{
		if (i == position)
		{
			SendMessage(Globals::var().myButton[i], BM_SETSTATE, BN_PUSHED, 0);
			CheckMenuItem(hMenu2, i, MF_CHECKED | MF_BYPOSITION);
		}
		else
		{
			SendMessage(Globals::var().myButton[i], BM_SETSTATE, 0, 0);
			CheckMenuItem(hMenu2, i, MF_UNCHECKED | MF_BYPOSITION);
		}
	}
}

void ChangeColorsSelectionState(int position, HMENU hMenu)
{
	if (Globals::var().currentDrawMode == DrawTexts && Globals::var().newText.startFinished)  //change newText
	{
		Globals::var().mlog.OP_modifyStart(&Globals::var().newText, -1);
		Globals::var().newText.color = position;
		Globals::var().modifyState = 1;
		Globals::var().mlog.OP_modifyEnd(&Globals::var().newText);
		InvalidateRect(Globals::var().hWndFather, NULL, FALSE);
	}
	else if (Globals::var().selectedObjectPtr != nullptr)  //change selected object
	{
		auto it = find(Globals::var().drawObjList.begin(), Globals::var().drawObjList.end(), Globals::var().selectedObjectPtr);
		if (it != Globals::var().drawObjList.end())
		{
			int pos = distance(Globals::var().drawObjList.begin(), it);
			Globals::var().mlog.OP_modifyStart(Globals::var().selectedObjectPtr, pos);
			Globals::var().selectedObjectPtr->color = position;
			Globals::var().modifyState = 1;
			Globals::var().mlog.OP_modifyEnd(Globals::var().selectedObjectPtr);
			InvalidateRect(Globals::var().hWndFather, NULL, FALSE);
		}
	}

	HMENU hMenu2 = GetSubMenu(hMenu, 2);   //hMenu2 = Edit
	HMENU hMenu3 = GetSubMenu(hMenu2, 6);  //hMenu3 = Edit - Delete
	for (int i = 0; i < 8; i++)
	{
		if (i == position)
		{
			CheckMenuItem(hMenu3, i, MF_CHECKED | MF_BYPOSITION);
		}
		else
		{
			CheckMenuItem(hMenu3, i, MF_UNCHECKED | MF_BYPOSITION);
		}
	}
}

void ChangeBGSelectionState(int position, HMENU hMenu)
{
	if (Globals::var().currentDrawMode == DrawTexts && Globals::var().newText.startFinished)  //change newText
	{
		Globals::var().mlog.OP_modifyStart(&Globals::var().newText, -1);
		Globals::var().newText.backgroundColor = position;
		Globals::var().modifyState = 1;
		Globals::var().mlog.OP_modifyEnd(&Globals::var().newText);
		InvalidateRect(Globals::var().hWndFather, NULL, FALSE);
	}
	else if (Globals::var().selectedObjectPtr != nullptr)  //change selected object
	{
		auto it = find(Globals::var().drawObjList.begin(), Globals::var().drawObjList.end(), Globals::var().selectedObjectPtr);
		if (it != Globals::var().drawObjList.end())
		{
			int pos = distance(Globals::var().drawObjList.begin(), it);
			Globals::var().mlog.OP_modifyStart(Globals::var().selectedObjectPtr, pos);
			Globals::var().selectedObjectPtr->backgroundColor = position;
			Globals::var().modifyState = 1;
			Globals::var().mlog.OP_modifyEnd(Globals::var().selectedObjectPtr);
			InvalidateRect(Globals::var().hWndFather, NULL, FALSE);
		}
	}

	HMENU hMenu2 = GetSubMenu(hMenu, 2);   //hMenu2 = Edit
	HMENU hMenu5 = GetSubMenu(hMenu2, 8);  //hMenu5 = Edit - Undo
	for (int i = 0; i < 8; i++)
	{
		if (i == position)
		{
			CheckMenuItem(hMenu5, i, MF_CHECKED | MF_BYPOSITION);
		}
		else
		{
			CheckMenuItem(hMenu5, i, MF_UNCHECKED | MF_BYPOSITION);
		}
	}
}

void ChangeLineSelectionState(int position, HMENU hMenu)
{
	if (Globals::var().selectedObjectPtr != nullptr && Globals::var().selectedObjectPtr->objectType != 4)
	{
		auto it = find(Globals::var().drawObjList.begin(), Globals::var().drawObjList.end(), Globals::var().selectedObjectPtr);
		if (it != Globals::var().drawObjList.end())
		{
			int pos = distance(Globals::var().drawObjList.begin(), it);
			Globals::var().mlog.OP_modifyStart(Globals::var().selectedObjectPtr, pos);
			Globals::var().selectedObjectPtr->lineWidth = position;
			Globals::var().mlog.OP_modifyEnd(Globals::var().selectedObjectPtr);
			InvalidateRect(Globals::var().hWndFather, NULL, FALSE);
		}
	}
	HMENU hMenu2 = GetSubMenu(hMenu, 2);   //hMenu2 = Edit
	HMENU hMenu4 = GetSubMenu(hMenu2, 7);  //hMenu4 = Redo
	for (int i = 0; i < 5; i++)
	{
		if (i == position - 1)
		{
			CheckMenuItem(hMenu4, i, MF_CHECKED | MF_BYPOSITION);
		}
		else
		{
			CheckMenuItem(hMenu4, i, MF_UNCHECKED | MF_BYPOSITION);
		}
	}
}
