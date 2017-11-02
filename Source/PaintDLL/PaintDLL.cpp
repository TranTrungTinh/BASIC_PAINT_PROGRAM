// PaintDLL.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "PaintDLL.h"

namespace PaintDLL {
	int Shape::GetX1() {
		return x1;
	}
	int  Shape::GetY1() {
		return y1;
	}
	int  Shape::GetX2() {
		return x2;
	}
	int Shape::GetY2() {
		return y2;
	}
	int  Shape::GetType() {
		return type;
	}
	Shape::Shape(int a, int b, int c, int d, int _type) {
		x1 = a;
		y1 = b;
		x2 = c;
		y2 = d;
		type = _type;
	}
	Shape::Shape() {
		x1 = 0;
		y1 = 0;
		x2 = 0;
		y2 = 0;
		type = 0;
	}

	//line
	Line::Line() :Shape() {};
	Line::Line(int a, int b, int c, int d, int _type) :Shape(a, b, c, d, _type) {};
	void Line::Draw(HDC hdc) {
		MoveToEx(hdc, x1, y1, NULL);
		LineTo(hdc, x2, y2);
	}

	//retangle
	Rectang::Rectang() :Shape() {};
	Rectang::Rectang(int a, int b, int c, int d, int _type) :Shape(a, b, c, d, _type) {};
	void Rectang::Draw(HDC hdc) {
		Rectangle(hdc, x1, y1, x2, y2);
	}

	//circle
	Circle::Circle() :Shape() {};
	Circle::Circle(int a, int b, int c, int d, int _type) :Shape(a, b, c, d, _type) {};
	void Circle::Draw(HDC hdc) {
		Ellipse(hdc, x1, y1, x2, y2);
	}
}

