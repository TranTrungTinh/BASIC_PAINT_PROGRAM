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
#include "json.hpp"
#include <string>
#include <list>
#include "DrawObj.h"


using namespace std;
using json = nlohmann::json;

//operation:
//0=add, 1=delete, 2=move, 3=resize, 4=modify, 5=modify text
class mylog
{
	list<json> ops;
	void PushObject(DrawObj* d, json j);
	json jmove;
	json jredo;
public:
	/*mylog();
	~mylog();*/
	void Undo(); // Back
	void Redo(); // UnBack
	void ClearLogs();

	void OP_add(DrawObj* d);               // new shape
	void OP_del(DrawObj* d, int pos);      // delete shape
	void OP_moveStart(DrawObj* d, int pos);// start index move shape
	void OP_moveEnd(DrawObj * d);          // stop index move shape
	void OP_sizeStart(DrawObj* d, int pos);// size index start move
	void OP_sizeEnd(DrawObj* d);           // size index stop move
	void OP_textStart(DrawObj* d, int pos);// start index move text
	void OP_textEnd(DrawObj* d);           // stop index move text
	void OP_modifyStart(DrawObj* d, int pos); // start edit text
	void OP_modifyEnd(DrawObj* d);            // stop edit text
};

