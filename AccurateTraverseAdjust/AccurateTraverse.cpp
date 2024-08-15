#include "pch.h"
#include "AccurateTraverse.h"
#include "CommonSurveyFunctions.h"
#include "math.h"
#include <locale.h>

CAccurateTraverse::CAccurateTraverse(void)
{
	m_pKnownPoint = new CControlPoint[2];//��֪������
	m_iKnownPointCount = 2;//��֪����� 
	m_pUnKnownPoint = NULL;//δ֪������
	m_iUnKnownPointCount = 0;      //δ֪�����
	m_pBearObs = NULL;    //����۲�ֵ����
	m_iBearObsCount = 0;          //����۲�ֵ���� 
	m_pDistObs = NULL;     //����۲�ֵ����
	m_iDistObsCount = 0;            //����۲�ֵ����
}

CAccurateTraverse::~CAccurateTraverse(void)
{
	//�ͷŶ�̬�����ڴ�
	if (m_pUnKnownPoint != NULL)
	{
		delete[] m_pUnKnownPoint;
		m_pUnKnownPoint = NULL;
	}

	if (m_pKnownPoint != NULL)
	{
		delete[] m_pKnownPoint;
		m_pKnownPoint = NULL;
	}

	if (m_pBearObs != NULL)
	{
		delete[] m_pBearObs;
		m_pBearObs = NULL;
	}

	if (m_pDistObs != NULL)
	{
		delete[] m_pDistObs;
		m_pDistObs = NULL;
	}
}

//��������
bool CAccurateTraverse::LoadObsData(const CString strFileName, CString& strObsData)
{
	//��ȡ����
	CStdioFile sf;
	if (!sf.Open(strFileName, CFile::modeRead)) return 0;
	CString strLine;
	strLine.Empty();
	BOOL bEOF = sf.ReadString(strLine);//���Ƶ�
	//�ļ����ݸ�ʽ����ͷ��أ������ 
	if (strLine != "[ControlPoint]")
	{
		isRightData = 0;
		return 0;
	}
	bEOF = sf.ReadString(strLine);//��֪�����
	int n = 0;
	//��ȡ��������֪������
	for (int i = 0; i < 2; i++)
	{
		sf.ReadString(strLine);
		CString* pstrData = SplitString(strLine, ',', n);
		for (int j = 0; j < n; j++)
			pstrData[j].Trim();
		m_pKnownPoint[i].strName = pstrData[0];
		m_pKnownPoint[i].emLevel = ControlLevel(_ttoi(pstrData[1]));
		m_pKnownPoint[i].X = _tstof(pstrData[2]);
		m_pKnownPoint[i].Y = _tstof(pstrData[3]);
		m_pKnownPoint[i].isKnown = 1;
		m_pKnownPoint[i].Delta = 0;
		delete[] pstrData;
		pstrData = NULL;
	}
	//��ʼ��ȡδ֪֪������
	sf.ReadString(strLine);//δ֪��
	sf.ReadString(strLine);//δ֪�����
	strLine.Trim();
	SetUnKnownPointSize(_ttoi(strLine));
	sf.ReadString(strLine);
	CString* pstrData = SplitString(strLine, ',', n);
	for (int i = 0; i < n; i++)
	{
		pstrData[i].Trim();
		m_pUnKnownPoint[i].ID = i;
		m_pUnKnownPoint[i].strName = pstrData[i];
		m_pUnKnownPoint[i].isKnown = 0;
		m_pUnKnownPoint[i].Delta = 1;
	}
	delete[] pstrData;
	pstrData = NULL;
	//��ʼ��ȡ�۲�߳��۲�ֵ����
	sf.ReadString(strLine);//�߳��۲�ֵ
	sf.ReadString(strLine);//�߳��۲�ֵ����
	strLine.Trim();
	m_iDistObsCount = _ttoi(strLine);
	m_pDistObs = new CDistObs[m_iDistObsCount];
	//��ȡ������߳��۲�ֵ����
	for (int i = 0; i < m_iDistObsCount; i++)
	{
		sf.ReadString(strLine);
		CString* pstrData = SplitString(strLine, ',', n);
		for (int j = 0; j < n; j++)
			pstrData[j].Trim();
		m_pDistObs[i].cpStart = SearchPointUsingName(pstrData[0]);
		m_pDistObs[i].cpEnd = SearchPointUsingName(pstrData[1]);
		m_pDistObs[i].dDist = _tstof(pstrData[2]);
		delete[] pstrData;
		pstrData = NULL;
	}
	//��ʼ��ȡ����۲�ֵ����
	sf.ReadString(strLine);//����۲�ֵ 
	sf.ReadString(strLine);//����۲�ֵ��������վ�� 
	pstrData = SplitString(strLine, ',', n);
	for (int i = 0; i < n; i++)
	{
		pstrData[i].Trim();
	}
	obsBearStationCount = _ttoi(pstrData[0]);
	m_iBearObsCount = _ttoi(pstrData[1]);
	m_pBearObs = new CBearingObs[m_iBearObsCount];//����۲�ֵ���� 
	delete[] pstrData;
	pstrData = NULL;
	int num_bear = 0;
	//����ÿһ��
	for (int i = 0; i < m_iBearObsCount + obsBearStationCount; i++)
	{
		sf.ReadString(strLine);
		CString* strTmp = SplitString(strLine, ',', n);

		//��¼��վ��
		CControlPoint* p;
		p = SearchPointUsingName(strTmp[0]);
		//�õ���վ�����׼����
		int num = _ttoi(strTmp[1]);
		for (int j = 0; j < num; j++, i++)
		{
			sf.ReadString(strLine);
			CString* strTmp0 = SplitString(strLine, ',', n);
			//��εĲ�վ��
			m_pBearObs[num_bear].cpStation = p;
			//��վ����׼�㼴����۲�ζ���ǰ�ַ���
			m_pBearObs[num_bear].cpObject = SearchPointUsingName(strTmp0[0]);
			//����۲�ֵ������۲�ζ��ź��ַ���ת��Ϊ������
			m_pBearObs[num_bear].angleObsValue(DMS) = _tstof(strTmp0[1]);
			num_bear++;
		}
	}
	//�ļ���ʽ2
/*
for (int i = 0; i < m_iBearObsCount + obsBearStationCount; i++)
 {
	sf.ReadString(strLine);
	CString* pstrData = SplitString(strLine, ',', n);
	for (int j = 0; j < n; j++)
		pstrData[j].Trim();
	m_pBearObs[i].cpStation = SearchPointUsingName(pstrData[0]);
	m_pBearObs[i].cpObject = SearchPointUsingName(pstrData[1]);
	m_pBearObs[i].angleObsValue = CAngle(_tstof(pstrData[2]), DMS);
	double a;
	if (i == 1)a = m_pBearObs[i].angleObsValue(DEG);
	delete[] pstrData;
	pstrData = NULL;
}*/
	sf.Close();
	//��ȡ�ļ������ڿؼ�����
	if (!sf.Open(strFileName, CFile::modeRead)) return 0;
	strObsData.Empty();
	CString strLine0;
	bEOF = sf.ReadString(strLine0);
	while (bEOF)
	{
		strObsData += strLine0;

		bEOF = sf.ReadString(strLine0);

		if (bEOF) strObsData += _T("\r\n");
	}
	sf.Close();
}

