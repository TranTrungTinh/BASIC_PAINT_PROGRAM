#include "stdafx.h"
#include "DrawObj.h"

DrawObj::DrawObj()
{
	color = 0;
	objectType = 0;
	startFinished = false;
	endFinished = false;
	backgroundColor = 0;
	lineWidth = 1;
}

void DrawObj::Clean()
{
	ptBeg.x = 0;
	ptBeg.y = 0;
	ptEnd.x = 0;
	ptEnd.y = 0;
	startFinished = false;
	endFinished = false;
	color = 0;
}

//DrawObj::~DrawObj() { }

void DrawObj::PaintMouseOnRect(HDC hdc, int Xoffset, int Yoffset)
{
	//prevent the object being deleted by openfile... etc.
	try
	{
		if (ptBeg.x == ptEnd.x && ptBeg.y == ptEnd.y)
			return;
	}
	catch (...)  //in case I can catch access violation exception...
	{
		return;
	}


	HPEN hpen, hpenOld;
	HGDIOBJ oldBrush = SelectObject(hdc, GetStockObject(NULL_BRUSH));
	hpen = CreatePen(PS_DOT, 1, RGB(0, 0, 0));
	hpenOld = (HPEN)SelectObject(hdc, hpen);
	SetBkColor(hdc, RGB(255, 255, 255));  //set the pen background
	SetBkMode(hdc, OPAQUE);  //set the pen BG mode to overwrite current background

							 // do something...
	int top = (ptBeg.y < ptEnd.y ? ptBeg.y : ptEnd.y) - Yoffset;
	int left = (ptBeg.x < ptEnd.x ? ptBeg.x : ptEnd.x) - Xoffset;
	int buttom = (ptBeg.y > ptEnd.y ? ptBeg.y : ptEnd.y) - Yoffset;
	int right = (ptBeg.x > ptEnd.x ? ptBeg.x : ptEnd.x) - Xoffset;
	Rectangle(hdc, left, top, right, buttom);

	//return the pen
	SelectObject(hdc, hpenOld);
	DeleteObject(hpen);
	SelectObject(hdc, oldBrush);
	DeleteObject(oldBrush);
	DeleteObject(hpenOld);
}

void DrawObj::MakeStart(int x, int y, int currentColor, int currentBgColor, int currentLineWidth)  //x and y is absolute position on background
{
	ptBeg.x = x;
	ptBeg.y = y;
	ptEnd.x = x;
	ptEnd.y = y;
	startFinished = true;
	endFinished = false;
	color = currentColor;
	backgroundColor = currentBgColor;
	lineWidth = currentLineWidth;
}

void DrawObj::MakeEnd(int x, int y, int xCurrentScroll, int yCurrentScroll)  //x and y is related position
{
	ptEnd.x = x + xCurrentScroll;
	ptEnd.y = y + yCurrentScroll;
	endFinished = true;
}

int DrawObj::CheckMouseIsOnSizingOpint(int mouseX, int mouseY)
{
	int top = (ptBeg.y < ptEnd.y ? ptBeg.y : ptEnd.y);
	int left = (ptBeg.x < ptEnd.x ? ptBeg.x : ptEnd.x);
	int buttom = (ptBeg.y > ptEnd.y ? ptBeg.y : ptEnd.y);
	int right = (ptBeg.x > ptEnd.x ? ptBeg.x : ptEnd.x);

	if (mouseY < top - 4 || mouseY> buttom + 4 || mouseX < left - 4 || mouseX > right + 4)  //first if mouse out of range, just return false
		return 0;

	if (mouseY >= top - 4 && mouseY <= top + 1 && mouseX >= left - 4 && mouseX <= left + 1)  //左上
		return 1;
	else if (mouseX >= right - 1 && mouseX <= right + 4 && mouseY >= buttom - 1 && mouseY <= buttom + 4)  //右下
		return 2;
	else if (mouseX >= left - 4 && mouseX <= left + 1 && mouseY >= buttom - 1 && mouseY <= buttom + 4)  //左下
		return 4;
	else if (mouseX >= right - 1 && mouseX <= right + 4 && mouseY >= top - 4 && mouseY <= top + 1) //右上
		return 3;
	else if (mouseX >= left - 4 && mouseX <= left + 1 && mouseY >= (buttom + top) / 2 - 3 && mouseY <= (buttom + top) / 2 + 2)  //左
		return 5;
	else if (mouseX >= right - 1 && mouseX <= right + 4 && mouseY >= (buttom + top) / 2 - 3 && mouseY <= (buttom + top) / 2 + 2)  //右
		return 6;
	else if (mouseX >= (right + left) / 2 - 3 && mouseX <= (right + left) / 2 + 2 && mouseY >= top - 4 && mouseY <= top + 1) //上
		return 7;
	else if (mouseX >= (right + left) / 2 - 3 && mouseX <= (right + left) / 2 + 2 && mouseY >= buttom - 1 && mouseY <= buttom + 4) //下
		return 8;

	//check mouse is on the selecting rectangle. only apply for line/circle
	if (objectType == 1 || objectType == 3)
	{
		if ((abs(mouseX - left) <= 1 || abs(mouseX - right) <= 1) && (mouseY > top && mouseY < buttom))
		{
			return 9;
		}
		if ((abs(mouseY - top) <= 1 || abs(mouseY - buttom) <= 1) && (mouseX > left && mouseX < right))
		{
			return 9;
		}
	}

	return 0;
}

//record the old ptBeg and ptEnd before moving
void DrawObj::StartToMove(int mouseX, int mouseY)
{
	originalMouseX = mouseX;
	originalMouseY = mouseY;
	originalBegin = ptBeg;
	originalEnd = ptEnd;
}

