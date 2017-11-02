#include "stdafx.h"
#include "DrawObj.h"
#include "globals.h"

TextObj::TextObj()
{
	text.clear();
	text.push_back("");
	objectType = 4;
	textHeight = 13 + 1;
	textWidth = 5 * 8 + 1;
	tailPos.x = 0;
	tailPos.y = 0;
	inputPos.x = 0;
	inputPos.y = 0;
	caretPos = tailPos;
}

void TextObj::Clean()
{
	ptBeg.x = -8;
	ptBeg.y = -13;
	ptEnd.x = -8;
	ptEnd.y = -13;
	tailPos.x = -8;
	tailPos.y = -13;
	inputPos.x = 0;
	inputPos.y = 0;
	caretPos = tailPos;
	textHeight = 13 + 1;
	textWidth = 5 * 8 + 1;
	startFinished = false;
	endFinished = false;
	color = 0;
	text.clear();
	text.push_back("");
}

TextObj::~TextObj()
{

}

void TextObj::Paint(HDC hdc, int Xoffset, int Yoffset)
{
	if (!startFinished)
		return;

	string s = "";  //string for print on screen
	RECT rc;
	SetRect(&rc, ptBeg.x - Xoffset, ptBeg.y - Yoffset, ptBeg.x - Xoffset + 300, ptBeg.y - Yoffset + 300);

	switch (color)
	{
	case 0:
		SetTextColor(hdc, RGB(0, 0, 0));
		break;
	case 1:
		SetTextColor(hdc, RGB(180, 180, 180));
		break;
	case 2:
		SetTextColor(hdc, RGB(255, 0, 0));
		break;
	case 3:
		SetTextColor(hdc, RGB(0, 255, 0));
		break;
	case 4:
		SetTextColor(hdc, RGB(0, 0, 255));
		break;
	case 5:
		SetTextColor(hdc, RGB(0, 255, 255));
		break;
	case 6:
		SetTextColor(hdc, RGB(255, 255, 0));
		break;
	case 7:
		SetTextColor(hdc, RGB(255, 0, 255));
		break;
	default:
		SetTextColor(hdc, RGB(0, 0, 0));
	}

	//draw the background rect
	HPEN hPen = (HPEN)GetStockObject(NULL_PEN);
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

	HPEN oldPen = (HPEN)SelectObject(hdc, hPen);
	if (hBrush != NULL)
		SelectObject(hdc, hBrush);
	else
		SelectObject(hdc, GetStockObject(NULL_BRUSH));
	Rectangle(hdc, ptBeg.x - Xoffset, ptBeg.y - Yoffset, ptEnd.x - Xoffset, ptEnd.y - Yoffset);  //BG rectangle
	if (hBrush != NULL)
		DeleteObject(hBrush);
	SelectObject(hdc, oldPen);
	DeleteObject(hPen);
	DeleteObject(oldPen);

	// Retrieve a handle to the variable stock font. 
	HFONT hFont, hOldFont;
	hFont = (HFONT)GetStockObject(ANSI_FIXED_FONT);  //目前寬8，高13

	textWidth = ptEnd.x - ptBeg.x;
	textHeight = ptEnd.y - ptBeg.y;

	int lineSize = textWidth / 8;
	if (lineSize <= 0)
		lineSize = 1;
	// Select the variable stock font into the specified device context.
	if (hOldFont = (HFONT)SelectObject(hdc, hFont))
	{
		// Display the text string.  Using a Stock Font to Draw Text.
		tailPos.y = -13;
		for (auto it : text)
		{
			//add all the strings to one and print it out
			int i = 0;
			tailPos.x = 0;
			for (auto it2 : it)
			{
				if (i > 0 && i%lineSize == 0)
				{
					s.push_back('\n');
					tailPos.y += 13;
					tailPos.x = 0;
				}
				s.push_back(it2);
				i++;
				tailPos.x += 8;
			}
			s += '\n';
			tailPos.y += 13;
		}
		SetBkMode(hdc, TRANSPARENT);  //set text background to transparent
		DrawTextA(hdc, s.c_str(), s.length(), &rc, DT_LEFT | DT_NOCLIP);

		if (Globals::var().debugMode)
		{
			s = "*line number " + to_string(text.size()) + ",char number " + (text.size() > 0 ? to_string(text.back().size()) : "NULL");
			TextOutA(hdc, ptBeg.x - Xoffset, ptBeg.y - Yoffset - 13, s.c_str(), s.length());
			s = "*input pos=" + to_string(inputPos.x) + ", " + to_string(inputPos.y);
			TextOutA(hdc, ptBeg.x - Xoffset, ptBeg.y - Yoffset - 26, s.c_str(), s.length());
		}

		// Restore the original font.
		SelectObject(hdc, hOldFont);
	}
	SetTextColor(hdc, RGB(0, 0, 0));
	DeleteObject(hFont);
	DeleteObject(hOldFont);

	CalculateCaretPosition();
}