//����������������Ƿ���֪�����ض�Ӧ�߳��۲�
bool CAccurateTraverse::isBearObsDistKnown(CBearingObs m_pBearObs0, CDistObs& dist0)
{
	bool flag = 0;
	for (int i = 0; i < m_iDistObsCount; i++)
	{
		//��������εĲ�վ����ڱ߳���ε���㣬�����ε���׼����ڱ߳���ε��յ�
		if (m_pDistObs[i].cpStart->strName == m_pBearObs0.cpStation->strName
			&& m_pDistObs[i].cpEnd->strName == m_pBearObs0.cpObject->strName)
		{
			flag = 1;
			dist0 = m_pDistObs[i];
		}
		//��������εĲ�վ����ڱ߳���ε��յ㣬�����ε���׼����ڱ߳���ε����
		if (m_pDistObs[i].cpStart->strName == m_pBearObs0.cpObject->strName
			&& m_pDistObs[i].cpEnd->strName == m_pBearObs0.cpStation->strName)
		{
			flag = 1;
			dist0 = m_pDistObs[i];
		}
	}
	return flag;
}

//����δ֪�����
void CAccurateTraverse::SetUnKnownPointSize(int size)
{
	m_iUnKnownPointCount = size;
	m_pUnKnownPoint = new CControlPoint[size];
}

//�ָ��ַ���
CString* CAccurateTraverse::SplitString(CString str, char split, int&iSubStrs)
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

//���ݵ�Ŵ���֪���������ҵ����Ƶ㣬�����ظõ��ָ��
CControlPoint* CAccurateTraverse::SearchKnownPointUsingName(CString strName)
{
	for (int i = 0; i < 2; i++)
	{
		if (strName == m_pKnownPoint[i].strName)
		{
			return &m_pKnownPoint[i];
		}
	}
	return NULL;
}

//���ݵ�Ŵ�δ֪���������ҵ����Ƶ㣬�����ظõ��ָ��
CControlPoint* CAccurateTraverse::SearchUnknownPointUsingName(CString strName)
{
	for (int i = 0; i < m_iUnKnownPointCount; i++)
	{
		if (strName == m_pUnKnownPoint[i].strName)
		{
			return &m_pUnKnownPoint[i];
		}
	}
	return NULL;
}

//���ݵ�Ŵ�δ֪�����֪���������ҵ����Ƶ㣬�����ظõ��ָ��
CControlPoint* CAccurateTraverse::SearchPointUsingName(CString strName)
{
	CControlPoint* pCP = NULL;
	pCP = SearchKnownPointUsingName(strName);
	if (pCP == NULL)
	{
		pCP = SearchUnknownPointUsingName(strName);
	}
	return pCP;
}

//����֪�������еĵ�pos�㸳ֵ��pos��0��ʼ
bool CAccurateTraverse::SetKnownPoint(int pos, const CControlPoint& CP)
{
	if (pos >= 2) return false;
	m_pKnownPoint[pos] = CP;
	return true;
}

//��δ֪�������еĵ�pos�㸳ֵ��pos��0��ʼ
bool CAccurateTraverse::SetUnKnownPoint(int pos, const CControlPoint& CP)
{
	if (pos >= m_iUnKnownPointCount) return false;
	m_pUnKnownPoint[pos] = CP;
	return true;
}

//�Է���۲������еĵ�pos���۲�ֵ��ֵ��pos��0��ʼ
bool CAccurateTraverse::SetBearObs(int pos, const CBearingObs& BearObs)
{
	if (pos >= m_iBearObsCount) return false;
	m_pBearObs[pos] = BearObs;
	return true;
}

//�Ծ���۲������еĵ�pos���۲�ֵ��ֵ��pos��0��ʼ
bool CAccurateTraverse::SetDistObs(int pos, const CDistObs& DistObs)
{
	if (pos >= m_iDistObsCount) return false;
	m_pDistObs[pos] = DistObs;
	return true;
}

//����δ֪�����
int CAccurateTraverse::GetUnKnownPointSize()
{
	return m_iUnKnownPointCount;
}

//��֪�������Ƶ㣬��P1->P2�ķ�λ��
CAngle CAccurateTraverse::Azi(const CControlPoint& P1, const CControlPoint& P2)
{
	CAngle angAzi;

	angAzi(RAD) = Azimuth(P1.X, P1.Y, P2.X, P2.Y)(RAD);

	return angAzi;
}

//��֪�������Ƶ㣬��P1->P2�ľ���
double CAccurateTraverse::HDist(const CControlPoint& P1, const CControlPoint& P2)
{
	return Dist(P1.X, P1.Y, P2.X, P2.Y);
}

//�Ƿ�����δ֪�㶼�Ѽ����ȡ
bool  CAccurateTraverse::isAllKnown()
{
	bool flag = 1;
	//��������δ֪��
	for (int i = 0; i < m_iUnKnownPointCount; i++)
	{
		//ֻҪ����һ��δ֪����δ֪�����ǲ�������δ֪���Ѽ����ȡ
		if (!m_pUnKnownPoint[i].isKnown)
		{
			flag = 0;
			break;
		}
	}
	return flag;
}

