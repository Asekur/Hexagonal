#pragma once
class Point
{
	float X;
	float Y;
public:
	//����� ������������ ���������
	void SetCoordinate(float pX, float pY)
	{
		X = pX;
		Y = pY;
	}
	//����� ��������� ���������� X
	float GetX()
	{
		return X;
	}
	//����� ��������� ���������� Y
	float GetY()
	{
		return Y;
	}
	//����������� � �����������
	Point(float pX, float pY)
		: X(pX)
		, Y(pY)
	{
	}
};

