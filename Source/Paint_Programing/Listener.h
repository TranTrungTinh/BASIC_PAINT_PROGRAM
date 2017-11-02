#pragma once
/****************************** Module Header ******************************\
* Module Name:	DrawObj.h
* Project:		Paint_Programing
* Verson: 1.0
*
* History:
* * 3/12/2016 Tran Trung Tinh Created
\***************************************************************************/
#include "stdafx.h"
#include <unordered_map>
#include <functional>
struct Parameter
{
	HWND hWnd_;     // handle object
	WPARAM wParam_; // event
	LPARAM lParam_; // 
	UINT message_;
	Parameter(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
		:hWnd_(hWnd)
		, message_(message)
		, wParam_(wParam)
		, lParam_(lParam)
	{}
};

using Event = std::function<LRESULT(Parameter& param)>;

class Listener
{
private:
	std::unordered_map<UINT, Event> evnetHandlerMap_;
	Event defaultEventHandler_;
public:
	HINSTANCE hInst;
	static Listener& WinProcMsgListener();
	inline void AddDefaultEvent(Event e) { defaultEventHandler_ = e; }
	inline void AddEvent(UINT msg, Event e) { evnetHandlerMap_[msg] = e; }
	LRESULT Trig(UINT msg, Parameter& param);
};
