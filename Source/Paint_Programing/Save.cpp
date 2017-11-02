#include "stdafx.h"
#include "Save.h"
#include "Globals.h"

int SaveToFile(const list<DrawObj*>& saveList, string& fileName)
{
	//put each object to json
	json j;
	for (auto& it : saveList)  //Draw each object in DrawObjList
	{
		json jit;
		jit["objectType"] = it->objectType;
		jit["ptBeg"] = { it->ptBeg.x, it->ptBeg.y };
		jit["ptEnd"] = { it->ptEnd.x, it->ptEnd.y };
		jit["color"] = it->color;
		jit["backgroundColor"] = it->backgroundColor;
		jit["lineWidth"] = it->lineWidth;
		if (it->objectType == 4)
		{
			TextObj* t = dynamic_cast<TextObj*>(it);
			vector<string> vs = t->text;
			jit["text"] = vs;
		}
		j.push_back(jit);
	}

	// open a file in write mode.
	ofstream outfile;
	PWSTR filePath = BasicFileSave();
	if (filePath)
	{
		string fp = WstrToStr(filePath);
		if (!EndWith(fp, ".json"))
			fp.append(".json");
		outfile.open(fp);
		outfile << j << endl;
		outfile.close();
		Globals::var().lastFilePath = filePath;
	}
	else
	{
		MessageBox(NULL, L"You did not save.", L"Warning!", MB_OK);
		return 1;
	}
	outfile.close();

	//pass the file name to main
	string name = WstrToStr(filePath);
	auto const pos = name.find_last_of('\\');
	auto const result = name.substr(pos + 1);
	fileName = result;

	return 0;
}

int ReadFromFile(list<DrawObj*>& readList, string& fileName)
{
	Globals::var().preSelectObject = nullptr;  //this is for deleted pointer workaround
	PWSTR filePath = BasicFileOpen();  //open file path
	if (!filePath)
	{
		MessageBox(NULL, L"File open failed.", L"Error!", MB_OK);
		return 1;
	}

	json j;

	// open a file in read mode.
	ifstream infile;
	infile.open(filePath);
	if (!infile)
	{
		MessageBox(NULL, L"File open failed.", L"Error!", MB_OK);
		return 1;
	}
	infile >> j;
	infile.close();

	//pass the file name to main
	Globals::var().lastFilePath = filePath;
	string name = WstrToStr(filePath);
	auto const pos = name.find_last_of('\\');
	auto const result = name.substr(pos + 1);
	fileName = result;

	readList.clear();

	int i = 0;
	for (json::iterator it = j.begin(); it != j.end(); it++)
	{
		json j5 = *it;
		switch ((int)j5["objectType"])
		{
		case Line:
		{
			LineObj newLine;
			newLine.MakeStart(j5["ptBeg"][0], j5["ptBeg"][1], j5["color"], j5["backgroundColor"], j5["lineWidth"]);
			newLine.MakeEnd(j5["ptEnd"][0], j5["ptEnd"][1], 0, 0);
			readList.push_back(new LineObj(newLine));
			break;
		}
		case Rect:
		{
			RectangularObj newRect;
			newRect.MakeStart(j5["ptBeg"][0], j5["ptBeg"][1], j5["color"], j5["backgroundColor"], j5["lineWidth"]);
			newRect.MakeEnd(j5["ptEnd"][0], j5["ptEnd"][1], 0, 0);
			readList.push_back(new RectangularObj(newRect));
			break;
		}
		case Circle:
		{
			CircleObj newCircle;
			newCircle.MakeStart(j5["ptBeg"][0], j5["ptBeg"][1], j5["color"], j5["backgroundColor"], j5["lineWidth"]);
			newCircle.MakeEnd(j5["ptEnd"][0], j5["ptEnd"][1], 0, 0);
			readList.push_back(new CircleObj(newCircle));
			break;
		}
		case Text:
		{
			TextObj newText;
			newText.MakeStart(j5["ptBeg"][0], j5["ptBeg"][1], j5["color"], j5["backgroundColor"], j5["lineWidth"]);
			newText.MakeEnd(j5["ptEnd"][0], j5["ptEnd"][1], 0, 0);
			vector<string> text = j5["text"];
			newText.text = text;
			newText.lineWidth = newText.ptEnd.x - newText.ptBeg.x;
			//calculate text box
			{
				int textHeight = newText.ptEnd.y - newText.ptBeg.y;
				int maxLine = (newText.ptEnd.y - newText.ptBeg.y) / 13;
				int lineSize = (newText.ptEnd.x - newText.ptBeg.x) / 8;
				if (lineSize <= 0)
					lineSize = 1;
				POINT t;
				t.x = 0;
				t.y = 0;

				for (auto it : text)
				{
					//add all the strings to one and print it
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
				if (t.y > maxLine)  //in case the hight is not enough...
				{
					textHeight = t.y * 13;
					newText.ptEnd.y = newText.ptBeg.y + textHeight;
				}
			}
			readList.push_back(new TextObj(newText));
			break;
		}
		default:
			//error!
			MessageBox(NULL, L"JSON read error!", L"ERROR", MB_OK);
			break;
		}
		i++;
	}
	j.clear();
	return 0;
}

int SaveToLastFilePath(const list<DrawObj*>& saveList)
{
	//put each object to json
	json j;
	for (auto& it : saveList)  //Draw each object in DrawObjList
	{
		json jit;
		jit["objectType"] = it->objectType;
		jit["ptBeg"] = { it->ptBeg.x, it->ptBeg.y };
		jit["ptEnd"] = { it->ptEnd.x, it->ptEnd.y };
		jit["color"] = it->color;
		jit["backgroundColor"] = it->backgroundColor;
		jit["lineWidth"] = it->lineWidth;
		if (it->objectType == 4)
		{
			TextObj* t = dynamic_cast<TextObj*>(it);
			vector<string> ls = t->text;
			jit["text"] = ls;
		}
		j.push_back(jit);
	}

	// open a file in write mode.
	ofstream outfile;
	if (Globals::var().lastFilePath.size() > 0)
	{
		string fp = WstrToStr(Globals::var().lastFilePath);
		outfile.open(fp);
		if (outfile.is_open())
		{
			outfile << j << endl;
			outfile.close();
		}
		else
		{
			MessageBox(NULL, L"SaveToLastFilePath Failed", L"ERROR!", MB_OK);
		}
	}
	else
	{
		MessageBox(NULL, L"SaveToLastFilePath Failed", L"ERROR!", MB_OK);
		return 1;
	}
	j.clear();
	return 0;
}

PWSTR BasicFileOpen()
{
	PWSTR pszFilePath = NULL;
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	if (SUCCEEDED(hr))
	{
		IFileOpenDialog *pFileOpen;

		// Create the FileOpenDialog object.
		hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
			IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));
		//no need to release the instance, only release pFileOpen
		//CoCreateInstance is not allocating memory. CCI is creating an instance of an COM object.

		if (SUCCEEDED(hr))
		{
			// Show the Open dialog box.
			hr = pFileOpen->Show(NULL);

			// Get the file name from the dialog box.
			if (SUCCEEDED(hr))
			{
				IShellItem *pItem;
				hr = pFileOpen->GetResult(&pItem);
				if (SUCCEEDED(hr))
				{
					hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

					// Display the file name to the user.
					if (SUCCEEDED(hr))
					{
						// Send to back
					}
					pItem->Release();
				}
			}
			pFileOpen->Release();
		}
		CoUninitialize();
	}
	return pszFilePath;
}

