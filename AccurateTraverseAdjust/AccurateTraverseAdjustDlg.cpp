
// AccurateTraverseAdjustDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "AccurateTraverseAdjust.h"
#include "AccurateTraverseAdjustDlg.h"
#include "afxdialogex.h"
#include <locale.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CAccurateTraverseAdjustDlg 对话框



CAccurateTraverseAdjustDlg::CAccurateTraverseAdjustDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ACCURATETRAVERSEADJUST_DIALOG, pParent)
	, strObsData(_T(""))
	, strRoughEstCoor(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAccurateTraverseAdjustDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_ObsData, strObsData);
	DDX_Text(pDX, IDC_RoughCoor, strRoughEstCoor);
}

BEGIN_MESSAGE_MAP(CAccurateTraverseAdjustDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_LoadData, &CAccurateTraverseAdjustDlg::OnBnClickedLoaddata)
	ON_BN_CLICKED(IDC_RoughCal, &CAccurateTraverseAdjustDlg::OnBnClickedRoughcal)
	ON_BN_CLICKED(IDC_RigAdjust, &CAccurateTraverseAdjustDlg::OnBnClickedRigadjust)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_DrawControlNet, &CAccurateTraverseAdjustDlg::OnBnClickedDrawcontrolnet)
	ON_BN_CLICKED(IDC_Close, &CAccurateTraverseAdjustDlg::OnBnClickedClose)
END_MESSAGE_MAP()


// CAccurateTraverseAdjustDlg 消息处理程序

BOOL CAccurateTraverseAdjustDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CAccurateTraverseAdjustDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CAccurateTraverseAdjustDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CAccurateTraverseAdjustDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

CString* CAccurateTraverseAdjustDlg::SplitString(CString str, char split, int& iSubStrs)
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

void CAccurateTraverseAdjustDlg::OnBnClickedLoaddata()//加载数据
{
	if (isLoadData)
	{
		MessageBox(_T("已完成数据导入，请勿重复导入!"));
		return;
	}
	CFileDialog dlgFile(TRUE, _T("dat"), NULL,
		OFN_ALLOWMULTISELECT | OFN_EXPLORER,
		_T("(文本文件)|*.dat"));
	if (dlgFile.DoModal() == IDCANCEL) return;
	CString strFileName = dlgFile.GetPathName();
	m_CAccurateTraverse.LoadObsData(strFileName,strObsData);
	if (!m_CAccurateTraverse.isRightData)
	{
		MessageBox(_T("文件数据格式错误，请重新选择文件!"));
		m_CAccurateTraverse.isRightData = 1;//恢复默认状态，即可读取文件状态
		return;
	}
	isLoadData = 1;//标记已读取文件
	UpdateData(0);
}

void CAccurateTraverseAdjustDlg::OnBnClickedRoughcal()                                                                              
{
	if (!isLoadData)
	{
		MessageBox(_T("请先完成数据导入!"));
		return;
	}
	if (isRoughCoor)
	{
		MessageBox(_T("已完成坐标概算，请勿重复计算!"));
		return;
	}
	m_CAccurateTraverse.CoorRoughEst();
	isRoughCoor = 1;//标记已坐标概算
	CString strLine;
	for (int i = 0; i < m_CAccurateTraverse.GetUnKnownPointSize(); i++)
	{
		strLine.Format(_T("%s,%.4f,%.4f"),
			m_CAccurateTraverse.GetUnKnownPoint(i).strName,
			m_CAccurateTraverse.GetUnKnownPoint(i).X,
			m_CAccurateTraverse.GetUnKnownPoint(i).Y);
			strRoughEstCoor += strLine;
			strRoughEstCoor += _T("\r\n");
	}
	strRoughEstCoor.TrimRight();
	UpdateData(0);
}

void CAccurateTraverseAdjustDlg::OnBnClickedRigadjust()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!isRoughCoor)
	{
		MessageBox(_T("请先完成坐标概算!"));
		return;
	}
	if (isRigAdjust)
	{
		MessageBox(_T("已完成平差计算，请勿重复计算!"));
		return;
	}
	CFileDialog dlgFile(FALSE, _T("txt"), NULL, OFN_EXPLORER,
		_T("(文本文件)|*.txt"));
	if (dlgFile.DoModal() == IDCANCEL) return;
	CString strFileName = dlgFile.GetPathName();
	setlocale(LC_ALL, "");
	m_CAccurateTraverse.RigorousAdjust(strFileName);
	isRigAdjust = 1;//标记已严密平差 
}

void CAccurateTraverseAdjustDlg::OnBnClickedDrawcontrolnet()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!isRigAdjust)
	{
		MessageBox(_T("请先完成严密平差!"));
		return;
	}
	CWnd *pWin;
	CRect rect;
	pWin = GetDlgItem(IDC_DrawFrame);
	pWin->GetClientRect(rect);
	CDC* pDC = pWin->GetDC();
	m_CAccurateTraverse.DrawControlNet(pDC, rect,0.3);
	UpdateData(0);
}

void CAccurateTraverseAdjustDlg::OnBnClickedClose()//关闭
{
	// TODO: 在此添加控件通知处理程序代码
	OnCancel();
}
