#include "pch.h"
#include "SingleTraverse.h"
#include "CommonSurveyFunctions.h"
#include "math.h"
#include <locale.h>
CSingleTraverse::CSingleTraverse(void)
{
	m_pKnownPoint = new CControlPoint[2];//已知点数组
	m_iKnownPointCount = 2;//已知点个数 
	m_pUnknownPoint = NULL;//未知点数组
	m_iUnknownPointCount = 0;      //未知点个数
	m_pAngleObs = NULL;    //角度观测值数组
	m_iAngleObsCount = 0;          //角度观测值个数 
	m_pDistObs = NULL;     //距离观测值数组
	m_iDistObsCount = 0;            //距离观测值个数
}

CSingleTraverse::~CSingleTraverse(void)
{
	//释放动态数组内存
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
//根据单导线类型，设置已知点个数和数组
void CSingleTraverse::SetKnownPointSize(short nType)
{
	m_nType = nType;

	if (m_nType == 1)//附合导线，4个已知点
	{
		m_pKnownPoint = new CControlPoint[4];//已知点数组
		m_iKnownPointCount = 4;//已知点个数 
	}
	else if (m_nType == 2 || m_nType == 3)//闭合导线与无定向导线，2个已知点
	{
		m_pKnownPoint = new CControlPoint[2];//已知点数组
		m_iKnownPointCount = 2;//已知点个数 
	}
	else if (m_nType == 4)//支导线
	{
		m_pKnownPoint = new CControlPoint;//已知点数组
		m_iKnownPointCount = 1;//已知点个数 
	}
	else
	{
		m_pKnownPoint = NULL;  //已知点数组
		m_iKnownPointCount = 0;        //已知点个数 
	}
}
//设置未知点个数，根据导线类型计算出角度与距离观测值个数
//注意：调用该函数后，原有数据被删除
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
	if (m_nType == 1 || m_nType == 2)//闭合导线或附合导线
	{
		m_pAngleObs = new CAngleObs[size + 2];
		m_iAngleObsCount = size + 2;
		m_pDistObs = new CDistObs[size + 1];
		m_iDistObsCount = size + 1;
	}
	else if (m_nType == 3)//无定向导线
	{
		m_pAngleObs = new CAngleObs[size];
		m_iAngleObsCount = size;
		m_pDistObs = new CDistObs[size + 1];
		m_iDistObsCount = size + 1;
	}
	else//支导线
	{
		m_pAngleObs = new CAngleObs[size];
		m_iAngleObsCount = size;
		m_pDistObs = new CDistObs[size];
		m_iDistObsCount = size;
	}

}
//对已知点数组中的第pos点赋值，pos从0开始
bool CSingleTraverse::SetKnownPoint(int pos, const CControlPoint& CP)
{
	if (pos >= m_iKnownPointCount) return false;
	m_pKnownPoint[pos] = CP;
	return true;
}
//对未知点数组中的第pos点赋值，pos从0开始
bool CSingleTraverse::SetUnKnownPoint(int pos, const CControlPoint& CP)
{
	if (pos >= m_iUnknownPointCount) return false;
	m_pUnknownPoint[pos] = CP;
	return true;
}
//对角度观测数组中的第pos个观测值赋值，pos从0开始
bool CSingleTraverse::SetAngleObs(int pos, const CAngleObs& AngObs)
{
	if (pos >= m_iAngleObsCount) return false;
	m_pAngleObs[pos] = AngObs;
	return true;
}

//对距离观测数组中的第pos个观测值赋值，pos从0开始
bool CSingleTraverse::SetDistObs(int pos, const CDistObs& DistObs)
{
	if (pos >= m_iDistObsCount) return false;
	m_pDistObs[pos] = DistObs;
	return true;
}
//已知两个控制点，求P1->P2的方位角
CAngle CSingleTraverse::Azi(const CControlPoint& P1, const CControlPoint& P2)
{
	CAngle angAzi;

	angAzi(RAD) = Azimuth(P1.X, P1.Y, P2.X, P2.Y);

	return angAzi;
}
//计算角度闭合差
CAngle CSingleTraverse::ComAngleClosedError(void)
{
	CAngle angClosedError;//闭合差角度对象

	/*  A(i+1)=A(i)+左角－180
	  A(i+1)=A(i)-右角＋180
	  如果大于360度还要减360*/

	if (m_nType == 1) //附合导线
	{
		CAngle StartBearing;//起始方位角
		CAngle EndBearing;//终点附合方位角

		StartBearing = Azi(m_pKnownPoint[0], m_pKnownPoint[1]);
		EndBearing = Azi(m_pKnownPoint[2], m_pKnownPoint[3]);

		//沿导线路线递推终止边的方位角
		for (int i = 0; i < m_iAngleObsCount; i++)
		{
			if (m_nAngleType == 1)//左角
				StartBearing(DEG) = StartBearing(DEG) + m_pAngleObs[i].ObsValue(DEG) - 180;
			else//右角
				StartBearing(DEG) = StartBearing(DEG) - m_pAngleObs[i].ObsValue(DEG) + 180;

			if (StartBearing(DEG) > 360)
				StartBearing(DEG) = StartBearing(DEG) - 360;
			else if (StartBearing(DEG) < 0)
				StartBearing(DEG) = StartBearing(DEG) + 360;
		}
		angClosedError = StartBearing - EndBearing;
	}

	else if (m_nType == 2)//闭合导线
	{
		CAngle angSum(0, DEG);
		//第一个角为连接角，不参与闭合差计算与角度分配
		for (int i = 1; i < m_iAngleObsCount; i++)
		{
			angSum = angSum + m_pAngleObs[i].ObsValue;
		}
		angClosedError(DEG) = angSum(DEG) - (m_iAngleObsCount - 2) * 180;
	}

	return angClosedError;
}

