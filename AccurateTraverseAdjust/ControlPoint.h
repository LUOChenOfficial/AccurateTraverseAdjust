#pragma once
#include "Angle.h"
//���Ƶ�ȼ�ö������
enum ControlLevel
{
	FIRST, SECOND, THIRD, FORTH,FIFTH,//һ�����
	ONE, TWO,//һ�����ߺͶ������ߵ�
	A, B, C, D, E//GPS���Ƶȼ�
};

// ���Ƶ�����
enum ControlType
{
	HZ,//ƽ����Ƶ�,
	VT,//�߳̿��Ƶ�,
	TD//��ά���Ƶ�
};
//���Ƶ���
class CControlPoint
{
public:
	CControlPoint(void)
	{
		X = Y = 0;
		strName = _T("");
		emLevel = FORTH;
		emType = HZ;
		dE = dF = 0;
		dAlfa = 0;
		dMx = dMy = dMk = 0;
	};
	~CControlPoint(void) {};
public:
	double X, Y;//����ֵ
	CString strName;//���Ƶ���
	int ID;//���Ƶ����
	ControlLevel emLevel;//���Ƶ�ȼ�
	ControlType emType;//���Ƶ�����
	double dE, dF;//�����Բ�ĳ�����Ͷ̰���
	double dAlfa;//�����Բ������ķ�λ��
	double dMx, dMy, dMk;//��λ���
	bool isKnown;//�Ƿ���֪ 
	int Delta;//��ƽ���п���ϵ������δ֪�㲻�ı���ϵ������֪��ϵ��Ϊ0��
};

//����۲�ֵ��
class CDistObs
{
public:
	CDistObs(void)
	{
		dDist = 0;
	};
	~CDistObs(void) {};
public:
	CControlPoint* cpStart, * cpEnd;//�����յ�
	double dDist;//����۲�ֵ
};

//����۲�ֵ��
class CBearingObs
{
public:
	CBearingObs(void) {};
	~CBearingObs(void) {};
public:
	CControlPoint* cpStation;//��վ��
	CControlPoint* cpObject;//��׼��
	CAngle angleObsValue;//����۲�ֵ
};

//�Ƕȹ۲�ֵ��
class CAngleObs
{
public:
	CAngleObs(void) {};
	~CAngleObs(void) {};
public:
	CControlPoint* cpStation;//��վ��
	CControlPoint* cpStartObj;//��ʼ�����
	CControlPoint* cpEndObj;//�۲��
	CAngle  ObsValue;//�Ƕȹ۲�ֵ
};


