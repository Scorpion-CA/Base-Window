#include "Window.h"
#include "Input.h"

#ifdef _DEBUG
#include <iostream>

#endif

LRESULT CALLBACK wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_CLOSE: {
		DestroyWindow(hWnd);
		PostQuitMessage(0);
		break;
	}
	case WM_KEYDOWN: {
		I::Get()->HandleKey(wParam);
		break;
	}
	case WM_MOUSEMOVE:{
		I::Get()->HandleMouse(wParam, MAKEPOINTS(lParam).x, MAKEPOINTS(lParam).y);
		break;
	}
	case WM_LBUTTONDOWN: 
		I::Get()->HandleMouse(MINPUT_LEFT_CLICK, MAKEPOINTS(lParam).x, MAKEPOINTS(lParam).y);
		break;
	case WM_RBUTTONDOWN:
		I::Get()->HandleMouse(MINPUT_RIGHT_CLICK, MAKEPOINTS(lParam).x, MAKEPOINTS(lParam).y);
		break;
	case WM_MBUTTONDOWN:
		I::Get()->HandleMouse(MINPUT_SCROLL_CLICK, MAKEPOINTS(lParam).x, MAKEPOINTS(lParam).y);
		break;
	case WM_XBUTTONDOWN:
		I::Get()->HandleMouse((HIWORD(wParam) == 0x001 ? MINPUT_XBUTTON1 : MINPUT_XBUTTON2), MAKEPOINTS(lParam).x, MAKEPOINTS(lParam).y);
		break;

	case WM_MOUSEWHEEL: {
		I::Get()->HandleMouse(MINPUT_SCROLL, MAKEPOINTS(wParam).y, 0);
	}
	}

	return DefWindowProcA(hWnd, uMsg, wParam, lParam);
}

bool Window::Init(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow) {
	WNDCLASS wnd = { };
	wnd.lpfnWndProc = (WNDPROC)wndProc;
	wnd.hCursor = (HCURSOR)LoadCursor(hInst, "IDC_ARROW");
	wnd.hInstance = hInst;
	wnd.lpszClassName = "WndClass";

	RegisterClass(&wnd);

	m_hwnd = CreateWindow("WndClass", "Window", WS_OVERLAPPEDWINDOW, 400, 200, 800, 600, NULL, NULL, hInst, NULL);
	if (!m_hwnd)
		return false;

	ShowWindow(m_hwnd, nCmdShow);

	Window::OnCreate();

	MSG msg = { };
	while (WM_QUIT != msg.message)
	{
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) > 0) //Or use an if statement
		{
			TranslateMessage(&msg);
			DispatchMessageA(&msg);

			//input dependant shit goes here
		}
		Window::OnUpdate();

		//inputless shit goes here

		
	}

	Window::OnDestroy();

	return true;
}

HWND Window::GetHWND() {
	return m_hwnd;
}

Window* Window::Get() {
	static Window* wnd = new Window;
	return wnd;
}

void onMove(int x, int y) {
	I::Get()->SetMousePos({ x, y });
}

void Window::OnCreate() {
#ifdef _DEBUG
	AllocConsole();
	if (!freopen_s(&m_conOut, "CONOUT$", "w", stdout))
		std::cout << "Window Successfully Created" << std::endl;
#endif // DEBUG
	I::Get()->Mouse->AddMouseInputHandler(MINPUT_MOVE, "Update MPos", onMove);	//adds an input handler that will update the mouse position when it moves
}

void Window::OnUpdate() {
	
}

void Window::OnDestroy() {
#ifdef _DEBUG
	m_conOut = nullptr;
	delete m_conOut;
	FreeConsole();
#endif // DEBUG

}
