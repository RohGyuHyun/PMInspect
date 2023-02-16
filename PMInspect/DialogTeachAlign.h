#pragma once

#include	"include\IMGALIGN\IMGALIGN.h"
#include	"include\WNDDISPHALCON\WNDDISPHALCON.h"
#include	"DialogMotion.h"
#include	"DialogDispAlign.h"

#define MAX_ALIGN_DOUBLE_PARA_CNT		18
const	enum TEACHALIGN_PARA_IDX
{
	TEACHALIGN_PARA_IDX_UPPXX,
	TEACHALIGN_PARA_IDX_UPPXY,
	TEACHALIGN_PARA_IDX_UPPLX,
	TEACHALIGN_PARA_IDX_UPPLY,
	TEACHALIGN_PARA_IDX_ANGPPL,
	TEACHALIGN_PARA_IDX_DISTROTX,
	TEACHALIGN_PARA_IDX_DISTROTY,
	TEACHALIGN_PARA_IDX_RECHECKALIGN,
	TEACHALIGN_PARA_IDX_XPOS1,
	TEACHALIGN_PARA_IDX_YPOS1,
	TEACHALIGN_PARA_IDX_TPOS1,
	TEACHALIGN_PARA_IDX_LIGHT1,
	TEACHALIGN_PARA_IDX_THRESHOLD1,
	TEACHALIGN_PARA_IDX_XPOS2,
	TEACHALIGN_PARA_IDX_YPOS2,
	TEACHALIGN_PARA_IDX_TPOS2,
	TEACHALIGN_PARA_IDX_LIGHT2,
	TEACHALIGN_PARA_IDX_THRESHOLD2
};

#define	TEACHALIGN_MOTION_SPEED_TYPE_MAX	4
const	enum TEACHALIGN_MOTION_SPEED
{
	TEACHALIGN_MOTION_SPEED_LOW = 0,
	TEACHALIGN_MOTION_SPEED_MID = 1,
	TEACHALIGN_MOTION_SPEED_HIGH = 2,
	TEACHALIGN_MOTION_SPEED_STEP = 3
};

static	LPCTSTR	TEACHALIGN_MOTION_SPEED_TYPE_STR[TEACHALIGN_MOTION_SPEED_TYPE_MAX] =
{
	_T("Low"), _T("Mid"), _T("High"), _T("Step")
};

// DialogTeachAlign 대화 상자입니다.

class CDialogTeachAlign : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogTeachAlign)

public:
	CDialogTeachAlign(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDialogTeachAlign();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_TEACH_ALIGN };
#endif

private:
	CDialogDispAlign*		m_pWndDisplay;
	WNDDISPHALCON			m_WndDispHalcon[2];
	double					m_edit_dAlignPara[MAX_ALIGN_DOUBLE_PARA_CNT];
	long					m_edit_lAlignLightVal;
	long					m_edit_lThreshold;
	P_IMGALIGN				m_pAlign;
	CDialogMotion*			m_pDlgMotion;
	IMGALIGN_PARAM			m_AlignParam;
	double					m_AlignPos[2][3];
	BOOL					m_bBuildPtn[2];
	Hlong					m_WndHandle[2];
	CSliderCtrl				m_slider_align_light;
	TEACHALIGN_MOTION_SPEED	m_SpeedType;
	double					m_dJogSpeed;

	BOOL			Live(BOOL bOn);
	BOOL			DisplayPattern(int Idx);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

private:
	VOID	UpdatePara(BOOL bSave);

public:
	VOID	ReloadParameter();
	BOOL	ApplyParameter();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	DECLARE_EVENTSINK_MAP()
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnClose();

	void ClickBtnenhctrlAlignCamUp();
	void ClickBtnenhctrlAlignCamDown();
	void ClickBtnenhctrlAlignCamLeft();
	void ClickBtnenhctrlAlignCamRight();
	void ClickBtnenhctrlAlignCamSpeed();
	void ClickBtnenhctrlAlignCamCw();
	void ClickBtnenhctrlAlignCamCcw();
	void ClickBtnenhctrlAlignBuildPattern1();
	void ClickBtnenhctrlAlignBuildPattern2();
	void ClickBtnenhctrlAlignAutoPosTeaching();
	void ClickBtnenhctrlApply();
	void MouseDownBtnenhctrlAlignCamUp(short Button, short Shift, long x, long y);
	void MouseDownBtnenhctrlAlignCamDown(short Button, short Shift, long x, long y);
	void MouseDownBtnenhctrlAlignCamLeft(short Button, short Shift, long x, long y);
	void MouseDownBtnenhctrlAlignCamRight(short Button, short Shift, long x, long y);
	void MouseDownBtnenhctrlAlignCamCw(short Button, short Shift, long x, long y);
	void MouseDownBtnenhctrlAlignCamCcw(short Button, short Shift, long x, long y);
	void MouseUpBtnenhctrlAlignCamUp(short Button, short Shift, long x, long y);
	void MouseUpBtnenhctrlAlignCamDown(short Button, short Shift, long x, long y);
	void MouseUpBtnenhctrlAlignCamLeft(short Button, short Shift, long x, long y);
	void MouseUpBtnenhctrlAlignCamRight(short Button, short Shift, long x, long y);
	void MouseUpBtnenhctrlAlignCamCw(short Button, short Shift, long x, long y);
	void MouseUpBtnenhctrlAlignCamCcw(short Button, short Shift, long x, long y);
	double m_dblAlignStep;
	afx_msg void OnMove(int x, int y);
};
