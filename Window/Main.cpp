#include "Window.h"

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow) {
	if (!Window::Get()->Init(hInst, hPrevInst, lpCmdLine, nCmdShow)) { 
		MessageBox(NULL, "Window Creation Failed", "Error", NULL); 
		return -1; 
	}
	
	return 0;
}