//����δ֪������
CControlPoint CAccurateTraverse::GetUnKnownPoint(int pos)
{
	return m_pUnKnownPoint[pos];
}

//�������
void CAccurateTraverse::CoorRoughEst()
{
	while (!isAllKnown())
	{
		//��������δ֪��
		for (int i = 0; i < m_iUnKnownPointCount; i++)
			//���δ֪����Ϊδ֪��������㣬���������õ�
			if (!m_pUnKnownPoint[i].isKnown)
			{
				//�������з���۲�ֵ
				for (int j = 0; j < m_iBearObsCount; j++)
				{
					if (m_pUnKnownPoint[i].isKnown)
						break;
					//Ѱ���Ըõ�Ϊ��׼������о�����֪�ķ�����
					if (m_pBearObs[j].cpObject->strName == m_pUnKnownPoint[i].strName)
					{
						CDistObs dist0;
						if (isBearObsDistKnown(m_pBearObs[j], dist0))
						{
							//���Ըõ�Ϊ��׼��Ĳ�վ����֪
							if (m_pBearObs[j].cpStation->isKnown)
							{
								//Ѱ������ĸò�վ�����֪��׼��
								for (int k = 0; k < m_iBearObsCount; k++)
								{
									CAngle azi0, azi1;
									if (m_pBearObs[k].cpStation->strName == m_pBearObs[j].cpStation->strName
										&& m_pBearObs[k].cpObject->strName != m_pBearObs[j].cpObject->strName
										&& m_pBearObs[k].cpObject->isKnown == 1)
									{
										azi0 = Azi(*(m_pBearObs[k].cpObject), *(m_pBearObs[k].cpStation));
										azi1(DEG) = azi0(DEG) + m_pBearObs[j].angleObsValue(DEG) - m_pBearObs[k].angleObsValue(DEG) - 180;
										if (azi1(DEG) > 360)
											azi1(DEG) = azi1(DEG) - 360;
										if (azi1(DEG) < 0)
											azi1(DEG) = azi1(DEG) + 360;
										m_pUnKnownPoint[i].X = m_pBearObs[j].cpStation->X + dist0.dDist * cos(azi1(RAD));
										m_pUnKnownPoint[i].Y = m_pBearObs[j].cpStation->Y + dist0.dDist * sin(azi1(RAD));
										m_pUnKnownPoint[i].isKnown = 1;
										break;
									}
								}
							}
						}
					}
				}
			}
	}
}

//��Ȩ(��������۲�;���۲�)
//SigmaBear����۲��������飩������Ϊ��λ
//SigmaDist����۲���������), ��mmΪ��λ
CMatrix CAccurateTraverse::Weight(double SigmaBear, double SigmaDist)
{
	int iTotalObsCount = m_iBearObsCount + m_iDistObsCount;
	CMatrix P(iTotalObsCount, iTotalObsCount);
	P.Unit();
	for (int i = 0; i < m_iDistObsCount; i++)
	{
		P(m_iBearObsCount + i, m_iBearObsCount + i) =
			(SigmaBear * SigmaBear) / (SigmaDist* SigmaDist * m_pDistObs[i].dDist/1000);
	}
	return P;
}

//����ĳ�η���۲�ֵ���̵�ϵ���ͳ����� 
//Ai��BiΪ��վ������δ֪����Ӧϵ����
//Aj��BjΪ��׼������δ֪����Ӧ��ϵ����
void CAccurateTraverse::ComBearObsEff(const CBearingObs& BearObs, double& Lij,
	double& Ai, double& Bi,
	double& Aj, double& Bj)
{
	double Aij, Bij;
	double Sij;
	//ϵ����λΪ( ��/mm)
	Sij = HDist(*BearObs.cpStation, *BearObs.cpObject);
	Aij = Ro * (BearObs.cpObject->Y - BearObs.cpStation->Y) / (Sij * Sij) / 1000;
	Bij = -Ro * (BearObs.cpObject->X - BearObs.cpStation->X) / (Sij * Sij) / 1000;
	Ai = Aij;
	Bi = Bij;
	Aj = -Aij;
	Bj = -Bij;

	CAngle  Fij,Tij,Zi;
	Tij = Azi(*BearObs.cpStation, *BearObs.cpObject);
	for (int j = 0; j < m_iBearObsCount; j++)
	{
		if (BearObs.cpStation->strName == m_pBearObs[j].cpStation->strName
			&& m_pBearObs[j].angleObsValue(RAD) == 0)
		{ 
			Zi = Azi(*m_pBearObs[j].cpStation, *m_pBearObs[j].cpObject);
			break;
		}
	}
	double det = Tij(RAD) - Zi(RAD);
	if (det < 0)det += 2 * PI;
	Lij= (BearObs.angleObsValue(RAD) -det)*Ro;
}

// ����ĳ�ξ���۲ⷽ�̵�ϵ���ͳ�����
//Ci��DiΪ��վ������δ֪����Ӧϵ����
//Ck,DkΪ��׼������δ֪����Ӧ��ϵ��
void CAccurateTraverse::ComDistObsEff(const CDistObs & DistObs, double& Li,
	double& Ci, double& Di,
	double& Ck, double& Dk)
{
	double Sik;
	Sik = HDist(*DistObs.cpStart, *DistObs.cpEnd);
	double c, d;
	c = (DistObs.cpEnd->X - DistObs.cpStart->X) / Sik;
	d = (DistObs.cpEnd->Y - DistObs.cpStart->Y) / Sik;
	Ci = -c;
	Di = -d;
	Ck = c;
	Dk = d;
	Li = (DistObs.dDist-Sik ) * 1000;//�������Ժ���Ϊ��λ
}

