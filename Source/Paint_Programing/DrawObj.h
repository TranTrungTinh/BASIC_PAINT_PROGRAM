#pragma once
/****************************** Module Header ******************************\
* Module Name:	DrawObj.h
* Project:		Paint_Programing
* Verson: 1.0
*
* History:
* * 2/12/2016 Tran Trung Tinh Created
\***************************************************************************/
#include "Windows.h"
#include <string>
#include <list>
#include <vector>
using namespace std;
enum DrawType { Line = 1, Rect, Circle, Text };

class DrawObj
{
public:
	POINT ptBeg;
	POINT ptEnd;
	int objectType;  // Type if 1 = line; 2 = rect; 3 = elip; 4 = text; 0 = undefined
	int color;
	int backgroundColor;  //Color 0 = transparent
	int lineWidth; //Width 1pt - 5pt
	bool startFinished, endFinished; // Mouse Pointer
	DrawObj();
	void Clean();
	//virtual ~DrawObj();
	virtual void Paint(HDC hdc, int Xoffset, int Yoffset) = 0;
	virtual void PaintSelectedRect(HDC hdc, int Xoffset, int Yoffset) = 0;
	virtual bool CheckObjectCollision(int mouseX, int mouseY) = 0;  //x and y is absolute position on background
	void PaintMouseOnRect(HDC hdc, int Xoffset, int Yoffset);
	void MakeStart(int x, int y, int currentColor, int currentBgColor, int currentLineWidth); //x and y is absolute position on background
	void MakeEnd(int x, int y, int xCurrentScroll, int yCurrentScroll); //x and y is related position
	int CheckMouseIsOnSizingOpint(int mouseX, int mouseY);	//return 0~4, 0 is normal mouse
	void StartToMove(int mouseX, int mouseY);  //prepare to move or resize the object. x and y is absolute position on background
	virtual void Moving(int mouseX, int mouseY);
	void Resizing(int mouseX, int mouseY, int mode);
protected:
	int originalMouseX, originalMouseY;
	POINT originalBegin, originalEnd;
	HPEN SwitchColor();
	void ReleaseColor(HDC hdc);
};

class LineObj : public DrawObj
{
public:
	LineObj();
	virtual ~LineObj() {}
	virtual void Paint(HDC hdc, int Xoffset, int Yoffset) override;
	virtual void PaintSelectedRect(HDC hdc, int Xoffset, int Yoffset) override;
	virtual bool CheckObjectCollision(int mouseX, int mouseY);
};

class TextObj : public DrawObj
{
private:
	int textWidth, textHeight, maxTextWidth;
	bool AddChar(int c);
	bool AddNewLine();
	bool Backspace();
	bool Del();
public:
	//int textWidth, textHeight, maxTextWidth;
	vector<string> text;
	POINT tailPos;  //tailPos is the last char position of text vector
	POINT caretPos; //carerPos is calculated from inputPos
	POINT inputPos; //inputPos is related to text vector
	TextObj();
	void Clean();
	virtual ~TextObj();
	virtual void Paint(HDC hdc, int Xoffset, int Yoffset);
	virtual void PaintSelectedRect(HDC hdc, int Xoffset, int Yoffset);
	virtual bool CheckObjectCollision(int mouseX, int mouseY);
	bool KeyIn(int wParam);  //return false if no change
	void ResizingText(int mouseX, int mouseY, int mode);
	bool CheckTextBoxBigEnough(int X, int Y);
	void CalculateCaretPosition();
	void Moving(int mouseX, int mouseY);
};

class RectangularObj : public DrawObj
{
public:
	RectangularObj();
	virtual ~RectangularObj();
	virtual void Paint(HDC hdc, int Xoffset, int Yoffset) override;
	virtual void PaintSelectedRect(HDC hdc, int Xoffset, int Yoffset) override;
	virtual bool CheckObjectCollision(int mouseX, int mouseY) override;
};

class CircleObj : public DrawObj
{
public:
	CircleObj();
	virtual ~CircleObj();
	virtual void Paint(HDC hdc, int Xoffset, int Yoffset) override;
	virtual void PaintSelectedRect(HDC hdc, int Xoffset, int Yoffset) override;
	virtual bool CheckObjectCollision(int mouseX, int mouseY) override;
};
