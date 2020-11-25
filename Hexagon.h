#pragma once
#include "Point.h"
class Hexagon
{
	Point p1;
	Point p2;
	Point p3;
	Point p4;
	Point p5;
	Point p6;
	int master;

public:
	Point GetFirst()
	{
		return p1;
	}
	Point GetSecond()
	{
		return p2;
	}
	Point GetThird()
	{
		return p3;
	}
	Point GetForth()
	{
		return p4;
	}
	Point GetFifth()
	{
		return p5;
	}
	Point GetSixth()
	{
		return p6;
	}
	int GetPlayer() {
		//0 - красный
		//1 - зелёный
		//-1 - не занятый
		return master;
	}
	//конструктор с параметрами Hexagon
	Hexagon(Point p1, Point p2, Point p3, Point p4, Point p5, Point p6, int master)
		: p1(p1)
		, p2(p2)
		, p3(p3)
		, p4(p4)
		, p5(p5)
		, p6(p6)
		, master(master)
	{
	}
};

