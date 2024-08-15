#include "pch.h"
#include "SingleTraverse.h"
#include "CommonSurveyFunctions.h"
#include "math.h"
#include <locale.h>
CSingleTraverse::CSingleTraverse(void)
{
	m_pKnownPoint = new CControlPoint[2];//��֪������
	m_iKnownPointCount = 2;//��֪����� 
	m_pUnknownPoint = NULL;//δ֪������
	m_iUnknownPointCount = 0;      //δ֪�����
	m_pAngleObs = NULL;    //�Ƕȹ۲�ֵ����
	m_iAngleObsCount = 0;          //�Ƕȹ۲�ֵ���� 
	m_pDistObs = NULL;     //����۲�ֵ����
	m_iDistObsCount = 0;            //����۲�ֵ����
}

CSingleTraverse::~CSingleTraverse(void)
{
	//�ͷŶ�̬�����ڴ�
	if (m_pUnknownPoint != NULL)
	{
		delete[] m_pUnknownPoint;
		m_pUnknownPoint = NULL;
	}

	if (m_pKnownPoint != NULL)
	{
		delete[] m_pKnownPoint;
		m_pKnownPoint = NULL;
	}

	if (m_pAngleObs != NULL)
	{
		delete[] m_pAngleObs;
		m_pAngleObs = NULL;
	}

	if (m_pDistObs != NULL)
	{
		delete[] m_pDistObs;
		m_pDistObs = NULL;
	}
}
//���ݵ��������ͣ�������֪�����������
void CSingleTraverse::SetKnownPointSize(short nType)
{
	m_nType = nType;

	if (m_nType == 1)//���ϵ��ߣ�4����֪��
	{
		m_pKnownPoint = new CControlPoint[4];//��֪������
		m_iKnownPointCount = 4;//��֪����� 
	}
	else if (m_nType == 2 || m_nType == 3)//�պϵ������޶����ߣ�2����֪��
	{
		m_pKnownPoint = new CControlPoint[2];//��֪������
		m_iKnownPointCount = 2;//��֪����� 
	}
	else if (m_nType == 4)//֧����
	{
		m_pKnownPoint = new CControlPoint;//��֪������
		m_iKnownPointCount = 1;//��֪����� 
	}
	else
	{
		m_pKnownPoint = NULL;  //��֪������
		m_iKnownPointCount = 0;        //��֪����� 
	}
}
//����δ֪����������ݵ������ͼ�����Ƕ������۲�ֵ����
//ע�⣺���øú�����ԭ�����ݱ�ɾ��
void CSingleTraverse::SetUnknownPointSize(int size)
{
	if (m_pUnknownPoint != NULL)
	{
		delete[] m_pUnknownPoint;
		m_pUnknownPoint = NULL;
	}

	if (m_pAngleObs != NULL)
	{
		delete[] m_pAngleObs;
		m_pAngleObs = NULL;
	}

	if (m_pDistObs != NULL)
	{
		delete[] m_pDistObs;
		m_pDistObs = NULL;
	}

	m_iUnknownPointCount = size;
	m_pUnknownPoint = new CControlPoint[size];
	if (m_nType == 1 || m_nType == 2)//�պϵ��߻򸽺ϵ���
	{
		m_pAngleObs = new CAngleObs[size + 2];
		m_iAngleObsCount = size + 2;
		m_pDistObs = new CDistObs[size + 1];
		m_iDistObsCount = size + 1;
	}
	else if (m_nType == 3)//�޶�����
	{
		m_pAngleObs = new CAngleObs[size];
		m_iAngleObsCount = size;
		m_pDistObs = new CDistObs[size + 1];
		m_iDistObsCount = size + 1;
	}
	else//֧����
	{
		m_pAngleObs = new CAngleObs[size];
		m_iAngleObsCount = size;
		m_pDistObs = new CDistObs[size];
		m_iDistObsCount = size;
	}

}
//����֪�������еĵ�pos�㸳ֵ��pos��0��ʼ
bool CSingleTraverse::SetKnownPoint(int pos, const CControlPoint& CP)
{
	if (pos >= m_iKnownPointCount) return false;
	m_pKnownPoint[pos] = CP;
	return true;
}
//��δ֪�������еĵ�pos�㸳ֵ��pos��0��ʼ
bool CSingleTraverse::SetUnKnownPoint(int pos, const CControlPoint& CP)
{
	if (pos >= m_iUnknownPointCount) return false;
	m_pUnknownPoint[pos] = CP;
	return true;
}
//�ԽǶȹ۲������еĵ�pos���۲�ֵ��ֵ��pos��0��ʼ
bool CSingleTraverse::SetAngleObs(int pos, const CAngleObs& AngObs)
{
	if (pos >= m_iAngleObsCount) return false;
	m_pAngleObs[pos] = AngObs;
	return true;
}

