#include "pch.h"
#include "CommonSurveyFunctions.h"
// ��ƽ������������
double Dist(double X1, double Y1, double X2, double Y2)
{
	double d;
	d = sqrt((X2 - X1) * (X2 - X1) + (Y2 - Y1) * (Y2 - Y1));
	return d;
}

//���أ���ռ�����������
double Dist(double X1, double Y1, double Z1,
	double X2, double Y2, double Z2)
{
	double d;
	d = sqrt((X2 - X1) * (X2 - X1) + (Y2 - Y1) * (Y2 - Y1) + (Z2 - Z1) * (Z2 - Z1));
	return d;
}

//������ķ�λ��
CAngle  Azimuth(const double& X1, const double& Y1,
	const double& X2, const double& Y2)
{
	double dx, dy;
	CAngle angle;
	dx = X2 - X1;
	dy = Y2 - Y1 + EPSILON;
	angle(RAD)= PI - sgn(dy) * PI / 2 - atan(dx / dy);
	return angle;
}
//���ź���
int sgn(double x)
{
	if (x >= 0)
		return(1);//�Ǹ��򷵻�1
	else
		return(-1);//
}
