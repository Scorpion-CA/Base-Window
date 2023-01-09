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

#include <string>
#include <vector>

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

class WndItem;
class Window {
public:
	Window() : m_pDirect2dFactory(NULL),
			   m_pRenderTarget(NULL),
			   m_pWriteFactory(NULL),
			   m_pTextFormat(NULL),
			   m_pRectBrush(NULL),
			   m_pLineBrush(NULL),
			   m_pTextBrush(NULL) { }

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
	friend class TextBox;

private:
	HRESULT CreateDeviceIndependantResources();
	HRESULT CreateDeviceResources();

	void DiscardDeviceResources();

	void Render(float fElapsed);

private:
	ID2D1Factory* m_pDirect2dFactory;
	IDWriteFactory* m_pWriteFactory;
	ID2D1HwndRenderTarget* m_pRenderTarget;
	IDWriteTextFormat* m_pTextFormat;

	ID2D1SolidColorBrush* m_pRectBrush;
	ID2D1SolidColorBrush* m_pLineBrush;
	ID2D1SolidColorBrush* m_pTextBrush;

	D2D1_COLOR_F m_cDarkGrey = D2D1::ColorF(0.1f, 0.1f, 0.1f, 1.0f);
	D2D1_COLOR_F m_cLightGrey = D2D1::ColorF(0.25f, 0.25f, 0.25f, 1.0f);
	D2D1_COLOR_F m_cWhite = D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f);
	D2D1_COLOR_F m_cRed = D2D1::ColorF(1.0f, 0.0f, 0.0f, 1.0f);
	HWND m_hwnd = NULL;
#ifdef _DEBUG
	FILE* m_conOut = stdout; //for debugging purposes
#endif
};

class Draw {
public:
	static Draw* Get();
	void Rect(float X, float Y, float Width, float Height, D2D1_COLOR_F Color);
	void Line(float X1, float Y1, float X2, float Y2, D2D1_COLOR_F Color, float Thickness);
	void Text(std::wstring Text, float X, float Y, float Width, float Height, D2D1_COLOR_F Color);
private:
	friend class Window;
	friend class TextBox;
private:
	RECT m_rRect;
	std::vector<WndItem*> m_vItems;
};

class WndItem {
public:
	virtual void Run() = 0;

private:
	friend class TextBox;

private:
	float m_X, m_Y;
	float m_Width, m_Height;
};

class TextBox : public WndItem {
public:
	void Run() override;
	TextBox(float X, float Y, float Width, float Height, std::string* Var, std::string Name = "", std::string PlaceHolder = "");
private:
	std::string* m_sContent;
	std::string m_sTitle;
	std::string m_sPlaceHolder;
	bool m_bIsFocused;
	float m_X, m_Y;
	float m_Width, m_Height;
};