//�Ծ���۲������еĵ�pos���۲�ֵ��ֵ��pos��0��ʼ
bool CSingleTraverse::SetDistObs(int pos, const CDistObs& DistObs)
{
	if (pos >= m_iDistObsCount) return false;
	m_pDistObs[pos] = DistObs;
	return true;
}
//��֪�������Ƶ㣬��P1->P2�ķ�λ��
CAngle CSingleTraverse::Azi(const CControlPoint& P1, const CControlPoint& P2)
{
	CAngle angAzi;

	angAzi(RAD) = Azimuth(P1.X, P1.Y, P2.X, P2.Y);

	return angAzi;
}
//����Ƕȱպϲ�
CAngle CSingleTraverse::ComAngleClosedError(void)
{
	CAngle angClosedError;//�պϲ�Ƕȶ���

	/*  A(i+1)=A(i)+��ǣ�180
	  A(i+1)=A(i)-�ҽǣ�180
	  �������360�Ȼ�Ҫ��360*/

	if (m_nType == 1) //���ϵ���
	{
		CAngle StartBearing;//��ʼ��λ��
		CAngle EndBearing;//�յ㸽�Ϸ�λ��

		StartBearing = Azi(m_pKnownPoint[0], m_pKnownPoint[1]);
		EndBearing = Azi(m_pKnownPoint[2], m_pKnownPoint[3]);

		//�ص���·�ߵ�����ֹ�ߵķ�λ��
		for (int i = 0; i < m_iAngleObsCount; i++)
		{
			if (m_nAngleType == 1)//���
				StartBearing(DEG) = StartBearing(DEG) + m_pAngleObs[i].ObsValue(DEG) - 180;
			else//�ҽ�
				StartBearing(DEG) = StartBearing(DEG) - m_pAngleObs[i].ObsValue(DEG) + 180;

			if (StartBearing(DEG) > 360)
				StartBearing(DEG) = StartBearing(DEG) - 360;
			else if (StartBearing(DEG) < 0)
				StartBearing(DEG) = StartBearing(DEG) + 360;
		}
		angClosedError = StartBearing - EndBearing;
	}

	else if (m_nType == 2)//�պϵ���
	{
		CAngle angSum(0, DEG);
		//��һ����Ϊ���ӽǣ�������պϲ������Ƕȷ���
		for (int i = 1; i < m_iAngleObsCount; i++)
		{
			angSum = angSum + m_pAngleObs[i].ObsValue;
		}
		angClosedError(DEG) = angSum(DEG) - (m_iAngleObsCount - 2) * 180;
	}

	return angClosedError;
}

