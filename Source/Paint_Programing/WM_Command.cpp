#include "stdafx.h"
#include "Windowsx.h "
#include "WM_Command.h"
#include "Globals.h"
#include "Save.h"
#include "Paint_Programing.h"
#include "mylog.h"

static int mouseX, mouseY;
static LineObj  newLine;
static RectangularObj newRect;
static CircleObj newCircle;
static bool mouseHasDown = false;

static RECT rect;
static TCHAR szBuffer[50];
static int cxChar, cyChar;
static int xPasteDir, yPasteDir;  // paste = 1 ; don't paste = 0

SCROLLINFO si;
// These variables are required by BitBlt.
//static HDC hdcWin;           // window DC
static HDC hdcScreen;        // DC for entire screen
							 ///static HDC hdcScreenCompat;  // memory DC for screen
							 ///static HBITMAP hbmpCompat;   // bitmap handle to old DC
static BITMAP bmp;           // bitmap data structure
static BOOL fBlt;            // TRUE if BitBlt occurred
static BOOL fScroll;         // TRUE if scrolling occurred
static BOOL fSize;           // TRUE if fBlt & WM_SIZE

							 // These variables are required for horizontal scrolling.
static int xMinScroll;       // minimum horizontal scroll value
static int xCurrentScroll;   // current horizontal scroll value
static int xMaxScroll;       // maximum horizontal scroll value

							 // These variables are required for vertical scrolling.
static int yMinScroll;       // minimum vertical scroll value
static int yCurrentScroll;   // current vertical scroll value
static int yMaxScroll;       // maximum vertical scroll value
static HMENU hMenu = NULL;     //try to get the system menu
static HBITMAP hBmp;         //bitmap for memory DC

static int currentColor;        //0=black, 0~7 kinds of color
static int currentBgColor;      //0=transparent, 0~7 kinds of color
static int currentLineWidth;    //width 1pt-5pt
static int currentCursorMode;   //0=original 1=black 2=gray 3=red 4=green 5=emerald green 6=blue 7=yellow 8=pink
static HCURSOR cursors[6];      //0=original 1=black 2=gray 3=red 4=green 5=blue 
static bool isMoving, isResizing;
static int hScrollResize = 0;   //counter for preventing fullscreen scroll size change. skip if count to 2
static HCURSOR * currentCursor;

LRESULT DefaultEvnetHandler(Parameter& param)
{
	return DefWindowProc(param.hWnd_, param.message_, param.wParam_, param.lParam_);
}

