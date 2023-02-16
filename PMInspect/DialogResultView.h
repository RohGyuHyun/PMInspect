#pragma once
#include "afxcmn.h"
#include "_CtrlImageRect.h"

typedef	struct DLGRESULTVIEW_DATA
{
	RECT	m_InspArea;
	BOOL	m_IsGood;
	int		m_Result;	//	에러 종류(수량 불량, 쉬프트 기타 등등)
	int		m_CntDent;
}DLGRESULTVIEW_DATA, *P_DLGRESULTVIEW_DATA;

const	enum DLGRESULTVIEW_RESULT_TYPE
{
	DLGRESULTVIEW_RESULT_TYPE_LESSCNT	=	0,
	DLGRESULTVIEW_RESULT_TYPE_LSHIFT	=	1,
	DLGRESULTVIEW_RESULT_TYPE_RSHIFT	=	2,
	DLGRESULTVIEW_RESULT_TYPE_USHIFT	=	3,
	DLGRESULTVIEW_RESULT_TYPE_DSHIFT	=	4,
	DLGRESULTVIEW_RESULT_TYPE_DISTRBT	=	5,
	DLGRESULTVIEW_RESULT_TYPE_SIZE		=	6,
	DLGRESULTVIEW_RESULT_TYPE_RESERVE	=	7
};

#define	DLGRESULTVIEW_IMG_NUM_TYPE	2
const	enum DLGRESULTVIEW_IMG_TYPE
{
	DLGRESULTVIEW_IMG_TYPE_GRAB		=	0,
	DLGRESULTVIEW_IMG_TYPE_FILTER	=	1
};

#define	DLGRESULTVIEW_REGION_NUM_TYPE	2
const	enum DLGRESULTVIEW_REGION_TYPE
{
	DLGRESULTVIEW_REGION_TYPE_AREA = 0,
	DLGRESULTVIEW_REGION_TYPE_DENT = 1
};


#define	DLGRESULTVIEW_FILEFORMAT_STR_RESULT_FOLDERNAME _T("Result")
#define	DLGRESULTVIEW_FILEFORMAT_STR_RESULT_DATAFORMAT _T("rslt")
#define	DLGRESULTVIEW_FILEFORMAT_STR_RESULT_IMGFORMAT _T("bmp")
#define	DLGRESULTVIEW_FILEFORMAT_STR_RESULT_RGNFORMAT _T("hobj")

#define	DLGRESULTVIEW_FILEFORMAT_ID	0x001010FA

typedef	struct DLGRESULTVIEW_FILEFORMAT_HEADER
{
	DWORD					m_ID;
	LONG					m_ImageWidth;
	LONG					m_ImageHeight;
	LONG					m_CountResultArea;
}DLGRESULTVIEW_FILEFORMAT_HEADER, *P_DLGRESULTVIEW_FILEFORMAT_HEADER;

typedef	struct DLGRESULTVIEW_FILEFORMAT
{
	DLGRESULTVIEW_FILEFORMAT_HEADER	m_Header;
	P_DLGRESULTVIEW_DATA			m_pResultData;


	DLGRESULTVIEW_FILEFORMAT()
	{
		ZeroMemory(this, sizeof(DLGRESULTVIEW_FILEFORMAT));
		m_Header.m_ID = DLGRESULTVIEW_FILEFORMAT_ID;
	}
	~DLGRESULTVIEW_FILEFORMAT()
	{
		Release();
	}

	VOID	Release()
	{
		if (m_pResultData != NULL)
			delete[]	m_pResultData;

		ZeroMemory(this, sizeof(DLGRESULTVIEW_FILEFORMAT));
		m_Header.m_ID = DLGRESULTVIEW_FILEFORMAT_ID;
	}
}DLGRESULTVIEW_FILEFORMAT, *P_DLGRESULTVIEW_FILEFORMAT;


// CDialogResultView 대화 상자입니다.

class CDialogResultView : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogResultView)

public:
	CDialogResultView(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDialogResultView();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_RESULT_VIEW };
#endif

private:
	CString						m_strModelDir;
	CString						m_strID;
	C_CtrlImageRect				*m_pDlgImgRect;
	CListCtrl					m_ListResult;
	CListCtrl					m_ListDescResult;
	Hobject						m_ImageResult[DLGRESULTVIEW_IMG_NUM_TYPE];
	Hobject						m_Region[DLGRESULTVIEW_REGION_NUM_TYPE];
	DLGRESULTVIEW_FILEFORMAT	m_ResultData;
	LONG						m_SelectedIdx;

	VOID					ResetData();
	BOOL					SaveData(LPCTSTR strDir, LPCTSTR strFileName);
	BOOL					LoadData(LPCTSTR strDir = NULL, LPCTSTR strFileName = NULL);	//	NULL일 경우 현재 데이터 로드
	VOID					RefreshData();
	VOID					ResultSelected(LONG Idx);
	VOID					DisplayImage();

public:
	VOID	Release();
	BOOL	ChangeModel(LPCTSTR strModelDir);
	BOOL	SaveAndViewResultData(LPSYSTEMTIME pSysTime, LPCTSTR strID = NULL);

protected:
	LRESULT OnImageRectMessage(WPARAM para0, LPARAM para1);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnClose();
	int m_IdxDispImg;
	BOOL m_bDisplayRegion;
	afx_msg void OnBnClickedRadioResultGrab();
	afx_msg void OnBnClickedRadioResultFilter();
	afx_msg void OnBnClickedCheckResultRegion();
	afx_msg void OnBnClickedBtnDlgresulviewOk();
	afx_msg void OnBnClickedBtnDlgresulviewOther();
	afx_msg void OnNMDblclkListResult(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMReturnListResult(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
