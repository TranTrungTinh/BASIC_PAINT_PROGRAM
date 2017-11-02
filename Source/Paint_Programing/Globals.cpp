#include "stdafx.h"
#include "Globals.h"

Globals::Globals()
{
	fileName = "My Paint";
	currentDrawMode = 0;
	modifyState = 0;
	selectedObjectPtr = NULL;
	preSelectObject = NULL;
	mouseHasDown = false;
	wcsncpy_s(szChildClass, L"123", 3);  //name for child window class. cannot be null !
	debugMode = false;
}

Globals & Globals::var()
{
	
	static Globals g;
	return g;
}
