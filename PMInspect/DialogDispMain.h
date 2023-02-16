#pragma once

#include "_CtrlImageRect.h"
#include "PRSPTNFINDER\PRSPTNFINDER.h"
#include "PARAMANAGER\PARAMANAGER.h"
#include "include\VisionModule\VisionModule.h"

#include "MILCtrl/milctrl.h"

// CDialogDispMain ��ȭ �����Դϴ�.

class CDialogDispMain : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogDispMain)

public:
	CDialogDispMain(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDialogDispMain();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_DISP_MAIN };
#endif

private:
	C_CtrlImageRect *m_pDlgImgRect;
	Hobject			m_HalGrabImage;
	MILCTRL_IMGINFO	m_ImageInfo;
	PRSPTNFINDER	m_PrsPtnFinder;
	PARAMANAGER		m_ParaManager;
	typeInspPara	m_Param;
	CString			m_strModelPath;

	BOOL	InitDialog();	//	��ü ��� �ʱ�ȭ

	//ngh
	CVisionModule*				m_pVision;
	BOOL m_bLive;
	BOOL m_isWinVisb;
	int m_nXIdx;
	int m_nYIdx;

	BOOL m_bSelectRectView;
	BOOL m_bTestGrabStart;
public:
	BOOL	ConvImgToHalBuf(Hobject *pHalBuf, P_MILCTRL_IMGINFO pImg);
	BOOL	LoadModelData(LPCTSTR strModelPath);
	BOOL	SetActiveTracker(BOOL bActive);
	BOOL	SaveParam();

	void	MoveWindow(int x, int y, int nWidth, int nHeight, BOOL bRepaint = TRUE);
	void	MoveWindow(LPCRECT lpRect, BOOL bRepaint = TRUE);


	//ngh
	long GetDisplay() { return m_pDlgImgRect->GetView(); };
	void UpdateInspRgn();
	void UpdateSelectRect(BOOL isSelect = FALSE);
	void SetGrab();
	void SetInspection(char *cSaveImagePath);
	typeInspPara GetInspPara();
	void SetInspPara(typeInspPara *InspPara);
	BOOL LoadModelData(char *strModelPath);
	BOOL SaveModelData();
	void SetUpdateInspRgn(BOOL isWinVisb);
	void SetSelectRect(int x_idx, int y_idx) { m_nXIdx = x_idx; m_nYIdx = y_idx; UpdateSelectRect(); };
	BOOL GetGrabEnd() { return g_MilCtrl.GetGrabEnd(); };
	void InitRslt() { m_pVision->InitRslt(); };
	void SetInspType(int nInspType) { m_pVision->SetInspIdx(nInspType); };
	void SetInspRgn(int nInspType);

	BOOL GetGrabWait() { return g_MilCtrl.GetGrabWait(); };
	BOOL GetInspEnd() { return m_pVision->GetInspectionEnd(); };
	BOOL GetInspRslt(int nInspType) { return m_pVision->GetInspRslt(nInspType); };
	void UpdateStartEndInspRect(int nInspType) { m_pVision->SetStartEndInspRectView(nInspType); };
	void SetUpdateSelectRect(BOOL isView) { m_bSelectRectView = isView; };
	CVisionModule*	GetVisionModule() { return	m_pVision; };	//	bjs �ӽ� �߰�
	void SetATFHomming();
	void SetGrabEnd(BOOL isEnd) { g_MilCtrl.SetGrabEnd(isEnd); };
	
protected:
	LRESULT OnImageRectMessage(WPARAM para0, LPARAM para1);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