void TextObj::PaintSelectedRect(HDC hdc, int Xoffset, int Yoffset)
{
	if (ptBeg.x == ptEnd.x && ptBeg.y == ptEnd.y)
		return;

	HPEN hpen, hpenOld;
	hpen = CreatePen(PS_DOT, 1, RGB(0, 0, 0));
	hpenOld = (HPEN)SelectObject(hdc, hpen);
	HGDIOBJ oldBrush = SelectObject(hdc, GetStockObject(NULL_BRUSH));

	// do something...
	int top = ptBeg.y - Yoffset;
	int left = ptBeg.x - Xoffset;
	int buttom = ptEnd.y - Yoffset;
	int right = ptEnd.x - Xoffset;
	Rectangle(hdc, left, top, right, buttom);

	//return the pen and brush
	SelectObject(hdc, hpenOld);
	DeleteObject(hpen);
	DeleteObject(hpenOld);

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

bool TextObj::AddChar(int c)
{
	if (text.size() == 0)
		text.push_back("");

	vector<string> vs = text;  //make a backup in case keyin failed
	POINT inputPosBackup = inputPos;

	//do oversize checking
	if (text.size() < inputPos.y + 1)
		inputPos.y = text.size() - 1;
	if (text[inputPos.y].size() < inputPos.x)
		inputPos.x = text[inputPos.y].size();

	//add char to input position
	if (c >= 65 && c <= 90)          //A~Z
		text[inputPos.y].insert(inputPos.x, 1, c + 32);
	else if (c >= 48 && c <= 57)     //0~9
		text[inputPos.y].insert(inputPos.x, 1, c);
	else if (c >= 0x60 && c <= 0x69) //Numeric keypad 0~9
		text[inputPos.y].insert(inputPos.x, 1, c + -48);
	else if (c == 0x20)              //space
		text[inputPos.y].insert(inputPos.x, 1, ' ');

	inputPos.x++;

	//check for Y borders
	int lineSize = (ptEnd.x - ptBeg.x) / 8;
	if (lineSize <= 0)
		lineSize = 1;

	int y = 0;
	for (auto it : text)
	{
		//simulating add all the strings to one and print it
		int i = 0;
		for (auto it2 : it)
		{
			if (i > 0 && i%lineSize == 0)
			{
				y += 1;
			}
			i++;
		}
		y += 1;
	}
	if (y * 13 + ptBeg.y > 1996)  //revert old values
	{
		text = vs;
		inputPos = inputPosBackup;
		return false;
	}
	else
		return true;
}

bool TextObj::AddNewLine()
{
	if (text.size() == 0)
		text.push_back("");

	//check if new y goes over window border
	int y = ptBeg.y + tailPos.y + 13 * 2;
	if (y > 1995)
		return false;

	//do oversize checking
	if (text.size() < inputPos.y + 1)
		inputPos.y = text.size() - 1;

	//cut down string by X
	string tail = text[inputPos.y].substr(inputPos.x);

	//cut the original string by X
	text[inputPos.y].erase(text[inputPos.y].begin() + inputPos.x, text[inputPos.y].end());

	//insert empty line at Y
	text.insert(text.begin() + inputPos.y + 1, "");

	//set X and Y again
	inputPos.y++;
	inputPos.x = 0;

	//put everything after X into new line
	text[inputPos.y].append(tail);
	return true;
}

bool TextObj::Backspace()
{
	bool returnValue = false;
	if (text.size() > 0)
	{
		//if caret X is at 0, delete a "\n"
		if (inputPos.x == 0 && inputPos.y > 0)
		{
			//backup everything in Yth line
			string s = text[inputPos.y];

			//remove Yth line
			text.erase(text.begin() + inputPos.y);
			inputPos.y--;
			inputPos.x = text[inputPos.y].size();

			//put back s to Y-1th line
			text[inputPos.y].append(s);
			returnValue = true;
		}
		else if (inputPos.x > 0)  //else remove one char
		{
			text[inputPos.y].erase(text[inputPos.y].begin() + inputPos.x - 1);
			inputPos.x--;
			returnValue = true;
		}
	}

	if (text.size() == 0)  //size is 0, put a empty one
	{
		text.push_back("");
	}
	return returnValue;
}

bool TextObj::Del()  //this function is not used now...
{
	bool returnValue = false;
	if (inputPos.x >= text[inputPos.y].size() && inputPos.y + 1 < text.size())  //X is at end of string, delete the next "\n"
	{
		//backup everything in Y+1th line
		string s = text[inputPos.y + 1];
		text.erase(text.begin() + inputPos.y + 1);
		text[inputPos.y].append(s);
		returnValue = true;
	}
	else if (inputPos.x < text[inputPos.y].size()) //else remove one char
	{
		text[inputPos.y].erase(text[inputPos.y].begin() + inputPos.x);
		returnValue = true;
	}
	return returnValue;
}

bool TextObj::CheckObjectCollision(int mouseX, int mouseY)
{
	if (mouseY >= ptBeg.y && mouseY <= ptEnd.y && mouseX >= ptBeg.x && mouseX <= ptEnd.x)
		return true;
	return false;
}

bool TextObj::KeyIn(int wParam)
{
	bool returnValue = false;
	if ((wParam >= 65 && wParam <= 90) || (wParam >= 48 && wParam <= 57) || (wParam >= 0x60 && wParam <= 0x69) || (wParam == 0x20))
	{
		if ((text.back().size() + 1) * 8 > ptEnd.x - ptBeg.x) //if x > 2000 add new line and add new char
		{
			int newy = ptBeg.y + (text.size() + 1) * 13;
			if (newy < 1996)  //if y < 2000-4 add new line
			{
				//continue
			}
			else
				return false;  //do nothing
		}
		returnValue = AddChar(wParam);
	}
	else if (wParam == 0x0D)  //enter
	{
		if (ptBeg.y + (text.size() + 1) * 13 < 1996)
		{
			returnValue = AddNewLine();  //insert a "" string to back
		}
	}
	else if (wParam == 0x08)  //backspace <-|
	{
		returnValue = Backspace();
	}
	else if (wParam == VK_HOME) //Home
	{
		short iState = GetKeyState(VK_CONTROL);
		if (iState & 0x8000)  //ctrl is pressed
		{
			inputPos.y = inputPos.x = 0;
		}
		else
		{
			inputPos.x = 0;
		}

	}
	else if (wParam == VK_END) //End
	{
		SHORT iState = GetKeyState(VK_CONTROL);
		if (iState & 0x8000)  //ctrl is pressed
		{
			inputPos.y = text.size() - 1;
			inputPos.x = text[inputPos.y].size();
		}
		else
		{
			inputPos.x = text[inputPos.y].size();
		}
	}
	else if (wParam == VK_LEFT) //4 arrow keys
	{
		if (inputPos.x > 0)
			inputPos.x--;
	}
	else if (wParam == VK_RIGHT)
	{
		if (inputPos.x < text[inputPos.y].size())
			inputPos.x++;
	}
	else if (wParam == VK_UP)
	{
		int w = textWidth / 8;
		if (inputPos.x > w)
			inputPos.x -= w;
		else if (inputPos.y > 0)
		{
			inputPos.y--;
			//check if x is larger than previous line size
			//if previous line has only one line, compare tha tail position
			if (text[inputPos.y].size() <= w && inputPos.x > text[inputPos.y].size())
				inputPos.x = text[inputPos.y].size();
			else if (text[inputPos.y].size() > w)  //else, compare x with last line's length
			{
				if (inputPos.x > text[inputPos.y].size() % w)
					inputPos.x = text[inputPos.y].size();
				else
					inputPos.x = text[inputPos.y].size() - (text[inputPos.y].size() % w - inputPos.x);
			}
		}
	}
	else if (wParam == VK_DOWN)
	{
		int w = textWidth / 8;
		int linesize = text[inputPos.y].size() / w + 1;
		if (text[inputPos.y].size() > 0 && text[inputPos.y].size() % w == 0)
			linesize--;
		if (text[inputPos.y].size() > w && (inputPos.x - 1) / w < linesize - 1)  //just move in this line
		{
			if (inputPos.x <= text[inputPos.y].size() - w)
				inputPos.x += w;
			else
				inputPos.x = text[inputPos.y].size();
		}
		else if (inputPos.y < text.size() - 1)  //move to next line
		{
			inputPos.y++;
			//if next line is smaller than w (single line), compare x with size
			if (text[inputPos.y].size() <= w)
			{
				if (inputPos.x % w > text[inputPos.y].size())
					inputPos.x = text[inputPos.y].size();
				else if (inputPos.x >0 && inputPos.x % w == 0)
					inputPos.x = text[inputPos.y].size();
				else
					inputPos.x = inputPos.x % w;
			}
			else  //if have multiple line, x must > w, get the x position of previous line first.
			{
				if (inputPos.x > 0 && inputPos.x % w == 0)
				{
					//rightmost position
					inputPos.x = w;
				}
				else
				{
					int x = inputPos.x % w;
					inputPos.x = x;
				}
			}
		}
	}

	//calculate text box
	{
		int maxLine = (ptEnd.y - ptBeg.y) / 13;
		int lineSize = (ptEnd.x - ptBeg.x) / 8;
		if (lineSize <= 0)
			lineSize = 1;

		POINT t;
		t.x = 0;
		t.y = 0;

		for (auto it : text)
		{
			//simulating add all the strings to one and print it
			int i = 0;
			t.x = 0;
			for (auto it2 : it)
			{
				if (i > 0 && i%lineSize == 0)
				{
					t.y += 1;
					t.x = 0;
				}
				i++;
				t.x += 1;
			}
			t.y += 1;
		}
		if (t.y > maxLine)
			textHeight = t.y * 13;
		else
			textHeight = ptEnd.y - ptBeg.y;
	}

	ptEnd.x = ptBeg.x + textWidth;
	ptEnd.y = ptBeg.y + textHeight;
	return returnValue;
}

void TextObj::ResizingText(int mouseX, int mouseY, int mode)
{
	int deltaX, deltaY;
	deltaX = mouseX - originalMouseX;
	deltaY = mouseY - originalMouseY;

	POINT oldBeg, oldEnd;  //make a backup
	oldBeg = ptBeg;
	oldEnd = ptEnd;

	int beginDeltaX = 0, beginDeltaY = 0, endDeltaX = 0, endDeltaY = 0;  //assume begin at upper left...

	switch (mode)
	{
	case 1:  //左上
		beginDeltaX = deltaX;
		beginDeltaY = deltaY;
		break;
	case 2:  //右下
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
	}

	//find which point is upper right and apply delta
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

	//check new box size for text
	if (!CheckTextBoxBigEnough(ptEnd.x - ptBeg.x, ptEnd.y - ptBeg.y))
	{
		//revert to old value
		ptBeg = oldBeg;
		ptEnd = oldEnd;
	}

	//check if < 1*1 text box
	if (ptEnd.x - ptBeg.x <8 || ptEnd.y - ptBeg.y<13)
	{
		//revert to old value
		ptBeg = oldBeg;
		ptEnd = oldEnd;
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
}

bool TextObj::CheckTextBoxBigEnough(int X, int Y)  //X and Y is size of box
{
	if (text.size() <= 0)
		return true;

	if (X < 8 || Y < 13)
		return false;

	int maxLine = (ptEnd.y - ptBeg.y) / 13;    //width of box
	int lineSize = (ptEnd.x - ptBeg.x) / 8;  //height of box
	if (lineSize <= 0)
		lineSize = 1;

	POINT t;
	t.x = 0;
	t.y = 0;

	for (auto it : text)
	{
		//simulating add all the strings to one and print it
		int i = 0;
		t.x = 0;
		for (auto it2 : it)
		{
			if (i > 0 && i%lineSize == 0)
			{
				t.y += 1;
				t.x = 0;
			}
			i++;
			t.x += 1;
		}
		t.y += 1;
	}

	if (t.y > maxLine)
		return false;
	else
		return true;
}

void TextObj::CalculateCaretPosition()
{
	//calculate caret position
	int lineSize = textWidth / 8;
	if (lineSize <= 0)
		lineSize = 1;
	int y = inputPos.y;
	for (int i = 0; i <= inputPos.y; i++)
	{
		int s = text[i].size();
		if (i == inputPos.y)
			s = inputPos.x;
		y += (s - 1) / lineSize;
	}
	caretPos.y = ptBeg.y + y * 13;
	caretPos.x = ptBeg.x + ((inputPos.x) % lineSize) * 8;
	if (inputPos.x > 0 && inputPos.x% lineSize == 0) //if at a x of box end...
		caretPos.x += lineSize * 8;
}

void TextObj::Moving(int mouseX, int mouseY)
{
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


