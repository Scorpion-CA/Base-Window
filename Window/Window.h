#pragma once
#include <windows.h>

#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <wchar.h>
#include <math.h>

#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>
#include <string>

#pragma comment(lib, "D2D1.lib")
#pragma comment(lib, "DWrite.lib")

template<class Interface>
inline void SafeRelease(
	Interface** ppInterfaceToRelease)
{
	if (*ppInterfaceToRelease != NULL)
	{
		(*ppInterfaceToRelease)->Release();
		(*ppInterfaceToRelease) = NULL;
	}
}

#ifndef Assert
#if defined( DEBUG ) || defined( _DEBUG )
#define Assert(b) do {if (!(b)) {OutputDebugStringA("Assert: " #b "\n");}} while(0)
#else
#define Assert(b)
#endif //DEBUG || _DEBUG
#endif

#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

class Window {
public:
	Window() : m_pDirect2dFactory(NULL),
			   m_pRenderTarget(NULL),
			   m_pWriteFactory(NULL),
			   m_pTextFormat(NULL),
			   m_pDarkGrey(NULL),
			   m_pLightGrey(NULL),
			   m_pWhite(NULL),
			   m_pRed(NULL) {}

	bool Init(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow);
	HWND GetHWND();
	static Window* Get();

	virtual void OnCreate();
	virtual void OnUpdate();
	virtual void OnDestroy();

	HRESULT OnRender();

	void OnResize(UINT width, UINT height);
private:
	friend class Draw;
private:
	HRESULT CreateDeviceIndependantResources();
	HRESULT CreateDeviceResources();

	void DiscardDeviceResources();

	void Render();
private:
	ID2D1Factory* m_pDirect2dFactory;
	IDWriteFactory* m_pWriteFactory;
	ID2D1HwndRenderTarget* m_pRenderTarget;
	IDWriteTextFormat* m_pTextFormat;
	ID2D1SolidColorBrush* m_pDarkGrey;
	ID2D1SolidColorBrush* m_pLightGrey;
	ID2D1SolidColorBrush* m_pWhite;
	ID2D1SolidColorBrush* m_pRed;
	HWND m_hwnd = NULL;
#ifdef _DEBUG
	FILE* m_conOut = stdout; //for debugging purposes
#endif
};

class Draw {
public:
	static Draw* Get();
	void Rect(float X, float Y, float Width, float Height, ID2D1SolidColorBrush* Brush);
	void Line(float X1, float Y1, float X2, float Y2, ID2D1SolidColorBrush* Brush, float Thickness);
	void Text(std::wstring Text, float X, float Y, float Width, float Height, ID2D1SolidColorBrush* Brush);
private:
	friend class Window;
private:
	RECT m_rRect;
};