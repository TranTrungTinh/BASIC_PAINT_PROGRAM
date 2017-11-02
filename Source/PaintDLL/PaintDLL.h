#pragma once
/****************************** Module Header ******************************\
* Module Name:	PaintDLL.h
* Project:		PaintDLL
* Verson: 1.0
*
* History:
* * 1/12/2016 Tran Trung Tinh Created
\***************************************************************************/
#include <Windows.h>


#define PAINTLIBRARY_API __declspec(dllexport) 

namespace PaintDLL {
	class PAINTLIBRARY_API Shape {
	protected:
		int type;
		int x1;
		int y1;
		int x2;
		int y2;
	public:
		int GetX1();
		int GetY1();
		int GetX2();
		int GetY2();
		int GetType();
		Shape(int a, int b, int c, int d, int _type);
		Shape();

		virtual void Draw(HDC hdc) = 0;
	};


	class PAINTLIBRARY_API Line : public Shape {
	public:
		Line();
		Line(int a, int b, int c, int d, int _type);
		void Draw(HDC hdc);
	};

	class PAINTLIBRARY_API Rectang : public Shape {
	public:
		Rectang();
		Rectang(int a, int b, int c, int d, int _type);
		void Draw(HDC hdc);
	};
	class PAINTLIBRARY_API Circle : public Shape {
	public:
		Circle();
		Circle(int a, int b, int c, int d, int _type);
		void Draw(HDC hdc);
	};
}
