#include "stdafx.h"
#include "DrawObj.h"

RectangularObj::RectangularObj()
{
	objectType = 2;
}

RectangularObj::~RectangularObj()
{}

void RectangularObj::Paint(HDC hdc, int Xoffset, int Yoffset)
{
	if (!startFinished)
		return;
	if (ptBeg.x == ptEnd.x && ptBeg.y == ptEnd.y)
		return;

	HPEN hPen = SwitchColor();
	SelectObject(hdc, hPen);
	//HBRUSH hBrush = switchBackgroundColor();
	HBRUSH hBrush;
	switch (backgroundColor)
	{
	case 1:
		hBrush = CreateSolidBrush(RGB(180, 180, 180));
		break;
	case 2:
		hBrush = CreateSolidBrush(RGB(255, 0, 0));
		break;
	case 3:
		hBrush = CreateSolidBrush(RGB(0, 255, 0));
		break;
	case 4:
		hBrush = CreateSolidBrush(RGB(0, 0, 255));
		break;
	case 5:
		hBrush = CreateSolidBrush(RGB(0, 255, 255));
		break;
	case 6:
		hBrush = CreateSolidBrush(RGB(255, 255, 0));
		break;
	case 7:
		hBrush = CreateSolidBrush(RGB(255, 0, 255));
		break;
	default:
		hBrush = NULL;
	}

	if (hBrush != NULL)
		SelectObject(hdc, hBrush);
	else
		SelectObject(hdc, GetStockObject(NULL_BRUSH));

	int top = (ptBeg.y < ptEnd.y ? ptBeg.y : ptEnd.y);
	int left = (ptBeg.x < ptEnd.x ? ptBeg.x : ptEnd.x);
	int buttom = (ptBeg.y > ptEnd.y ? ptBeg.y : ptEnd.y);
	int right = (ptBeg.x > ptEnd.x ? ptBeg.x : ptEnd.x);
	Rectangle(hdc, left - Xoffset, top - Yoffset, right - Xoffset, buttom - Yoffset);

	DeleteObject(hPen);
	if (hBrush != NULL)
		DeleteObject(hBrush);
	ReleaseColor(hdc);
}

void RectangularObj::PaintSelectedRect(HDC hdc, int Xoffset, int Yoffset)
{
	if (ptBeg.x == ptEnd.x && ptBeg.y == ptEnd.y)
		return;

	HPEN hpen, hpenOld;
	hpen = CreatePen(PS_DASH, 1, RGB(0, 0, 0));
	hpenOld = (HPEN)SelectObject(hdc, hpen);
	HGDIOBJ oldBrush = SelectObject(hdc, GetStockObject(NULL_BRUSH));  //paint an empty dash rectangle

																	   // do something...
	int top = (ptBeg.y < ptEnd.y ? ptBeg.y : ptEnd.y) - Yoffset;
	int left = (ptBeg.x < ptEnd.x ? ptBeg.x : ptEnd.x) - Xoffset;
	int buttom = (ptBeg.y > ptEnd.y ? ptBeg.y : ptEnd.y) - Yoffset;
	int right = (ptBeg.x > ptEnd.x ? ptBeg.x : ptEnd.x) - Xoffset;
	Rectangle(hdc, left, top, right, buttom);

	//return the pen
	SelectObject(hdc, hpenOld);
	DeleteObject(hpen);

	SelectObject(hdc, GetStockObject(WHITE_BRUSH));
	//draw the 8-points
	{
		//左上
		Rectangle(hdc, left - 4, top - 4, left + 1, top + 1);
		//右上
		Rectangle(hdc, right - 1, top - 4, right + 4, top + 1);
		//左下
		Rectangle(hdc, left - 4, buttom - 1, left + 1, buttom + 4);
		//右下
		Rectangle(hdc, right - 1, buttom - 1, right + 4, buttom + 4);
		//左中
		Rectangle(hdc, left - 4, (buttom + top) / 2 - 3, left + 1, (buttom + top) / 2 + 2);
		//右中
		Rectangle(hdc, right - 1, (buttom + top) / 2 - 3, right + 4, (buttom + top) / 2 + 2);
		//上中
		Rectangle(hdc, (right + left) / 2 - 3, top - 4, (right + left) / 2 + 2, top + 1);
		//下中
		Rectangle(hdc, (right + left) / 2 - 3, buttom - 1, (right + left) / 2 + 2, buttom + 4);
	}
	SelectObject(hdc, oldBrush);
	DeleteObject(oldBrush);
}

bool RectangularObj::CheckObjectCollision(int mouseX, int mouseY)
{
	int top = (ptBeg.y < ptEnd.y ? ptBeg.y : ptEnd.y);
	int left = (ptBeg.x < ptEnd.x ? ptBeg.x : ptEnd.x);
	int buttom = (ptBeg.y > ptEnd.y ? ptBeg.y : ptEnd.y);
	int right = (ptBeg.x > ptEnd.x ? ptBeg.x : ptEnd.x);

	if (mouseY < top || mouseY> buttom || mouseX < left || mouseX > right)  //first if mouse out of range, just return false
		return false;

	if (backgroundColor != 0)
	{
		if (mouseY >= top && mouseY <= buttom && mouseX >= left && mouseX <= right)
			return true;
	}
	else
	{
		if ((abs(mouseX - left) <= lineWidth + 1 || abs(mouseX - right) <= lineWidth + 1) && (mouseY > top && mouseY < buttom))
		{
			return true;
		}
		else if ((abs(mouseY - top) <= lineWidth + 1 || abs(mouseY - buttom) <= lineWidth + 1) && (mouseX > left && mouseX < right))
		{
			//MessageBox(NULL, L"aaaaa", L"Titile", MB_OK);
			return true;
		}
	}
	return false;
}
