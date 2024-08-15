#pragma once
#include "Angle.h"
//控制点等级枚举类型
enum ControlLevel
{
	FIRST, SECOND, THIRD, FORTH,FIFTH,//一至五等
	ONE, TWO,//一级导线和二级导线点
	A, B, C, D, E//GPS控制等级
};

// 控制点类型
enum ControlType
{
	HZ,//平面控制点,
	VT,//高程控制点,
	TD//三维控制点
};
//控制点类
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
	double X, Y;//坐标值
	CString strName;//控制点名
	int ID;//控制点序号
	ControlLevel emLevel;//控制点等级
	ControlType emType;//控制点类型
	double dE, dF;//误差椭圆的长半轴和短半轴
	double dAlfa;//误差椭圆长半轴的方位角
	double dMx, dMy, dMk;//点位误差
	bool isKnown;//是否已知 
	int Delta;//在平差中控制系数阵（如未知点不改变其系数，已知点系数为0）
};

//距离观测值类
class CDistObs
{
public:
	CDistObs(void)
	{
		dDist = 0;
	};
	~CDistObs(void) {};
public:
	CControlPoint* cpStart, * cpEnd;//起点和终点
	double dDist;//距离观测值
};

//方向观测值类
class CBearingObs
{
public:
	CBearingObs(void) {};
	~CBearingObs(void) {};
public:
	CControlPoint* cpStation;//测站点
	CControlPoint* cpObject;//照准点
	CAngle angleObsValue;//方向观测值
};

//角度观测值类
class CAngleObs
{
public:
	CAngleObs(void) {};
	~CAngleObs(void) {};
public:
	CControlPoint* cpStation;//测站点
	CControlPoint* cpStartObj;//起始定向点
	CControlPoint* cpEndObj;//观测点
	CAngle  ObsValue;//角度观测值
};