//�������,B Ϊϵ������LΪ����������
void CAccurateTraverse::FormErrorEquations(CMatrix& B, CMatrix& L)
{
	int iObsCount, iUnknownCount;
	iObsCount = m_iBearObsCount + m_iDistObsCount;
	iUnknownCount = m_iUnKnownPointCount * 3;//ÿ��δ֪����xy�������,z�������� 

	B.SetSize(iObsCount, iUnknownCount + 2);
	L.SetSize(iObsCount, 1);

	//���㷽��۲�ֵϵ�����볣������
	double Ai, Bi, Aj, Bj, Li;
	for (int i = 0; i < m_iBearObsCount; i++)
	{
		ComBearObsEff(m_pBearObs[i], Li, Ai, Bi, Aj, Bj);
		CControlPoint* pStation = m_pBearObs[i].cpStation;
		CControlPoint* pObject = m_pBearObs[i].cpObject;
		L(i, 0) = Li;
		if (pStation->Delta == 1)
		{
			B(i, 2 * pStation->ID) = Ai;
			B(i, 2 * pStation->ID + 1) = Bi;
		}
		if (pObject->Delta == 1)
		{
			B(i, 2 * pObject->ID) = Aj ;
			B(i, 2 * pObject->ID + 1) = Bj ;
		}
	}

	//����߳��۲�ֵϵ�����볣������
	double Ci, Di, Ck, Dk;
	for (int i = 0; i < m_iDistObsCount; i++)
	{
		ComDistObsEff(m_pDistObs[i], Li, Ci, Di, Ck, Dk);
		CControlPoint* pStart = m_pDistObs[i].cpStart;
		CControlPoint* pEnd = m_pDistObs[i].cpEnd;
		L(i + m_iBearObsCount, 0) = Li;
		if (pStart->Delta == 1)
		{
			B(i + m_iBearObsCount, 2 * pStart->ID) = Ci;
			B(i + m_iBearObsCount, 2 * pStart->ID + 1) = Di;
		}
		if (pEnd->Delta == 1)
		{
			B(i + m_iBearObsCount, 2 * pEnd->ID) = Ck;
			B(i + m_iBearObsCount, 2 * pEnd->ID + 1) = Dk;
		}
	}

	//����۲�ֵ��ͬ��վ��z���ϵ��Ϊ-1��
	for (int i = 0; i < m_iBearObsCount; i++)
	{
		bool tag = 0;
		for (int j = 0;j< 2; j++)
			if (m_pBearObs[i].cpStation->strName == m_pKnownPoint[j].strName)
			{
				B(i, 2 * m_iUnKnownPointCount + j) = -1;
				tag = 1;
				break;
			}
		if(!tag)
		{ 
		for (int k = 0; k < m_iUnKnownPointCount; k++)
			if (m_pBearObs[i].cpStation->strName == m_pUnKnownPoint[k].strName)
			{
				B(i, 2 * m_iUnKnownPointCount + 2 + k) = -1;
				break;
			}
		}
	}
}

  //����ƽ�����������
