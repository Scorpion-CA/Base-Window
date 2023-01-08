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

	case WM_PAINT: {
		Window::Get()->OnRender();
		ValidateRect(hWnd, NULL);
		break;
	}

	case WM_SIZE: {
		UINT width = LOWORD(lParam);
		UINT height = HIWORD(lParam);
		Window::Get()->OnResize(width, height);
		break;
	}

	case WM_DISPLAYCHANGE: {
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	}

	case WM_NCHITTEST: {
		RECT rc;
		GetWindowRect(hWnd, &rc);
		
		if (MAKEPOINTS(lParam).y - rc.top <= 30 && MAKEPOINTS(lParam).x - rc.left <= 700)
			return HTCAPTION;
		break;
	}
	}

	return DefWindowProcA(hWnd, uMsg, wParam, lParam);
}

bool Window::Init(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow) {
#ifdef _DEBUG
	AllocConsole();
	if (!freopen_s(&m_conOut, "CONOUT$", "w", stdout))
		std::cout << "Window Successfully Created" << std::endl;
#endif // DEBUG

	HRESULT hr;

	hr = CreateDeviceIndependantResources();

#ifdef _DEBUG
	if (FAILED(hr)) {
		std::cout << "Failed to create Device Independant Resources" << std::endl;
		return false;
	} else {
		std::cout << "Created Device Independant Resources" << std::endl;
	}
#endif

	if (FAILED(hr)) {
		MessageBox(NULL, "Failed to create Device Independant Resources", "Error", MB_OK);
		return false;
	}

	WNDCLASS wnd = { };
	wnd.lpfnWndProc = (WNDPROC)wndProc;
	wnd.hCursor = (HCURSOR)LoadCursor(hInst, "IDC_ARROW");
	wnd.hInstance = hInst;
	wnd.lpszClassName = "WndClass";

	RegisterClass(&wnd);

	m_hwnd = CreateWindow("WndClass", "Server Manager", WS_POPUPWINDOW, 400, 200, 800, 600, NULL, NULL, hInst, NULL);
	if (!m_hwnd)
		return false;

	ShowWindow(m_hwnd, nCmdShow);

	Window::OnCreate();

	MSG msg = { };
	while (WM_QUIT != msg.message) {
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) > 0) {
			TranslateMessage(&msg);
			DispatchMessageA(&msg);

			//input dependant shit goes here
		}
		Window::OnUpdate();

		//inputless shit goes here

		SendMessage(m_hwnd, WM_PAINT, NULL, NULL);
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

void checkClick(int x, int y) {
	RECT rc;
	GetClientRect(Window::Get()->GetHWND(), &rc);
	if (x > rc.right - 50.0f && x <= rc.right && y >= 0 && y <= 30)
		PostQuitMessage(0);
	if (x >= rc.right - 100.0f && x <= rc.right - 50.0f && y >= 0 && y <= 30)
		ShowWindow(Window::Get()->GetHWND(), SW_MINIMIZE);
}

void Window::OnCreate() {
	I::Get()->Mouse->AddMouseInputHandler(MINPUT_MOVE, "Update MPos", onMove);	//adds an input handler that will update the mouse position when it moves
	I::Get()->Mouse->AddMouseInputHandler(MINPUT_LEFT_CLICK, "Exit Click Check", checkClick);
}

void Window::OnUpdate() {
	I::Get()->Key->UpdateKeys();

	if (I::Get()->Key->IsKeyDown(VK_SPACE))
		std::cout << "thing" << std::endl;
}

void Window::OnDestroy() {
#ifdef _DEBUG
	m_conOut = nullptr;
	delete m_conOut;
	FreeConsole();
#endif // DEBUG
	SafeRelease(&m_pDirect2dFactory);
	SafeRelease(&m_pRenderTarget);
	SafeRelease(&m_pDarkGrey);
	SafeRelease(&m_pLightGrey);
	SafeRelease(&m_pWhite);
	SafeRelease(&m_pRed);
}

HRESULT Window::CreateDeviceIndependantResources() {
	return D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pDirect2dFactory);
}

