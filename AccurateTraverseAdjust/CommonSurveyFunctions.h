//���ò������㺯��
#pragma once
#include"Angle.h"

const double EPSILON = 1.0E-10;
const double PI = 3.1415926535897932;
// ��ƽ������������
double Dist(double X1, double Y1, double X2, double Y2);

//���أ���ռ�����������
double Dist(double X1, double Y1, double Z1,
	double X2, double Y2, double Z2);

//������ķ�λ��
CAngle  Azimuth(const double& X1, const double& Y1,
	const double& X2, const double& Y2);
//���ź���
int sgn(double x);
