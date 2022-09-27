#pragma once
#include <Windows.h>
#include <string>
class Window {
public:
	bool Init(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow);
	HWND GetHWND();
	static Window* Get();

	virtual void OnCreate();
	virtual void OnUpdate();
	virtual void OnDestroy();
private:
	HWND m_hwnd = NULL;
#ifdef _DEBUG
	FILE* m_conOut = stdout; //for debugging purposes
#endif
};

