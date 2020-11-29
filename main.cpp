#define _CRT_SECURE_NO_WARNINGS
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

#define TIMER_ID 1 
#define GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))
//отрисовка по таймеру

const SIZE MIN_WINDOW_SIZE = { 700,500 };
const COLORREF BACKGROUND_COLOR = RGB(255, 255, 255);

HexagonList newList; //работа со списком хексагончиков
std::vector <Hexagon> listHex;
std::pair<int, int> gamersHex(0, 0);
Point clickMouse;
HWND reTime, re;

//стадии игры
enum game {red, green, lastred, lastgreen, none, end};
int gameState, valueSeconds = -1;
bool stopped = true;

void sort() {
	Hexagon tmp = listHex[0];
	for (int i = 0; i < listHex.size(); i++) {
		for (int j = (listHex.size() - 1); j >= (i + 1); j--) {
			if (listHex[j].GetCenter().GetX() < listHex[j - 1].GetCenter().GetX()) {
				tmp = listHex[j];
				listHex[j] = listHex[j - 1];
				listHex[j - 1] = tmp;
			}
		}
	}
	for (int i = 0; i < listHex.size(); i++) {
		for (int j = (listHex.size() - 1); j >= (i + 1); j--) {
			if (listHex[j].GetCenter().GetY() < listHex[j - 1].GetCenter().GetY()) {
				tmp = listHex[j];
				listHex[j] = listHex[j - 1];
				listHex[j - 1] = tmp;
			}
		}
	}
}

void createMap() {
	Point center = Point(220, 220);
	float size = 20;
	listHex = newList.createList(center, size);
	//сортировка листа сверху вниз
	sort();
}

void winnerView() {
	char buf[10000];
	std::string addToBuf;
	GetWindowText(re, buf, sizeof(buf));
	if (gamersHex.first > gamersHex.second) {
		addToBuf = "\nRed won. Score:";
	}
	if (gamersHex.first < gamersHex.second) {
		addToBuf = "\nGreen won. Score:";
	}
	if (gamersHex.first == gamersHex.second) {

	}
	std::string result = "\nRed: " + std::to_string(gamersHex.first) + ". Green : " + std::to_string(gamersHex.second) +".\n";
	addToBuf += result;
	strcat(buf, addToBuf.c_str());
	SetWindowText(re, buf);
}

void fillRichEdit(int iterator) {
	char buf[10000];
	std::string addToBuf;
	GetWindowText(re, buf, sizeof(buf));
	if (gameState == red) {
		addToBuf = "\nRed goes to:\nHex є" + std::to_string(iterator) + "\n";
	}
	if (gameState == green) {
		addToBuf = "\nGreen goes to:\nHex є" + std::to_string(iterator) + "\n";
	}
	strcat(buf, addToBuf.c_str());
	SetWindowText(re, buf);
}