//����Ƕȱպϲ�
void CSingleTraverse::DistributeAngleError(const CAngle& AngleError)
{
	if (m_nType == 1) //���ϵ���
	{
		for (int i = 0; i < m_iAngleObsCount; i++)
		{
			m_pAngleObs[i].ObsValue(DEG) = m_pAngleObs[i].ObsValue(DEG) -
				(AngleError(DEG) / m_iAngleObsCount);
		}
	}
	else if (m_nType == 2)//�պϵ���
	{
		//��һ����Ϊ���ӽǣ�������պϲ������Ƕȷ���
		for (int i = 1; i < m_iAngleObsCount; i++)
		{
			m_pAngleObs[i].ObsValue(DEG) = m_pAngleObs[i].ObsValue(DEG) -
				(AngleError(DEG) / (m_iAngleObsCount - 1));
		}
	}
}
//�����������㼰����պϲ����
//ע�⣺fx,fy�����õķ�ʽ���ã��ѱպϲ�ݳ���
void CSingleTraverse::CoorClosedError(double& fx, double& fy)
{
	double dx, dy;
	if (m_nType == 1 || m_nType == 2)//���ϵ��߻�պϵ���
	{
		CAngle StartBearing;//��ʼ��λ��       
		StartBearing = Azi(m_pKnownPoint[0], m_pKnownPoint[1]);

		//�ص���·�ߵ�����ֹ�ߵķ�λ��
		for (int i = 0; i < m_iAngleObsCount - 1; i++)
		{
			if (m_nAngleType == 1)//���
				StartBearing(DEG) = StartBearing(DEG) + m_pAngleObs[i].ObsValue(DEG) - 180;
			else//�ҽ�
				StartBearing(DEG) = StartBearing(DEG) - m_pAngleObs[i].ObsValue(DEG) + 180;

			if (StartBearing(DEG) > 360)
				StartBearing(DEG) = StartBearing(DEG) - 360;
			else if (StartBearing(DEG) < 0)
				StartBearing(DEG) = StartBearing(DEG) + 360;

			//������������
			dx = m_pDistObs[i].dDist * cos(StartBearing(RAD));
			dy = m_pDistObs[i].dDist * sin(StartBearing(RAD));
			if (i == m_iAngleObsCount - 2)//��������һ���㣬�����պϲ�
			{
				fx = m_pAngleObs[i].cpStation->X + dx - m_pAngleObs[i].cpEndObj->X;
				fy = m_pAngleObs[i].cpStation->Y + dy - m_pAngleObs[i].cpEndObj->Y;
			}
			else//����δ֪���������
			{
				m_pAngleObs[i].cpEndObj->X = m_pAngleObs[i].cpStation->X + dx;
				m_pAngleObs[i].cpEndObj->Y = m_pAngleObs[i].cpStation->Y + dy;
			}
		}
	}
}



CString* CSingleTraverse::SplitString(CString str, char split, int iSubStrs)
{
	int iPos = 0; //�ָ��λ��
	int iNums = 0; //�ָ��������
	CString strTemp = str;
	CString strRight;
	//�ȼ������ַ���������
	while (iPos != -1)
	{
		iPos = strTemp.Find(split);
		if (iPos == -1)
		{
			break;
		}
		strRight = strTemp.Mid(iPos + 1, str.GetLength());
		strTemp = strRight;
		iNums++;
	}
	if (iNums == 0) //û���ҵ��ָ��
	{
		//���ַ����������ַ�������
		iSubStrs = 1;
		return NULL;
	}
	//���ַ�������
	iSubStrs = iNums + 1; //�Ӵ������� = �ָ������ + 1
	CString* pStrSplit;
	pStrSplit = new CString[iSubStrs];
	strTemp = str;
	CString strLeft;
	for (int i = 0; i < iNums; i++)
	{
		iPos = strTemp.Find(split);
		//���Ӵ�
		strLeft = strTemp.Left(iPos);
		//���Ӵ�
		strRight = strTemp.Mid(iPos + 1, strTemp.GetLength());
		strTemp = strRight;
		pStrSplit[i] = strLeft;
	}
	pStrSplit[iNums] = strTemp;
	return pStrSplit;
}

