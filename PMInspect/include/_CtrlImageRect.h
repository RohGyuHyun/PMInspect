#pragma once


// C_CtrlImageRect ��ȭ �����Դϴ�.
#include "resource.h"
#include "3DBtn/btnenhctrl.h"
#include "hasp_api.h"

#define	C_CtrlImageRect_NUM_UI	12
const	enum C_CtrlImageRect_UI
{
	C_CtrlImageRect_UI_LIVE,
	C_CtrlImageRect_UI_LOADIMG,
	C_CtrlImageRect_UI_SAVEIMG,
	C_CtrlImageRect_UI_ZOOMIN,
	C_CtrlImageRect_UI_ZOOMOUT,
	C_CtrlImageRect_UI_ZOOMORG,
	C_CtrlImageRect_UI_ZOOMFIT,
	C_CtrlImageRect_UI_COORD,
	C_CtrlImageRect_UI_LABEL,
	C_CtrlImageRect_UI_GRAY,
	C_CtrlImageRect_UI_GRID,
	C_CtrlImageRect_UI_GENERAL
};

#define USER_MSG_IMG_RECT			WM_USER+1 
//0 : display
//1 : live on
//2 : live off
//3 : image load
//4 : image save
//5 : gray value
class CDialogMainView;
class AFX_EXT_CLASS C_CtrlImageRect : public CDialog
{
	DECLARE_DYNAMIC(C_CtrlImageRect)

public:
	C_CtrlImageRect(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~C_CtrlImageRect();

	// ��ȭ ���� �������Դϴ�.
	enum { IDD = 10000 };



protected:
	CDialogMainView* m_DlgMainView;

	Hlong m_lWindowID;

	long m_lGrayX;
	long m_lGrayY;
	int m_iGray;
	int m_iCamIndex;

#ifdef LOCK_KEY_USE
	hasp_status_t m_SafeNet_USB_status;
	hasp_feature_t m_SafeNet_USB_feature;
	hasp_handle_t m_SafeNet_USB_handle;

	BOOL SafeNet_Lock_LogIn();//���α׷� ���� ��
	void SafeNet_Lock_LogOut();//���α׷� ���� ��
#endif
	double m_dZoomRatio;

	double m_dPixelSize[2];

	HWND m_hnd;
public:
	CBtnenhctrl m_Title;
	CBtnenhctrl m_Coord;
	CBtnenhctrl m_Gray;
	CBtnenhctrl m_btnLive;
	CBtnenhctrl m_btnGray;

public:
	//ImageRectDLL Method
	//Tracker
	void SetActiveTracker(BOOL active);

	//void SetActiveTracker(int idx, BOOL active);
	void AddTracker(short type, long left, long top, long right, long bottom, long color, char *cap);
	short GetNum();
	short GetType(short idx);
	BOOL SetTracker(short idx, long left, long top, long right, long bottom);
	BOOL GetTracker(short idx, long *left, long *top, long *right, long *bottom);
	BOOL DeleteTracker(short idx);
	void DeleteAll();

	//Grid
	void SetActiveGrid(BOOL active);
	BOOL GetActiveGrid();
	void SetGridPitch(double pitch);
	double GetGridPitch();

	//Gray
	void SetActiveGray(BOOL active);
	BOOL GetActiveGray();
	void GetGrayPos(long *x, long *y);
	void SetGray(int gray);

	//Zoom
	void SetZoom(long zoom, long width, long height);
	void SetZoomRatio(double ratio);
	void GetZoom(long *zoom, long *image_width, long *image_height, long *start_x, long *start_y, long *end_x, long *end_y);

	//Display
	void Display();

	//Set
	void SetCamIndex(int idx, CString title, HWND wnd);

	//Live
	BOOL GetActiveLive();
	void SetActiveLive(BOOL active);

	//view handle
	Hlong GetView();

	void Open();

	void Invalidate();
	void PixelGrayValue(long x, long y, short gray);

	void SetPart(long sx, long sy, long ex, long ey);

	void SetPixelSize(double WpixelSize, double HpixelSize);
	double *GetPixelSize();

	void SetDragDrow(BOOL isDrag);
	BOOL GetDragDrow();

	//force position move
	void SetPos(LPRECT pCenterArea);

	BOOL Create(CWnd * pParentWnd);

	virtual	void MoveWindow(int x, int y, int nWidth, int nHeight, BOOL bRepaint = TRUE);
	virtual void MoveWindow(LPCRECT lpRect, BOOL bRepaint = TRUE);
	void EnableUI(C_CtrlImageRect_UI UI, BOOL bEnable = TRUE);
	void ShowUI(C_CtrlImageRect_UI UI, BOOL bShow = TRUE);

private:
	BOOL	m_bVisibleUI[C_CtrlImageRect_NUM_UI];
	RECT	m_RectUI[C_CtrlImageRect_NUM_UI];
	void RepositionUI();

protected:

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnPaint();
	afx_msg void ClickBtnenhctrlLive();
	afx_msg void ClickBtnenhctrlImgOpen();
	afx_msg void ClickBtnenhctrlImgSave();
	afx_msg void ClickBtnenhctrlZoomin();
	afx_msg void ClickBtnenhctrlZoomout();
	afx_msg void ClickBtnenhctrlZoom11();
	afx_msg void ClickBtnenhctrlZoomfit();
	afx_msg void ClickBtnenhctrlGray();
	afx_msg void ClickBtnenhctrlGrid();
	afx_msg void ClickBtnenhctrlGeneral();

	afx_msg void OnRegionConfirm();
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);

	DECLARE_EVENTSINK_MAP()

	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

};