LRESULT WM_CommandEvent(Parameter& param)
{
	int wmId = LOWORD(param.wParam_);
	// Event
	switch (wmId)
	{
	case IDM_ABOUT:  // About program
		DialogBox(Globals::var().hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), param.hWnd_, About);
		break;
	case IDM_EXIT:  //Exit program
		SendMessage(param.hWnd_, WM_CLOSE, NULL, NULL);
		break;
		DestroyWindow(param.hWnd_);
		break;
	case ID_COMMAND_1:  //Clear Painter
	{
		if (Globals::var().modifyState == 1) // Aks user disply save dialog
		{
			if (DisplayConfirmClearMessageBox(Globals::var().fileName) == IDYES)
			{
				PushCurrentNewText(Globals::var().newText);
				if (Globals::var().lastFilePath.size() < 1)
				{
					SaveToFile(Globals::var().drawObjList, Globals::var().fileName);
					SetTitle(Globals::var().fileName, param.hWnd_);
					Globals::var().modifyState = 2;
				}
				else
				{
					SaveToLastFilePath(Globals::var().drawObjList);
					Globals::var().modifyState = 2;
				}
			}
		}
		int oldState = Globals::var().modifyState;
		Globals::var().mlog.ClearLogs();
		if (Globals::var().drawObjList.size() > 0)  //set modify state
			Globals::var().modifyState = 1;
		else
			Globals::var().modifyState = oldState;
		newLine.Clean();
		newRect.Clean();
		Globals::var().newText.Clean();
		newCircle.Clean();
		Globals::var().selectedObjectPtr = NULL;
		currentCursorMode = 0;
		Globals::var().hasSelected = false;
		CleanObjects(param.hWnd_);
		break;
	}
	case 120:
		SetFocus(param.hWnd_);  //return the focus back to main window
	case ID_LineTool:
		Globals::var().currentDrawMode = DrawLine;
		ChangeToolsSelectionState(Globals::var().currentDrawMode, hMenu);
		PushCurrentNewText(Globals::var().newText);
		Globals::var().hasSelected = false;
		InvalidateRect(param.hWnd_, NULL, FALSE);
		break;
	case 121:
		SetFocus(param.hWnd_);
	case ID_RectTool:
		Globals::var().currentDrawMode = DrawRect;
		ChangeToolsSelectionState(Globals::var().currentDrawMode, hMenu);
		PushCurrentNewText(Globals::var().newText);
		Globals::var().hasSelected = false;
		InvalidateRect(param.hWnd_, NULL, FALSE);
		break;
	case 122:
		SetFocus(param.hWnd_);
	case ID_CircleTool:
		Globals::var().currentDrawMode = DrawCircle;
		ChangeToolsSelectionState(Globals::var().currentDrawMode, hMenu);
		PushCurrentNewText(Globals::var().newText);
		Globals::var().hasSelected = false;
		InvalidateRect(param.hWnd_, NULL, FALSE);
		break;
	case 123:
		SetFocus(param.hWnd_);
	case ID_TextTool:
		Globals::var().currentDrawMode = DrawTexts;
		Globals::var().hasSelected = false;
		ChangeToolsSelectionState(Globals::var().currentDrawMode, hMenu);
		break;
	case 124:
		SetFocus(param.hWnd_);
	case ID_SelectTool:
		Globals::var().currentDrawMode = SelectTool;
		PushCurrentNewText(Globals::var().newText);
		ChangeToolsSelectionState(Globals::var().currentDrawMode, hMenu);
		InvalidateRect(param.hWnd_, NULL, FALSE);
		break;
	case ID_BLACK:
		currentColor = 0;
		ChangeColorsSelectionState(currentColor, hMenu);
		break;
	case ID_GRAY:
		currentColor = 1;
		ChangeColorsSelectionState(currentColor, hMenu);
		break;
	case ID_RED:
		currentColor = 2;
		ChangeColorsSelectionState(currentColor, hMenu);
		break;
	case ID_GREEN:
		currentColor = 3;
		ChangeColorsSelectionState(currentColor, hMenu);
		break;
	case ID_BLU:
		currentColor = 4;
		ChangeColorsSelectionState(currentColor, hMenu);
		break;
	case ID_CYAN:
		currentColor = 5;
		ChangeColorsSelectionState(currentColor, hMenu);
		break;
	case ID_YELLOW:
		currentColor = 6;
		ChangeColorsSelectionState(currentColor, hMenu);
		break;
	case ID_Magenta:
		currentColor = 7;
		ChangeColorsSelectionState(currentColor, hMenu);
		break;
	case ID_BG_Transparent:
		currentBgColor = 0;
		ChangeBGSelectionState(currentBgColor, hMenu);
		break;
	case  ID_BG_GRAY:
		currentBgColor = 1;
		ChangeBGSelectionState(currentBgColor, hMenu);
		break;
	case  ID_BG_RED:
		currentBgColor = 2;
		ChangeBGSelectionState(currentBgColor, hMenu);
		break;
	case  ID_BG_GREEN:
		currentBgColor = 3;
		ChangeBGSelectionState(currentBgColor, hMenu);
		break;
	case  ID_BG_BLU:
		currentBgColor = 4;
		ChangeBGSelectionState(currentBgColor, hMenu);
		break;
	case  ID_BG_CYAN:
		currentBgColor = 5;
		ChangeBGSelectionState(currentBgColor, hMenu);
		break;
	case  ID_BG_YELLOW:
		currentBgColor = 6;
		ChangeBGSelectionState(currentBgColor, hMenu);
		break;
	case  ID_BG_Magenta:
		currentBgColor = 7;
		ChangeBGSelectionState(currentBgColor, hMenu);
		break;
	case ID_Wide1:
		currentLineWidth = 1;
		ChangeLineSelectionState(currentLineWidth, hMenu);
		break;
	case ID_Wide2:
		currentLineWidth = 2;
		ChangeLineSelectionState(currentLineWidth, hMenu);
		break;
	case ID_Wide3:
		currentLineWidth = 3;
		ChangeLineSelectionState(currentLineWidth, hMenu);
		break;
	case ID_Wide4:
		currentLineWidth = 4;
		ChangeLineSelectionState(currentLineWidth, hMenu);
		break;
	case ID_Wide5:
		currentLineWidth = 5;
		ChangeLineSelectionState(currentLineWidth, hMenu);
		break;
	case ID_SAVE:
		if (Globals::var().modifyState == 0 || Globals::var().lastFilePath.size() <1)
			goto SAVE_AS_NEW_FILE;  //there doesn't exist an old file path
		PushCurrentNewText(Globals::var().newText);
		SaveToLastFilePath(Globals::var().drawObjList);
		Globals::var().modifyState = 2;
		SetTitle(Globals::var().fileName, param.hWnd_);
		break;
	case ID_SAVE_AS:
	{
	SAVE_AS_NEW_FILE:
		PushCurrentNewText(Globals::var().newText);
		SaveToFile(Globals::var().drawObjList, Globals::var().fileName);
		Globals::var().modifyState = 2;
		SetTitle(Globals::var().fileName, param.hWnd_);
		break;
	}
	case ID_NEW_FILE:
		if (Globals::var().modifyState == 1) //disply save dialog
		{
			if (DisplayConfirmNewFileMessageBox(Globals::var().fileName) == IDYES)
			{
				PushCurrentNewText(Globals::var().newText);
				if (Globals::var().modifyState == 0 || Globals::var().lastFilePath.size() < 1)
					SaveToFile(Globals::var().drawObjList, Globals::var().fileName);  //do not have last opened file					
				else
					SaveToLastFilePath(Globals::var().drawObjList);  //save to last opened file
			}
		}
		Globals::var().fileName = "My Paint";
		Globals::var().lastFilePath.clear();
		SetTitle(Globals::var().fileName, param.hWnd_);
		newLine.Clean();
		newRect.Clean();
		Globals::var().newText.Clean();
		newCircle.Clean();
		Globals::var().selectedObjectPtr = NULL;
		currentCursorMode = 0;
		Globals::var().hasSelected = false;
		CleanObjects(param.hWnd_);
		Globals::var().mlog.ClearLogs();
		Globals::var().modifyState = 0;  //<-this must be set after ClearLogs(), else it will be changed inside.
		break;
	case ID_OPEN_FILE:
	{
		if (Globals::var().modifyState == 1) //disply save dialog
		{
			if (DisplayConfirmNewFileMessageBox(Globals::var().fileName) == IDYES)
			{
				PushCurrentNewText(Globals::var().newText);
				if (Globals::var().modifyState == 0 || Globals::var().lastFilePath.size() < 1)
					SaveToFile(Globals::var().drawObjList, Globals::var().fileName);  //do not have last opened file					
				else
					SaveToLastFilePath(Globals::var().drawObjList);  //save to last opened file
			}
		}
		Globals::var().selectedObjectPtr = NULL;
		Globals::var().hasSelected = false;
		currentCursorMode = 0;
		Globals::var().mlog.ClearLogs();
		CleanObjects(param.hWnd_);
		ReadFromFile(Globals::var().drawObjList, Globals::var().fileName);
		newLine.Clean();
		newRect.Clean();
		Globals::var().newText.Clean();
		newCircle.Clean();
		Globals::var().mlog.ClearLogs();
		Globals::var().modifyState = 2;
		SetTitle(Globals::var().fileName, param.hWnd_);
		InvalidateRect(param.hWnd_, NULL, TRUE);
		break;
	}
	case ID_Copy:
	{
		if (Globals::var().selectedObjectPtr == nullptr)
			break;

		if (Globals::var().pastebinObjectPtr != nullptr)
			delete Globals::var().pastebinObjectPtr;

		xPasteDir = 0;  //init paste direction is upper left
		yPasteDir = 0;

		switch (Globals::var().selectedObjectPtr->objectType)
		{
		case Line:
		{
			LineObj *temp = (LineObj*)Globals::var().selectedObjectPtr;
			Globals::var().pastebinObjectPtr = new LineObj(*temp);
			break;
		}
		case Rect:
		{
			RectangularObj *temp = (RectangularObj*)Globals::var().selectedObjectPtr;
			Globals::var().pastebinObjectPtr = new RectangularObj(*temp);
			break;
		}
		case Circle:
		{
			CircleObj *temp = (CircleObj*)Globals::var().selectedObjectPtr;
			Globals::var().pastebinObjectPtr = new CircleObj(*temp);
			break;
		}
		case Text:
		{
			TextObj *temp = (TextObj*)Globals::var().selectedObjectPtr;
			Globals::var().pastebinObjectPtr = new TextObj(*temp);
			break;
		}
		default:
			break;
		}
		break;
	}
	case ID_Cut:
	{
		if (Globals::var().selectedObjectPtr == nullptr)
			break;

		if (Globals::var().pastebinObjectPtr != nullptr)
			delete Globals::var().pastebinObjectPtr;

		xPasteDir = 0;  //init paste direction is upper left
		yPasteDir = 0;

		switch (Globals::var().selectedObjectPtr->objectType)
		{
		case Line:
		{
			LineObj *temp = (LineObj*)Globals::var().selectedObjectPtr;
			Globals::var().pastebinObjectPtr = new LineObj(*temp);
			break;
		}
		case Rect:
		{
			RectangularObj *temp = (RectangularObj*)Globals::var().selectedObjectPtr;
			Globals::var().pastebinObjectPtr = new RectangularObj(*temp);
			break;
		}
		case Circle:
		{
			CircleObj *temp = (CircleObj*)Globals::var().selectedObjectPtr;
			Globals::var().pastebinObjectPtr = new CircleObj(*temp);
			break;
		}
		case Text:
		{
			TextObj *temp = (TextObj*)Globals::var().selectedObjectPtr;
			Globals::var().pastebinObjectPtr = new TextObj(*temp);
			break;
		}
		default:
			break;
		}

		//if cut on newText, clear it.
		if (Globals::var().selectedObjectPtr == &Globals::var().newText)
		{
			Globals::var().newText.Clean();
			Globals::var().selectedObjectPtr = NULL;
			Globals::var().hasSelected = false;
			InvalidateRect(param.hWnd_, NULL, FALSE);
			break;
		}

		//find the position of the selected object in list
		auto it = std::find(Globals::var().drawObjList.begin(), Globals::var().drawObjList.end(), Globals::var().selectedObjectPtr);
		if (it != Globals::var().drawObjList.end())
		{
			int pos = distance(Globals::var().drawObjList.begin(), it);
			Globals::var().mlog.OP_del(Globals::var().selectedObjectPtr, pos);  //log the cut object (delete)
			delete Globals::var().selectedObjectPtr;
			Globals::var().hasSelected = false;
			Globals::var().selectedObjectPtr = nullptr;
			currentCursorMode = 0;
			Globals::var().drawObjList.erase(it);
			InvalidateRect(param.hWnd_, NULL, FALSE);
		}

		break;
	}
	case ID_Paste:
	{
		if (Globals::var().pastebinObjectPtr == nullptr)
			break;

		//adjust new paste position
		POINT p = MovePastedObj();
		AutoScroll(param.hWnd_, p.x - xCurrentScroll + 1, p.y - yCurrentScroll + 1, xCurrentScroll, yCurrentScroll, rect);

		//insert the pastbin object into list
		switch (Globals::var().pastebinObjectPtr->objectType)
		{
		case Line:
		{
			Globals::var().drawObjList.push_back(new LineObj(*(LineObj*)Globals::var().pastebinObjectPtr));
			break;
		}
		case Rect:
		{
			Globals::var().drawObjList.push_back(new RectangularObj(*(RectangularObj*)Globals::var().pastebinObjectPtr));
			break;
		}
		case Circle:
		{
			Globals::var().drawObjList.push_back(new CircleObj(*(CircleObj*)Globals::var().pastebinObjectPtr));
			break;
		}
		case Text:
		{
			Globals::var().drawObjList.push_back(new TextObj(*(TextObj*)Globals::var().pastebinObjectPtr));
			break;
		}
		default:
			break;
		}

		//select the new object
		Globals::var().selectedObjectPtr = Globals::var().drawObjList.back();
		Globals::var().hasSelected = true;

		//prepare undo logs
		Globals::var().mlog.OP_add(Globals::var().selectedObjectPtr);

		InvalidateRect(param.hWnd_, NULL, FALSE);
		break;
	}
	case ID_Undo:
	{
		Undo();
		InvalidateRect(param.hWnd_, NULL, FALSE);
		break;
	}
	case ID_Redo:
	{
		Redo();
		InvalidateRect(param.hWnd_, NULL, FALSE);
		break;
	}
	case ID_Delete:
	{

		if (Globals::var().selectedObjectPtr == nullptr)
			break;

		if (Globals::var().currentDrawMode == DrawTexts && Globals::var().newText.startFinished) //delete the newText
		{
			//record the log
			Globals::var().mlog.OP_del(&Globals::var().newText, -1);

			Globals::var().newText.Clean();

			//select the last object in list
			if (Globals::var().drawObjList.size() > 0)
			{
				Globals::var().selectedObjectPtr = Globals::var().drawObjList.back();
				FocusSelectedObject(Globals::var().selectedObjectPtr, param.hWnd_);
			}
			else
			{
				Globals::var().selectedObjectPtr = nullptr;
				Globals::var().hasSelected = false;
			}
			InvalidateRect(param.hWnd_, NULL, FALSE);
			break;
		}

		//find the position of the selected object in list
		auto it = std::find(Globals::var().drawObjList.begin(), Globals::var().drawObjList.end(), Globals::var().selectedObjectPtr);
		if (it != Globals::var().drawObjList.end())
		{
			//record the log
			int pos = distance(Globals::var().drawObjList.begin(), it);
			Globals::var().mlog.OP_del(*it, pos);

			//doing delete
			delete Globals::var().selectedObjectPtr;
			Globals::var().drawObjList.erase(it);

			//select the previous object
			it = Globals::var().drawObjList.begin();
			if (Globals::var().drawObjList.size() > 0 && *it != NULL)
			{
				if (pos > 0)
					advance(it, pos - 1);
				Globals::var().selectedObjectPtr = *it;
				FocusSelectedObject(Globals::var().selectedObjectPtr, param.hWnd_);
			}
			else
			{
				Globals::var().selectedObjectPtr = NULL;
				Globals::var().hasSelected = false;
			}
			currentCursorMode = 0;

			InvalidateRect(param.hWnd_, NULL, FALSE);
		}
		break;
	}
	case ID_DebugMode:
	{
		Globals::var().debugMode = !Globals::var().debugMode;
		InvalidateRect(param.hWnd_, NULL, FALSE);
		break;
	}
	default:
	{
		if (Globals::var().debugMode)
		{
			wsprintf(szBuffer, TEXT("Button ID %d : %d"), param.wParam_, param.lParam_);
			MessageBox(param.hWnd_, szBuffer, TEXT("Pressed"), MB_OK);
		}
		return DefWindowProc(param.hWnd_, param.message_, param.wParam_, param.lParam_);
	}
	}
	return 0;
}

