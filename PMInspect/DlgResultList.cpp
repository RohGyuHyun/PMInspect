// DlgResultList.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "PMInspect.h"
#include "DlgResultList.h"
#include "afxdialogex.h"


// CDlgResultList ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CDlgResultList, CDialogEx)

CDlgResultList::CDlgResultList(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_RESULT_LIST, pParent)
{
	m_IsSelected = FALSE;
}

CDlgResultList::~CDlgResultList()
{
}

void CDlgResultList::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_RESULT, m_TreeResult);
}


BEGIN_MESSAGE_MAP(CDlgResultList, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgResultList::OnBnClickedOk)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_RESULT, &CDlgResultList::OnNMDblclkTreeResult)
END_MESSAGE_MAP()


// CDlgResultList �޽��� ó�����Դϴ�.


BOOL CDlgResultList::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
			return	FALSE;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CDlgResultList::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	HTREEITEM	hCurItem, hTmpItem;
	int			Level;
	CString		strPath;

	m_IsSelected = FALSE;

	Level = 0;
	hCurItem = hTmpItem = m_TreeResult.GetSelectedItem();
	if (hCurItem == NULL)
		return;

	while (hTmpItem != NULL)
	{
		strPath.Insert(0, m_TreeResult.GetItemText(hTmpItem));
		strPath.Insert(0, _T('\\'));
		hTmpItem = m_TreeResult.GetParentItem(hTmpItem);
		Level++;
	}

	if (Level != 4)
		return;

	m_strID = m_TreeResult.GetItemText(hCurItem);
	m_strDir = m_strRoot + strPath;
	m_IsSelected = TRUE;

	CDialogEx::OnOK();
}


BOOL CDlgResultList::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	m_IsSelected = FALSE;

	if (m_strRoot.IsEmpty())
	{
		CDialogEx::OnCancel();
		return	FALSE;
	}

	DWORD	Attr = GetFileAttributes(m_strRoot);
	if (Attr == -1 || (Attr & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY)
	{
		CDialogEx::OnCancel();
		return	FALSE;
	}

	int	MaxLevel = 0, Count = 0;
	FindAndInsertList(m_strRoot, TVI_ROOT, 0, 3, &MaxLevel, &Count);

	if( MaxLevel < 3 )
	{
		CDialogEx::OnCancel();
		return	FALSE;
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

VOID	CDlgResultList::SetRootDir(LPCTSTR strRoot)
{
	m_strRoot.Empty();

	if (strRoot == NULL)
		return;

	if (_tcslen(strRoot) == 0)
		return;

	m_strRoot = strRoot;
}

LPCTSTR	CDlgResultList::GetSelectedDir()
{
	if (!m_IsSelected)
		return	NULL;

	return	m_strDir;
}

LPCTSTR	CDlgResultList::GetSelectedID()
{
	if (!m_IsSelected)
		return	NULL;

	return	m_strID;
}

VOID	CDlgResultList::FindAndInsertList(LPCTSTR strRoot, HTREEITEM hParent, int Level, int LimitLevel/*-1*/, int *pMaxLevel/*NULL*/, int *pMaxLevelCnt/*NULL*/)
{
	if (strRoot == NULL || Level < 0 )
		return;

	if (LimitLevel > -1 && Level > LimitLevel)
		return;

	CString		strTmp;
	CFileFind	Finder;
	BOOL		bWorking;
	HTREEITEM	hItem;

	strTmp.Format(_T("%s\\*.*"), strRoot);
	bWorking = Finder.FindFile(strTmp);

	while (bWorking)
	{
		bWorking = Finder.FindNextFile();

		if (Finder.IsDots())
			continue;

		if (Finder.IsDirectory())
		{
			if (pMaxLevel != NULL)
			{
				if (*pMaxLevel < Level)
				{
					*pMaxLevel = Level;
					if (pMaxLevelCnt != NULL)
						*pMaxLevelCnt = 1;
				}
				else if (*pMaxLevel == Level)
				{
					if (pMaxLevelCnt != NULL)
						*pMaxLevelCnt = *pMaxLevelCnt + 1;
				}
			}
			strTmp = Finder.GetFileName();
			hItem = m_TreeResult.InsertItem(strTmp, hParent, TVI_LAST);

			if (Level < LimitLevel)
			{
				strTmp = Finder.GetFilePath();
				FindAndInsertList(strTmp, hItem, Level + 1, LimitLevel, pMaxLevel, pMaxLevelCnt);
			}
		}
	}
}



void CDlgResultList::OnNMDblclkTreeResult(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;
	OnBnClickedOk();
}
