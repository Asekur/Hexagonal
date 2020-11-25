#pragma once
class Point
{
	float X;
	float Y;
public:
	//метод установления координат
	void SetCoordinate(float pX, float pY)
	{
		X = pX;
		Y = pY;
	}
	//метод получения координаты X
	float GetX()
	{
		return X;
	}
	//метод получения координаты Y
	float GetY()
	{
		return Y;
	}
	//конструктор с параметрами
	Point(float pX, float pY)
		: X(pX)
		, Y(pY)
	{
	}
};

