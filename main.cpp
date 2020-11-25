#include <windows.h>
#include <gdiplus.h>
#include "resource.h"
#include <Richedit.h>
#include <time.h>
#include "Point.h"
#include "Hexagon.h"
#include "HexagonList.h"
#include <cmath>
#include <list>
#include <vector>

#define TIMER_SECOND 1 
//отрисовка по таймеру

const SIZE MIN_WINDOW_SIZE = { 700,500 };
const COLORREF BACKGROUND_COLOR = RGB(255, 255, 255);

HexagonList newList; //работа со списком хексагончиков
std::list <Hexagon> listHex;

void createMap() {
	Point center = Point(220, 220);
	float size = 20;
	listHex = newList.createList(center, size);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message) {
	case WM_CREATE:
		createMap();
		break;
	case WM_TIMER:
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		newList.drawListOfHexagons(listHex, hdc);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	//хранение состо€ни€
	ULONG_PTR gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);

	WNDCLASSEX windClass;
	HWND handWind, re;
	MSG msg;

	LoadLibrary(TEXT("Riched32.dll"));

	windClass.cbSize = sizeof(WNDCLASSEX);
	windClass.style = CS_HREDRAW | CS_VREDRAW;
	windClass.lpfnWndProc = WndProc;
	windClass.cbClsExtra = 0; //без выделени€ дополнительных байтов
	windClass.cbWndExtra = 0;
	windClass.hInstance = hInstance; //экземпл€р окна
	windClass.hIcon = LoadIcon(GetModuleHandle(NULL), (LPCTSTR)MAIN_ICON);
	windClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	windClass.hbrBackground = CreateSolidBrush(BACKGROUND_COLOR);
	windClass.lpszMenuName = nullptr;
	windClass.lpszClassName = "Main";
	windClass.hIconSm = windClass.hIcon;
	RegisterClassEx(&windClass);

	handWind = CreateWindow("Main", "Hexagonal",
		WS_SYSMENU|WS_MINIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT,
		MIN_WINDOW_SIZE.cx, MIN_WINDOW_SIZE.cy, nullptr, nullptr, hInstance, nullptr);
	re = CreateWindowA("RICHEDIT", "Here will be a history!\n\n", WS_BORDER | WS_CHILD | WS_VISIBLE | ES_MULTILINE | WS_VSCROLL, 
		440, 80, 220, 300, handWind, 0, hInstance, 0);
	SendMessage(re, EM_SETBKGNDCOLOR, 0, RGB(255, 255, 255));
	SendMessage(re, EM_SETREADONLY, 1, 0);
	ShowWindow(handWind, nCmdShow);
	UpdateWindow(re);
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	Gdiplus::GdiplusShutdown(gdiplusToken);
	return (int)msg.wParam;
}