HRESULT Window::CreateDeviceResources() {
	HRESULT hr = S_OK;
	if (!m_pRenderTarget) {
		RECT rc;
		GetClientRect(m_hwnd, &rc);

		D2D1_SIZE_U size = D2D1::SizeU(
			rc.right - rc.left,
			rc.bottom - rc.top);

		// Create a Direct2D render target.
		hr = m_pDirect2dFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(m_hwnd, size),
			&m_pRenderTarget);

		hr = DWriteCreateFactory(
			DWRITE_FACTORY_TYPE_SHARED,
			__uuidof(m_pWriteFactory),
			reinterpret_cast<IUnknown**>(&m_pWriteFactory)
		);

		hr = m_pWriteFactory->CreateTextFormat(
			L"Segoe UI",
			NULL,
			DWRITE_FONT_WEIGHT_NORMAL,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			14,
			L"", //locale
			&m_pTextFormat
		);

		if (SUCCEEDED(hr))
		{
			// Create a gray brush.
			hr = m_pRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF(0.1f, 0.1f, 0.1f, 1.0f)),
				&m_pDarkGrey
			);
		}

		if (SUCCEEDED(hr))
		{
			// Create a gray brush.
			hr = m_pRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF(0.25f, 0.25f, 0.25f, 1.0f)),
				&m_pLightGrey
			);
		}
		if (SUCCEEDED(hr))
		{
			// Create a gray brush.
			hr = m_pRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f)),
				&m_pWhite
			);
		}

		if (SUCCEEDED(hr))
		{
			// Create a gray brush.
			hr = m_pRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF(1.0f, 0.0f, 0.0f, 1.0f)),
				&m_pRed
			);
		}

		
	}

	return hr;
}

HRESULT Window::OnRender() {
	HRESULT hr = S_OK;

	hr = CreateDeviceResources();

	if (SUCCEEDED(hr)) {
		m_pRenderTarget->BeginDraw();
		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
		m_pRenderTarget->Clear(D2D1::ColorF(0.15f, 0.15f, 0.15f, 1.0f));

		D2D1_SIZE_F rtSize = m_pRenderTarget->GetSize();

		Render();

		hr = m_pRenderTarget->EndDraw();
	}

	if (hr == D2DERR_RECREATE_TARGET)
	{
		hr = S_OK;
		DiscardDeviceResources();
	}

	return hr;
}

void Window::DiscardDeviceResources() {
	SafeRelease(&m_pRenderTarget);
	SafeRelease(&m_pDarkGrey);
	SafeRelease(&m_pLightGrey);
	SafeRelease(&m_pWhite);
	SafeRelease(&m_pRed);
}

void Window::OnResize(UINT width, UINT height) {
	if (m_pRenderTarget)
		m_pRenderTarget->Resize(D2D1::SizeU(width, height));
}

void Window::Render() {
	RECT rc;
	GetWindowRect(m_hwnd, &rc);

	POINT Mouse = I::Get()->Mouse->GetMousePos();
	GetCursorPos(&Mouse);
	Mouse.x -= rc.left;
	Mouse.y -= rc.top;

	Draw::Get()->Rect(0.0f, 0.0f, (rc.right - rc.left) - 100.0f, 30.0f, m_pDarkGrey);
	
	if (Mouse.x >= (rc.right - rc.left) - 100.0f && Mouse.x <= (rc.right - rc.left) - 50.0f && 
		Mouse.y >= 0 && Mouse.y <= 30) {	//minimize
		Draw::Get()->Rect((rc.right - rc.left) - 100.0f, 0.0f, 50.0f, 30.0f, m_pLightGrey);
	}
	Draw::Get()->Line((rc.right - rc.left) - 100.0f + 20, 15, (rc.right - rc.left) - 100.0f + 30, 15, m_pWhite, 1.5f);

	if (Mouse.x > (rc.right - rc.left) - 50.0f && Mouse.x <= (rc.right - rc.left) &&
		Mouse.y >= 0 && Mouse.y <= 30) {			//Exit
		Draw::Get()->Rect((rc.right - rc.left) - 50.0f, 0.0f, 50.0f, 30.0f, m_pRed);
		
	}
	Draw::Get()->Line((rc.right - rc.left) - 50.0f + 20, 10, (rc.right - rc.left) - 50.0f + 30, 20, m_pWhite, 1.0f);
	Draw::Get()->Line((rc.right - rc.left) - 50.0f + 20, 20, (rc.right - rc.left) - 50.0f + 30, 10, m_pWhite, 1.0f);

	Draw::Get()->Text(L"Window", 7, 5, (rc.right - rc.left) - 100.0f, 30, m_pWhite);
}

Draw* Draw::Get() {
	static Draw* self = new Draw;
	return self;
}

void Draw::Rect(float X, float Y, float Width, float Height, ID2D1SolidColorBrush* Brush) {
	Window::Get()->m_pRenderTarget->FillRectangle(D2D1::RectF(X, Y, X + Width, Y + Height), Brush);
}

void Draw::Line(float X1, float Y1, float X2, float Y2, ID2D1SolidColorBrush* Brush, float Thickness = 1.0f) {
	Window::Get()->m_pRenderTarget->DrawLine(D2D1::Point2F(X1, Y1), D2D1::Point2F(X2, Y2), Brush, Thickness);
}

void Draw::Text(std::wstring Text, float X, float Y, float Width, float Height, ID2D1SolidColorBrush* Brush) {
	Window::Get()->m_pRenderTarget->DrawTextA(Text.c_str(), Text.size(), Window::Get()->m_pTextFormat, D2D1::RectF(X, Y, X + Width, Y + Height), Brush);
}