void allAround(std::vector <Hexagon>&hexList, int iterator, HDC hdc) {
	const float pix = 30;
	int i = -1;
	std::vector<Point> pointsAround{Point(hexList[iterator].GetCenter().GetX(), hexList[iterator].GetCenter().GetY() - pix),
		Point(hexList[iterator].GetCenter().GetX() + pix, hexList[iterator].GetCenter().GetY() - pix),
		Point(hexList[iterator].GetCenter().GetX() + pix, hexList[iterator].GetCenter().GetY() + pix),
		Point(hexList[iterator].GetCenter().GetX(), hexList[iterator].GetCenter().GetY() + pix),
		Point(hexList[iterator].GetCenter().GetX() - pix, hexList[iterator].GetCenter().GetY() + pix),
		Point(hexList[iterator].GetCenter().GetX() - pix, hexList[iterator].GetCenter().GetY() - pix)};
	for (Point point : pointsAround) {
		i = newList.hexagonIn(point, hexList, gameState);
		if (i != -1) {
			if (hexList[i].GetMaster() != -1 && hexList[i].GetMaster() == !gameState) {
				newList.drawHex(listHex[i], hdc, gameState);
			}
		}
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	HWND hwndButton;
	CHARFORMAT2 cfm;
	PARAFORMAT2 paraformat;
	COLORREF color = RGB(255, 255, 255);
	int x, y, iterator;

	switch (message) {
	case WM_CREATE:
		createMap();
		hwndButton = CreateWindow("BUTTON", "START", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
			440, 20, 60, 60, hWnd, (HMENU)1001, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
		re = CreateWindowA("RICHEDIT", "Here will be a history!\n", WS_BORDER | WS_CHILD | WS_VISIBLE | ES_MULTILINE | WS_VSCROLL,
			440, 100, 220, 300, hWnd, (HMENU)1002, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), 0);
		reTime = CreateWindowA("RICHEDIT", "Green's turn:\n20", WS_BORDER | WS_CHILD | WS_VISIBLE | ES_MULTILINE ,
			510, 20, 150, 60, hWnd, (HMENU)1003, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), 0);
		cfm.cbSize = sizeof(cfm);
		cfm.dwMask = CFM_PROTECTED | CFM_COLOR;
		cfm.dwEffects = CFE_PROTECTED;
		cfm.crTextColor = color;
		paraformat.cbSize = sizeof(PARAFORMAT2);
		paraformat.dwMask = PFM_ALIGNMENT;
		paraformat.wAlignment = PFA_CENTER;
		//сообщени€ дл€ красивого отображени€
		SendMessage(re, EM_SETPARAFORMAT, (WPARAM)SCF_ALL, (LPARAM)& paraformat);
		SendMessage(re, EM_SETBKGNDCOLOR, 0, RGB(255, 255, 255));
		SendMessage(re, EM_SETREADONLY, 1, 0);
		SendMessage(re, EM_LIMITTEXT, (WPARAM)-1, (LPARAM)0); // убираю по умолчанию 64 б
		SendMessage(reTime, EM_SETPARAFORMAT, (WPARAM)SCF_ALL, (LPARAM)& paraformat);
		SendMessage(reTime, EM_SETCHARFORMAT, (WPARAM)SCF_ALL, (LPARAM)& cfm);
		SendMessage(reTime, EM_SETBKGNDCOLOR, 0, RGB(0, 0, 0));
		SendMessage(reTime, EM_SETREADONLY, 1, 0);

		SetTimer(hWnd, TIMER_ID, 1000, NULL);
		gameState = none;
		break;
	case WM_TIMER: {
		std::string buffer;
		if (valueSeconds == 0) {
			if (gameState == green) {
				buffer = "Game end.\nLoser: green.\nWinner: red";
			}
			else if (gameState == red) {
				buffer = "Game end.\nLoser: red.\nWinner: green";
			}
			SetWindowText(reTime, buffer.c_str());
			gameState = end;
			KillTimer(hWnd, TIMER_ID);
		}
		if (valueSeconds > 0 && !stopped) {
			valueSeconds--;
			if (gameState == green) {
				buffer = "Green's turn:\n" + std::to_string(valueSeconds);
			}
			else if (gameState == red) {
				buffer = "Red's turn:\n" + std::to_string(valueSeconds);
			}
			SetWindowText(reTime, buffer.c_str());
		}
		if (gameState == end) {
			if (gamersHex.first > gamersHex.second) {
				buffer = "Game end.\nLoser: green.\nWinner: red";
			}
			else if (gamersHex.first < gamersHex.second) {
				buffer = "Game end.\nLoser: red.\nWinner: green";
			}
			else if (gamersHex.first == gamersHex.second) {
				buffer = "Game end.\nFriendship won :)";
			}
			SetWindowText(reTime, buffer.c_str());
			KillTimer(hWnd, TIMER_ID);
		}
		break; 
	}
	case WM_COMMAND: 
		if (wParam == 1001 && (gameState == none || gameState == lastgreen || gameState == lastred) ) {
			if (gameState == none) {
				gameState = green;
				valueSeconds = 20;
			}
			if (gameState == lastgreen) {
				gameState = green;
			}
			if (gameState == lastred) {
				gameState = red;
			}
			stopped = false;
			SetWindowText((HWND)lParam, "PAUSE");
			SetTimer(hWnd, TIMER_ID, 1000, NULL);
		}
		else if (wParam == 1001 && gameState != none) {
			if (gameState == red) {
				gameState = lastred;
			}
			if (gameState == green) {
				gameState = lastgreen;
			}
			stopped = true;
			SetWindowText((HWND)lParam, "START");
			SetWindowText(reTime, "Game stopped");
			KillTimer(hWnd, TIMER_ID);
		}
		break;
	case WM_LBUTTONDOWN:
		x = LOWORD(lParam); //узнаЄм координаты
		y = HIWORD(lParam);
		clickMouse = Point(x, y);
		
	case WM_PAINT:
		InvalidateRect(hWnd, NULL, false);
		hdc = BeginPaint(hWnd, &ps);
		newList.drawListOfHexagons(listHex, hdc);
		if (gameState == red) {
			iterator = newList.hexagonIn(clickMouse, listHex, 0);
			if (listHex[iterator].GetMaster() == -1) {
				valueSeconds = 20;
				newList.drawHex(listHex[iterator], hdc, 0);
				allAround(listHex, iterator, hdc);
				fillRichEdit(iterator);
				gameState = green;
			}
			else {
				gameState = red;
			}
		}
		else {
			if (gameState == green) {
				iterator = newList.hexagonIn(clickMouse, listHex, 1);
				if (listHex[iterator].GetMaster() == -1) {
					valueSeconds = 20;
					newList.drawHex(listHex[iterator], hdc, 1);
					allAround(listHex, iterator, hdc);
					fillRichEdit(iterator);
					gameState = red;
				}
				else {
					gameState = green;
				}
			}
		}
		//количество оставшихс€ неприсвоенных
		gamersHex = newList.valueNonFreeHexagons(listHex);
		if ((gamersHex.first + gamersHex.second) == listHex.size()) {
			gameState = end;
			winnerView();
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
	HWND handWind;
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
	ShowWindow(handWind, nCmdShow);
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	Gdiplus::GdiplusShutdown(gdiplusToken);
	return (int)msg.wParam;
}