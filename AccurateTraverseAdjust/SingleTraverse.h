#pragma once
#include "ControlPoint.h"
#include "Angle.h"
#include <cstring>
const double Ro = 206264.806247; //һ���ȶ�Ӧ������

//��������
class CSingleTraverse
{
public:
	CSingleTraverse(void);
	~CSingleTraverse(void);
private:
	CControlPoint* m_pUnknownPoint;//δ֪������
	int m_iUnknownPointCount;      //δ֪�����
	CControlPoint* m_pKnownPoint;  //��֪������
	int m_iKnownPointCount;        //��֪����� 
	CAngleObs* m_pAngleObs;    //�Ƕȹ۲�ֵ����
	int m_iAngleObsCount;          //�Ƕȹ۲�ֵ���� 
	CDistObs* m_pDistObs;     //����۲�ֵ����
	int m_iDistObsCount;            //����۲�ֵ����
public:
	short m_nType;//���������ͣ�=���ϵ��ߣ���=�պϵ��ߣ�3=�޶�����,��=֧����
	short m_nAngleType;//�Ƕ����ͣ�������ǣ������ҽ�
public:
	void SetUnknownPointSize(int size);
	bool SetKnownPoint(int pos, const CControlPoint& CP);
	bool SetUnKnownPoint(int pos, const CControlPoint& CP);
	bool SetAngleObs(int pos, const CAngleObs& AngObs);
	bool SetDistObs(int pos, const CDistObs& DistObs);
private:
	void CoorRoughEst();//�������
	CString* SplitString(CString str, char split, int iSubStrs);//�ָ��ַ���
public:
	CAngle Azi(const CControlPoint& P1, const CControlPoint& P2);//�������㷽λ�ǣ���֪�㣩
	bool LoadObsData(const CString& strFileName);//��������
private:
	void SetKnownPointSize(short nType);//���õ��������ͣ���֪�������
public:
	//���ݵ���Ѱ�Ҳ��õ�������
	CControlPoint* SearchKnownPointUsingName(CString strName);
	CControlPoint* SearchUnknownPointUsingName(CString strName);
	CControlPoint* SearchPointUsingName(CString strName);
};

