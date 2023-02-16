#pragma once
#include "afxcmn.h"


// CDlgResultList 대화 상자입니다.

class CDlgResultList : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgResultList)

public:
	CDlgResultList(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgResultList();

private:
	CString	m_strRoot;
	CString	m_strDir;
	CString	m_strID;
	BOOL	m_IsSelected;

	//	LimitLevel -1은 무한대, 그 이상은 해당 레벨까지만 처리
	VOID	FindAndInsertList(LPCTSTR strRoot, HTREEITEM hParent, int Level, int LimitLevel = -1, int *pMaxLevel = NULL, int *pMaxLevelCnt = NULL);

public:
	VOID	SetRootDir(LPCTSTR strRoot);
	LPCTSTR	GetSelectedDir();
	LPCTSTR	GetSelectedID();

	BOOL	IsSelected() {	return	m_IsSelected;	}


// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_RESULT_LIST };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedOk();
	CTreeCtrl m_TreeResult;
	virtual BOOL OnInitDialog();
	afx_msg void OnNMDblclkTreeResult(NMHDR *pNMHDR, LRESULT *pResult);
};
