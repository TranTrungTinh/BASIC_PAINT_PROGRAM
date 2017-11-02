#include "stdafx.h"
#include "DrawObj.h"

CircleObj::CircleObj() { objectType = 3; }

CircleObj::~CircleObj() {}

void CircleObj::Paint(HDC hdc, int Xoffset, int Yoffset)
{
	if (!startFinished)
		return;
	if (ptBeg.x == ptEnd.x && ptBeg.y == ptEnd.y)
		return;
	HPEN hPen = SwitchColor();
	SelectObject(hdc, hPen);
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
	Ellipse(hdc, left - Xoffset, top - Yoffset, right - Xoffset, buttom - Yoffset);

	DeleteObject(hPen);
	if (hBrush != NULL)
		DeleteObject(hBrush);
	ReleaseColor(hdc);
}

void CircleObj::PaintSelectedRect(HDC hdc, int Xoffset, int Yoffset)
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
	int bottom = (ptBeg.y > ptEnd.y ? ptBeg.y : ptEnd.y) - Yoffset;
	int right = (ptBeg.x > ptEnd.x ? ptBeg.x : ptEnd.x) - Xoffset;
	Rectangle(hdc, left, top, right, bottom);

	//return the pen
	SelectObject(hdc, hpenOld);
	DeleteObject(hpen);
	DeleteObject(hpenOld);

	SelectObject(hdc, GetStockObject(WHITE_BRUSH));  //paint 8 white small rectangles
													 //draw the 8-points
	{
		//左上
		Rectangle(hdc, left - 4, top - 4, left + 1, top + 1);
		//右上
		Rectangle(hdc, right - 1, top - 4, right + 4, top + 1);
		//左下
		Rectangle(hdc, left - 4, bottom - 1, left + 1, bottom + 4);
		//右下
		Rectangle(hdc, right - 1, bottom - 1, right + 4, bottom + 4);
		//左中
		Rectangle(hdc, left - 4, (bottom + top) / 2 - 3, left + 1, (bottom + top) / 2 + 2);
		//右中
		Rectangle(hdc, right - 1, (bottom + top) / 2 - 3, right + 4, (bottom + top) / 2 + 2);
		//上中
		Rectangle(hdc, (right + left) / 2 - 3, top - 4, (right + left) / 2 + 2, top + 1);
		//下中
		Rectangle(hdc, (right + left) / 2 - 3, bottom - 1, (right + left) / 2 + 2, bottom + 4);
	}
	SelectObject(hdc, oldBrush);
	DeleteObject(oldBrush);
}

bool CircleObj::CheckObjectCollision(int mouseX, int mouseY)
{
	//circle function: (x-h)^2/a^2 + (y-k)^2/b^2 = 1
	int top = (ptBeg.y < ptEnd.y ? ptBeg.y : ptEnd.y);
	int left = (ptBeg.x < ptEnd.x ? ptBeg.x : ptEnd.x);
	int buttom = (ptBeg.y > ptEnd.y ? ptBeg.y : ptEnd.y);
	int right = (ptBeg.x > ptEnd.x ? ptBeg.x : ptEnd.x);

	if (mouseY < top || mouseY> buttom || mouseX < left || mouseX > right)  //first if mouse out of range, just return false
		return false;

	float a = (float)(right - left) / 2.0;
	float b = (float)(buttom - top) / 2.0;

	float h = (float)(right + left) / 2.0;
	float k = (float)(buttom + top) / 2.0;

	float result = pow(mouseX - h, 2) / pow(a, 2) + pow(mouseY - k, 2) / pow(b, 2);
	float delta = 0.1;

	if (backgroundColor != 0)
	{
		if (result <= 1)
			return true;
	}
	else if (result <= 1 + delta && result >= 1 - delta)
	{
		return true;
	}

	return false;
}

