#pragma once
#include "afxcmn.h"


// CDlgResultList ��ȭ �����Դϴ�.

class CDlgResultList : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgResultList)

public:
	CDlgResultList(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgResultList();

private:
	CString	m_strRoot;
	CString	m_strDir;
	CString	m_strID;
	BOOL	m_IsSelected;

	//	LimitLevel -1�� ���Ѵ�, �� �̻��� �ش� ���������� ó��
	VOID	FindAndInsertList(LPCTSTR strRoot, HTREEITEM hParent, int Level, int LimitLevel = -1, int *pMaxLevel = NULL, int *pMaxLevelCnt = NULL);

public:
	VOID	SetRootDir(LPCTSTR strRoot);
	LPCTSTR	GetSelectedDir();
	LPCTSTR	GetSelectedID();

	BOOL	IsSelected() {	return	m_IsSelected;	}


// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_RESULT_LIST };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedOk();
	CTreeCtrl m_TreeResult;
	virtual BOOL OnInitDialog();
	afx_msg void OnNMDblclkTreeResult(NMHDR *pNMHDR, LRESULT *pResult);
};
