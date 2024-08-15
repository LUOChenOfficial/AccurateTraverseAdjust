#include "pch.h"
#include "AccurateTraverse.h"
#include "CommonSurveyFunctions.h"
#include "math.h"
#include <locale.h>

CAccurateTraverse::CAccurateTraverse(void)
{
	m_pKnownPoint = new CControlPoint[2];//已知点数组
	m_iKnownPointCount = 2;//已知点个数 
	m_pUnKnownPoint = NULL;//未知点数组
	m_iUnKnownPointCount = 0;      //未知点个数
	m_pBearObs = NULL;    //方向观测值数组
	m_iBearObsCount = 0;          //方向观测值个数 
	m_pDistObs = NULL;     //距离观测值数组
	m_iDistObsCount = 0;            //距离观测值个数
}

CAccurateTraverse::~CAccurateTraverse(void)
{
	//释放动态数组内存
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

//加载数据
bool CAccurateTraverse::LoadObsData(const CString strFileName, CString& strObsData)
{
	//读取数据
	CStdioFile sf;
	if (!sf.Open(strFileName, CFile::modeRead)) return 0;
	CString strLine;
	strLine.Empty();
	BOOL bEOF = sf.ReadString(strLine);//控制点
	//文件数据格式错误就返回，并标记 
	if (strLine != "[ControlPoint]")
	{
		isRightData = 0;
		return 0;
	}
	bEOF = sf.ReadString(strLine);//已知点个数
	int n = 0;
	//读取并保存已知点数据
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
	//开始读取未知知点数据
	sf.ReadString(strLine);//未知点
	sf.ReadString(strLine);//未知点个数
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
	//开始读取观测边长观测值数据
	sf.ReadString(strLine);//边长观测值
	sf.ReadString(strLine);//边长观测值个数
	strLine.Trim();
	m_iDistObsCount = _ttoi(strLine);
	m_pDistObs = new CDistObs[m_iDistObsCount];
	//读取并保存边长观测值数据
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
	//开始读取方向观测值数据
	sf.ReadString(strLine);//方向观测值 
	sf.ReadString(strLine);//方向观测值个数及测站数 
	pstrData = SplitString(strLine, ',', n);
	for (int i = 0; i < n; i++)
	{
		pstrData[i].Trim();
	}
	obsBearStationCount = _ttoi(pstrData[0]);
	m_iBearObsCount = _ttoi(pstrData[1]);
	m_pBearObs = new CBearingObs[m_iBearObsCount];//方向观测值个数 
	delete[] pstrData;
	pstrData = NULL;
	int num_bear = 0;
	//遍历每一行
	for (int i = 0; i < m_iBearObsCount + obsBearStationCount; i++)
	{
		sf.ReadString(strLine);
		CString* strTmp = SplitString(strLine, ',', n);

		//记录测站点
		CControlPoint* p;
		p = SearchPointUsingName(strTmp[0]);
		//得到测站点的照准点数
		int num = _ttoi(strTmp[1]);
		for (int j = 0; j < num; j++, i++)
		{
			sf.ReadString(strLine);
			CString* strTmp0 = SplitString(strLine, ',', n);
			//测段的测站点
			m_pBearObs[num_bear].cpStation = p;
			//测站的照准点即方向观测段逗号前字符串
			m_pBearObs[num_bear].cpObject = SearchPointUsingName(strTmp0[0]);
			//方向观测值即方向观测段逗号后字符串转化为浮点型
			m_pBearObs[num_bear].angleObsValue(DMS) = _tstof(strTmp0[1]);
			num_bear++;
		}
	}
	//文件格式2
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
	//读取文件至窗口控件变量
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

//方向测段中两点距离是否已知并返回对应边长观测
bool CAccurateTraverse::isBearObsDistKnown(CBearingObs m_pBearObs0, CDistObs& dist0)
{
	bool flag = 0;
	for (int i = 0; i < m_iDistObsCount; i++)
	{
		//如果方向测段的测站点等于边长测段的起点，方向测段的照准点等于边长测段的终点
		if (m_pDistObs[i].cpStart->strName == m_pBearObs0.cpStation->strName
			&& m_pDistObs[i].cpEnd->strName == m_pBearObs0.cpObject->strName)
		{
			flag = 1;
			dist0 = m_pDistObs[i];
		}
		//如果方向测段的测站点等于边长测段的终点，方向测段的照准点等于边长测段的起点
		if (m_pDistObs[i].cpStart->strName == m_pBearObs0.cpObject->strName
			&& m_pDistObs[i].cpEnd->strName == m_pBearObs0.cpStation->strName)
		{
			flag = 1;
			dist0 = m_pDistObs[i];
		}
	}
	return flag;
}

//设置未知点个数
void CAccurateTraverse::SetUnKnownPointSize(int size)
{
	m_iUnKnownPointCount = size;
	m_pUnKnownPoint = new CControlPoint[size];
}

//分割字符串
CString* CAccurateTraverse::SplitString(CString str, char split, int&iSubStrs)
{
	int iPos = 0; //分割符位置
	int iNums = 0; //分割符的总数
	CString strTemp = str;
	CString strRight;
	//先计算子字符串的数量
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
	if (iNums == 0) //没有找到分割符
	{
		//子字符串数就是字符串本身
		iSubStrs = 1;
		return NULL;
	}
	//子字符串数组
	iSubStrs = iNums + 1; //子串的数量 = 分割符数量 + 1
	CString* pStrSplit;
	pStrSplit = new CString[iSubStrs];
	strTemp = str;
	CString strLeft;
	for (int i = 0; i < iNums; i++)
	{
		iPos = strTemp.Find(split);
		//左子串
		strLeft = strTemp.Left(iPos);
		//右子串
		strRight = strTemp.Mid(iPos + 1, strTemp.GetLength());
		strTemp = strRight;
		pStrSplit[i] = strLeft;
	}
	pStrSplit[iNums] = strTemp;
	return pStrSplit;
}

//根据点号从已知点数组中找到控制点，并返回该点的指针
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

//根据点号从未知点数组中找到控制点，并返回该点的指针
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

//根据点号从未知点和已知点数组中找到控制点，并返回该点的指针
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

//对已知点数组中的第pos点赋值，pos从0开始
bool CAccurateTraverse::SetKnownPoint(int pos, const CControlPoint& CP)
{
	if (pos >= 2) return false;
	m_pKnownPoint[pos] = CP;
	return true;
}

//对未知点数组中的第pos点赋值，pos从0开始
bool CAccurateTraverse::SetUnKnownPoint(int pos, const CControlPoint& CP)
{
	if (pos >= m_iUnKnownPointCount) return false;
	m_pUnKnownPoint[pos] = CP;
	return true;
}

//对方向观测数组中的第pos个观测值赋值，pos从0开始
bool CAccurateTraverse::SetBearObs(int pos, const CBearingObs& BearObs)
{
	if (pos >= m_iBearObsCount) return false;
	m_pBearObs[pos] = BearObs;
	return true;
}

//对距离观测数组中的第pos个观测值赋值，pos从0开始
bool CAccurateTraverse::SetDistObs(int pos, const CDistObs& DistObs)
{
	if (pos >= m_iDistObsCount) return false;
	m_pDistObs[pos] = DistObs;
	return true;
}

//返回未知点个数
int CAccurateTraverse::GetUnKnownPointSize()
{
	return m_iUnKnownPointCount;
}

//已知两个控制点，求P1->P2的方位角
CAngle CAccurateTraverse::Azi(const CControlPoint& P1, const CControlPoint& P2)
{
	CAngle angAzi;

	angAzi(RAD) = Azimuth(P1.X, P1.Y, P2.X, P2.Y)(RAD);

	return angAzi;
}

//已知两个控制点，求P1->P2的距离
double CAccurateTraverse::HDist(const CControlPoint& P1, const CControlPoint& P2)
{
	return Dist(P1.X, P1.Y, P2.X, P2.Y);
}

//是否所有未知点都已计算获取
bool  CAccurateTraverse::isAllKnown()
{
	bool flag = 1;
	//遍历所有未知点
	for (int i = 0; i < m_iUnKnownPointCount; i++)
	{
		//只要存在一个未知点仍未知，则标记并非所有未知点已计算获取
		if (!m_pUnKnownPoint[i].isKnown)
		{
			flag = 0;
			break;
		}
	}
	return flag;
}

//返回未知点坐标
CControlPoint CAccurateTraverse::GetUnKnownPoint(int pos)
{
	return m_pUnKnownPoint[pos];
}

//坐标概算
void CAccurateTraverse::CoorRoughEst()
{
	while (!isAllKnown())
	{
		//遍历所有未知点
		for (int i = 0; i < m_iUnKnownPointCount; i++)
			//如果未知点仍为未知则继续解算，否则跳过该点
			if (!m_pUnKnownPoint[i].isKnown)
			{
				//遍历所有方向观测值
				for (int j = 0; j < m_iBearObsCount; j++)
				{
					if (m_pUnKnownPoint[i].isKnown)
						break;
					//寻找以该点为照准点的所有距离已知的方向测段
					if (m_pBearObs[j].cpObject->strName == m_pUnKnownPoint[i].strName)
					{
						CDistObs dist0;
						if (isBearObsDistKnown(m_pBearObs[j], dist0))
						{
							//若以该点为照准点的测站点已知
							if (m_pBearObs[j].cpStation->isKnown)
							{
								//寻找最近的该测站点的已知照准点
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

//定权(包括方向观测和距离观测)
//SigmaBear方向观测中误差（先验），以秒为单位
//SigmaDist距离观测中误差（先验), 以mm为单位
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

//计算某段方向观测值误差方程的系数和常数项 
//Ai、Bi为测站点坐标未知数对应系数，
//Aj、Bj为照准点坐标未知数对应的系数，
void CAccurateTraverse::ComBearObsEff(const CBearingObs& BearObs, double& Lij,
	double& Ai, double& Bi,
	double& Aj, double& Bj)
{
	double Aij, Bij;
	double Sij;
	//系数单位为( 秒/mm)
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

// 计算某段距离观测方程的系数和常数项
//Ci、Di为测站点坐标未知数对应系数，
//Ck,Dk为照准点坐标未知数对应的系数
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
	Li = (DistObs.dDist-Sik ) * 1000;//常数项以毫米为单位
}

//组成误差方程,B 为系数矩阵，L为常数项向量
void CAccurateTraverse::FormErrorEquations(CMatrix& B, CMatrix& L)
{
	int iObsCount, iUnknownCount;
	iObsCount = m_iBearObsCount + m_iDistObsCount;
	iUnknownCount = m_iUnKnownPointCount * 3;//每个未知点有xy坐标误差,z定向角误差 

	B.SetSize(iObsCount, iUnknownCount + 2);
	L.SetSize(iObsCount, 1);

	//计算方向观测值系数阵与常数项阵
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

	//计算边长观测值系数阵与常数项阵
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

	//方向观测值中同侧站的z误差系数为-1；
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

  //严密平差计算主函数
void CAccurateTraverse::RigorousAdjust(CString strFileName)
{
	//-----------------------------------------------输出数据文件预步骤--------------------------------------------------------
	CStdioFile sf;
	setlocale(LC_ALL, "");
	if (!sf.Open(strFileName, CFile::modeCreate | CFile::modeWrite)) return;
	CString strLine;

	//-------------------------------------------------严密平差计算步骤----------------------------------------------------------
	//第一步：计算未知点近似坐标
	//第二步：计算角度观测值误差方程的系数和常数项
	//第三步：计算边长观测值误差方程的系数和常数项
	//第四步：组成误差方程
	//第三、四、五步包含在FormErrorEquations函数中
	//第五步：定权，得到权矩阵
	int iTotalObsCount = m_iBearObsCount + m_iDistObsCount;
	CMatrix P(iTotalObsCount, iTotalObsCount);
	P = Weight(2, 2);

	//第六步：组成并解算法方程
	CMatrix B, L;
	CMatrix BT, N, InvN, detX, V, Qx;

	//注意：detX，即坐标改正数的单位为mm
	int tag = 0;
	do {
		FormErrorEquations(B, L);
		BT = ~B;
		N = BT * P * B;
		InvN = N.Inv();
		detX = InvN * (BT * P * L);
		tag++;
	} while (tag < 3);

	//第七步：计算观测值改正数、平差值以及未知点坐标平差值并输出至文件
	V = B * detX - L;
	for (int i = 0; i < m_iUnKnownPointCount; i++)
	{
		m_pUnKnownPoint[i].X += detX(2 * m_pUnKnownPoint[i].ID, 0) / 1000;
		m_pUnKnownPoint[i].Y += detX(2 * m_pUnKnownPoint[i].ID + 1, 0) / 1000;
	}

	// 输出控制点成果表
	sf.WriteString(_T("---------------------------------------控制点成果表---------------------------------------\r\n"));
	strLine.Format(_T("点名\t     X(m)\t     Y(m)\r\n"));
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

	//输出方向观测成果表
	sf.WriteString(_T("---------------------------------------方向观测成果表---------------------------------------\r\n"));
	strLine.Format(_T("测站点\t照准点\t方向值(dms)\t改正数(s)\t平差后值(dms)\r\n"));
	sf.WriteString(strLine);
	CAngle bearAdj;
	double bearDeltaZ;
	for (int i = 0; i < m_iBearObsCount; i++)
	{
		// 计算测站定向角误差
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

	//输出距离观测成果表
	sf.WriteString(_T("---------------------------------------距离观测成果表---------------------------------------\r\n"));
	strLine.Format(_T("测站点\t照准点\t距离值(m)\t改正数(mm)\t平差后值(m)\t方位角(dms)\r\n"));
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

	//第八步：精度评定
   //计算单位权中误差Sigma 
	CMatrix Omiga;
	Omiga = ~V * P * V;
	Qx = InvN;
	double Sigma;
	Sigma = sqrt(Omiga(0, 0) / (m_iBearObsCount + m_iDistObsCount - (m_iUnKnownPointCount * 3 + 2)));

	//计算点位中误差与对应椭圆参数并输出至文件
	for (int i = 0; i < m_iUnKnownPointCount; i++)
	{
		//Q矩阵元素
		double temp_Qx = Qx(2 * i, 2 * i);
		double temp_Qy = Qx(2 * i + 1, 2 * i + 1);
		double temp_Qxy = Qx(2 * i, 2 * i + 1);
		//计算点位误差
		m_pUnKnownPoint[i].dMx = Sigma * sqrt(Qx(2 * i, 2 * i));
		m_pUnKnownPoint[i].dMy = Sigma * sqrt(Qx(2 * i + 1, 2 * i + 1));
		m_pUnKnownPoint[i].dMk = sqrt(pow(m_pUnKnownPoint[i].dMx, 2) + pow(m_pUnKnownPoint[i].dMy, 2));
		//计算误差椭圆相应参数
		double temp_K = sqrt((temp_Qx - temp_Qy) * (temp_Qx - temp_Qy) + 4 * temp_Qxy * temp_Qxy);
		double temp_dQe = 0.5 * (temp_Qx + temp_Qy + temp_K);
		double temp_dQf = 0.5 * (temp_Qx + temp_Qy - temp_K);
		m_pUnKnownPoint[i].dE = Sigma * sqrt(temp_dQe);
		m_pUnKnownPoint[i].dF = Sigma * sqrt(temp_dQf);
		m_pUnKnownPoint[i].dAlfa = atan((temp_dQe - temp_Qx) / temp_Qxy);
		m_pUnKnownPoint[i].dAlfa = m_pUnKnownPoint[i].dAlfa > 0 ? m_pUnKnownPoint[i].dAlfa : m_pUnKnownPoint[i].dAlfa + 2 * PI;//存储为弧度制
	};


	sf.WriteString(_T("单位权中误差\r\n"));
	strLine.Format(_T("%.4f"), Sigma);
	sf.WriteString(_T("\r\n"));

	//输出平面点位误差表
	sf.WriteString(_T("---------------------------------------平面点位误差表---------------------------------------\r\n"));
	strLine.Format(_T("点名\tMx(mm)\t My(mm)\t Mk(mm)\r\n"));
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

	//输出误差椭圆参数表
	sf.WriteString(_T("---------------------------------------误差椭圆参数表---------------------------------------\r\n"));
	strLine.Format(_T("点名\tAlpha(dms)\tE(mm)\t   F(mm)\r\n"));
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

	//计算方向和边长中误差并输出至文件
	sf.WriteString(_T("---------------------------------------观测值平差后中误差---------------------------------------\r\n"));
	strLine.Format(_T("测站点\t照准点\t方向中误差(s)\r\n"));
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
	strLine.Format(_T("测站点\t照准点\t边长中误差(mm)\r\n"));
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

  //矩阵输出至文件
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

  //得到XY最大最小值从而计算差值
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

  //绘制三角形，用以绘制已知点符号
  void CAccurateTraverse::DrawTri(CDC* pDC, double x, double y, double length, double a)
  {
	  double  l = length / sqrt(3) * 0.5 * a;
	  CPen pen;
	  pen.CreatePen(PS_SOLID, 1.8, RGB(123, 25, 123));//实线画笔
	  CPen* pOldPen;
	  pOldPen = pDC->SelectObject(&pen);
	  pDC->MoveTo(x - sqrt(3) * l, y + l);
	  pDC->LineTo(x + sqrt(3) * l, y + l);
	  pDC->LineTo(x, y - 2 * l);
	  pDC->LineTo(x - sqrt(3) * l, y + l);
  }

  //绘制控制网图形
  //coefficient 图形占整个用户区缩放系数
   //coefficient2 逻辑坐标转设备坐标的投影系数
   //coefficient3 椭圆占控制网图形缩放系数
  void CAccurateTraverse::DrawControlNet(CDC* pDC, CRect& rect, double coefficient)
  {
	  int zero_y1 = rect.top;//矩形框的左上角元素逻辑x   
	  int zero_x1 = rect.left;//矩形框的左上角元素逻辑y
	  int zero_y2 = rect.bottom;//矩形框的右下角元素逻辑x
	  int zero_x2 = rect.right; //矩形框的右下角元素逻辑y
	  pDC->Rectangle(zero_x1, zero_y1, zero_x2, zero_y2);//创建空白矩形
	  double axis_y = coefficient * (zero_x2 - zero_x1);//y坐标轴长度
	  double axis_x = coefficient * (zero_y2 - zero_y1);//x坐标轴长度
	  GetMaxMinXY();
	  double x0 = zero_y2 - (1 - coefficient) * 0.5 * (zero_y2 - zero_y1) - 30;
	  double y0 = zero_x1 + (1 - coefficient) * 0.5 * (zero_x2 - zero_x1) + 50;//起始坐标 原点(图形逻辑中心)
	  double coefficient2 = ((Xmax - Xmin) > (Ymax - Ymin)) ? ((Xmax - Xmin) / axis_x) : ((Ymax - Ymin) / axis_y);//比较x,y;实际坐标投影至图中的系数
	  //绘制X轴
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
	  //绘制Y轴
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

	  CFont Font;//创建字体
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
		  _T("宋体")));                 // lpszFacename

	  pDC->SetTextColor(RGB(255, 0, 0));///改变字体颜色
	  CFont* pOldFont = pDC->SelectObject(&Font);


	  //画已知点用三角形(等边)表示
	  for (int i = 0; i < 2; i++)
	  {
		  CString str;
		  str.Format(_T("%s"), m_pKnownPoint[i].strName);
		  double temp_1 = (m_pKnownPoint[i].Y - Ymin) / coefficient2 + y0;
		  double temp_2 = x0 - (m_pKnownPoint[i].X - Xmin) / coefficient2;
		  DrawTri(pDC, temp_1, temp_2, 30, coefficient / 0.8);
		  pDC->TextOut(temp_1, temp_2, str);
	  }
	  //画点名 
	  for (int i = 0; i < m_iUnKnownPointCount; i++)
	  {
		  CString str;
		  str.Format(_T("%s"), m_pUnKnownPoint[i].strName);
		  double temp_1 = (m_pUnKnownPoint[i].Y - Ymin) / coefficient2 + y0 - 25;
		  double temp_2 = x0 - (m_pUnKnownPoint[i].X - Xmin) / coefficient2 + 5;
		  pDC->TextOut(temp_1, temp_2, str);
	  }
	  Font.DeleteObject();

	  //画方向观测线段
	  for (int i = 0; i < m_iBearObsCount; i++)
	  {

		  CPen pen;
		  pen.CreatePen(PS_SOLID, 2, RGB(0, 0, 0));//实线画笔
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

	  //画已知点间线段
	  for (int i = 0; i < 1; i++)
	  {
		  CPen pen;
		  pen.CreatePen(PS_SOLID, 2, RGB(255, 0, 0));//实线画笔
		  CPen* pOldPen;
		  pOldPen = pDC->SelectObject(&pen);
		  //双实线表示方向、距离已知
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

	  //画误差椭圆
	  double coefficient3 = (0.5 / coefficient) * (m_pUnKnownPoint[0].dE) / 15;//误差椭圆比例系数
	  for (int i = 0; i < m_iUnKnownPointCount; i++)
	  {
		  CPen pen;
		  pen.CreatePen(PS_SOLID, 1, RGB(0, 0, 255));//实线画笔
		  CPen* pOldPen;
		  pOldPen = pDC->SelectObject(&pen);
		  //一条直线每次转dAlf弧度,计算椭圆上点与椭圆中心的相对坐标,拟合出椭圆
		  for (int j = 0; j < 120; j += 3)
		  {
			  double dAlf = j * 2 * PI / 120;
			  double xae, ybf;
			  double c = 0;
			  double tralatazi;
			  //利用椭圆参数方程 
			  xae = m_pUnKnownPoint[i].dE / coefficient3 * cos(dAlf);
			  ybf = m_pUnKnownPoint[i].dF / coefficient3 * sin(dAlf);
			  //将椭圆长轴旋转椭圆中心的方位角，从而计算出椭圆上点相对于控制网坐标系的坐标
			  tralatazi = m_pUnKnownPoint[i].dAlfa - PI / 2;
			  double temp_1 = xae * cos(tralatazi) + ybf * sin(tralatazi) + (m_pUnKnownPoint[i].Y - Ymin) / coefficient2 + y0;
			  double temp_2 = -ybf * cos(tralatazi) + xae * sin(tralatazi) - (m_pUnKnownPoint[i].X - Xmin) / coefficient2 + x0;
			  if (j == 0) { pDC->MoveTo(temp_1, temp_2); }
			  pDC->LineTo(temp_1, temp_2);
		  }
		  //绘制长轴
		  double temp1 = (m_pUnKnownPoint[i].Y - Ymin) / coefficient2 + y0 + (m_pUnKnownPoint[i].dF * sin(m_pUnKnownPoint[i].dAlfa - PI / 2)) / coefficient3;//长轴右上角 x
		  double temp2 = x0 - (m_pUnKnownPoint[i].X - Xmin) / coefficient2 - (m_pUnKnownPoint[i].dF * cos(m_pUnKnownPoint[i].dAlfa - PI / 2)) / coefficient3;//长轴右上角 y
		  double temp3 = (m_pUnKnownPoint[i].Y - Ymin) / coefficient2 + y0 - (m_pUnKnownPoint[i].dF * sin(m_pUnKnownPoint[i].dAlfa - PI / 2)) / coefficient3;//长轴左下角 x
		  double temp4 = x0 - (m_pUnKnownPoint[i].X - Xmin) / coefficient2 + (m_pUnKnownPoint[i].dF * cos(m_pUnKnownPoint[i].dAlfa - PI / 2)) / coefficient3;//长轴左下角 y
		  pDC->MoveTo(temp1, temp2);
		  pDC->LineTo(temp3, temp4);
		  pen.DeleteObject();
		  //绘制短轴
		  double _temp1 = (m_pUnKnownPoint[i].Y - Ymin) / coefficient2 + y0 + (m_pUnKnownPoint[i].dE * cos(m_pUnKnownPoint[i].dAlfa - PI / 2)) / coefficient3;//短轴右下角 x
		  double _temp2 = x0 - (m_pUnKnownPoint[i].X - Xmin) / coefficient2 + (m_pUnKnownPoint[i].dE * sin(m_pUnKnownPoint[i].dAlfa - PI / 2)) / coefficient3;//短轴右上角 y
		  double _temp3 = (m_pUnKnownPoint[i].Y - Ymin) / coefficient2 + y0 - (m_pUnKnownPoint[i].dE * cos(m_pUnKnownPoint[i].dAlfa - PI / 2)) / coefficient3;//短轴左上角 x
		  double _temp4 = x0 - (m_pUnKnownPoint[i].X - Xmin) / coefficient2 - (m_pUnKnownPoint[i].dE * sin(m_pUnKnownPoint[i].dAlfa - PI / 2)) / coefficient3;//短轴左上角 y
		  pDC->MoveTo(_temp1, _temp2);
		  pDC->LineTo(_temp3, _temp4);

		  pen.DeleteObject();
	  }

	  //画比例尺
	  CPen pen;
	  pen.CreatePen(PS_SOLID, 2, RGB(0, 0, 0));//实线画笔
	  CPen* pOldPen;
	  pOldPen = pDC->SelectObject(&pen);
	  double ruler = axis_y * 500 / (Ymax - Ymin);//1:500比例尺
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