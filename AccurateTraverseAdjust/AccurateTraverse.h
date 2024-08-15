#pragma once
#include "ControlPoint.h"
#include "Angle.h"
#include"Matrix.h"
#include <cstring>
const double Ro = 206264.806247; //一弧度对应的秒数

class CAccurateTraverse
{
public:
	CAccurateTraverse(void);
	~CAccurateTraverse(void);
private:
	CControlPoint* m_pUnKnownPoint;//未知点数组
	int m_iUnKnownPointCount;      //未知点个数
	CControlPoint* m_pKnownPoint;  //已知点数组
	int m_iKnownPointCount;        //已知点个数 
	CBearingObs* m_pBearObs;    //方向观测值数组
	int m_iBearObsCount;          //方向观测值个数 
	int obsBearStationCount;//测站数
	CDistObs* m_pDistObs;     //距离观测值数组
	int m_iDistObsCount;            //距离观测值个数

public:
	bool isRightData = 1;//判断文件数据格式是否正确  
	bool LoadObsData(const CString strFileName, CString& strObsData);//加载数据
	void SetUnKnownPointSize(int size);//设置未知点个数
	bool SetKnownPoint(int pos, const CControlPoint& CP);//设置已知点数据
	bool SetUnKnownPoint(int pos, const CControlPoint& CP);//设置未知点数据
	bool SetBearObs(int pos, const CBearingObs& AngObs);//设置方向观测值
	bool SetDistObs(int pos, const CDistObs& DistObs);//设置距离观测值
	int GetUnKnownPointSize();//返回未知点个数

	//---------------------------------------------------坐标概算模块------------------------------------------------------------------
public:
	void CoorRoughEst();//坐标概算
private:
	CString* SplitString(CString str, char split, int& iSubStrs);//分割字符串
	CAngle Azi(const CControlPoint& P1, const CControlPoint& P2);//计算两点方位角（已知点）
	double HDist(const CControlPoint& P1, const CControlPoint& P2);
	//根据点名寻找并得到点数据
	CControlPoint* SearchKnownPointUsingName(CString strName);
	CControlPoint* SearchUnknownPointUsingName(CString strName);
	CControlPoint* SearchPointUsingName(CString strName);
	bool isBearObsDistKnown(CBearingObs m_pBearObs,CDistObs& dist0);//方向测段中两点距离是否已知并返回对应边长观测
	bool isAllKnown();//是否所有未知点都已计算获取

	//---------------------------------------------------严密平差模块------------------------------------------------------------------
public:
	//严密平差主函数
	void RigorousAdjust(CString strFileName);

private:
	//定权
	CMatrix Weight(double SigmaBear, double SigmaDist);

	//计算某段方向观测方程的系数和常数项
	void ComBearObsEff(const CBearingObs& BearObs, double& Li,
		double& Ai, double& Bi,
		double& Aj, double& Bj);

	//计算某段距离观测方程的系数和常数项 
	void ComDistObsEff(const CDistObs& DistObs, double& Li,
		double& Ci, double& Di,
		double& Ck, double& Dk);

	//组成误差方程,B 为系数矩阵，L为常数项向量
	void FormErrorEquations(CMatrix& B, CMatrix& L);

	//---------------------------------------------------返回成果模块------------------------------------------------------------------
public:
	void OutMatrixToFile(const CMatrix& mat, CStdioFile& sf);//矩阵输出至文件
	CControlPoint GetUnKnownPoint(int pos);//返回未知点坐标

	//---------------------------------------------------图形绘制模块------------------------------------------------------------------
public:
	void DrawControlNet(CDC* pDC, CRect& rect, double coefficient);//绘制控制网图形
private:
	void DrawTri(CDC* pDC, double x, double y, double length, double a);//绘制三角形
	double Xmin, Xmax, Ymin, Ymax;//XY最值
	void GetMaxMinXY();//得到最大最小值从而计算差值
};

