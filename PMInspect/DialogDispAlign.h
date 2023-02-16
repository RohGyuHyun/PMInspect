#pragma once

#include "_CtrlImageRect.h"
#include "PYLONCTRL\PYLONCTRL.h"

// CDialogDispAlign 대화 상자입니다.
class CDialogDispAlign : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogDispAlign)

public:
	CDialogDispAlign(LONG AlignIdx, CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDialogDispAlign();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_DISP_ALIGN };
#endif

private:
	C_CtrlImageRect		*m_pDlgImgRect;
	PYLONCTRL_IMGINFO	m_CamImage;
	Hobject				m_HalconImage;
	LONG				m_AlignIdx;
	RECT				m_ProcessArea[2];
	BOOL				m_bIsLive;

	BOOL	InitDialog();	//	전체 모듈 초기화
	BOOL	ConvImgToHalBuf(Hobject *pHalBuf, P_PYLONCTRL_IMGINFO pImg);
	BOOL	DisplayCross();

	BOOL	GrabContinuous(BOOL bOn);

public:
	BOOL	SetActiveTracker(BOOL bActive, LPRECT pPatternArea = NULL, LPRECT pSearchArea = NULL);
	BOOL	GetTrackerArea(LPRECT pPatternArea, LPRECT pSearchArea);

	void	MoveWindow(int x, int y, int nWidth, int nHeight, BOOL bRepaint = TRUE);
	void	MoveWindow(LPCRECT lpRect, BOOL bRepaint = TRUE);
	void	SetZoomFit();
	void	SetZoomOrg();

	BOOL	Live(BOOL bOn);
	BOOL	GrabSync();
	BOOL	GreabAsync();
	BOOL	IsGrabbing();
	BOOL	ForceDisplay();
	void	SetTitle(LPCTSTR strTitle);

	BOOL		IsLive() { return	m_bIsLive; }
	Hobject*	GetImage() { return	&m_HalconImage; }
	Hlong		GetView() { return m_pDlgImgRect->GetView(); }

	void EnableUI(C_CtrlImageRect_UI UI, BOOL bEnable = TRUE) {	m_pDlgImgRect->EnableUI(UI, bEnable);	}
	void ShowUI(C_CtrlImageRect_UI UI, BOOL bShow = TRUE) { m_pDlgImgRect->ShowUI(UI, bShow); }

protected:
	LRESULT OnImageRectMessage(WPARAM para0, LPARAM para1);
	LRESULT OnGrabReturnMessage(WPARAM para0, LPARAM para1);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.


	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