//分配角度闭合差
void CSingleTraverse::DistributeAngleError(const CAngle& AngleError)
{
	if (m_nType == 1) //附合导线
	{
		for (int i = 0; i < m_iAngleObsCount; i++)
		{
			m_pAngleObs[i].ObsValue(DEG) = m_pAngleObs[i].ObsValue(DEG) -
				(AngleError(DEG) / m_iAngleObsCount);
		}
	}
	else if (m_nType == 2)//闭合导线
	{
		//第一个角为连接角，不参与闭合差计算与角度分配
		for (int i = 1; i < m_iAngleObsCount; i++)
		{
			m_pAngleObs[i].ObsValue(DEG) = m_pAngleObs[i].ObsValue(DEG) -
				(AngleError(DEG) / (m_iAngleObsCount - 1));
		}
	}
}
//近似坐标推算及坐标闭合差计算
//注意：fx,fy以引用的方式调用，把闭合差传递出来
void CSingleTraverse::CoorClosedError(double& fx, double& fy)
{
	double dx, dy;
	if (m_nType == 1 || m_nType == 2)//附合导线或闭合导线
	{
		CAngle StartBearing;//起始方位角       
		StartBearing = Azi(m_pKnownPoint[0], m_pKnownPoint[1]);

		//沿导线路线递推终止边的方位角
		for (int i = 0; i < m_iAngleObsCount - 1; i++)
		{
			if (m_nAngleType == 1)//左角
				StartBearing(DEG) = StartBearing(DEG) + m_pAngleObs[i].ObsValue(DEG) - 180;
			else//右角
				StartBearing(DEG) = StartBearing(DEG) - m_pAngleObs[i].ObsValue(DEG) + 180;

			if (StartBearing(DEG) > 360)
				StartBearing(DEG) = StartBearing(DEG) - 360;
			else if (StartBearing(DEG) < 0)
				StartBearing(DEG) = StartBearing(DEG) + 360;

			//计算坐标增量
			dx = m_pDistObs[i].dDist * cos(StartBearing(RAD));
			dy = m_pDistObs[i].dDist * sin(StartBearing(RAD));
			if (i == m_iAngleObsCount - 2)//如果是最后一个点，则计算闭合差
			{
				fx = m_pAngleObs[i].cpStation->X + dx - m_pAngleObs[i].cpEndObj->X;
				fy = m_pAngleObs[i].cpStation->Y + dy - m_pAngleObs[i].cpEndObj->Y;
			}
			else//计算未知点近似坐标
			{
				m_pAngleObs[i].cpEndObj->X = m_pAngleObs[i].cpStation->X + dx;
				m_pAngleObs[i].cpEndObj->Y = m_pAngleObs[i].cpStation->Y + dy;
			}
		}
	}
}



CString* CSingleTraverse::SplitString(CString str, char split, int iSubStrs)
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

bool CSingleTraverse::LoadObsData(const CString& strFileName)
{
	CStdioFile sf;  //创建文件对象

	//以读的形式打开文件，如果打开失败则返回
	if (!sf.Open(strFileName, CFile::modeRead)) return false;

	CString strLine;
	BOOL bEOF = sf.ReadString(strLine);//读取第一行,单导线类型
	m_nType = _ttoi((strLine)); //把读取的第一行字符串转换为数值型

	SetKnownPointSize(m_nType);//根据单导线类型设置已知点个数及数组大小
	//开始读取已知点数据
	sf.ReadString(strLine);//已知点个数
	if (m_iKnownPointCount != _ttoi(strLine))
	{
		return false;
	}
	int n = 0;
	//读取并保存已知点数据
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
	//开始读取未知知点数据
	sf.ReadString(strLine);//未知点个数
	SetUnknownPointSize(_ttoi(strLine));
	//读取并保存未知点数据
	for (int i = 0; i < m_iUnknownPointCount; i++)
	{
		sf.ReadString(strLine);
		CString* pstrData = SplitString(strLine, ',', n);
		m_pUnknownPoint[i].strName = pstrData[0];
		delete[] pstrData;
		pstrData = NULL;
	}
	//开始读取角度观测值数据
	sf.ReadString(strLine);//角度类型
	m_nAngleType = _ttoi(strLine);

	sf.ReadString(strLine);//角度观测值个数
	if (m_iAngleObsCount != _ttoi(strLine))
	{
		return false;
	}
	//读取并保存角度观测值数据
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

	//开始读取边长观测值数据
	sf.ReadString(strLine);//边长观测值个数
	if (m_iDistObsCount != _ttoi(strLine))
	{
		return false;
	}
	//读取并保存边长观测值数据
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

//根据点名从已知点数组中找到控制点，并返回该点的指针
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

//根据点名从未知点数组中找到控制点，并返回该点的指针
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

//根据点名从未知点和已知点数组中找到控制点，并返回该点的指针
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