LRESULT WM_CreateEvent(Parameter& param)
{
	// Load DLL file
	if (LoadLibrary(L"PaintDLL.dll") == NULL)
	{
		MessageBox(0, L"Can not load DLL file.", L"Error",MB_OK);
		return 0;
	}
	currentColor = 0;
	Globals::var().hasSelected = false;
	cxChar = LOWORD(GetDialogBaseUnits());
	cyChar = HIWORD(GetDialogBaseUnits());

	HBITMAP bmpIcon1, bmpIcon2, bmpIcon3, bmpIcon4, bmpIcon5, bmpIcon6, bmpIcon7, bmpIcon8, bmpIcon9;
	HBITMAP line1, line2, line3, line4, line5;
	HBITMAP checkedIcon;

	//image for child window buttons
	bmpIcon1 = (HBITMAP)LoadImage(NULL, L"black.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	bmpIcon2 = (HBITMAP)LoadImage(NULL, L"grey.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	bmpIcon3 = (HBITMAP)LoadImage(NULL, L"red.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	bmpIcon4 = (HBITMAP)LoadImage(NULL, L"green.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	bmpIcon5 = (HBITMAP)LoadImage(NULL, L"blu.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	bmpIcon6 = (HBITMAP)LoadImage(NULL, L"cyan.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	bmpIcon7 = (HBITMAP)LoadImage(NULL, L"yellow.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	bmpIcon8 = (HBITMAP)LoadImage(NULL, L"magenta.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	bmpIcon9 = (HBITMAP)LoadImage(NULL, L"transparent.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	checkedIcon = (HBITMAP)LoadImage(NULL, L"checkedIcon.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	line1 = (HBITMAP)LoadImage(NULL, L"wide1.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	line2 = (HBITMAP)LoadImage(NULL, L"wide2.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	line3 = (HBITMAP)LoadImage(NULL, L"wide3.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	line4 = (HBITMAP)LoadImage(NULL, L"wide4.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	line5 = (HBITMAP)LoadImage(NULL, L"wide5.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

	//add image to menu
	hMenu = GetMenu(param.hWnd_);
	if (hMenu)
	{
		HMENU hMenu2 = GetSubMenu(hMenu, 2);   //hMenu2 = File
		HMENU hMenu3 = GetSubMenu(hMenu2, 6);  //hMenu3 = Edit
		HMENU hMenu4 = GetSubMenu(hMenu2, 7);  //hMenu4 = Tool
		HMENU hMenu5 = GetSubMenu(hMenu2, 8);  //hMenu5 = Help

		ModifyMenu(hMenu3, 0, MF_BYPOSITION | MF_BITMAP, ID_BLACK, (LPCTSTR)bmpIcon1);
		ModifyMenu(hMenu3, 1, MF_BYPOSITION | MF_BITMAP, ID_GRAY, (LPCTSTR)bmpIcon2);
		ModifyMenu(hMenu3, 2, MF_BYPOSITION | MF_BITMAP, ID_RED, (LPCTSTR)bmpIcon3);
		ModifyMenu(hMenu3, 3, MF_BYPOSITION | MF_BITMAP, ID_GREEN, (LPCTSTR)bmpIcon4);
		ModifyMenu(hMenu3, 4, MF_BYPOSITION | MF_BITMAP, ID_BLU, (LPCTSTR)bmpIcon5);
		ModifyMenu(hMenu3, 5, MF_BYPOSITION | MF_BITMAP, ID_CYAN, (LPCTSTR)bmpIcon6);
		ModifyMenu(hMenu3, 6, MF_BYPOSITION | MF_BITMAP, ID_YELLOW, (LPCTSTR)bmpIcon7);
		ModifyMenu(hMenu3, 7, MF_BYPOSITION | MF_BITMAP, ID_Magenta, (LPCTSTR)bmpIcon8);
		ModifyMenu(hMenu5, 0, MF_BYPOSITION | MF_BITMAP, ID_BG_Transparent, (LPCTSTR)bmpIcon9);
		ModifyMenu(hMenu5, 1, MF_BYPOSITION | MF_BITMAP, ID_BG_GRAY, (LPCTSTR)bmpIcon2);
		ModifyMenu(hMenu5, 2, MF_BYPOSITION | MF_BITMAP, ID_BG_RED, (LPCTSTR)bmpIcon3);
		ModifyMenu(hMenu5, 3, MF_BYPOSITION | MF_BITMAP, ID_BG_GREEN, (LPCTSTR)bmpIcon4);
		ModifyMenu(hMenu5, 4, MF_BYPOSITION | MF_BITMAP, ID_BG_BLU, (LPCTSTR)bmpIcon5);
		ModifyMenu(hMenu5, 5, MF_BYPOSITION | MF_BITMAP, ID_BG_CYAN, (LPCTSTR)bmpIcon6);
		ModifyMenu(hMenu5, 6, MF_BYPOSITION | MF_BITMAP, ID_BG_YELLOW, (LPCTSTR)bmpIcon7);
		ModifyMenu(hMenu5, 7, MF_BYPOSITION | MF_BITMAP, ID_BG_Magenta, (LPCTSTR)bmpIcon8);
		ModifyMenu(hMenu4, 0, MF_BYPOSITION | MF_BITMAP, ID_Wide1, (LPCTSTR)line1);
		ModifyMenu(hMenu4, 1, MF_BYPOSITION | MF_BITMAP, ID_Wide2, (LPCTSTR)line2);
		ModifyMenu(hMenu4, 2, MF_BYPOSITION | MF_BITMAP, ID_Wide3, (LPCTSTR)line3);
		ModifyMenu(hMenu4, 3, MF_BYPOSITION | MF_BITMAP, ID_Wide4, (LPCTSTR)line4);
		ModifyMenu(hMenu4, 4, MF_BYPOSITION | MF_BITMAP, ID_Wide5, (LPCTSTR)line5);

		CheckMenuItem(hMenu3, ID_BLACK, MF_CHECKED);
		CheckMenuItem(hMenu4, ID_Wide1, MF_CHECKED);
		CheckMenuItem(hMenu5, ID_BG_Transparent, MF_CHECKED);
		for (int i = 0; i < 8; i++)
		{
			SetMenuItemBitmaps(hMenu3, i, MF_BYPOSITION, NULL, checkedIcon);
			SetMenuItemBitmaps(hMenu5, i, MF_BYPOSITION, NULL, checkedIcon);
			if (i < 5)
				SetMenuItemBitmaps(hMenu4, i, MF_BYPOSITION, NULL, checkedIcon);
		}
	}
	else
		MessageBox(param.hWnd_, 0, TEXT("NO MENU"), MB_OK);

	cursors[0] = LoadCursor(NULL, IDC_ARROW);     // 0 = default mouse
	cursors[1] = LoadCursor(NULL, IDC_SIZENWSE);  // 1 = size point start
	cursors[2] = LoadCursor(NULL, IDC_SIZENESW);  // 2 = size point end
	cursors[3] = LoadCursor(NULL, IDC_SIZEWE);    // 3 = size obj
	cursors[4] = LoadCursor(NULL, IDC_SIZENS);    // 4 = size end
	cursors[5] = LoadCursor(NULL, IDC_SIZEALL);   // 5 = size all
	currentCursor = &cursors[0];

	// Create a normal DC and a memory DC for the entire 
	// screen. The normal DC provides a snapshot of the 
	// screen contents. The memory DC keeps a copy of this 
	// snapshot in the associated bitmap. 
	hdcScreen = CreateDC(L"DISPLAY", (PCTSTR)NULL, (PCTSTR)NULL, (CONST DEVMODE *) NULL);

	// Retrieve the metrics for the bitmap associated with the 
	// regular device context.  bmp size=1920*1080
	bmp.bmBitsPixel = (BYTE)GetDeviceCaps(hdcScreen, BITSPIXEL);
	bmp.bmPlanes = (BYTE)GetDeviceCaps(hdcScreen, PLANES);
	bmp.bmWidth = 2000;//GetDeviceCaps(hdcScreen, HORZRES);
	bmp.bmHeight = 2000;//GetDeviceCaps(hdcScreen, VERTRES);

						// The width must be byte-aligned. 
	bmp.bmWidthBytes = ((bmp.bmWidth + 15) &~15) / 8;

	// Initialize the flags. 
	fBlt = FALSE;
	fScroll = FALSE;
	fSize = FALSE;

	// Initialize the horizontal scrolling variables. 
	xMinScroll = 0;
	xCurrentScroll = 0;
	xMaxScroll = 0;

	// Initialize the vertical scrolling variables. 
	yMinScroll = 0;
	yCurrentScroll = 0;
	yMaxScroll = 0;

	isMoving = false;
	isResizing = false;

	DeleteDC(hdcScreen);
	return 0;
}

LRESULT WM_MouseMoveEvent(Parameter& param)
{
	mouseX = GET_X_LPARAM(param.lParam_) + xCurrentScroll;
	mouseY = GET_Y_LPARAM(param.lParam_) + yCurrentScroll;

	if (Globals::var().currentDrawMode == DrawLine && currentCursorMode == 0 && mouseHasDown) //draw the line
	{
		if (!newLine.endFinished && newLine.startFinished)
		{
			newLine.ptEnd.x = mouseX;
			newLine.ptEnd.y = mouseY;
			if (newLine.ptEnd.x < 0)
				newLine.ptEnd.x = 0;
			else if (newLine.ptEnd.x > 2000)
				newLine.ptEnd.x = 2000;
			if (newLine.ptEnd.y < 0)
				newLine.ptEnd.y = 0;
			else if (newLine.ptEnd.y > 2000)
				newLine.ptEnd.y = 2000;

			AutoScrollObjectWhenDrawing(param.hWnd_, &newLine, xCurrentScroll, yCurrentScroll, rect);
			InvalidateRect(param.hWnd_, NULL, FALSE);
		}
	}
	else if (Globals::var().currentDrawMode == DrawRect && currentCursorMode == 0 && mouseHasDown)
	{
		if (!newRect.endFinished && newRect.startFinished)
		{
			newRect.ptEnd.x = mouseX;
			newRect.ptEnd.y = mouseY;
			if (newRect.ptEnd.x < 0)
				newRect.ptEnd.x = 0;
			else if (newRect.ptEnd.x > 2000)
				newRect.ptEnd.x = 2000;
			if (newRect.ptEnd.y < 0)
				newRect.ptEnd.y = 0;
			else if (newRect.ptEnd.y > 2000)
				newRect.ptEnd.y = 2000;

			AutoScrollObjectWhenDrawing(param.hWnd_, &newRect, xCurrentScroll, yCurrentScroll, rect);
			InvalidateRect(param.hWnd_, NULL, FALSE);
		}
	}
	else if (Globals::var().currentDrawMode == DrawCircle && currentCursorMode == 0 && mouseHasDown)
	{
		if (!newCircle.endFinished && newCircle.startFinished)
		{
			newCircle.ptEnd.x = mouseX;
			newCircle.ptEnd.y = mouseY;
			if (newCircle.ptEnd.x < 0)
				newCircle.ptEnd.x = 0;
			else if (newCircle.ptEnd.x > 2000)
				newCircle.ptEnd.x = 2000;
			if (newCircle.ptEnd.y < 0)
				newCircle.ptEnd.y = 0;
			else if (newCircle.ptEnd.y > 2000)
				newCircle.ptEnd.y = 2000;

			AutoScrollObjectWhenDrawing(param.hWnd_, &newCircle, xCurrentScroll, yCurrentScroll, rect);
			InvalidateRect(param.hWnd_, NULL, FALSE);
		}
	}
	else if (Globals::var().currentDrawMode == DrawTexts)  //text tool
	{
		//if mouse is not down on object, only change the mouse icon
		if (Globals::var().hasSelected && !mouseHasDown)
		{
			//draw a double arrow mouse if mouse is on the 8-points
			currentCursorMode = Globals::var().selectedObjectPtr->CheckMouseIsOnSizingOpint(mouseX, mouseY);

			//draw a moving arrow if mouse is on the object
			if (currentCursorMode == 0 && Globals::var().selectedObjectPtr->CheckObjectCollision(mouseX, mouseY))
			{
				currentCursor = &cursors[5];
				currentCursorMode = 9;
			}
			else
				currentCursor = &cursors[(currentCursorMode + 1) / 2];
		}
		else if (Globals::var().hasSelected && mouseHasDown)  //if mouse is down on object, then perform move/resize
		{
			if (currentCursorMode == 9)  //perform move
			{
				if (!isMoving)
				{
					//store the old position of ptBeg
					isMoving = true;
					Globals::var().mlog.OP_moveStart(&Globals::var().newText, -1);  //log the movement of newText
				}
				Globals::var().selectedObjectPtr->Moving(mouseX, mouseY);
				AutoScrollObject(param.hWnd_, Globals::var().selectedObjectPtr, xCurrentScroll, yCurrentScroll, rect);
				InvalidateRect(param.hWnd_, NULL, FALSE);
			}
			else if (currentCursorMode > 0 && currentCursorMode < 9)  //perform resize
			{
				if (!isResizing)  //log the old size values
				{
					isResizing = true;
					currentCursor = &cursors[5];
					Globals::var().mlog.OP_sizeStart(&Globals::var().newText, -1);  //log the resize of newText
				}

				if (Globals::var().selectedObjectPtr->objectType == Text)
				{
					TextObj * temp;
					temp = (TextObj*)Globals::var().selectedObjectPtr;
					temp->ResizingText(mouseX, mouseY, currentCursorMode);
					AutoScrollObject(param.hWnd_, Globals::var().selectedObjectPtr, xCurrentScroll, yCurrentScroll, rect);
				}
				else
				{
					Globals::var().selectedObjectPtr->Resizing(mouseX, mouseY, currentCursorMode);
					AutoScrollObject(param.hWnd_, Globals::var().selectedObjectPtr, xCurrentScroll, yCurrentScroll, rect);
				}
				InvalidateRect(param.hWnd_, NULL, FALSE);
			}
		}
	}
	else  //currentDrawMode == 4
	{
		if (!mouseHasDown && Globals::var().currentDrawMode == SelectTool)
		{
			//check mouse & DrawObjList have collision or not (from tail)
			Globals::var().preSelectObject = NULL;
			for (auto it = Globals::var().drawObjList.crbegin(); it != Globals::var().drawObjList.crend(); ++it)
			{
				if ((*it)->CheckObjectCollision(mouseX, mouseY))
				{
					Globals::var().preSelectObject = (*it);
					break;
				}
				Globals::var().preSelectObject = NULL;
			}
			InvalidateRect(param.hWnd_, NULL, FALSE);
		}

		//if mouse is not down on object, only change the mouse icon
		if (Globals::var().hasSelected && !mouseHasDown)
		{
			//draw a double arrow mouse if mouse is on the 8-points
			currentCursorMode = Globals::var().selectedObjectPtr->CheckMouseIsOnSizingOpint(mouseX, mouseY);

			//draw a moving arrow if mouse is on the object
			if (currentCursorMode == 0 && Globals::var().selectedObjectPtr->CheckObjectCollision(mouseX, mouseY))
			{
				currentCursor = &cursors[5];
				currentCursorMode = 9;
			}
			else
				currentCursor = &cursors[(currentCursorMode + 1) / 2];
		}
		else if (Globals::var().hasSelected && mouseHasDown)  //if mouse is down on object, then perform move/resize
		{
			if (currentCursorMode == 9)  //perform move
			{
				if (!isMoving)
				{
					//store the old position of ptBeg
					isMoving = true;
					currentCursor = &cursors[5];

					//find the position of the selected object in list
					auto it = find(Globals::var().drawObjList.begin(), Globals::var().drawObjList.end(), Globals::var().selectedObjectPtr);
					if (it != Globals::var().drawObjList.end())
					{
						int pos = distance(Globals::var().drawObjList.begin(), it);
						Globals::var().mlog.OP_moveStart(Globals::var().selectedObjectPtr, pos);
					}
				}
				Globals::var().selectedObjectPtr->Moving(mouseX, mouseY);
				AutoScrollObject(param.hWnd_, Globals::var().selectedObjectPtr, xCurrentScroll, yCurrentScroll, rect);
				InvalidateRect(param.hWnd_, NULL, FALSE);
			}
			else if (currentCursorMode > 0 && currentCursorMode < 9)  //perform resize
			{
				if (!isResizing)  //log the old size values
				{
					isResizing = true;
					auto it = find(Globals::var().drawObjList.begin(), Globals::var().drawObjList.end(), Globals::var().selectedObjectPtr);
					if (it != Globals::var().drawObjList.end())
					{
						int pos = distance(Globals::var().drawObjList.begin(), it);
						Globals::var().mlog.OP_sizeStart(Globals::var().selectedObjectPtr, pos);
					}
				}

				if (Globals::var().selectedObjectPtr->objectType == Text)
				{
					TextObj * temp;
					temp = (TextObj*)Globals::var().selectedObjectPtr;
					temp->ResizingText(mouseX, mouseY, currentCursorMode);
					AutoScrollObjectResize(param.hWnd_, Globals::var().selectedObjectPtr, xCurrentScroll, yCurrentScroll, rect);
				}
				else
				{
					Globals::var().selectedObjectPtr->Resizing(mouseX, mouseY, currentCursorMode);
					AutoScrollObjectResize(param.hWnd_, Globals::var().selectedObjectPtr, xCurrentScroll, yCurrentScroll, rect);
				}
				InvalidateRect(param.hWnd_, NULL, FALSE);
			}
		}
		else
		{	/*do nothing here*/
		}
	}

	SetCursor(*currentCursor);
	return 0;
}

LRESULT WM_LButtonDownEvent(Parameter& param)
{
	if (param.wParam_ && !mouseHasDown)  //if mouse L is down
	{
		SetCapture(param.hWnd_);  //capture mouse even outside window
		mouseX = GET_X_LPARAM(param.lParam_) + xCurrentScroll;
		mouseY = GET_Y_LPARAM(param.lParam_) + yCurrentScroll;
		if (Globals::var().currentDrawMode == DrawLine && currentCursorMode == 0)  //start drawing
		{
			newLine.MakeStart(mouseX, mouseY, currentColor, currentBgColor, currentLineWidth);
			Globals::var().modifyState = 1;
		}
		else if (Globals::var().currentDrawMode == DrawRect && currentCursorMode == 0)
		{
			newRect.MakeStart(mouseX, mouseY, currentColor, currentBgColor, currentLineWidth);
			Globals::var().modifyState = 1;
		}
		else if (Globals::var().currentDrawMode == DrawCircle && currentCursorMode == 0)
		{
			newCircle.MakeStart(mouseX, mouseY, currentColor, currentBgColor, currentLineWidth);
			Globals::var().modifyState = 1;
		}
		else if (Globals::var().currentDrawMode == DrawTexts && currentCursorMode == 0)  //make a new text or move old text
		{
			//if mouse is on the 8 sizing point, start to resize
			if (currentCursorMode != 0)
			{
				mouseHasDown = true;
				Globals::var().selectedObjectPtr->StartToMove(mouseX, mouseY);
				return 0;
			}

			if (!Globals::var().newText.startFinished) //click to a new text position without previous start
			{
				Globals::var().newText.MakeStart(mouseX, mouseY, currentColor, currentBgColor, currentLineWidth);
				Globals::var().newText.ptEnd.x = Globals::var().newText.ptBeg.x + 8 * 5 + 1;
				Globals::var().newText.ptEnd.y = Globals::var().newText.ptBeg.y + 1 * 13 + 1;
			}
			else if (Globals::var().newText.startFinished && !Globals::var().newText.endFinished)  //push old text to drawObj list
			{
				PushCurrentNewText(Globals::var().newText);
				Globals::var().newText.MakeStart(mouseX, mouseY, currentColor, currentBgColor, currentLineWidth);
				Globals::var().newText.ptEnd.x = Globals::var().newText.ptBeg.x + 8 * 5 + 1;
				Globals::var().newText.ptEnd.y = Globals::var().newText.ptBeg.y + 1 * 13 + 1;
			}
			if (Globals::var().newText.ptBeg.y > 1982)  //check y is too low
			{
				Globals::var().newText.ptBeg.y = 1982;
				Globals::var().newText.ptEnd.y = Globals::var().newText.ptBeg.y + 1 * 13 + 1;
			}
			Globals::var().modifyState = 1;
			Globals::var().selectedObjectPtr = &Globals::var().newText;
			Globals::var().hasSelected = true;
		}
		else  //mode = 4, selection tool
		{
			bool b = false;

			//if mouse is on the 8 sizing point, start to resize
			if (currentCursorMode != 0 && Globals::var().hasSelected)
			{
				mouseHasDown = true;
				Globals::var().selectedObjectPtr->StartToMove(mouseX, mouseY);
				return 0;
			}

			//if no sizing or moving, detect a collision and select an object
			//check mouse & DrawObjList have collision or not (from tail)		
			mouseHasDown = false;
			for (auto it = Globals::var().drawObjList.crbegin(); it != Globals::var().drawObjList.crend(); ++it)
			{
				if ((*it)->CheckObjectCollision(mouseX, mouseY))
				{
					Globals::var().selectedObjectPtr = (*it);
					Globals::var().selectedObjectPtr->StartToMove(mouseX, mouseY);  //user may want to move when clicked on object
					Globals::var().hasSelected = true;
					mouseHasDown = true;
					currentCursorMode = 9;
					b = true;
					break;
				}
			}

			//if no operation, de-select everything
			if (!b)
			{
				Globals::var().selectedObjectPtr = nullptr;
				Globals::var().hasSelected = false;
			}

			//draw a selected rectangle			
			InvalidateRect(param.hWnd_, NULL, FALSE);
			return 0;
		}
		InvalidateRect(param.hWnd_, NULL, FALSE);
		mouseHasDown = true;
	}
	return 0;
}

LRESULT WM_LButtonUpEvent(Parameter& param)
{
	if (!param.wParam_)
	{
		ReleaseCapture();  //stop capture mouse
		if (Globals::var().currentDrawMode == DrawLine && currentCursorMode == 0 && mouseHasDown)  //finish draw line
		{
			newLine.MakeEnd(GET_X_LPARAM(param.lParam_), GET_Y_LPARAM(param.lParam_), xCurrentScroll, yCurrentScroll);

			//if line too short...
			int distancex = (newLine.ptEnd.x - newLine.ptBeg.x) * (newLine.ptEnd.x - newLine.ptBeg.x);
			int distancey = (newLine.ptEnd.y - newLine.ptBeg.y) * (newLine.ptEnd.y - newLine.ptBeg.y);
			int distance = sqrt(abs(distancex - distancey));
			if (distance < 3)
			{
				newLine.Clean();
				mouseHasDown = false;
				InvalidateRect(param.hWnd_, NULL, FALSE);
				return 0;
			}

			Globals::var().drawObjList.push_back(new LineObj(newLine));
			Globals::var().mlog.OP_add(&newLine);
			Globals::var().selectedObjectPtr = Globals::var().drawObjList.back();
			Globals::var().hasSelected = true;

			newLine.Clean();
		}
		else if (Globals::var().currentDrawMode == DrawRect && currentCursorMode == 0 && mouseHasDown)  //draw rect
		{
			newRect.MakeEnd(GET_X_LPARAM(param.lParam_), GET_Y_LPARAM(param.lParam_), xCurrentScroll, yCurrentScroll);

			//if rect is too small...
			if (abs(newRect.ptEnd.x - newRect.ptBeg.x) < 4 && abs(newRect.ptEnd.y - newRect.ptBeg.y) < 4)
			{
				newRect.Clean();
				mouseHasDown = false;
				InvalidateRect(param.hWnd_, NULL, FALSE);
				return 0;
			}

			Globals::var().drawObjList.push_back(new RectangularObj(newRect));
			Globals::var().mlog.OP_add(&newRect);
			Globals::var().selectedObjectPtr = Globals::var().drawObjList.back();
			Globals::var().hasSelected = true;

			newRect.Clean();
		}
		else if (Globals::var().currentDrawMode == DrawCircle && currentCursorMode == 0 && mouseHasDown)  //draw circle
		{
			newCircle.MakeEnd(GET_X_LPARAM(param.lParam_), GET_Y_LPARAM(param.lParam_), xCurrentScroll, yCurrentScroll);

			//if rect is too small...
			if (abs(newCircle.ptEnd.x - newCircle.ptBeg.x) < 4 && abs(newCircle.ptEnd.y - newCircle.ptBeg.y) < 4)
			{
				newCircle.Clean();
				mouseHasDown = false;
				InvalidateRect(param.hWnd_, NULL, FALSE);
				return 0;
			}
			Globals::var().drawObjList.push_back(new CircleObj(newCircle));
			Globals::var().mlog.OP_add(&newCircle);
			Globals::var().selectedObjectPtr = Globals::var().drawObjList.back();
			Globals::var().hasSelected = true;

			newCircle.Clean();
		}
		else if (Globals::var().currentDrawMode == DrawTexts)  //only stop moving text
		{
			currentCursorMode = 0;

			//disable the moving flag
			if (isMoving)
			{
				isMoving = false;
				//record the new position of ptBeg
				Globals::var().mlog.OP_moveEnd(&Globals::var().newText);
			}

			if (isResizing)
			{
				isResizing = false;
				Globals::var().mlog.OP_sizeEnd(&Globals::var().newText);
			}
		}
		else  //stop moving/resizing
		{
			//stop resizing
			currentCursorMode = 0;

			//disable the moving flag
			if (isMoving)
			{
				isMoving = false;
				//record the new position of ptBeg
				Globals::var().mlog.OP_moveEnd(Globals::var().selectedObjectPtr);
			}

			if (isResizing)
			{
				isResizing = false;
				Globals::var().mlog.OP_sizeEnd(Globals::var().selectedObjectPtr);
			}
		}
		InvalidateRect(param.hWnd_, NULL, FALSE);
		mouseHasDown = false;
	}
	return 0;
}

LRESULT WM_KeyDownEvent(Parameter& param)
{
	if (param.wParam_)
	{
		//filter the allowed keys: a~z, 0~9, space/arrors/home/end/backspace/enter
		if ((param.wParam_ >= 65 && param.wParam_ <= 90) ||
			(param.wParam_ >= 48 && param.wParam_ <= 57) ||
			(param.wParam_ >= 0x60 && param.wParam_ <= 0x69) ||
			(param.wParam_ == 0x20) ||
			param.wParam_ == 0x0D ||
			param.wParam_ == 0x08 ||
			param.wParam_ == VK_HOME ||
			param.wParam_ == VK_END ||
			(param.wParam_ >= VK_LEFT && param.wParam_ <= VK_DOWN))
		{
			//continue;
		}
		else
			return 0;

		bool modified = false;
		if ((Globals::var().currentDrawMode == DrawTexts && Globals::var().newText.startFinished))  //key into newText
		{
			Globals::var().mlog.OP_textStart(&Globals::var().newText, -1); //make a log
			modified = Globals::var().newText.KeyIn(param.wParam_);

			int x, y;  //x and y is current caret position on window
			Globals::var().newText.CalculateCaretPosition();
			x = Globals::var().newText.caretPos.x - xCurrentScroll;
			y = Globals::var().newText.caretPos.y - yCurrentScroll;
			mouseX = Globals::var().newText.caretPos.x;
			mouseY = Globals::var().newText.caretPos.y;
			AutoScroll(param.hWnd_, x, y + 14, xCurrentScroll, yCurrentScroll, rect);
			InvalidateRect(param.hWnd_, NULL, FALSE);

			if (modified)  //make change to log
				Globals::var().mlog.OP_textEnd(&Globals::var().newText);
		}
		else if (Globals::var().currentDrawMode == SelectTool && Globals::var().selectedObjectPtr != nullptr && Globals::var().selectedObjectPtr->objectType == 4)
		{
			TextObj * temp;
			temp = (TextObj*)Globals::var().selectedObjectPtr;

			auto it = find(Globals::var().drawObjList.begin(), Globals::var().drawObjList.end(), Globals::var().selectedObjectPtr);
			if (it != Globals::var().drawObjList.end())
			{
				int pos = distance(Globals::var().drawObjList.begin(), it);
				Globals::var().mlog.OP_textStart(Globals::var().selectedObjectPtr, pos);  //make a log

				modified = temp->KeyIn(param.wParam_);

				int x, y;  //x and y is current caret position on window
				temp->CalculateCaretPosition();
				x = temp->caretPos.x - xCurrentScroll;
				y = temp->caretPos.y - yCurrentScroll;
				mouseX = temp->caretPos.x;
				mouseY = temp->caretPos.y;
				AutoScroll(param.hWnd_, x, y + 14, xCurrentScroll, yCurrentScroll, rect);
				InvalidateRect(param.hWnd_, NULL, FALSE);
			}

			if (modified)  //make change to log
				Globals::var().mlog.OP_textEnd(Globals::var().selectedObjectPtr);
		}
	}
	return 0;
}

LRESULT WM_PaintEvent(Parameter& param)
{
	PAINTSTRUCT ps;
	RECT clientRec = rect;
	HDC hdc = BeginPaint(param.hWnd_, &ps);  //this will return display device id


	GetClientRect(param.hWnd_, &clientRec);
	HDC memoryDC = CreateCompatibleDC(hdc);
	hBmp = CreateCompatibleBitmap(hdc, clientRec.right, clientRec.bottom);  // Create a bitmap big enough for our client rectangle.

																			// Select the bitmap into the off-screen DC.
	SelectObject(memoryDC, hBmp);

	// Erase the background.
	HBRUSH hbrBkGnd;
	hbrBkGnd = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
	FillRect(memoryDC, &clientRec, hbrBkGnd);
	DeleteObject(hbrBkGnd);

	if (Globals::var().modifyState == 1) //add a * at file name
	{
		string title = Globals::var().fileName;
		title.append("*");
		SetTitle(title, param.hWnd_);
	}
	else  //remove last * at file name
	{
		SetTitle(Globals::var().fileName, param.hWnd_);
	}

	//---------------------------------------------------------------


	string s2 = "";  //string for debug
	for (auto& it : Globals::var().drawObjList)  //Draw each object in DrawObjList
	{
		it->Paint(memoryDC, xCurrentScroll, yCurrentScroll);
	}

	//draw the new objects (if any)
	newLine.Paint(memoryDC, xCurrentScroll, yCurrentScroll);
	newRect.Paint(memoryDC, xCurrentScroll, yCurrentScroll);
	newCircle.Paint(memoryDC, xCurrentScroll, yCurrentScroll);
	Globals::var().newText.Paint(memoryDC, xCurrentScroll, yCurrentScroll);

	if (mouseHasDown)  //paint the drawing rectangle based on object type
	{
		switch (Globals::var().currentDrawMode)
		{
		case 0:
			newLine.PaintMouseOnRect(memoryDC, xCurrentScroll, yCurrentScroll);
			break;
		case 1:
			newRect.PaintMouseOnRect(memoryDC, xCurrentScroll, yCurrentScroll);
			break;
		case 2:
			newCircle.PaintMouseOnRect(memoryDC, xCurrentScroll, yCurrentScroll);
			break;
		default:
			break;
		}
	}

	if (Globals::var().preSelectObject != nullptr)  //paint the pre-select rectangle
	{
		Globals::var().preSelectObject->PaintMouseOnRect(memoryDC, xCurrentScroll, yCurrentScroll);
		if (Globals::var().debugMode)
		{
			s2 = "paint the pre-select rectangle";
			TextOutA(memoryDC, 700 - xCurrentScroll, 600 - yCurrentScroll, s2.c_str(), s2.length());
		}
	}

	if (Globals::var().hasSelected)  //paint the selected rectangle
		Globals::var().selectedObjectPtr->PaintSelectedRect(memoryDC, xCurrentScroll, yCurrentScroll);

	if (Globals::var().currentDrawMode == DrawTexts && Globals::var().newText.startFinished && !Globals::var().newText.endFinished)
	{
		// Create a solid black caret on newText. 
		CreateCaret(param.hWnd_, (HBITMAP)NULL, 3, 14);
		SetCaretPos(Globals::var().newText.caretPos.x - xCurrentScroll, Globals::var().newText.caretPos.y - yCurrentScroll);
		if (Globals::var().debugMode)
		{
			s2 = "caretPos=" + to_string(Globals::var().newText.caretPos.x) + ", " + to_string(Globals::var().newText.caretPos.y);
			TextOutA(memoryDC, 500 - xCurrentScroll, 500 - yCurrentScroll, s2.c_str(), s2.length());
		}
	}
	else if (Globals::var().hasSelected && Globals::var().currentDrawMode == SelectTool && Globals::var().selectedObjectPtr->objectType == Text)
	{
		//caret is on selected text object
		TextObj* t = (TextObj *)Globals::var().selectedObjectPtr;
		CreateCaret(param.hWnd_, (HBITMAP)NULL, 3, 14);
		SetCaretPos(t->caretPos.x - xCurrentScroll, t->caretPos.y - yCurrentScroll);
	}
	else
		DestroyCaret();

	if (Globals::var().debugMode)
	{
		s2 = "xCurrentScroll=" + to_string(xCurrentScroll) + " yCurrentScroll=" + to_string(yCurrentScroll);
		TextOutA(memoryDC, 700 - xCurrentScroll, 640 - yCurrentScroll, s2.c_str(), s2.length());
		s2 = "mousex=" + to_string(mouseX) + " mousey=" + to_string(mouseY);
		TextOutA(memoryDC, 700 - xCurrentScroll, 620 - yCurrentScroll, s2.c_str(), s2.length());
		s2 = "currentDrawMode = " + to_string(Globals::var().currentDrawMode);
		TextOutA(memoryDC, 700 - xCurrentScroll, 660 - yCurrentScroll, s2.c_str(), s2.length());
		s2 = "object count: " + to_string(Globals::var().drawObjList.size());
		TextOutA(memoryDC, 700 - xCurrentScroll, 680 - yCurrentScroll, s2.c_str(), s2.length());
	}


	//---------------------------------------------------------------
	// Blt the changes to the screen DC.
	BitBlt(hdc,
		clientRec.left, clientRec.top,
		clientRec.right - clientRec.left, clientRec.bottom - clientRec.top,
		memoryDC,
		0, 0,
		SRCCOPY);

	DeleteDC(memoryDC);  //release a memory DC
	DeleteObject(hBmp);
	EndPaint(param.hWnd_, &ps);

	return 0;
}

LRESULT WM_SizeEvent(Parameter& param)
{
	rect.left = 0;
	rect.top = 0;
	rect.right = LOWORD(param.lParam_);
	rect.bottom = HIWORD(param.lParam_);

	int xNewSize;
	int yNewSize;
	xNewSize = LOWORD(param.lParam_);
	yNewSize = HIWORD(param.lParam_);

	if (fBlt)
		fSize = TRUE;

	// The horizontal scrolling range is defined by 
	// (bitmap_width) - (client_width). The current horizontal 
	// scroll value remains within the horizontal scrolling range. 
	xMaxScroll = max(bmp.bmWidth - xNewSize, 0);
	xCurrentScroll = min(xCurrentScroll, xMaxScroll);
	si.cbSize = sizeof(si);
	si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
	si.nMin = xMinScroll;
	si.nMax = bmp.bmWidth;
	si.nPage = xNewSize;
	si.nPos = xCurrentScroll;
	SetScrollInfo(param.hWnd_, SB_HORZ, &si, TRUE);

	// The vertical scrolling range is defined by 
	// (bitmap_height) - (client_height). The current vertical 
	// scroll value remains within the vertical scrolling range. 
	if (hScrollResize < 5)
		hScrollResize++;
	if (hScrollResize != 2) //ignore for 2nd time. still don't know why??
	{
		yMaxScroll = max(bmp.bmHeight - yNewSize, 0);
	}
	yCurrentScroll = min(yCurrentScroll, yMaxScroll);
	si.cbSize = sizeof(si);
	si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
	si.nMin = yMinScroll;
	si.nMax = bmp.bmHeight;
	si.nPage = yNewSize;
	si.nPos = yCurrentScroll;
	SetScrollInfo(param.hWnd_, SB_VERT, &si, TRUE);

	if (Globals::var().myChildWindow)
		SetWindowPos(Globals::var().myChildWindow, NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	InvalidateRect(param.hWnd_, NULL, false);
	return 0;
}

LRESULT WM_HScrollEvent(Parameter& param)
{
	int xDelta;     // xDelta = new_pos - current_pos  
	int xNewPos;    // new position 
	int yDelta = 0;

	switch (LOWORD(param.wParam_))
	{
		// User clicked the scroll bar shaft left of the scroll box. 
	case SB_PAGEUP:
		xNewPos = xCurrentScroll - 50;
		break;

		// User clicked the scroll bar shaft right of the scroll box. 
	case SB_PAGEDOWN:
		xNewPos = xCurrentScroll + 50;
		break;

		// User clicked the left arrow. 
	case SB_LINEUP:
		xNewPos = xCurrentScroll - 5;
		break;

		// User clicked the right arrow. 
	case SB_LINEDOWN:
		xNewPos = xCurrentScroll + 5;
		break;

		// User dragged the scroll box. 
	case SB_THUMBTRACK:
		xNewPos = HIWORD(param.wParam_);
		break;

	default:
		xNewPos = xCurrentScroll;
	}

	// New position must be between 0 and the screen width. 
	xNewPos = max(0, xNewPos);
	xNewPos = min(xMaxScroll, xNewPos);

	// If the current position does not change, do not scroll.
	if (xNewPos == xCurrentScroll)
		return 0;

	// Set the scroll flag to TRUE. 
	fScroll = TRUE;

	// Determine the amount scrolled (in pixels).
	xDelta = xNewPos - xCurrentScroll;

	// Reset the current scroll position. 
	xCurrentScroll = xNewPos;

	// Scroll the window. (The system repaints most of the 
	// client area when ScrollWindowEx is called; however, it is 
	// necessary to call UpdateWindow in order to repaint the 
	// rectangle of pixels that were invalidated.) 
	if (!Globals::var().autoScrolling)
	{
		ScrollWindowEx(param.hWnd_, -xDelta, -yDelta, (CONST RECT *) NULL,
			(CONST RECT *) NULL, (HRGN)NULL, (PRECT)NULL,
			SW_INVALIDATE);
		UpdateWindow(param.hWnd_);
	}

	// Reset the scroll bar. 
	si.cbSize = sizeof(si);
	si.fMask = SIF_POS;
	si.nPos = xCurrentScroll;
	SetScrollInfo(param.hWnd_, SB_HORZ, &si, TRUE);
	return 0;
}

LRESULT WM_VScrollEvent(Parameter& param)
{
	int xDelta = 0;
	int yDelta;     // yDelta = new_pos - current_pos 
	int yNewPos;    // new position 

	switch (LOWORD(param.wParam_))
	{
		// User clicked the scroll bar shaft above the scroll box. 
	case SB_PAGEUP:
		yNewPos = yCurrentScroll - 50;
		break;

		// User clicked the scroll bar shaft below the scroll box. 
	case SB_PAGEDOWN:
		yNewPos = yCurrentScroll + 50;
		break;

		// User clicked the top arrow. 
	case SB_LINEUP:
		yNewPos = yCurrentScroll - 5;
		break;

		// User clicked the bottom arrow. 
	case SB_LINEDOWN:
		yNewPos = yCurrentScroll + 5;
		break;

		// User dragged the scroll box. 
	case SB_THUMBTRACK:
		yNewPos = HIWORD(param.wParam_);
		if (yNewPos == 1004)
			yNewPos = yMaxScroll;  //workaround for Y scroll to bottom
		break;

	default:
		yNewPos = yCurrentScroll;
	}

	//if (yNewPos > yCurrentScroll)
	//	yNewPos = yCurrentScroll + 10;
	//else if (yNewPos < yCurrentScroll)
	//	yNewPos = yCurrentScroll - 10;

	// New position must be between 0 and the screen height. 
	yNewPos = max(0, yNewPos);
	yNewPos = min(yMaxScroll, yNewPos);

	// If the current position does not change, do not scroll.
	if (yNewPos == yCurrentScroll)
		return 0;

	// Set the scroll flag to TRUE. 
	fScroll = TRUE;

	// Determine the amount scrolled (in pixels). 
	yDelta = yNewPos - yCurrentScroll;

	// Reset the current scroll position. 
	yCurrentScroll = yNewPos;

	// Scroll the window. (The system repaints most of the 
	// client area when ScrollWindowEx is called; however, it is 
	// necessary to call UpdateWindow in order to repaint the 
	// rectangle of pixels that were invalidated.) 
	if (!Globals::var().autoScrolling)
	{
		ScrollWindowEx(param.hWnd_, -xDelta, -yDelta, (CONST RECT *) NULL,  // <-Don't use this! Just call InvalidateRect
			NULL, (HRGN)NULL, (PRECT)NULL,       //   and redraw the all area.
			SW_INVALIDATE);
		UpdateWindow(param.hWnd_);
	}

	// Reset the scroll bar. 
	si.cbSize = sizeof(si);
	si.fMask = SIF_POS;
	si.nPos = yCurrentScroll;
	SetScrollInfo(param.hWnd_, SB_VERT, &si, TRUE);
	return 0;
}

LRESULT WM_CloseEvent(Parameter& param)
{
	if (Globals::var().modifyState == 1)  //show the save file dialog
	{
		if (DisplayConfirmNewFileMessageBox(Globals::var().fileName) == IDYES)
		{
			PushCurrentNewText(Globals::var().newText);
			if (Globals::var().lastFilePath.size() < 1)
			{
				SaveToFile(Globals::var().drawObjList, Globals::var().fileName);
				SetTitle(Globals::var().fileName, param.hWnd_);
				Globals::var().modifyState = 2;
			}
			else
			{
				SaveToLastFilePath(Globals::var().drawObjList);
				Globals::var().modifyState = 2;
			}
		}
	}
	DestroyWindow(param.hWnd_);
	return 0;
}

LRESULT WM_DestroyEvent(Parameter& param)
{
	PostQuitMessage(0);
	return 0;
}

LRESULT WM_SetCursorEvent(Parameter& param)
{
	return 0;  //handle this message to prevent mouse icon turn back
}

LRESULT WM_GetMinMaxInfo(Parameter & param)  //set the min size of window
{
	LPMINMAXINFO lpMMI = (LPMINMAXINFO)param.lParam_;
	lpMMI->ptMinTrackSize.x = 77;
	lpMMI->ptMinTrackSize.y = 415;
	return 0;
}

// [關於] 方塊的訊息處理常式。
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

POINT MovePastedObj()  //move the pasted object by 30
{
	int xdir, ydir;
	if (xPasteDir == 0)
	{
		xdir = -30;
	}
	else
	{
		xdir = 30;
	}

	if (yPasteDir == 0)
	{
		ydir = -30;
	}
	else
	{
		ydir = 30;
	}

	POINT newBeg = Globals::var().pastebinObjectPtr->ptBeg;
	POINT newEnd = Globals::var().pastebinObjectPtr->ptEnd;
	newBeg.x += xdir;
	newBeg.y += ydir;
	newEnd.x += xdir;
	newEnd.y += ydir;

	if (newBeg.x < 0)   //change direction if hit the borders
	{
		int delta = 0 - newBeg.x;
		newBeg.x = 0;
		newEnd.x += delta;
		xPasteDir = 1;
	}
	if (newBeg.y < 0)
	{
		int delta = 0 - newBeg.y;
		newBeg.y = 0;
		newEnd.y += delta;
		yPasteDir = 1;
	}

	if (newBeg.x > 1990)
	{
		int delta = 1990 - newBeg.x;
		newBeg.x = 1990;
		newEnd.x += delta;
		xPasteDir = 0;
	}
	if (newBeg.y > 1989)
	{
		int delta = 1990 - newBeg.y;
		newBeg.y = 1990;
		newEnd.y += delta;
		yPasteDir = 0;
	}
	//dealing with newEnd
	if (newEnd.x < 0)
	{
		int delta = 0 - newEnd.x;
		newEnd.x = 0;
		newBeg.x += delta;
		xPasteDir = 1;
	}
	if (newEnd.y < 0)
	{
		int delta = 0 - newEnd.y;
		newEnd.y = 0;
		newBeg.y += delta;
		yPasteDir = 1;
	}

	if (newEnd.x > 1990)
	{
		int delta = 1990 - newEnd.x;
		newEnd.x = 1990;
		newBeg.x += delta;
		xPasteDir = 0;
	}
	if (newEnd.y > 1989)
	{
		int delta = 1990 - newEnd.y;
		newEnd.y = 1990;
		newBeg.y += delta;
		yPasteDir = 0;
	}

	Globals::var().pastebinObjectPtr->ptBeg = newBeg;
	Globals::var().pastebinObjectPtr->ptEnd = newEnd;

	int xfocus, yfocus;
	if (xPasteDir == 0)  //get the focus point according to pasting directions
		xfocus = newBeg.x < newEnd.x ? newBeg.x : newEnd.x;
	else
		xfocus = newBeg.x > newEnd.x ? newBeg.x : newEnd.x;

	if (yPasteDir == 0)
		yfocus = newBeg.y < newEnd.y ? newBeg.y : newEnd.y;
	else
		yfocus = newBeg.y > newEnd.y ? newBeg.y : newEnd.y;

	POINT r;
	r.x = xfocus;
	r.y = yfocus;
	return r;
}

void PushCurrentNewText(TextObj& T)
{
	if (T.text.size() > 0)
	{
		T.endFinished = true;
		Globals::var().drawObjList.push_back(new TextObj(T));
		Globals::var().mlog.OP_add(&T);
	}
	T.Clean();
	Globals::var().selectedObjectPtr = nullptr;
	Globals::var().hasSelected = false;
}

void Undo()
{
	HMENU hMenu2 = GetSubMenu(hMenu, 1);
	EnableMenuItem(hMenu2, ID_Redo, MF_BYCOMMAND | MF_ENABLED);  //toggle undo/redo buttons
	EnableMenuItem(hMenu2, ID_Undo, MF_BYCOMMAND | MF_GRAYED);
	Globals::var().mlog.Undo();
}

void Redo()
{
	HMENU hMenu2 = GetSubMenu(hMenu, 1);
	EnableMenuItem(hMenu2, ID_Redo, MF_BYCOMMAND | MF_GRAYED);  //toggle undo/redo buttons
	EnableMenuItem(hMenu2, ID_Undo, MF_BYCOMMAND | MF_ENABLED);
	Globals::var().mlog.Redo();
}

//update newText for undo/redo operation
void UpdateNewText(vector<string> vs, POINT in)
{
	Globals::var().newText.text = vs;
	Globals::var().newText.inputPos = in;
	Globals::var().newText.CalculateCaretPosition();
}

void ToggleUndoButton()
{
	HMENU hMenu2 = GetSubMenu(hMenu, 1);
	EnableMenuItem(hMenu2, ID_Redo, MF_BYCOMMAND | MF_GRAYED);
	EnableMenuItem(hMenu2, ID_Undo, MF_BYCOMMAND | MF_ENABLED);
	Globals::var().modifyState = 1;
}

//scroll the screen if center of p is out of screen
void FocusSelectedObject(DrawObj* p, HWND hwnd)
{
	if (!Globals::var().hasSelected || p == nullptr)
		return;

	//check if center out of screen
	int xcenter = (p->ptBeg.x + p->ptEnd.x) / 2 - xCurrentScroll;
	int ycenter = (p->ptBeg.y + p->ptEnd.y) / 2 - yCurrentScroll;
	AutoScroll(hwnd, xcenter, ycenter, xCurrentScroll, yCurrentScroll, rect);
}