#pragma once
#include "ControlPoint.h"
#include "Angle.h"
#include"Matrix.h"
#include <cstring>
const double Ro = 206264.806247; //һ���ȶ�Ӧ������

class CAccurateTraverse
{
public:
	CAccurateTraverse(void);
	~CAccurateTraverse(void);
private:
	CControlPoint* m_pUnKnownPoint;//δ֪������
	int m_iUnKnownPointCount;      //δ֪�����
	CControlPoint* m_pKnownPoint;  //��֪������
	int m_iKnownPointCount;        //��֪����� 
	CBearingObs* m_pBearObs;    //����۲�ֵ����
	int m_iBearObsCount;          //����۲�ֵ���� 
	int obsBearStationCount;//��վ��
	CDistObs* m_pDistObs;     //����۲�ֵ����
	int m_iDistObsCount;            //����۲�ֵ����

public:
	bool isRightData = 1;//�ж��ļ����ݸ�ʽ�Ƿ���ȷ  
	bool LoadObsData(const CString strFileName, CString& strObsData);//��������
	void SetUnKnownPointSize(int size);//����δ֪�����
	bool SetKnownPoint(int pos, const CControlPoint& CP);//������֪������
	bool SetUnKnownPoint(int pos, const CControlPoint& CP);//����δ֪������
	bool SetBearObs(int pos, const CBearingObs& AngObs);//���÷���۲�ֵ
	bool SetDistObs(int pos, const CDistObs& DistObs);//���þ���۲�ֵ
	int GetUnKnownPointSize();//����δ֪�����

	//---------------------------------------------------�������ģ��------------------------------------------------------------------
public:
	void CoorRoughEst();//�������
private:
	CString* SplitString(CString str, char split, int& iSubStrs);//�ָ��ַ���
	CAngle Azi(const CControlPoint& P1, const CControlPoint& P2);//�������㷽λ�ǣ���֪�㣩
	double HDist(const CControlPoint& P1, const CControlPoint& P2);
	//���ݵ���Ѱ�Ҳ��õ�������
	CControlPoint* SearchKnownPointUsingName(CString strName);
	CControlPoint* SearchUnknownPointUsingName(CString strName);
	CControlPoint* SearchPointUsingName(CString strName);
	bool isBearObsDistKnown(CBearingObs m_pBearObs,CDistObs& dist0);//����������������Ƿ���֪�����ض�Ӧ�߳��۲�
	bool isAllKnown();//�Ƿ�����δ֪�㶼�Ѽ����ȡ

	//---------------------------------------------------����ƽ��ģ��------------------------------------------------------------------
public:
	//����ƽ��������
	void RigorousAdjust(CString strFileName);

private:
	//��Ȩ
	CMatrix Weight(double SigmaBear, double SigmaDist);

	//����ĳ�η���۲ⷽ�̵�ϵ���ͳ�����
	void ComBearObsEff(const CBearingObs& BearObs, double& Li,
		double& Ai, double& Bi,
		double& Aj, double& Bj);

	//����ĳ�ξ���۲ⷽ�̵�ϵ���ͳ����� 
	void ComDistObsEff(const CDistObs& DistObs, double& Li,
		double& Ci, double& Di,
		double& Ck, double& Dk);

	//�������,B Ϊϵ������LΪ����������
	void FormErrorEquations(CMatrix& B, CMatrix& L);

	//---------------------------------------------------���سɹ�ģ��------------------------------------------------------------------
public:
	void OutMatrixToFile(const CMatrix& mat, CStdioFile& sf);//����������ļ�
	CControlPoint GetUnKnownPoint(int pos);//����δ֪������

	//---------------------------------------------------ͼ�λ���ģ��------------------------------------------------------------------
public:
	void DrawControlNet(CDC* pDC, CRect& rect, double coefficient);//���ƿ�����ͼ��
private:
	void DrawTri(CDC* pDC, double x, double y, double length, double a);//����������
	double Xmin, Xmax, Ymin, Ymax;//XY��ֵ
	void GetMaxMinXY();//�õ������Сֵ�Ӷ������ֵ
};

