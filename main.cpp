#include <windows.h>
#include <gdiplus.h>
#include "resource.h"
#include <Richedit.h>
#include <time.h>
#include "Point.h"
#include "Hexagon.h"
#include "HexagonList.h"
#include <cmath>
#include <vector>
#include <string>

#define TIMER_SECOND 1 
#define GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))
//отрисовка по таймеру

const SIZE MIN_WINDOW_SIZE = { 700,500 };
const COLORREF BACKGROUND_COLOR = RGB(255, 255, 255);

HexagonList newList; //работа со списком хексагончиков
std::vector <Hexagon> listHex;
Point clickMouse;

//стадии игры
enum game {red, green, none};
int gameState;
bool getValue = true;

void createMap() {
	Point center = Point(220, 220);
	float size = 20;
	listHex = newList.createList(center, size);
}

void allAround(std::vector <Hexagon>&hexList, int iterator, HDC hdc, int color) {
	const float pix = 30;
	int i = -1;
	std::vector<Point> pointsAround{Point(hexList[iterator].GetCenter().GetX(), hexList[iterator].GetCenter().GetY() - pix),
		Point(hexList[iterator].GetCenter().GetX() + pix, hexList[iterator].GetCenter().GetY() - pix),
		Point(hexList[iterator].GetCenter().GetX() + pix, hexList[iterator].GetCenter().GetY() + pix),
		Point(hexList[iterator].GetCenter().GetX(), hexList[iterator].GetCenter().GetY() + pix),
		Point(hexList[iterator].GetCenter().GetX() - pix, hexList[iterator].GetCenter().GetY() + pix),
		Point(hexList[iterator].GetCenter().GetX() - pix, hexList[iterator].GetCenter().GetY() - pix)};
	for (Point point : pointsAround) {
		i = newList.hexagonIn(point, hexList, color);
		if (i != -1) {
			if (hexList[i].GetMaster() != -1 && hexList[i].GetMaster() == !color) {
				newList.drawHex(listHex[i], hdc, color);
			}
		}
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	HWND hwndButton;
	int x, y, iterator;

	switch (message) {
	case WM_CREATE:
		createMap();
		hwndButton = CreateWindow("BUTTON", "START", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
			440, 20, 60, 60, hWnd, (HMENU)1001, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
		gameState = none;
		break;
	case WM_TIMER:
		break;
	case WM_COMMAND: 
		if (wParam == 1001 && gameState == none) {
			gameState = red;
			SetWindowText((HWND)lParam, "PAUSE");
		}
		else if (wParam == 1001 && gameState != none) {
			gameState = none;
			SetWindowText((HWND)lParam, "START");
		}
		break;
	case WM_LBUTTONDOWN:
		x = LOWORD(lParam); //узнаЄм координаты
		y = HIWORD(lParam);
		clickMouse = Point(x, y);
		if (getValue) {
			switch (gameState) {
			case red: 
				gameState = green;
				break;
			case green:
				gameState = red;
				break;
			default:
				gameState = none;
				break;
			}
		}
		
	case WM_PAINT:
		InvalidateRect(hWnd, NULL, false);
		hdc = BeginPaint(hWnd, &ps);
		newList.drawListOfHexagons(listHex, hdc);
		if (gameState == red) {
			iterator = newList.hexagonIn(clickMouse, listHex, 0);
			if (listHex[iterator].GetMaster() == -1) {
				getValue = true;
				newList.drawHex(listHex[iterator], hdc, 0);
				allAround(listHex, iterator, hdc, 0);
			}
			else {
				getValue = false;
			}
		}
		if (gameState == green) {
			iterator = newList.hexagonIn(clickMouse, listHex, 1);
			if (listHex[iterator].GetMaster() == -1) {
				getValue = true;
				newList.drawHex(listHex[iterator], hdc, 1);
				allAround(listHex, iterator, hdc, 1);
			}
			else {
				getValue = false;
			}
		}
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
		440, 100, 220, 300, handWind, 0, hInstance, 0);
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