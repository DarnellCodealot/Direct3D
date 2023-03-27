#pragma once
#include <windows.h>
#include <cstdlib>
#include <string>

#define _CRT_SECURE_NO_WARNINGS

class Window{
	class WindowClass{
		public:
		static const wchar_t* GetName();
		static HINSTANCE GetInstance();
		private:
		WindowClass();
		~WindowClass();
		static constexpr const wchar_t* wndClassName = L"MyWindowMyWindow";
		static WindowClass wndClass;
		HINSTANCE hInst;
	};
	public:
	Window( int width,int height, std::string name );
	~Window();
	bool Run();
	private:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	MSG msg;
	public:
	HWND hWnd;
};