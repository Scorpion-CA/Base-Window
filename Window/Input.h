#pragma once
#include <string>
#include <unordered_map>
#include <functional>
#include <Windows.h>

//high key ugly but I really do not care, but there are the different mouse input flags, the ones with C in front are literally only that for readability and 
//differentiating from a dragging click, which woudl not be necessary if I was not smooth brained and click starts with C
#define MINPUT_MOVE 0
#define MINPUT_LEFT_CLICK 0xC01
#define MINPUT_RIGHT_CLICK 0xC02
#define MINPUT_LEFT_DRAG 0x001
#define MINPUT_RIGHT_DRAG 0x002
#define MINPUT_XBUTTON1 0xC20
#define MINPUT_XBUTTON2 0xC40
#define MINPUT_XBUTTON1_DRAG 0x20
#define MINPUT_XBUTTON2_DRAG 0x40
#define MINPUT_SCROLL_CLICK 0xC10
#define MINPUT_SCROLL_DRAG 0x010
#define MINPUT_SCROLL 7

struct s_HandleData {
	char vKey = 0;
	std::function<void()> Func;
};

struct s_MouseHandleData {
	int Type = MINPUT_MOVE;
	std::function<void(int, int)> Func;
};

class iKeyboard;
class iMouse;
class Window;
class I {
public:
	I();

	typedef std::function<void()> Handler;
	typedef std::function<void(int, int)> MouseHandler;
	static I* Get();
	void HandleKey(char vKey);
	void HandleMouse(int Type, int in1, int in2);
	POINT GetMousePos();
	void SetMousePos(POINT in);

	iKeyboard* Key;
	iMouse* Mouse;

	friend class iKeyboard;
	friend class iMouse;
	friend class Window;

private:
	std::unordered_map<std::string, s_HandleData> m_mHandlers;
	std::unordered_map<std::string, s_MouseHandleData> m_mMouseHandlers;
	POINT m_pMousePos = {0,0};
};

class iKeyboard : public I {
public:
	void AddKeyInputHandler(char VKey, std::string Name, I::Handler Func);
	void RemoveKeyInputHandler(std::string Name);
	void UpdateKeys();
	bool IsKeyDown(char vKey);
};

class iMouse : public I {
public:
	//note, for laziness reasons, and function inputted has to have 2 ints as arguments
	void AddMouseInputHandler(int Type, std::string Name, I::MouseHandler Func);
	void RemoveMouseInputHandler(std::string Name);
};