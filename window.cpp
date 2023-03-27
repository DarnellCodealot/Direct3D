#include "window.h"

Window::WindowClass Window::WindowClass::wndClass;

Window::WindowClass::WindowClass()
	:
	hInst(GetModuleHandle(nullptr))
{
	//Register window class
    WNDCLASSEX wc = {0};//this is a structure that will hold information of our window class
    wc.cbSize = sizeof(wc);//this guy needs to know the size of the structure
    wc.style = CS_OWNDC;//style of the window
    wc.lpfnWndProc = Window::WndProc;//the window procedure
    wc.cbClsExtra = 0;//extra bytes for our class 
    wc.cbWndExtra = 0;//extra bytes for out window
    wc.hInstance = GetInstance();//this is self explanatory
    wc.hIcon = nullptr;//icon
    wc.hCursor = nullptr;//cursor
    wc.hbrBackground = nullptr;//i do not know what this is honestly
    wc.lpszMenuName = nullptr;//menu
    wc.lpszClassName = GetName();//class name
    wc.hIconSm = nullptr;//stands for Icon Small and i think its supposed to be a smaller version of your icon(the one you see when you open the program)
    RegisterClassEx( &wc );//here we register the class
}

Window::WindowClass::~WindowClass()
{
	UnregisterClass( wndClassName,GetInstance() );
}

const wchar_t* Window::WindowClass::GetName()
{
	return wndClassName;
}

HINSTANCE Window::WindowClass::GetInstance()
{
	return wndClass.hInst;
}

Window::Window(int width,int height, std::string name) : msg(MSG{0})
{
	// calculate window size based on desired client region size
	RECT wr;
	wr.left = 100;
	wr.right = width + wr.left;
	wr.top = 100;
	wr.bottom = height + wr.top;
	AdjustWindowRect( &wr,WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,FALSE );
	//convert from char to wchar_t
	std::wstring wc(name.size(), L'#');
	mbstowcs(&wc[0], name.c_str(), name.size());
	// create window & get hWnd
	hWnd = CreateWindow(
		WindowClass::GetName(),wc.c_str(),
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT,CW_USEDEFAULT,wr.right - wr.left,wr.bottom - wr.top,
		nullptr,nullptr,WindowClass::GetInstance(),nullptr
	);
	// show window
	ShowWindow( hWnd,SW_SHOWDEFAULT );
}

Window::~Window()
{
	DestroyWindow( hWnd );
}

bool Window::Run() {
	if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return this->msg.message != WM_QUIT;
}

LRESULT CALLBACK Window::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg){
        case WM_CLOSE:
            PostQuitMessage(69);
            break;
        case WM_KEYDOWN:
			break;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}