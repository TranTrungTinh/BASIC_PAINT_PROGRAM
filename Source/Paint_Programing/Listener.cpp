#include "stdafx.h"
#include "Listener.h"

//map each msg to different listener functions
LRESULT Listener::Trig(UINT msg, Parameter& param)
{
	if (evnetHandlerMap_[msg] != nullptr)
		return evnetHandlerMap_[msg](param);
	return DefWindowProc(param.hWnd_, msg, param.wParam_, param.lParam_);
}

Listener& Listener::WinProcMsgListener() {
	static Listener l;
	return l;
}