void DrawObj::Moving(int mouseX, int mouseY)
{
	//set new X Y position
	int deltaX, deltaY;
	deltaX = mouseX - originalMouseX;
	deltaY = mouseY - originalMouseY;

	ptBeg.x = originalBegin.x + deltaX;
	ptBeg.y = originalBegin.y + deltaY;
	ptEnd.x = originalEnd.x + deltaX;
	ptEnd.y = originalEnd.y + deltaY;

	//check for x, y bondaries
	if (ptBeg.x < 1)
	{
		int delta = 1 - ptBeg.x;
		ptBeg.x = 1;
		ptEnd.x += delta;
	}
	else if (ptEnd.x > 1994)
	{
		int delta = 1994 - ptEnd.x;
		ptEnd.x = 1994;
		ptBeg.x += delta;
	}

	if (ptBeg.y < 1)
	{
		int delta = 1 - ptBeg.y;
		ptBeg.y = 1;
		ptEnd.y += delta;
	}
	else if (ptEnd.y > 1995)
	{
		int delta = 1995 - ptEnd.y;
		ptEnd.y = 1995;
		ptBeg.y += delta;
	}

	return;
}

void DrawObj::Resizing(int mouseX, int mouseY, int mode)
{
	//set new X Y position
	int deltaX, deltaY;
	deltaX = mouseX - originalMouseX;
	deltaY = mouseY - originalMouseY;

	int beginDeltaX = 0, beginDeltaY = 0, endDeltaX = 0, endDeltaY = 0;  //assume begin at upper left...

	switch (mode)
	{
	case 1:
		beginDeltaX = deltaX;
		beginDeltaY = deltaY;
		break;
	case 2:
		endDeltaX = deltaX;
		endDeltaY = deltaY;
		break;
	case 3:  //右上
		endDeltaX = deltaX;
		beginDeltaY = deltaY;
		break;
	case 4:  //左下
		beginDeltaX = deltaX;
		endDeltaY = deltaY;
		break;
	case 5:  //左
		beginDeltaX = deltaX;
		break;
	case 6:  //右
		endDeltaX = deltaX;
		break;
	case 7:  //上
		beginDeltaY = deltaY;
		break;
	case 8:  //下
		endDeltaY = deltaY;
		break;
	default:
		return;
		break;
	}

	//find which point is upper right
	if (originalBegin.x < originalEnd.x && originalBegin.y < originalEnd.y)  //ptBeg 在左上
	{
		ptBeg.x = originalBegin.x + beginDeltaX;
		ptBeg.y = originalBegin.y + beginDeltaY;
		ptEnd.x = originalEnd.x + endDeltaX;
		ptEnd.y = originalEnd.y + endDeltaY;
	}
	else if (originalBegin.x < originalEnd.x && originalBegin.y >= originalEnd.y) //ptBeg 在左下
	{
		ptBeg.x = originalBegin.x + beginDeltaX;
		ptBeg.y = originalBegin.y + endDeltaY;
		ptEnd.x = originalEnd.x + endDeltaX;
		ptEnd.y = originalEnd.y + beginDeltaY;
	}
	else if (originalBegin.x >= originalEnd.x && originalBegin.y < originalEnd.y) //ptBeg 在右上
	{
		ptBeg.x = originalBegin.x + endDeltaX;
		ptBeg.y = originalBegin.y + beginDeltaY;
		ptEnd.x = originalEnd.x + beginDeltaX;
		ptEnd.y = originalEnd.y + endDeltaY;
	}
	else  //ptBeg在右下
	{
		ptBeg.x = originalBegin.x + endDeltaX;
		ptBeg.y = originalBegin.y + endDeltaY;
		ptEnd.x = originalEnd.x + beginDeltaX;
		ptEnd.y = originalEnd.y + beginDeltaY;
	}

	//check borders
	if (ptEnd.x > 1990)
		ptEnd.x = 1990;
	if (ptEnd.y > 1990)
		ptEnd.y = 1990;

	if (ptBeg.x < 1)
		ptBeg.x = 1;
	if (ptBeg.y < 1)
		ptBeg.y = 1;

	//the other side borders
	if (ptBeg.x > 1990)
		ptBeg.x = 1990;
	if (ptBeg.y > 1990)
		ptBeg.y = 1990;

	if (ptEnd.x < 1)
		ptEnd.x = 1;
	if (ptEnd.y < 1)
		ptEnd.y = 1;
}

//switch the pen (foreground) color
HPEN DrawObj::SwitchColor()
{
	HPEN hPen;
	switch (color)
	{
	case 0:
		hPen = CreatePen(PS_SOLID, lineWidth, RGB(0, 0, 0));
		break;
	case 1:
		hPen = CreatePen(PS_SOLID, lineWidth, RGB(180, 180, 180));
		break;
	case 2:
		hPen = CreatePen(PS_SOLID, lineWidth, RGB(255, 0, 0));
		break;
	case 3:
		hPen = CreatePen(PS_SOLID, lineWidth, RGB(0, 255, 0));
		break;
	case 4:
		hPen = CreatePen(PS_SOLID, lineWidth, RGB(0, 0, 255));
		break;
	case 5:
		hPen = CreatePen(PS_SOLID, lineWidth, RGB(0, 255, 255));
		break;
	case 6:
		hPen = CreatePen(PS_SOLID, lineWidth, RGB(255, 255, 0));
		break;
	case 7:
		hPen = CreatePen(PS_SOLID, lineWidth, RGB(255, 0, 255));
		break;
	default:
		hPen = CreatePen(PS_SOLID, lineWidth, RGB(0, 0, 0));
	}
	return hPen;
}

void DrawObj::ReleaseColor(HDC hdc)
{
	SelectObject(hdc, GetStockObject(DC_PEN));
	SelectObject(hdc, GetStockObject(NULL_BRUSH));
}