PWSTR BasicFileSave()
{
	PWSTR pszFilePath = NULL;
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	if (SUCCEEDED(hr))
	{
		IFileSaveDialog *pFileSave;

		// Create the FileOpenDialog object.
		hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL,
			IID_IFileSaveDialog, reinterpret_cast<void**>(&pFileSave));

		// Set the options on the dialog.
		DWORD dwFlags;

		// Before setting, always get the options first in order 
		// not to override existing options.
		hr = pFileSave->GetOptions(&dwFlags);

		if (SUCCEEDED(hr))
		{
			// In this case, get shell items only for file system items.
			hr = pFileSave->SetOptions(dwFlags | FOS_FORCEFILESYSTEM);

			// Set the file types to display only. 
			// Notice that this is a 1-based array.
			COMDLG_FILTERSPEC rgSpec[] =
			{
				{ L"JSON Files (*.json)", L"*.json" },
				
			};
			hr = pFileSave->SetFileTypes(ARRAYSIZE(rgSpec), rgSpec);

			// Set the default extension to be ".doc" file.
			//hr = pFileSave->SetDefaultExtension(L"txt");

			// Show the Open dialog box.
			hr = pFileSave->Show(NULL);

			// Get the file name from the dialog box.
			if (SUCCEEDED(hr))
			{
				IShellItem *pItem;
				hr = pFileSave->GetResult(&pItem);
				if (SUCCEEDED(hr))
				{
					hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

					// Display the file name to the user.
					if (SUCCEEDED(hr))
					{
						//MessageBox(NULL, pszFilePath, L"File Path", MB_OK);
						//CoTaskMemFree(pszFilePath);
					}
					pItem->Release();
				}
			}
			pFileSave->Release();
		}
		CoUninitialize();
	}
	return pszFilePath;
}

std::string WstrToStr(const std::wstring &wstr)
{
	std::string strTo;
	char *szTo = new char[wstr.length() + 1];
	szTo[wstr.size()] = '\0';
	WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, szTo, (int)wstr.length(), NULL, NULL);
	strTo = szTo;
	delete[] szTo;
	return strTo;
}

bool EndWith(const std::string &str, const std::string &suffix)
{
	return str.size() >= suffix.size() &&
		str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

std::wstring StrToWstr(const std::string& s)
{
	wstring stemp = wstring(s.begin(), s.end());
	return stemp;
}

int DisplayConfirmClearMessageBox(const string fileName)
{
	wstring ws = StrToWstr(fileName);
	ws += L" has been modified.\nDo you want to save it?";
	int msgboxID = MessageBox(
		NULL,
		ws.c_str(),
		L"Confirm to Clear",
		MB_ICONEXCLAMATION | MB_YESNO
	);

	return msgboxID;
}

int DisplayConfirmNewFileMessageBox(const string fileName)
{
	wstring ws = StrToWstr(fileName);
	ws += L" has been modified.\nDo you want to save it?";
	int msgboxID = MessageBox(
		NULL,
		ws.c_str(),
		L"Confirm Save",
		MB_ICONEXCLAMATION | MB_YESNO
	);

	return msgboxID;
}