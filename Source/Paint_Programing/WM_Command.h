#pragma once
/****************************** Module Header ******************************\
* Module Name:	VM_Command.h
* Project:		Paint_Programing
* Verson: 1.0
*
* History:
* * 13/12/2016 18:30 PM Tran Trung Tinh Created
\***************************************************************************/
#include "stdafx.h"
#include "Listener.h"
#include "DrawObj.h"
// includes Event 
LRESULT DefaultEvnetHandler(Parameter& param);
//---------------------------------------------------------
LRESULT WM_CommandEvent(Parameter& param);
LRESULT WM_CreateEvent(Parameter& param);
LRESULT WM_MouseMoveEvent(Parameter& param);
LRESULT WM_LButtonDownEvent(Parameter& param);
LRESULT WM_LButtonUpEvent(Parameter& param);
LRESULT WM_KeyDownEvent(Parameter& param);
LRESULT WM_PaintEvent(Parameter& param);
LRESULT WM_SizeEvent(Parameter& param);
LRESULT WM_HScrollEvent(Parameter& param);
LRESULT WM_VScrollEvent(Parameter& param);
LRESULT WM_CloseEvent(Parameter& param);
LRESULT WM_DestroyEvent(Parameter& param);
LRESULT WM_SetCursorEvent(Parameter& param);
LRESULT WM_GetMinMaxInfo(Parameter& param);
//---------------------------------------------------------
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

POINT MovePastedObj();
void PushCurrentNewText(TextObj&);

void Redo();
void Undo();
void UpdateNewText(vector<string>, POINT);

void ToggleUndoButton();
void FocusSelectedObject(DrawObj*, HWND);

enum DrawMode { DrawLine, DrawRect, DrawCircle, DrawTexts, SelectTool };
