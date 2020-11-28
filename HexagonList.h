#pragma once
#include "Hexagon.h"
#include "Point.h"
#include <vector>
#include <Windows.h>
#include <iostream>

#define AMOUNT_EDGE 6
#define M_PI 3.141592653589

class HexagonList
{
	std::vector<Hexagon> listHex;

	Hexagon hexCorner(Point center, float size) {
		Point p1 = Point(center.GetX() - cos((60 * M_PI) / 180) * size, center.GetY() - sin((60 * M_PI) / 180) * size);
		Point p2 = Point(center.GetX() + cos((60 * M_PI) / 180) * size, center.GetY() - sin((60 * M_PI) / 180) * size);
		Point p3 = Point(center.GetX() + size, center.GetY());
		Point p4 = Point(center.GetX() + cos((60 * M_PI) / 180) * size, center.GetY() + sin((60 * M_PI) / 180) * size);
		Point p5 = Point(center.GetX() - cos((60 * M_PI) / 180) * size, center.GetY() + sin((60 * M_PI) / 180) * size);
		Point p6 = Point(center.GetX() - size, center.GetY());

		return Hexagon(p1, p2, p3, p4, p5, p6, center, -1);
	}

	std::vector<Point> shiftPointList = { Point(+1, +0.5), Point(+1, -0.5), Point(0, -1),
									  Point(-1, -0.5), Point(-1, +0.5), Point(0, +1) };
	std::vector<Point> pointsThroughFirst = { Point(+1, +1.5), Point(-1, +1.5), Point(+2, 0),
											  Point(+1, -1.5), Point(-1, -1.5), Point(-2, 0) };
	std::vector<Point> pointsThroughSecond = { Point(+2, +2), Point(-2, +2), Point(+1, +2.5), Point(-1, +2.5),
											   Point(+2, -2), Point(-2, -2), Point(+1, -2.5), Point(-1, -2.5),
											   Point(-3, +0.5), Point(-3, -0.5), Point(+3, -0.5), Point(+3, +0.5) };

public:
	//раскраска
	void drawHex(Hexagon& hex, HDC hdc, int color) {
		//красный 0
		HBRUSH hBrush = NULL;
		if (color == 0) {
			hBrush = CreateSolidBrush(RGB(255, 0, 0));
			hex.SetMaster(0);
		}
		//зеленый 1
		if (color == 1) {
			hBrush = CreateSolidBrush(RGB(0, 255, 0));
			hex.SetMaster(1);
		}
		SelectObject(hdc, hBrush);
		FloodFill(hdc, hex.GetCenter().GetX(), hex.GetCenter().GetY(), RGB(0, 0, 0));
	}

	//отрисовка хексагончиков
	void drawListOfHexagons(std::vector<Hexagon> list, HDC hdc) {
		HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
		SelectObject(hdc, hPen);
		for (Hexagon hex : list)
		{
			MoveToEx(hdc, hex.GetFirst().GetX(), hex.GetFirst().GetY(), NULL);
			LineTo(hdc, hex.GetSecond().GetX(), hex.GetSecond().GetY());
			LineTo(hdc, hex.GetThird().GetX(), hex.GetThird().GetY());
			LineTo(hdc, hex.GetForth().GetX(), hex.GetForth().GetY());
			LineTo(hdc, hex.GetFifth().GetX(), hex.GetFifth().GetY());
			LineTo(hdc, hex.GetSixth().GetX(), hex.GetSixth().GetY());
			LineTo(hdc, hex.GetFirst().GetX(), hex.GetFirst().GetY());
		}
	}

	//создание списка
	std::vector<Hexagon> createList(Point center, int size) {
		std::vector<Hexagon> newList;
		int newShiftX, newShiftY;
		int shift;

		newList.push_back(hexCorner(center, size));

		for (int i = 0; i < AMOUNT_EDGE * 3; i++) {
			if (i < AMOUNT_EDGE) {
				shift = 36;
			}
			if (i >= AMOUNT_EDGE && i < AMOUNT_EDGE * 2) {
				shift = 72;
			}
			if (i >= AMOUNT_EDGE * 2) {
				shift = 108;
			}
			newShiftX = shift * shiftPointList[i % AMOUNT_EDGE].GetX() + center.GetX();
			newShiftY = shift * shiftPointList[i % AMOUNT_EDGE].GetY() + center.GetY();
			Point newCenter = Point(newShiftX, newShiftY);
			newList.push_back(hexCorner(newCenter, size));
		}

		for (int i = 0; i < AMOUNT_EDGE; i++) {
			shift = 36;
			newShiftX = shift * pointsThroughFirst[i].GetX() + center.GetX();
			newShiftY = shift * pointsThroughFirst[i].GetY() + center.GetY();
			Point newCenter = Point(newShiftX, newShiftY);
			newList.push_back(hexCorner(newCenter, size));
		}

		for (int i = 0; i < AMOUNT_EDGE * 2; i++) {
			shift = 36;
			newShiftX = shift * pointsThroughSecond[i].GetX() + center.GetX();
			newShiftY = shift * pointsThroughSecond[i].GetY() + center.GetY();
			Point newCenter = Point(newShiftX, newShiftY);
			newList.push_back(hexCorner(newCenter, size));
		}

		return newList;
	}

	//определить в каком хексагончике находится точка
	int hexagonIn (Point point, std::vector<Hexagon> &list, int color) {
		bool isIn = false;
		int i, j;
		int iterator = -1;
		for (Hexagon hex : list) {
			//массив точек из хексагона
			iterator++;
			std::vector<Point> arrP = hex.GetArrayPoints();
			for (i = 0, j = arrP.size() - 1; i < arrP.size(); j = i++)
			{
				if ((arrP[i].GetY() < point.GetY() && point.GetY() <= arrP[j].GetY() || 
					arrP[j].GetY() < point.GetY() && point.GetY() <= arrP[i].GetY())
					&& (point.GetX() > (arrP[j].GetX() - arrP[i].GetX()) * (point.GetY() - arrP[i].GetY()) /
					(arrP[j].GetY() - arrP[i].GetY()) + arrP[i].GetX())) {
					isIn = !isIn;
				}
			}
			if (isIn) {
				list[iterator] = hex;
				return iterator;
			}
		}
		return -1;
	}
};

