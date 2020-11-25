#pragma once
#include "Hexagon.h"
#include "Point.h"
#include <vector>
#include <list>
#include <Windows.h>

#define AMOUNT_EDGE 6
#define M_PI 3.141592653589

class HexagonList
{
	std::list<Hexagon> listHex;

	Hexagon hexCorner(Point center, float size) {
		Point p1 = Point(center.GetX() - cos((60 * M_PI) / 180) * size, center.GetY() - sin((60 * M_PI) / 180) * size);
		Point p2 = Point(center.GetX() + cos((60 * M_PI) / 180) * size, center.GetY() - sin((60 * M_PI) / 180) * size);
		Point p3 = Point(center.GetX() + size, center.GetY());
		Point p4 = Point(center.GetX() + cos((60 * M_PI) / 180) * size, center.GetY() + sin((60 * M_PI) / 180) * size);
		Point p5 = Point(center.GetX() - cos((60 * M_PI) / 180) * size, center.GetY() + sin((60 * M_PI) / 180) * size);
		Point p6 = Point(center.GetX() - size, center.GetY());

		return Hexagon(p1, p2, p3, p4, p5, p6, -1);
	}

	std::vector<Point> shiftPointList = { Point(+1, +0.5), Point(+1, -0.5), Point(0, -1),
									  Point(-1, -0.5), Point(-1, +0.5), Point(0, +1) };
	std::vector<Point> pointsThroughFirst = { Point(+1, +1.5), Point(-1, +1.5), Point(+2, 0),
											  Point(+1, -1.5), Point(-1, -1.5), Point(-2, 0) };
	std::vector<Point> pointsThroughSecond = { Point(+2, +2), Point(-2, +2), Point(+1, +2.5), Point(-1, +2.5),
											   Point(+2, -2), Point(-2, -2), Point(+1, -2.5), Point(-1, -2.5),
											   Point(-3, +0.5), Point(-3, -0.5), Point(+3, -0.5), Point(+3, +0.5) };

public:
	//отрисовка хексагончиков
	void drawListOfHexagons(std::list<Hexagon> list, HDC hdc) {
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
	std::list<Hexagon> createList(Point center, int size) {
		std::list<Hexagon> newList;
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

};

