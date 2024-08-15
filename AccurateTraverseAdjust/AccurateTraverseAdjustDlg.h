
// AccurateTraverseAdjustDlg.h: 头文件
//

#pragma once
#include"AccurateTraverse.h"

// CAccurateTraverseAdjustDlg 对话框
class CAccurateTraverseAdjustDlg : public CDialogEx
{
// 构造
public:
	CAccurateTraverseAdjustDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ACCURATETRAVERSEADJUST_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	bool isLoadData = 0;//判断是否已经加载数据
	bool isRoughCoor = 0;//判断是否已经坐标概算
	bool isRigAdjust = 0;//判断是否已经严密平差
	CString strObsData;
	CString strRoughEstCoor;
	CAccurateTraverse m_CAccurateTraverse;
	CString* SplitString(CString str, char split, int& iSubStrs);//分割字符串
	afx_msg void OnBnClickedLoaddata();//加载数据
	afx_msg void OnBnClickedRoughcal();//坐标概算
	afx_msg void OnBnClickedRigadjust();//严密平差
	afx_msg void OnBnClickedDrawcontrolnet();//绘制图形
	afx_msg void OnBnClickedClose();//关闭
};
