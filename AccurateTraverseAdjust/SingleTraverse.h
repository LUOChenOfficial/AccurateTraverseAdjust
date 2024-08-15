#pragma once
#include "ControlPoint.h"
#include "Angle.h"
#include <cstring>
const double Ro = 206264.806247; //一弧度对应的秒数

//单导线类
class CSingleTraverse
{
public:
	CSingleTraverse(void);
	~CSingleTraverse(void);
private:
	CControlPoint* m_pUnknownPoint;//未知点数组
	int m_iUnknownPointCount;      //未知点个数
	CControlPoint* m_pKnownPoint;  //已知点数组
	int m_iKnownPointCount;        //已知点个数 
	CAngleObs* m_pAngleObs;    //角度观测值数组
	int m_iAngleObsCount;          //角度观测值个数 
	CDistObs* m_pDistObs;     //距离观测值数组
	int m_iDistObsCount;            //距离观测值个数
public:
	short m_nType;//单导线类型１=附合导线，２=闭合导线，3=无定向导线,４=支导线
	short m_nAngleType;//角度类型，１＝左角，２＝右角
public:
	void SetUnknownPointSize(int size);
	bool SetKnownPoint(int pos, const CControlPoint& CP);
	bool SetUnKnownPoint(int pos, const CControlPoint& CP);
	bool SetAngleObs(int pos, const CAngleObs& AngObs);
	bool SetDistObs(int pos, const CDistObs& DistObs);
private:
	void CoorRoughEst();//坐标概算
	CString* SplitString(CString str, char split, int iSubStrs);//分割字符串
public:
	CAngle Azi(const CControlPoint& P1, const CControlPoint& P2);//计算两点方位角（已知点）
	bool LoadObsData(const CString& strFileName);//加载数据
private:
	void SetKnownPointSize(short nType);//设置导线网类型（已知点个数）
public:
	//根据点名寻找并得到点数据
	CControlPoint* SearchKnownPointUsingName(CString strName);
	CControlPoint* SearchUnknownPointUsingName(CString strName);
	CControlPoint* SearchPointUsingName(CString strName);
};