bool CSingleTraverse::LoadObsData(const CString& strFileName)
{
	CStdioFile sf;  //�����ļ�����

	//�Զ�����ʽ���ļ��������ʧ���򷵻�
	if (!sf.Open(strFileName, CFile::modeRead)) return false;

	CString strLine;
	BOOL bEOF = sf.ReadString(strLine);//��ȡ��һ��,����������
	m_nType = _ttoi((strLine)); //�Ѷ�ȡ�ĵ�һ���ַ���ת��Ϊ��ֵ��

	SetKnownPointSize(m_nType);//���ݵ���������������֪������������С
	//��ʼ��ȡ��֪������
	sf.ReadString(strLine);//��֪�����
	if (m_iKnownPointCount != _ttoi(strLine))
	{
		return false;
	}
	int n = 0;
	//��ȡ��������֪������
	for (int i = 0; i < m_iKnownPointCount; i++)
	{
		sf.ReadString(strLine);
		CString* pstrData = SplitString(strLine, ',', n);
		m_pKnownPoint[i].strName = pstrData[0];
		m_pKnownPoint[i].X = _tstof(pstrData[1]);
		m_pKnownPoint[i].Y = _tstof(pstrData[2]);
		delete[] pstrData;
		pstrData = NULL;
	}
	//��ʼ��ȡδ֪֪������
	sf.ReadString(strLine);//δ֪�����
	SetUnknownPointSize(_ttoi(strLine));
	//��ȡ������δ֪������
	for (int i = 0; i < m_iUnknownPointCount; i++)
	{
		sf.ReadString(strLine);
		CString* pstrData = SplitString(strLine, ',', n);
		m_pUnknownPoint[i].strName = pstrData[0];
		delete[] pstrData;
		pstrData = NULL;
	}
	//��ʼ��ȡ�Ƕȹ۲�ֵ����
	sf.ReadString(strLine);//�Ƕ�����
	m_nAngleType = _ttoi(strLine);

	sf.ReadString(strLine);//�Ƕȹ۲�ֵ����
	if (m_iAngleObsCount != _ttoi(strLine))
	{
		return false;
	}
	//��ȡ������Ƕȹ۲�ֵ����
	for (int i = 0; i < m_iAngleObsCount; i++)
	{
		sf.ReadString(strLine);
		CString* pstrData = SplitString(strLine, ',', n);
		m_pAngleObs[i].cpStation = SearchPointUsingName(pstrData[0]);
		m_pAngleObs[i].cpStartObj = SearchPointUsingName(pstrData[1]);
		m_pAngleObs[i].cpEndObj = SearchPointUsingName(pstrData[2]);
		m_pAngleObs[i].ObsValue = CAngle(_tstof(pstrData[3]), DMS);
		delete[] pstrData;
		pstrData = NULL;
	}

	//��ʼ��ȡ�߳��۲�ֵ����
	sf.ReadString(strLine);//�߳��۲�ֵ����
	if (m_iDistObsCount != _ttoi(strLine))
	{
		return false;
	}
	//��ȡ������߳��۲�ֵ����
	for (int i = 0; i < m_iDistObsCount; i++)
	{
		sf.ReadString(strLine);
		CString* pstrData = SplitString(strLine, ',', n);
		m_pDistObs[i].cpStart = SearchPointUsingName(pstrData[0]);
		m_pDistObs[i].cpEnd = SearchPointUsingName(pstrData[1]);
		m_pDistObs[i].dDist = _tstof(pstrData[2]);
		delete[] pstrData;
		pstrData = NULL;
	}
	sf.Close();
	return true;
}

//���ݵ�������֪���������ҵ����Ƶ㣬�����ظõ��ָ��
CControlPoint* CSingleTraverse::SearchKnownPointUsingName(CString strName)
{
	for (int i = 0; i < m_iKnownPointCount; i++)
	{
		if (strName == m_pKnownPoint[i].strName)
		{
			return &m_pKnownPoint[i];
		}
	}
	return NULL;
}

//���ݵ�����δ֪���������ҵ����Ƶ㣬�����ظõ��ָ��
CControlPoint* CSingleTraverse::SearchUnknownPointUsingName(CString strName)
{
	for (int i = 0; i < m_iUnknownPointCount; i++)
	{
		if (strName == m_pUnknownPoint[i].strName)
		{
			return &m_pUnknownPoint[i];
		}
	}
	return NULL;
}

//���ݵ�����δ֪�����֪���������ҵ����Ƶ㣬�����ظõ��ָ��
CControlPoint* CSingleTraverse::SearchPointUsingName(CString strName)
{
	CControlPoint* pCP = NULL;
	pCP = SearchKnownPointUsingName(strName);
	if (pCP == NULL)
	{
		pCP = SearchUnknownPointUsingName(strName);
	}
	return pCP;
}
