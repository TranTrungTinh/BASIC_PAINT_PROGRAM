#pragma once
#include "DrawObj.h"
#include "mylog.h"
#define MAX_LOADSTRING 100
/****************************** Module Header ******************************\
* Module Name:	DrawObj.h
* Project:		Paint_Programing
* Verson: 1.0
*
* History:
* * 2/12/2016 Tran Trung Tinh Created
\***************************************************************************/
// Include Globals Verieties 
class Globals
{
public:
	HINSTANCE hInst;                                // hIsnt
	WCHAR szTitle[MAX_LOADSTRING];                  // sizeTitle
	WCHAR szWindowClass[MAX_LOADSTRING];            // sizeWindowClass
	WCHAR szChildClass[MAX_LOADSTRING];				// sizeChildClass
	HWND hWndFather, myChildWindow;
	HWND myButton[5];
	int currentDrawMode;     //0=line, 1=rect, 2=circle, 3=text, 4=select ( Tool box )
	int modifyState;  //0=new file, 1=modified but not saved, 2=saved or opened
	string fileName;  //file name to display on window title
	DrawObj* pastebinObjectPtr;  // Paste after Copy of Cut
	DrawObj* selectedObjectPtr;  // Select Shape
	DrawObj* preSelectObject;    // Handle Select Shape
	wstring lastFilePath;  //path to last opened file. 

	int mouseX, mouseY;
	TextObj newText;  //only newText needs to be global currently
	bool mouseHasDown;
	bool hasSelected;

	RECT rect;
	bool autoScrolling;
	TCHAR szBuffer[50];
	int cxChar, cyChar;
	list<DrawObj*> drawObjList; // <-next time may use vector<unique_ptr<DrawObj>> DrawObjList;
	mylog mlog;

	//----------------------------------------------------------------------------------------------
	bool debugMode; // Run program on Debug Mode
	//----------------------------------------------------------------------------------------------

	Globals();
	static Globals& var();
};
