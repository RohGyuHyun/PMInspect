// DlgResultList.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "PMInspect.h"
#include "DlgResultList.h"
#include "afxdialogex.h"


// CDlgResultList 대화 상자입니다.

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


// CDlgResultList 메시지 처리기입니다.


BOOL CDlgResultList::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
			return	FALSE;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CDlgResultList::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

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

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
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
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
	OnBnClickedOk();
}