void CAccurateTraverse::RigorousAdjust(CString strFileName)
{
	//-----------------------------------------------��������ļ�Ԥ����--------------------------------------------------------
	CStdioFile sf;
	setlocale(LC_ALL, "");
	if (!sf.Open(strFileName, CFile::modeCreate | CFile::modeWrite)) return;
	CString strLine;

	//-------------------------------------------------����ƽ����㲽��----------------------------------------------------------
	//��һ��������δ֪���������
	//�ڶ���������Ƕȹ۲�ֵ���̵�ϵ���ͳ�����
	//������������߳��۲�ֵ���̵�ϵ���ͳ�����
	//���Ĳ����������
	//�������ġ��岽������FormErrorEquations������
	//���岽����Ȩ���õ�Ȩ����
	int iTotalObsCount = m_iBearObsCount + m_iDistObsCount;
	CMatrix P(iTotalObsCount, iTotalObsCount);
	P = Weight(2, 2);

	//����������ɲ����㷨����
	CMatrix B, L;
	CMatrix BT, N, InvN, detX, V, Qx;

	//ע�⣺detX��������������ĵ�λΪmm
	int tag = 0;
	do {
		FormErrorEquations(B, L);
		BT = ~B;
		N = BT * P * B;
		InvN = N.Inv();
		detX = InvN * (BT * P * L);
		tag++;
	} while (tag < 3);

	//���߲�������۲�ֵ��������ƽ��ֵ�Լ�δ֪������ƽ��ֵ��������ļ�
	V = B * detX - L;
	for (int i = 0; i < m_iUnKnownPointCount; i++)
	{
		m_pUnKnownPoint[i].X += detX(2 * m_pUnKnownPoint[i].ID, 0) / 1000;
		m_pUnKnownPoint[i].Y += detX(2 * m_pUnKnownPoint[i].ID + 1, 0) / 1000;
	}

	// ������Ƶ�ɹ���
	sf.WriteString(_T("---------------------------------------���Ƶ�ɹ���---------------------------------------\r\n"));
	strLine.Format(_T("����\t     X(m)\t     Y(m)\r\n"));
	sf.WriteString(strLine);
	for (int i = 0; i < m_iUnKnownPointCount; i++)
	{
		strLine.Format(_T("%s\t%.4f\t  %.4f\r\n"),
			m_pUnKnownPoint[i].strName,
			m_pUnKnownPoint[i].X,
			m_pUnKnownPoint[i].Y);
		sf.WriteString(strLine);
	}
	sf.WriteString(_T("\r\n"));

	//�������۲�ɹ���
	sf.WriteString(_T("---------------------------------------����۲�ɹ���---------------------------------------\r\n"));
	strLine.Format(_T("��վ��\t��׼��\t����ֵ(dms)\t������(s)\tƽ���ֵ(dms)\r\n"));
	sf.WriteString(strLine);
	CAngle bearAdj;
	double bearDeltaZ;
	for (int i = 0; i < m_iBearObsCount; i++)
	{
		// �����վ��������
		if(m_pBearObs[i].cpStation->Delta==0)
			for (int j = 0; j < 2; j++)
			{
				if (m_pBearObs[i].cpStation->strName == m_pKnownPoint[j].strName)
					bearDeltaZ = detX( 3*m_iUnKnownPointCount + j,0);
			}
		if (m_pBearObs[i].cpStation->Delta==1)
			bearDeltaZ = detX(2*m_iUnKnownPointCount+m_pBearObs[i].cpStation->ID,0);
		bearAdj(DEG) = m_pBearObs[i].angleObsValue(DEG) + V(i, 0) / 3600 + bearDeltaZ / 3600;
		strLine.Format(_T("%s\t%s\t%.6f\t%.2f\t\t%.6f\r\n"),
			m_pBearObs[i].cpStation->strName,
			m_pBearObs[i].cpObject->strName,
			m_pBearObs[i].angleObsValue(DMS),
			V(i, 0),
			bearAdj(DMS)
		);
		sf.WriteString(strLine);
	}
	sf.WriteString(_T("\r\n"));

	//�������۲�ɹ���
	sf.WriteString(_T("---------------------------------------����۲�ɹ���---------------------------------------\r\n"));
	strLine.Format(_T("��վ��\t��׼��\t����ֵ(m)\t������(mm)\tƽ���ֵ(m)\t��λ��(dms)\r\n"));
	sf.WriteString(strLine);
	double distAdj;
	double DistObsAzi;
	for (int i = 0; i < m_iDistObsCount; i++)
	{
		distAdj = m_pDistObs[i].dDist + V(i + m_iBearObsCount, 0);
		DistObsAzi = Azi(*m_pDistObs[i].cpStart, *m_pDistObs[i].cpEnd)(DMS);
		strLine.Format(_T("%s\t%s\t%.4f\t  %.4f\t  %.4f\t  %.6f\r\n"),
			m_pDistObs[i].cpStart->strName,
			m_pDistObs[i].cpEnd->strName,
			m_pDistObs[i].dDist,
			V(i + m_iBearObsCount, 0),
			distAdj,
			DistObsAzi
		);
		sf.WriteString(strLine);
	}
	sf.WriteString(_T("\r\n"));

	//�ڰ˲�����������
   //���㵥λȨ�����Sigma 
	CMatrix Omiga;
	Omiga = ~V * P * V;
	Qx = InvN;
	double Sigma;
	Sigma = sqrt(Omiga(0, 0) / (m_iBearObsCount + m_iDistObsCount - (m_iUnKnownPointCount * 3 + 2)));

	//�����λ��������Ӧ��Բ������������ļ�
	for (int i = 0; i < m_iUnKnownPointCount; i++)
	{
		//Q����Ԫ��
		double temp_Qx = Qx(2 * i, 2 * i);
		double temp_Qy = Qx(2 * i + 1, 2 * i + 1);
		double temp_Qxy = Qx(2 * i, 2 * i + 1);
		//�����λ���
		m_pUnKnownPoint[i].dMx = Sigma * sqrt(Qx(2 * i, 2 * i));
		m_pUnKnownPoint[i].dMy = Sigma * sqrt(Qx(2 * i + 1, 2 * i + 1));
		m_pUnKnownPoint[i].dMk = sqrt(pow(m_pUnKnownPoint[i].dMx, 2) + pow(m_pUnKnownPoint[i].dMy, 2));
		//���������Բ��Ӧ����
		double temp_K = sqrt((temp_Qx - temp_Qy) * (temp_Qx - temp_Qy) + 4 * temp_Qxy * temp_Qxy);
		double temp_dQe = 0.5 * (temp_Qx + temp_Qy + temp_K);
		double temp_dQf = 0.5 * (temp_Qx + temp_Qy - temp_K);
		m_pUnKnownPoint[i].dE = Sigma * sqrt(temp_dQe);
		m_pUnKnownPoint[i].dF = Sigma * sqrt(temp_dQf);
		m_pUnKnownPoint[i].dAlfa = atan((temp_dQe - temp_Qx) / temp_Qxy);
		m_pUnKnownPoint[i].dAlfa = m_pUnKnownPoint[i].dAlfa > 0 ? m_pUnKnownPoint[i].dAlfa : m_pUnKnownPoint[i].dAlfa + 2 * PI;//�洢Ϊ������
	};


	sf.WriteString(_T("��λȨ�����\r\n"));
	strLine.Format(_T("%.4f"), Sigma);
	sf.WriteString(_T("\r\n"));

	//���ƽ���λ����
	sf.WriteString(_T("---------------------------------------ƽ���λ����---------------------------------------\r\n"));
	strLine.Format(_T("����\tMx(mm)\t My(mm)\t Mk(mm)\r\n"));
	sf.WriteString(strLine);
	for (int i = 0; i < m_iUnKnownPointCount; i++)
	{
		strLine.Format(_T("%s\t%.4f\t %.4f\t %.4f\r\n"),
			m_pUnKnownPoint[i].strName,
			m_pUnKnownPoint[i].dMx,
			m_pUnKnownPoint[i].dMy,
			m_pUnKnownPoint[i].dMk
		);
		sf.WriteString(strLine);
	}
	sf.WriteString(_T("\r\n"));

	//��������Բ������
	sf.WriteString(_T("---------------------------------------�����Բ������---------------------------------------\r\n"));
	strLine.Format(_T("����\tAlpha(dms)\tE(mm)\t   F(mm)\r\n"));
	sf.WriteString(strLine);
	for (int i = 0; i < m_iUnKnownPointCount; i++)
	{
		strLine.Format(_T("%s\t%.4f\t%.4f\t   %.4f\r\n"),
			m_pUnKnownPoint[i].strName,
			CAngle(m_pUnKnownPoint[i].dAlfa, RAD)(DMS)>180? CAngle(m_pUnKnownPoint[i].dAlfa, RAD)(DMS)-180: CAngle(m_pUnKnownPoint[i].dAlfa, RAD)(DMS),
			m_pUnKnownPoint[i].dE,
			m_pUnKnownPoint[i].dF
		);
		sf.WriteString(strLine);
	}
	sf.WriteString(_T("\r\n"));

	//���㷽��ͱ߳�����������ļ�
	sf.WriteString(_T("---------------------------------------�۲�ֵƽ��������---------------------------------------\r\n"));
	strLine.Format(_T("��վ��\t��׼��\t���������(s)\r\n"));
	sf.WriteString(strLine);
	CMatrix QF = B * Qx * BT;
	for (int i = 0; i < m_iBearObsCount; i++)
	{
		double m = Sigma * sqrt(QF(i, i));
		strLine.Format(_T("%s\t%s\t%.4f\r\n"),
			m_pBearObs[i].cpStation->strName,
			m_pBearObs[i].cpObject->strName,
			m);
		sf.WriteString(strLine);
	}
	sf.WriteString(_T("\r\n"));
	strLine.Format(_T("��վ��\t��׼��\t�߳������(mm)\r\n"));
	sf.WriteString(strLine);
	for (int i = m_iBearObsCount; i < (m_iDistObsCount + m_iBearObsCount); i++)
	{
		double m = Sigma * sqrt(QF(i, i));
		strLine.Format(_T("%s\t%s\t%.4f\r\n"),
			m_pDistObs[i- m_iBearObsCount].cpStart->strName,
			m_pDistObs[i- m_iBearObsCount].cpEnd->strName,
			m);
		sf.WriteString(strLine);
	}
	sf.WriteString(_T("\r\n"));
  }

  //����������ļ�
  void CAccurateTraverse::OutMatrixToFile(const CMatrix& mat, CStdioFile& sf)
  {
	  CString strLine, strTmp;
	  for (int i = 0; i < mat.Row(); i++)
	  {
		  strLine.Empty();
		  for (int j = 0; j < mat.Col(); j++)
		  {
			  strTmp.Format(_T("%.6f    "), mat(i, j));
			  strLine = strLine + strTmp;
		  }
		  sf.WriteString(strLine + _T("\r\n"));
	  }
  }

  //�õ�XY�����Сֵ�Ӷ������ֵ
  void CAccurateTraverse::GetMaxMinXY()
  {
	  Xmin = Xmax = m_pUnKnownPoint[0].X;
	  Ymin = Ymax = m_pUnKnownPoint[0].Y;
	  for (int i = 1; i < m_iKnownPointCount; i++)
	  {
		  Xmin = Xmin < m_pUnKnownPoint[i].X ? Xmin : m_pUnKnownPoint[i].X;
		  Ymin = Ymin < m_pUnKnownPoint[i].Y ? Ymin : m_pUnKnownPoint[i].Y;
		  Xmax = Xmax > m_pUnKnownPoint[i].X ? Xmax : m_pUnKnownPoint[i].X;
		  Ymax = Ymax > m_pUnKnownPoint[i].Y ? Ymax : m_pUnKnownPoint[i].Y;
	  }
	  for (int i = 0; i < 2; i++)
	  {
		  Xmin = Xmin < m_pKnownPoint[i].X ? Xmin : m_pKnownPoint[i].X;
		  Ymin = Ymin < m_pKnownPoint[i].Y ? Ymin : m_pKnownPoint[i].Y;
		  Xmax = Xmax > m_pKnownPoint[i].X ? Xmax : m_pKnownPoint[i].X;
		  Ymax = Ymax > m_pKnownPoint[i].Y ? Ymax : m_pKnownPoint[i].Y;
	  }
  }

  //���������Σ����Ի�����֪�����
  void CAccurateTraverse::DrawTri(CDC* pDC, double x, double y, double length, double a)
  {
	  double  l = length / sqrt(3) * 0.5 * a;
	  CPen pen;
	  pen.CreatePen(PS_SOLID, 1.8, RGB(123, 25, 123));//ʵ�߻���
	  CPen* pOldPen;
	  pOldPen = pDC->SelectObject(&pen);
	  pDC->MoveTo(x - sqrt(3) * l, y + l);
	  pDC->LineTo(x + sqrt(3) * l, y + l);
	  pDC->LineTo(x, y - 2 * l);
	  pDC->LineTo(x - sqrt(3) * l, y + l);
  }

  //���ƿ�����ͼ��
  //coefficient ͼ��ռ�����û�������ϵ��
   //coefficient2 �߼�����ת�豸�����ͶӰϵ��
   //coefficient3 ��Բռ������ͼ������ϵ��
  void CAccurateTraverse::DrawControlNet(CDC* pDC, CRect& rect, double coefficient)
  {
	  int zero_y1 = rect.top;//���ο�����Ͻ�Ԫ���߼�x   
	  int zero_x1 = rect.left;//���ο�����Ͻ�Ԫ���߼�y
	  int zero_y2 = rect.bottom;//���ο�����½�Ԫ���߼�x
	  int zero_x2 = rect.right; //���ο�����½�Ԫ���߼�y
	  pDC->Rectangle(zero_x1, zero_y1, zero_x2, zero_y2);//�����հ׾���
	  double axis_y = coefficient * (zero_x2 - zero_x1);//y�����᳤��
	  double axis_x = coefficient * (zero_y2 - zero_y1);//x�����᳤��
	  GetMaxMinXY();
	  double x0 = zero_y2 - (1 - coefficient) * 0.5 * (zero_y2 - zero_y1) - 30;
	  double y0 = zero_x1 + (1 - coefficient) * 0.5 * (zero_x2 - zero_x1) + 50;//��ʼ���� ԭ��(ͼ���߼�����)
	  double coefficient2 = ((Xmax - Xmin) > (Ymax - Ymin)) ? ((Xmax - Xmin) / axis_x) : ((Ymax - Ymin) / axis_y);//�Ƚ�x,y;ʵ������ͶӰ��ͼ�е�ϵ��
	  //����X��
	  double temp_x001 = zero_x1 + 20;
	  double temp_x002 = zero_x1 + 20;
	  double temp_x003 = zero_x1 + 20;
	  double temp_x004 = zero_x1 + 20 - 7;
	  double temp_x005 = zero_x1 + 20;
	  double temp_x006 = zero_x1 + 20 + 7;
	  double temp_x007 = zero_x1 + 20 - 10;
	  double temp_y001 = zero_y2 - 10;
	  double temp_y002 = zero_y2 - 10 - (0.5 * coefficient) * (zero_y2 - zero_y1);
	  double temp_y003 = zero_y2 - 10 - (0.5 * coefficient) * (zero_y2 - zero_y1);
	  double temp_y004 = zero_y2 - 10 - (0.5 * coefficient) * (zero_y2 - zero_y1) + 7;
	  double temp_y005 = zero_y2 - 10 - (0.5 * coefficient) * (zero_y2 - zero_y1);
	  double temp_y006 = zero_y2 - 10 - (0.5 * coefficient) * (zero_y2 - zero_y1) + 7;
	  double temp_y007 = zero_y2 - 10 - (0.5 * coefficient) * (zero_y2 - zero_y1) - 20;
	  pDC->MoveTo(temp_x001, temp_y001);
	  pDC->LineTo(temp_x002, temp_y002);
	  pDC->MoveTo(temp_x003, temp_y003);
	  pDC->LineTo(temp_x004, temp_y004);
	  pDC->MoveTo(temp_x005, temp_y005);
	  pDC->LineTo(temp_x006, temp_y006);
	  pDC->TextOut(temp_x007, temp_y007, _T("X"));
	  //����Y��
	  temp_x001 = zero_x1 + 20;
	  temp_x002 = zero_x1 + 20 + (0.5 * coefficient) * (zero_x2 - zero_x1);
	  temp_x003 = zero_x1 + 20 + (0.5 * coefficient) * (zero_x2 - zero_x1);
	  temp_x004 = zero_x1 + 20 + (0.5 * coefficient) * (zero_x2 - zero_x1) - 7;
	  temp_x005 = zero_x1 + 20 + (0.5 * coefficient) * (zero_x2 - zero_x1);
	  temp_x006 = zero_x1 + 20 + (0.5 * coefficient) * (zero_x2 - zero_x1) - 7;
	  temp_x007 = zero_x1 + 20 + (0.5 * coefficient) * (zero_x2 - zero_x1);
	  temp_y001 = zero_y2 - 10;
	  temp_y002 = zero_y2 - 10;
	  temp_y003 = zero_y2 - 10;
	  temp_y004 = zero_y2 - 10 - 7;
	  temp_y005 = zero_y2 - 10;
	  temp_y006 = zero_y2 - 10 + 7;
	  temp_y007 = zero_y2 - 10 - 15;
	  pDC->MoveTo(temp_x001, temp_y001);
	  pDC->LineTo(temp_x002, temp_y002);
	  pDC->MoveTo(temp_x003, temp_y003);
	  pDC->LineTo(temp_x004, temp_y004);
	  pDC->MoveTo(temp_x005, temp_y005);
	  pDC->LineTo(temp_x006, temp_y006);
	  pDC->TextOut(temp_x007, temp_y007, _T("Y"));

	  CFont Font;//��������
	  VERIFY(Font.CreateFont(
		  14,                        // nHeight
		  0,                         // nWidth
		  0,                         // nEscapement
		  0,                         // nOrientation*
		  FW_NORMAL,                 // nWeight
		  FALSE,                     // bItalic
		  FALSE,                     // bUnderline
		  0,                         // cStrikeOut
		  ANSI_CHARSET,              // nCharSet
		  OUT_DEFAULT_PRECIS,        // nOutPrecision
		  CLIP_DEFAULT_PRECIS,       // nClipPrecision
		  DEFAULT_QUALITY,           // nQuality
		  DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		  _T("����")));                 // lpszFacename

	  pDC->SetTextColor(RGB(255, 0, 0));///�ı�������ɫ
	  CFont* pOldFont = pDC->SelectObject(&Font);


	  //����֪����������(�ȱ�)��ʾ
	  for (int i = 0; i < 2; i++)
	  {
		  CString str;
		  str.Format(_T("%s"), m_pKnownPoint[i].strName);
		  double temp_1 = (m_pKnownPoint[i].Y - Ymin) / coefficient2 + y0;
		  double temp_2 = x0 - (m_pKnownPoint[i].X - Xmin) / coefficient2;
		  DrawTri(pDC, temp_1, temp_2, 30, coefficient / 0.8);
		  pDC->TextOut(temp_1, temp_2, str);
	  }
	  //������ 
	  for (int i = 0; i < m_iUnKnownPointCount; i++)
	  {
		  CString str;
		  str.Format(_T("%s"), m_pUnKnownPoint[i].strName);
		  double temp_1 = (m_pUnKnownPoint[i].Y - Ymin) / coefficient2 + y0 - 25;
		  double temp_2 = x0 - (m_pUnKnownPoint[i].X - Xmin) / coefficient2 + 5;
		  pDC->TextOut(temp_1, temp_2, str);
	  }
	  Font.DeleteObject();

	  //������۲��߶�
	  for (int i = 0; i < m_iBearObsCount; i++)
	  {

		  CPen pen;
		  pen.CreatePen(PS_SOLID, 2, RGB(0, 0, 0));//ʵ�߻���
		  CPen* pOldPen;
		  pOldPen = pDC->SelectObject(&pen);

		  double t_x = (m_pBearObs[i].cpStation->Y - Ymin) / coefficient2 + y0;
		  double t_y = x0 - (m_pBearObs[i].cpStation->X - Xmin) / coefficient2;
		  double t_x2 = (m_pBearObs[i].cpObject->Y - Ymin) / coefficient2 + y0;
		  double t_y2 = x0 - (m_pBearObs[i].cpObject->X - Xmin) / coefficient2;
		  pDC->MoveTo(t_x, t_y);
		  pDC->LineTo(t_x2, t_y2);

		  pen.DeleteObject();

	  }

	  //����֪����߶�
	  for (int i = 0; i < 1; i++)
	  {
		  CPen pen;
		  pen.CreatePen(PS_SOLID, 2, RGB(255, 0, 0));//ʵ�߻���
		  CPen* pOldPen;
		  pOldPen = pDC->SelectObject(&pen);
		  //˫ʵ�߱�ʾ���򡢾�����֪
		  double t_x = (m_pKnownPoint[i].Y - Ymin) / coefficient2 + y0;
		  double t_y = x0 - (m_pKnownPoint[i].X - Xmin) / coefficient2;
		  double t_x2 = (m_pKnownPoint[i + 1].Y - Ymin) / coefficient2 + y0;
		  double t_y2 = x0 - (m_pKnownPoint[i + 1].X - Xmin) / coefficient2;

		  double t_x_ = (m_pKnownPoint[i].Y - Ymin) / coefficient2 + y0 + 3;
		  double t_y_ = x0 - (m_pKnownPoint[i].X - Xmin) / coefficient2 + 3;
		  double t_x2_ = (m_pKnownPoint[i + 1].Y - Ymin) / coefficient2 + y0 + 3;
		  double t_y2_ = x0 - (m_pKnownPoint[i + 1].X - Xmin) / coefficient2 + 3;
		  pDC->MoveTo(t_x, t_y);
		  pDC->LineTo(t_x2, t_y2);
		  pDC->MoveTo(t_x_, t_y_);
		  pDC->LineTo(t_x2_, t_y2_);

		  pen.DeleteObject();
	  }

	  //�������Բ
	  double coefficient3 = (0.5 / coefficient) * (m_pUnKnownPoint[0].dE) / 15;//�����Բ����ϵ��
	  for (int i = 0; i < m_iUnKnownPointCount; i++)
	  {
		  CPen pen;
		  pen.CreatePen(PS_SOLID, 1, RGB(0, 0, 255));//ʵ�߻���
		  CPen* pOldPen;
		  pOldPen = pDC->SelectObject(&pen);
		  //һ��ֱ��ÿ��תdAlf����,������Բ�ϵ�����Բ���ĵ��������,��ϳ���Բ
		  for (int j = 0; j < 120; j += 3)
		  {
			  double dAlf = j * 2 * PI / 120;
			  double xae, ybf;
			  double c = 0;
			  double tralatazi;
			  //������Բ�������� 
			  xae = m_pUnKnownPoint[i].dE / coefficient3 * cos(dAlf);
			  ybf = m_pUnKnownPoint[i].dF / coefficient3 * sin(dAlf);
			  //����Բ������ת��Բ���ĵķ�λ�ǣ��Ӷ��������Բ�ϵ�����ڿ���������ϵ������
			  tralatazi = m_pUnKnownPoint[i].dAlfa - PI / 2;
			  double temp_1 = xae * cos(tralatazi) + ybf * sin(tralatazi) + (m_pUnKnownPoint[i].Y - Ymin) / coefficient2 + y0;
			  double temp_2 = -ybf * cos(tralatazi) + xae * sin(tralatazi) - (m_pUnKnownPoint[i].X - Xmin) / coefficient2 + x0;
			  if (j == 0) { pDC->MoveTo(temp_1, temp_2); }
			  pDC->LineTo(temp_1, temp_2);
		  }
		  //���Ƴ���
		  double temp1 = (m_pUnKnownPoint[i].Y - Ymin) / coefficient2 + y0 + (m_pUnKnownPoint[i].dF * sin(m_pUnKnownPoint[i].dAlfa - PI / 2)) / coefficient3;//�������Ͻ� x
		  double temp2 = x0 - (m_pUnKnownPoint[i].X - Xmin) / coefficient2 - (m_pUnKnownPoint[i].dF * cos(m_pUnKnownPoint[i].dAlfa - PI / 2)) / coefficient3;//�������Ͻ� y
		  double temp3 = (m_pUnKnownPoint[i].Y - Ymin) / coefficient2 + y0 - (m_pUnKnownPoint[i].dF * sin(m_pUnKnownPoint[i].dAlfa - PI / 2)) / coefficient3;//�������½� x
		  double temp4 = x0 - (m_pUnKnownPoint[i].X - Xmin) / coefficient2 + (m_pUnKnownPoint[i].dF * cos(m_pUnKnownPoint[i].dAlfa - PI / 2)) / coefficient3;//�������½� y
		  pDC->MoveTo(temp1, temp2);
		  pDC->LineTo(temp3, temp4);
		  pen.DeleteObject();
		  //���ƶ���
		  double _temp1 = (m_pUnKnownPoint[i].Y - Ymin) / coefficient2 + y0 + (m_pUnKnownPoint[i].dE * cos(m_pUnKnownPoint[i].dAlfa - PI / 2)) / coefficient3;//�������½� x
		  double _temp2 = x0 - (m_pUnKnownPoint[i].X - Xmin) / coefficient2 + (m_pUnKnownPoint[i].dE * sin(m_pUnKnownPoint[i].dAlfa - PI / 2)) / coefficient3;//�������Ͻ� y
		  double _temp3 = (m_pUnKnownPoint[i].Y - Ymin) / coefficient2 + y0 - (m_pUnKnownPoint[i].dE * cos(m_pUnKnownPoint[i].dAlfa - PI / 2)) / coefficient3;//�������Ͻ� x
		  double _temp4 = x0 - (m_pUnKnownPoint[i].X - Xmin) / coefficient2 - (m_pUnKnownPoint[i].dE * sin(m_pUnKnownPoint[i].dAlfa - PI / 2)) / coefficient3;//�������Ͻ� y
		  pDC->MoveTo(_temp1, _temp2);
		  pDC->LineTo(_temp3, _temp4);

		  pen.DeleteObject();
	  }

	  //��������
	  CPen pen;
	  pen.CreatePen(PS_SOLID, 2, RGB(0, 0, 0));//ʵ�߻���
	  CPen* pOldPen;
	  pOldPen = pDC->SelectObject(&pen);
	  double ruler = axis_y * 500 / (Ymax - Ymin);//1:500������
	  temp_x001 = (zero_x1 + zero_x2) / 2 - 3 * ruler / 2;
	  temp_x002 = (zero_x1 + zero_x2) / 2 - 3 * ruler / 2;
	  temp_x003 = (zero_x1 + zero_x2) / 2 - ruler / 2;
	  temp_x004 = (zero_x1 + zero_x2) / 2 - ruler / 2;
	  temp_x005 = (zero_x1 + zero_x2) / 2 - ruler;

	  temp_y001 = 20;
	  temp_y002 = 23;
	  temp_y003 = 23;
	  temp_y004 = 20;
	  temp_y005 = 26;

	  pDC->MoveTo(temp_x001, temp_y001);
	  pDC->LineTo(temp_x002, temp_y002);
	  pDC->LineTo(temp_x003, temp_y003);
	  pDC->LineTo(temp_x004, temp_y004);


	  pDC->TextOut(temp_x005, temp_y005, _T("1:500"));
	  pen.DeleteObject();
  }