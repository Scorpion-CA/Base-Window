#include "Input.h"
#include <vector>
#include <Windows.h>

#ifdef _DEBUG
#include <iostream>
#endif

BYTE m_keys[256] = { 0 };		//this makes me sick but it won't work if I put as a member variable for some reason

I* I::Get() {
	static I* self = new I;
	return self;
}

I::I() {
	

#ifdef _DEBUG
	std::cout << "Input System Initialized" << std::endl;
#endif DEBUG
}

void I::HandleKey(char vKey) {
	for (auto i : m_mHandlers)
		if (i.second.vKey == vKey)
			i.second.Func();
}

void I::HandleMouse(int Type, int in1, int in2) {
	for (auto i : m_mMouseHandlers)
		if (i.second.Type == Type)
			i.second.Func(in1, in2);
}

void iKeyboard::AddKeyInputHandler(char vKey, std::string Name, I::Handler Func) {
	s_HandleData data = { vKey, Func };
	I::Get()->m_mHandlers[Name] = data;
}

void iKeyboard::RemoveKeyInputHandler(std::string Name) {
	I::Get()->m_mHandlers.erase("Name");
}

bool iKeyboard::IsKeyDown(char vKey) {
	return m_keys[vKey] > 1;
}

void iKeyboard::UpdateKeys() {
	memset(m_keys, 0, sizeof(256));
	if (GetKeyboardState(m_keys))
		return;		//literally just here to shut the warnings up
}

void iMouse::AddMouseInputHandler(int Type, std::string Name, I::MouseHandler Func) {
	s_MouseHandleData data = { Type, Func };
	I::Get()->m_mMouseHandlers[Name] = data;
}

void iMouse::RemoveMouseInputHandler(std::string Name) {
	I::Get()->m_mMouseHandlers.erase("Name");
}

void I::SetMousePos(POINT in) {
	m_pMousePos = in;
}

POINT I::GetMousePos() {
	return m_pMousePos;
}
