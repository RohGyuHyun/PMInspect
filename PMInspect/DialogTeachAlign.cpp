// DialogTeachAlign.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "PMInspectDlg.h"
#include "DialogTeach.h"
#include "DialogTeachAlign.h"
#include "afxdialogex.h"


// DialogTeachAlign 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDialogTeachAlign, CDialogEx)

CDialogTeachAlign::CDialogTeachAlign(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_TEACH_ALIGN, pParent)
	, m_dblAlignStep(0)
{
	m_pWndDisplay = NULL;
	ZeroMemory(&m_edit_dAlignPara, sizeof(double)*MAX_ALIGN_DOUBLE_PARA_CNT);
	m_edit_lAlignLightVal = 0;
	m_edit_lThreshold = 0;
	m_pAlign = NULL;
	m_pDlgMotion = NULL;
	m_WndHandle[0] = m_WndHandle[1] = 0;
	m_SpeedType = TEACHALIGN_MOTION_SPEED_LOW;
	m_dJogSpeed = 0.1;
}

CDialogTeachAlign::~CDialogTeachAlign()
{
	if (m_pWndDisplay != NULL)
	{
		delete	m_pWndDisplay;
		m_pWndDisplay = NULL;
	}

	close_window(m_WndHandle[0]);
	close_window(m_WndHandle[1]);
	m_WndHandle[0] = m_WndHandle[1] = 0;
}

void CDialogTeachAlign::DoDataExchange(CDataExchange* pDX)
{
	for (int i = 0; i < MAX_ALIGN_DOUBLE_PARA_CNT; i++)
		DDX_Text(pDX, IDC_EDIT_ALIGN_PARA_0 + i, m_edit_dAlignPara[i]);

	DDX_Text(pDX, IDC_EDIT_ALIGN_LIGHT, m_edit_lAlignLightVal);
	DDX_Text(pDX, IDC_EDIT_ALIGN_PARA_THRESHOLD, m_edit_lThreshold);
	DDX_Control(pDX, IDC_SLIDER_ALIGN_LIGHT, m_slider_align_light);
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_ALIGN_STEP, m_dblAlignStep);
}


BEGIN_MESSAGE_MAP(CDialogTeachAlign, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_WM_HSCROLL()
	ON_WM_CLOSE()
	ON_WM_MOVE()
END_MESSAGE_MAP()


// CDialogTeachAlign 메시지 처리기입니다.
BEGIN_EVENTSINK_MAP(CDialogTeachAlign, CDialogEx)
	ON_EVENT(CDialogTeachAlign, IDC_BTNENHCTRL_ALIGN_CAM_UP, DISPID_CLICK, CDialogTeachAlign::ClickBtnenhctrlAlignCamUp, VTS_NONE)
	ON_EVENT(CDialogTeachAlign, IDC_BTNENHCTRL_ALIGN_CAM_DOWN, DISPID_CLICK, CDialogTeachAlign::ClickBtnenhctrlAlignCamDown, VTS_NONE)
	ON_EVENT(CDialogTeachAlign, IDC_BTNENHCTRL_ALIGN_CAM_LEFT, DISPID_CLICK, CDialogTeachAlign::ClickBtnenhctrlAlignCamLeft, VTS_NONE)
	ON_EVENT(CDialogTeachAlign, IDC_BTNENHCTRL_ALIGN_CAM_RIGHT, DISPID_CLICK, CDialogTeachAlign::ClickBtnenhctrlAlignCamRight, VTS_NONE)
	ON_EVENT(CDialogTeachAlign, IDC_BTNENHCTRL_ALIGN_CAM_SPEED, DISPID_CLICK, CDialogTeachAlign::ClickBtnenhctrlAlignCamSpeed, VTS_NONE)
	ON_EVENT(CDialogTeachAlign, IDC_BTNENHCTRL_ALIGN_CAM_CW, DISPID_CLICK, CDialogTeachAlign::ClickBtnenhctrlAlignCamCw, VTS_NONE)
	ON_EVENT(CDialogTeachAlign, IDC_BTNENHCTRL_ALIGN_CAM_CCW, DISPID_CLICK, CDialogTeachAlign::ClickBtnenhctrlAlignCamCcw, VTS_NONE)
	ON_EVENT(CDialogTeachAlign, IDC_BTNENHCTRL_ALIGN_BUILD_PATTERN1, DISPID_CLICK, CDialogTeachAlign::ClickBtnenhctrlAlignBuildPattern1, VTS_NONE)
	ON_EVENT(CDialogTeachAlign, IDC_BTNENHCTRL_ALIGN_BUILD_PATTERN2, DISPID_CLICK, CDialogTeachAlign::ClickBtnenhctrlAlignBuildPattern2, VTS_NONE)
	ON_EVENT(CDialogTeachAlign, IDC_BTNENHCTRL_ALIGN_AUTO_POS_TEACHING, DISPID_CLICK, CDialogTeachAlign::ClickBtnenhctrlAlignAutoPosTeaching, VTS_NONE)
	ON_EVENT(CDialogTeachAlign, IDC_BTNENHCTRL_ALIGN__APPLY, DISPID_CLICK, CDialogTeachAlign::ClickBtnenhctrlApply, VTS_NONE)
	ON_EVENT(CDialogTeachAlign, IDC_BTNENHCTRL_ALIGN_CAM_UP, DISPID_MOUSEDOWN, CDialogTeachAlign::MouseDownBtnenhctrlAlignCamUp, VTS_I2 VTS_I2 VTS_I4 VTS_I4)
	ON_EVENT(CDialogTeachAlign, IDC_BTNENHCTRL_ALIGN_CAM_DOWN, DISPID_MOUSEDOWN, CDialogTeachAlign::MouseDownBtnenhctrlAlignCamDown, VTS_I2 VTS_I2 VTS_I4 VTS_I4)
	ON_EVENT(CDialogTeachAlign, IDC_BTNENHCTRL_ALIGN_CAM_LEFT, DISPID_MOUSEDOWN, CDialogTeachAlign::MouseDownBtnenhctrlAlignCamLeft, VTS_I2 VTS_I2 VTS_I4 VTS_I4)
	ON_EVENT(CDialogTeachAlign, IDC_BTNENHCTRL_ALIGN_CAM_RIGHT, DISPID_MOUSEDOWN, CDialogTeachAlign::MouseDownBtnenhctrlAlignCamRight, VTS_I2 VTS_I2 VTS_I4 VTS_I4)
	ON_EVENT(CDialogTeachAlign, IDC_BTNENHCTRL_ALIGN_CAM_CW, DISPID_MOUSEDOWN, CDialogTeachAlign::MouseDownBtnenhctrlAlignCamCw, VTS_I2 VTS_I2 VTS_I4 VTS_I4)
	ON_EVENT(CDialogTeachAlign, IDC_BTNENHCTRL_ALIGN_CAM_CCW, DISPID_MOUSEDOWN, CDialogTeachAlign::MouseDownBtnenhctrlAlignCamCcw, VTS_I2 VTS_I2 VTS_I4 VTS_I4)
	ON_EVENT(CDialogTeachAlign, IDC_BTNENHCTRL_ALIGN_CAM_UP, DISPID_MOUSEUP, CDialogTeachAlign::MouseUpBtnenhctrlAlignCamUp, VTS_I2 VTS_I2 VTS_I4 VTS_I4)
	ON_EVENT(CDialogTeachAlign, IDC_BTNENHCTRL_ALIGN_CAM_DOWN, DISPID_MOUSEUP, CDialogTeachAlign::MouseUpBtnenhctrlAlignCamDown, VTS_I2 VTS_I2 VTS_I4 VTS_I4)
	ON_EVENT(CDialogTeachAlign, IDC_BTNENHCTRL_ALIGN_CAM_LEFT, DISPID_MOUSEUP, CDialogTeachAlign::MouseUpBtnenhctrlAlignCamLeft, VTS_I2 VTS_I2 VTS_I4 VTS_I4)
	ON_EVENT(CDialogTeachAlign, IDC_BTNENHCTRL_ALIGN_CAM_RIGHT, DISPID_MOUSEUP, CDialogTeachAlign::MouseUpBtnenhctrlAlignCamRight, VTS_I2 VTS_I2 VTS_I4 VTS_I4)
	ON_EVENT(CDialogTeachAlign, IDC_BTNENHCTRL_ALIGN_CAM_CW, DISPID_MOUSEUP, CDialogTeachAlign::MouseUpBtnenhctrlAlignCamCw, VTS_I2 VTS_I2 VTS_I4 VTS_I4)
	ON_EVENT(CDialogTeachAlign, IDC_BTNENHCTRL_ALIGN_CAM_CCW, DISPID_MOUSEUP, CDialogTeachAlign::MouseUpBtnenhctrlAlignCamCcw, VTS_I2 VTS_I2 VTS_I4 VTS_I4)
END_EVENTSINK_MAP()


BOOL CDialogTeachAlign::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_slider_align_light.SetRange(0, 255);
	m_slider_align_light.SetRangeMin(0);
	m_slider_align_light.SetRangeMax(255);
	m_slider_align_light.SetPos(0);
	m_slider_align_light.SetTicFreq(10);
	m_slider_align_light.SetLineSize(1);

	CBtnenhctrl*	pBtn = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_ALIGN_CAM_SPEED);
	pBtn->SetCaption(TEACHALIGN_MOTION_SPEED_TYPE_STR[m_SpeedType]);

	CRect	rect;
	GetDlgItem(IDC_STATIC_LIVE)->GetWindowRect(&rect);

	m_pWndDisplay = new CDialogDispAlign(0);
	m_pWndDisplay->Create(IDD_DIALOG_DISP_ALIGN, this);
	m_pWndDisplay->ShowWindow(SW_SHOW);
	m_pWndDisplay->MoveWindow(rect.left, rect.top, rect.Width(), rect.Height(), TRUE);
	m_pWndDisplay->SetZoomFit();
	m_pWndDisplay->EnableUI(C_CtrlImageRect_UI_LIVE, FALSE);
	m_pWndDisplay->EnableUI(C_CtrlImageRect_UI_COORD, FALSE);
	m_pWndDisplay->EnableUI(C_CtrlImageRect_UI_LABEL, FALSE);
	m_pWndDisplay->EnableUI(C_CtrlImageRect_UI_GRAY, FALSE);
	m_pWndDisplay->EnableUI(C_CtrlImageRect_UI_GRID, FALSE);
	m_pWndDisplay->EnableUI(C_CtrlImageRect_UI_GENERAL, FALSE);
	m_pWndDisplay->ShowUI(C_CtrlImageRect_UI_LIVE, FALSE);
	m_pWndDisplay->ShowUI(C_CtrlImageRect_UI_COORD, FALSE);
	m_pWndDisplay->ShowUI(C_CtrlImageRect_UI_LABEL, FALSE);
	m_pWndDisplay->ShowUI(C_CtrlImageRect_UI_GRAY, FALSE);
	m_pWndDisplay->ShowUI(C_CtrlImageRect_UI_GRID, FALSE);
	m_pWndDisplay->ShowUI(C_CtrlImageRect_UI_GENERAL, FALSE);
	m_pWndDisplay->SetTitle(_T("Teach Align"));
	m_pWndDisplay->ShowWindow(SW_HIDE);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDialogTeachAlign::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	if (bShow)
	{
		//	Align, MotionDlg 포인터를 얻어온다
		m_pAlign = ((CPMInspectDlg*)(GetParent()->GetParent()))->m_pDispAlign;
		m_pDlgMotion = ((CPMInspectDlg*)(GetParent()->GetParent()))->m_pDlgMotion;

		m_pAlign->GetParameter(&m_AlignParam);
		if (!m_WndDispHalcon[0].IsCreate())
		{
			m_WndDispHalcon[0].Create(GetDlgItem(IDC_STATIC_PTN1)->m_hWnd, m_pAlign->GetCamWidth(), m_pAlign->GetCamHeight());
			m_WndDispHalcon[1].Create(GetDlgItem(IDC_STATIC_PTN2)->m_hWnd, m_pAlign->GetCamWidth(), m_pAlign->GetCamHeight());
		}

		m_pWndDisplay->SetActiveTracker(TRUE, &m_AlignParam.m_PatternBuildArea[0], &m_AlignParam.m_PatternSearchArea[0]);
		m_edit_lAlignLightVal = m_AlignParam.m_LightLevel[0];
		m_slider_align_light.SetPos(m_edit_lAlignLightVal);
		m_edit_lThreshold = m_AlignParam.m_Threshold[0];
	
		UpdatePara(FALSE);
//		m_pAlign->SetActiveTracker(TRUE);
		DisplayPattern(0);
		DisplayPattern(1);
		m_pWndDisplay->SetZoomFit();
		m_pWndDisplay->ShowWindow(SW_SHOW);
		m_pWndDisplay->Live(TRUE);
	}
	else
	{
		m_pAlign->LoadAlignData();
//		m_pAlign->SetActiveTracker(FALSE);
		m_pWndDisplay->ShowWindow(SW_HIDE);
		m_pWndDisplay->Live(FALSE);
	}

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

void CDialogTeachAlign::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (pScrollBar)
	{
		// 어떤 슬라이더인지 검사
		if (pScrollBar == (CScrollBar*)&m_slider_align_light)
		{
#ifdef ENABLE_LIGHT
			CDialogTeach*	pDlgTeach = (CDialogTeach*)GetParent();
			pDlgTeach->WritePacket(LIGHT_SOURCE_ALIGN, m_slider_align_light.GetPos());
#endif
			m_edit_lAlignLightVal = m_slider_align_light.GetPos();
		}
		UpdateData(FALSE);
	}

	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CDialogTeachAlign::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	m_pAlign->LoadAlignData();
//	m_pAlign->SetActiveTracker(FALSE);
	m_pWndDisplay->ShowWindow(SW_HIDE);
	m_pWndDisplay->Live(FALSE);
	CDialogEx::OnClose();
}

VOID	CDialogTeachAlign::UpdatePara(BOOL bSave)
{
	if (bSave)
	{
		UpdateData(TRUE);

		m_AlignParam.m_umPerPixelX = m_edit_dAlignPara[TEACHALIGN_PARA_IDX_UPPXX];
		m_AlignParam.m_umPerPixelY = m_edit_dAlignPara[TEACHALIGN_PARA_IDX_UPPXY];
		m_AlignParam.m_umPerPulseX = m_edit_dAlignPara[TEACHALIGN_PARA_IDX_UPPLX];
		m_AlignParam.m_umPerPulseY = m_edit_dAlignPara[TEACHALIGN_PARA_IDX_UPPLY];
		m_AlignParam.m_AnglePerPulse = m_edit_dAlignPara[TEACHALIGN_PARA_IDX_ANGPPL];
		m_AlignParam.m_DistRotX = m_edit_dAlignPara[TEACHALIGN_PARA_IDX_DISTROTX];
		m_AlignParam.m_DistRotY = m_edit_dAlignPara[TEACHALIGN_PARA_IDX_DISTROTY];
		m_AlignParam.m_bAlignCheckAllCam = m_edit_dAlignPara[TEACHALIGN_PARA_IDX_RECHECKALIGN];
		m_AlignParam.m_MotionXPos[0] = m_edit_dAlignPara[TEACHALIGN_PARA_IDX_XPOS1];
		m_AlignParam.m_MotionYPos[0] = m_edit_dAlignPara[TEACHALIGN_PARA_IDX_YPOS1];
		m_AlignParam.m_MotionTPos[0] = m_edit_dAlignPara[TEACHALIGN_PARA_IDX_TPOS1];
		m_AlignParam.m_LightLevel[0] = m_edit_dAlignPara[TEACHALIGN_PARA_IDX_LIGHT1];
		m_AlignParam.m_Threshold[0] = m_edit_dAlignPara[TEACHALIGN_PARA_IDX_THRESHOLD1];
		m_AlignParam.m_MotionXPos[1] = m_edit_dAlignPara[TEACHALIGN_PARA_IDX_XPOS2];
		m_AlignParam.m_MotionYPos[1] = m_edit_dAlignPara[TEACHALIGN_PARA_IDX_YPOS2];
		m_AlignParam.m_MotionTPos[1] = m_edit_dAlignPara[TEACHALIGN_PARA_IDX_TPOS2];
		m_AlignParam.m_LightLevel[1] = m_edit_dAlignPara[TEACHALIGN_PARA_IDX_LIGHT2];
		m_AlignParam.m_Threshold[1] = m_edit_dAlignPara[TEACHALIGN_PARA_IDX_THRESHOLD2];

		//AlignPara.m_PatPara[IMGALIGN_NUM_MAX_PTN];	//	리소스 수정 후 적용
		m_pAlign->SetParameter(&m_AlignParam);
	}
	else
	{
		m_edit_dAlignPara[TEACHALIGN_PARA_IDX_UPPXX] = m_AlignParam.m_umPerPixelX;
		m_edit_dAlignPara[TEACHALIGN_PARA_IDX_UPPXY] = m_AlignParam.m_umPerPixelY;
		m_edit_dAlignPara[TEACHALIGN_PARA_IDX_UPPLX] = m_AlignParam.m_umPerPulseX;
		m_edit_dAlignPara[TEACHALIGN_PARA_IDX_UPPLY] = m_AlignParam.m_umPerPulseY;
		m_edit_dAlignPara[TEACHALIGN_PARA_IDX_ANGPPL] = m_AlignParam.m_AnglePerPulse;
		m_edit_dAlignPara[TEACHALIGN_PARA_IDX_DISTROTX] = m_AlignParam.m_DistRotX;
		m_edit_dAlignPara[TEACHALIGN_PARA_IDX_DISTROTY] = m_AlignParam.m_DistRotY;
		m_edit_dAlignPara[TEACHALIGN_PARA_IDX_RECHECKALIGN] = m_AlignParam.m_bAlignCheckAllCam;
		m_edit_dAlignPara[TEACHALIGN_PARA_IDX_XPOS1] = m_AlignParam.m_MotionXPos[0];
		m_edit_dAlignPara[TEACHALIGN_PARA_IDX_YPOS1] = m_AlignParam.m_MotionYPos[0];
		m_edit_dAlignPara[TEACHALIGN_PARA_IDX_TPOS1] = m_AlignParam.m_MotionTPos[0];
		m_edit_dAlignPara[TEACHALIGN_PARA_IDX_LIGHT1] = m_AlignParam.m_LightLevel[0];
		m_edit_dAlignPara[TEACHALIGN_PARA_IDX_THRESHOLD1] = m_AlignParam.m_Threshold[0];
		m_edit_dAlignPara[TEACHALIGN_PARA_IDX_XPOS2] = m_AlignParam.m_MotionXPos[1];
		m_edit_dAlignPara[TEACHALIGN_PARA_IDX_YPOS2] = m_AlignParam.m_MotionYPos[1];
		m_edit_dAlignPara[TEACHALIGN_PARA_IDX_TPOS2] = m_AlignParam.m_MotionTPos[1];
		m_edit_dAlignPara[TEACHALIGN_PARA_IDX_LIGHT2] = m_AlignParam.m_LightLevel[1];
		m_edit_dAlignPara[TEACHALIGN_PARA_IDX_THRESHOLD2] = m_AlignParam.m_Threshold[1];

		//AlignPara.m_PatPara[IMGALIGN_NUM_MAX_PTN];	//	리소스 수정 후 적용
		CBtnenhctrl*	pBtn = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_ALIGN_AUTO_POS_TEACHING);
		if (!m_pAlign->IsPatternLoaded(0) || !m_pAlign->IsPatternLoaded(1))
		{
			pBtn->EnableWindow(FALSE);
		}
		else
		{
			pBtn->EnableWindow(TRUE);
			if (m_AlignParam.m_MotionYPos[0] == m_AlignParam.m_MotionYPos[1] &&
				m_AlignParam.m_MotionTPos[0] == m_AlignParam.m_MotionTPos[1] &&
				abs((m_pAlign->GetCamWidth() / 2) - m_AlignParam.m_PtnCenterX[0]) < 0.55 &&
				abs((m_pAlign->GetCamWidth() / 2) - m_AlignParam.m_PtnCenterX[1]) < 0.55 &&
				abs((m_pAlign->GetCamHeight() / 2) - m_AlignParam.m_PtnCenterY[0]) < 0.55 &&
				abs((m_pAlign->GetCamHeight() / 2) - m_AlignParam.m_PtnCenterY[1]) < 0.55)
			{
				pBtn->SetForeColor(RGB(0, 0, 0));
			}
			else
			{
				pBtn->SetForeColor(RGB(255, 0, 0));
			}
		}

		UpdateData(FALSE);
	}
}

VOID	CDialogTeachAlign::ReloadParameter()
{
	m_pAlign->GetParameter(&m_AlignParam);
	UpdatePara(FALSE);
}

BOOL	CDialogTeachAlign::ApplyParameter()
{
	UpdatePara(TRUE);
	m_pAlign->SetParameter(&m_AlignParam);
	m_pAlign->SaveAlignData();
	return	TRUE;
}






void CDialogTeachAlign::ClickBtnenhctrlAlignCamUp()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (!m_pDlgMotion->GetMotAxis(AXIS_1)->isOriginReturn())
		return;

	if (m_SpeedType != TEACHALIGN_MOTION_SPEED_STEP)
		return;
	UpdateData(TRUE);
	m_pDlgMotion->AxisRMove(AXIS_1, -m_dblAlignStep, true);
}


void CDialogTeachAlign::ClickBtnenhctrlAlignCamDown()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (!m_pDlgMotion->GetMotAxis(AXIS_1)->isOriginReturn())
		return;

	if (m_SpeedType != TEACHALIGN_MOTION_SPEED_STEP)
		return;
	UpdateData(TRUE);
	m_pDlgMotion->AxisRMove(AXIS_1, m_dblAlignStep, true);
}


void CDialogTeachAlign::ClickBtnenhctrlAlignCamLeft()
{
	if (!m_pDlgMotion->GetMotAxis(AXIS_0)->isOriginReturn())
		return;

	if (m_SpeedType != TEACHALIGN_MOTION_SPEED_STEP)
		return;
	UpdateData(TRUE);
	m_pDlgMotion->AxisRMove(AXIS_0, m_dblAlignStep, true);
}

void CDialogTeachAlign::ClickBtnenhctrlAlignCamRight()
{
	if (!m_pDlgMotion->GetMotAxis(AXIS_0)->isOriginReturn())
		return;

	if (m_SpeedType != TEACHALIGN_MOTION_SPEED_STEP)
		return;
	UpdateData(TRUE);
	m_pDlgMotion->AxisRMove(AXIS_0, -m_dblAlignStep, true);
}

void CDialogTeachAlign::ClickBtnenhctrlAlignCamSpeed()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	GetDlgItem(IDC_EDIT_ALIGN_STEP)->EnableWindow(FALSE);
	switch (m_SpeedType)
	{
		case	TEACHALIGN_MOTION_SPEED_LOW		:
			m_SpeedType = TEACHALIGN_MOTION_SPEED_MID;
			m_dJogSpeed = 1.0;
			break;
		case	TEACHALIGN_MOTION_SPEED_MID		:
			m_SpeedType = TEACHALIGN_MOTION_SPEED_HIGH;
			m_dJogSpeed = 2.0;
			break;
		case	TEACHALIGN_MOTION_SPEED_HIGH	:
			m_SpeedType = TEACHALIGN_MOTION_SPEED_STEP;
			m_dJogSpeed = 2.0;
			GetDlgItem(IDC_EDIT_ALIGN_STEP)->EnableWindow(TRUE);
			break;
		case	TEACHALIGN_MOTION_SPEED_STEP	:
			m_SpeedType = TEACHALIGN_MOTION_SPEED_LOW;
			m_dJogSpeed = 0.1;
			break;
		default	:
			return;
	}

	CBtnenhctrl*	pBtn = (CBtnenhctrl*)GetDlgItem(IDC_BTNENHCTRL_ALIGN_CAM_SPEED);
	pBtn->SetCaption(TEACHALIGN_MOTION_SPEED_TYPE_STR[m_SpeedType]);
}

void CDialogTeachAlign::ClickBtnenhctrlAlignCamCw()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (!m_pDlgMotion->GetMotAxis(AXIS_2)->isOriginReturn())
		return;

	if (m_SpeedType != TEACHALIGN_MOTION_SPEED_STEP)
		return;
	UpdateData(TRUE);
	m_pDlgMotion->AxisRMove(AXIS_2, m_dblAlignStep, true);
}

void CDialogTeachAlign::ClickBtnenhctrlAlignCamCcw()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (!m_pDlgMotion->GetMotAxis(AXIS_2)->isOriginReturn())
		return;

	if (m_SpeedType != TEACHALIGN_MOTION_SPEED_STEP)
		return;
	UpdateData(TRUE);
	m_pDlgMotion->AxisRMove(AXIS_2, -m_dblAlignStep, true);
}

void CDialogTeachAlign::ClickBtnenhctrlAlignBuildPattern1()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	UpdatePara(TRUE);
	m_pDlgMotion->AxisGetPosition(AXIS_0, m_AlignPos[0][0]);
	m_pDlgMotion->AxisGetPosition(AXIS_1, m_AlignPos[0][1]);
	m_pDlgMotion->AxisGetPosition(AXIS_2, m_AlignPos[0][2]);

	m_AlignParam.m_MotionXPos[0] = m_AlignPos[0][0];
	m_AlignParam.m_MotionYPos[0] = m_AlignPos[0][1];
	m_AlignParam.m_MotionTPos[0] = m_AlignPos[0][2];
	m_AlignParam.m_LightLevel[0] = m_edit_lAlignLightVal;
	m_AlignParam.m_Threshold[0] = m_edit_lThreshold;
	m_pAlign->SetParameter(&m_AlignParam);
	m_pWndDisplay->Live(FALSE);
	m_pWndDisplay->GrabSync();
	m_pAlign->BuildPattern(0, m_pWndDisplay);
	ReloadParameter();
	DisplayPattern(0);
	m_pWndDisplay->Live(TRUE);
}

void CDialogTeachAlign::ClickBtnenhctrlAlignBuildPattern2()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	UpdatePara(TRUE);
	m_pDlgMotion->AxisGetPosition(AXIS_0, m_AlignPos[1][0]);
	m_pDlgMotion->AxisGetPosition(AXIS_1, m_AlignPos[1][1]);
	m_pDlgMotion->AxisGetPosition(AXIS_2, m_AlignPos[1][2]);

	m_AlignParam.m_MotionXPos[1] = m_AlignPos[1][0];
	m_AlignParam.m_MotionYPos[1] = m_AlignPos[1][1];
	m_AlignParam.m_MotionTPos[1] = m_AlignPos[1][2];
	m_AlignParam.m_LightLevel[1] = m_edit_lAlignLightVal;
	m_AlignParam.m_Threshold[1] = m_edit_lThreshold;
	m_pAlign->SetParameter(&m_AlignParam);
	m_pWndDisplay->Live(FALSE);
	m_pWndDisplay->GrabSync();
	m_pAlign->BuildPattern(1, m_pWndDisplay);
	ReloadParameter();
	DisplayPattern(1);
	m_pWndDisplay->Live(TRUE);
}

void CDialogTeachAlign::ClickBtnenhctrlAlignAutoPosTeaching()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (m_bBuildPtn[0] && m_bBuildPtn[1])
	{
		CDialogTeach*	pDlgTeach = (CDialogTeach*)GetParent();
		double	dblDgree, dblX, dblY, dblDistance, Delay;
		RECT	PtnRT[2];
		LONG	Width, Height, RetryCount;
		clock_t start;

		UpdatePara(TRUE);
		m_pAlign->SetParameter(&m_AlignParam);
		m_pAlign->GetParameter(&m_AlignParam);

		if (m_pAlign->GetAlignCompPos(
			m_AlignPos[0][0],
			m_AlignPos[1][0],
			m_AlignPos[0][1],
			m_AlignPos[1][1],
			&dblDgree, &dblX, &dblY, &dblDistance, FALSE, TRUE))
		{
			m_AlignPos[1][0] += dblX;
			m_AlignPos[1][1] += dblY;
			m_AlignPos[1][2] += dblDgree;

			for (int i = 0; i < 2; i++)
			{
				Width = m_AlignParam.m_PatternBuildArea[i].right - m_AlignParam.m_PatternBuildArea[i].left;
				Height = m_AlignParam.m_PatternBuildArea[i].bottom - m_AlignParam.m_PatternBuildArea[i].top;
				PtnRT[i].left = (m_pAlign->GetCamWidth() - Width) / 2;
				PtnRT[i].right = PtnRT[i].left + Width;
				PtnRT[i].top = (m_pAlign->GetCamHeight() - Height) / 2;
				PtnRT[i].bottom = PtnRT[i].top + Width;
//				m_pAlign->SetTrackerPos(i, &PtnRT);
			}

			m_pWndDisplay->SetActiveTracker(TRUE, &PtnRT[1]);
			RetryCount = 0;
			while (RetryCount < 10)
			{
				m_pDlgMotion->AxisMove(AXIS_0, m_AlignPos[1][0], MOTION_VEL_MAGNIFICATION, FALSE);
				m_pDlgMotion->AxisMove(AXIS_1, m_AlignPos[1][1], MOTION_VEL_MAGNIFICATION, FALSE);
				m_pDlgMotion->AxisMove(AXIS_2, m_AlignPos[1][2], MOTION_VEL_MAGNIFICATION, FALSE);

				while (!m_pDlgMotion->AxisMoveDone(AXIS_0) ||
					!m_pDlgMotion->AxisMoveDone(AXIS_1) ||
					!m_pDlgMotion->AxisMoveDone(AXIS_2))
				{
					Sleep(100);
				}

				//	여기서 조명 처리
#ifdef	ENABLE_LIGHT
				pDlgTeach->WritePacket(LIGHT_SOURCE_ALIGN, m_AlignParam.m_LightLevel[1]);
#endif // ENABLE_LIGHT

				//	정지 후 리니어 안정화 될때까지 200ms 딜레이
				Delay = 200;
				start = clock();
				while (clock() - start < Delay);

				if (!m_pWndDisplay->GrabSync())
				{
					g_cMsgBox.OkMsgBox(_T("Failed. Pattern Align!"));
					return;
				}

				Delay = 200;
				start = clock();
				while (clock() - start < Delay);

				while (m_pWndDisplay->IsGrabbing());

				m_pWndDisplay->ForceDisplay();

				if (!m_pAlign->BuildPattern(1, m_pWndDisplay))
				{
					g_cMsgBox.OkMsgBox(_T("Failed. Pattern Align!"));
					return;
				}

				ReloadParameter();
				DisplayPattern(1);

				m_AlignParam.m_MotionXPos[1] = m_AlignPos[1][0];
				m_AlignParam.m_MotionYPos[1] = m_AlignPos[1][1];
				m_AlignParam.m_MotionTPos[1] = m_AlignPos[1][2];
				UpdatePara(FALSE);

				if (m_pAlign->GetAlignCompPos(
					m_AlignPos[0][0],
					m_AlignPos[1][0],
					m_AlignPos[0][1],
					m_AlignPos[1][1],
					&dblDgree, &dblX, &dblY, NULL, TRUE, TRUE))
				{
					if ((m_AlignParam.m_umPerPixelX / m_AlignParam.m_umPerPulseX * 0.5) < abs(dblX) ||
						(m_AlignParam.m_umPerPixelY / m_AlignParam.m_umPerPulseY * 0.5) < abs(dblY))
					{
						m_AlignPos[1][0] += dblX;
						m_AlignPos[1][1] += dblY;
					}
					else
					{
						m_AlignPos[0][0] = m_AlignPos[1][0] - dblDistance;
						m_AlignPos[0][1] = m_AlignPos[1][1];
						m_AlignPos[0][2] = m_AlignPos[1][2];

						m_pDlgMotion->AxisMove(AXIS_0, m_AlignPos[0][0], MOTION_VEL_MAGNIFICATION, FALSE);
						m_pDlgMotion->AxisMove(AXIS_1, m_AlignPos[0][1], MOTION_VEL_MAGNIFICATION, FALSE);
						m_pDlgMotion->AxisMove(AXIS_2, m_AlignPos[0][2], MOTION_VEL_MAGNIFICATION, FALSE);

						while (!m_pDlgMotion->AxisMoveDone(AXIS_0) ||
							!m_pDlgMotion->AxisMoveDone(AXIS_1) ||
							!m_pDlgMotion->AxisMoveDone(AXIS_2))
						{
							Sleep(100);
						}

						m_pWndDisplay->SetActiveTracker(TRUE, &PtnRT[0]);

						//	여기서 조명 처리
#ifdef ENABLE_LIGHT
						pDlgTeach->WritePacket(LIGHT_SOURCE_ALIGN, m_AlignParam.m_LightLevel[0]);
#endif // ENABLE_LIGHT

						//	정지 후 리니어 안정화 될때까지 200ms 딜레이
						Delay = 200;
						start = clock();
						while (clock() - start < Delay);

						if (!m_pWndDisplay->GrabSync())
						{
							g_cMsgBox.OkMsgBox(_T("Failed. Pattern Align!"));
							return;
						}

						Delay = 200;
						start = clock();
						while (clock() - start < Delay);

						while (m_pWndDisplay->IsGrabbing());

						m_pWndDisplay->ForceDisplay();

						if (!m_pAlign->BuildPattern(0, m_pWndDisplay))
						{
							g_cMsgBox.OkMsgBox(_T("Failed. Pattern Align!"));
							return;
						}

						ReloadParameter();
						DisplayPattern(0);

						m_AlignPos[0][0] = (double)((LONG)(m_AlignPos[0][0] * 10000) / 10000.0);
						m_AlignPos[0][1] = (double)((LONG)(m_AlignPos[0][1] * 1000) / 1000.0);
						m_AlignPos[0][2] = (double)((LONG)(m_AlignPos[0][2] * 1000) / 1000.0);
						m_AlignPos[1][0] = (double)((LONG)(m_AlignPos[1][0] * 10000) / 10000.0);
						m_AlignPos[1][1] = (double)((LONG)(m_AlignPos[1][1] * 1000) / 1000.0);
						m_AlignPos[1][2] = (double)((LONG)(m_AlignPos[1][2] * 1000) / 1000.0);

						m_pDlgMotion->SetAxisPos(AXIS_0, 0, m_AlignPos[0][0]);
						m_pDlgMotion->SetAxisPos(AXIS_1, 0, m_AlignPos[0][1]);
						m_pDlgMotion->SetAxisPos(AXIS_2, 0, m_AlignPos[0][2]);
						m_pDlgMotion->SetAxisPos(AXIS_0, 1, m_AlignPos[1][0]);
						m_pDlgMotion->SetAxisPos(AXIS_1, 1, m_AlignPos[1][1]);
						m_pDlgMotion->SetAxisPos(AXIS_2, 1, m_AlignPos[1][2]);

						m_AlignParam.m_MotionXPos[0] = m_pDlgMotion->GetAxisPos(AXIS_0, 0);
						m_AlignParam.m_MotionYPos[0] = m_pDlgMotion->GetAxisPos(AXIS_1, 0);
						m_AlignParam.m_MotionTPos[0] = m_pDlgMotion->GetAxisPos(AXIS_2, 0);
						m_AlignParam.m_MotionXPos[1] = m_pDlgMotion->GetAxisPos(AXIS_0, 1);
						m_AlignParam.m_MotionYPos[1] = m_pDlgMotion->GetAxisPos(AXIS_1, 1);
						m_AlignParam.m_MotionTPos[1] = m_pDlgMotion->GetAxisPos(AXIS_2, 1);
						UpdatePara(FALSE);

						//	여기서 위치 정보 저장


						g_cMsgBox.OkMsgBox(_T("Teaching Complete!"));
						return;
					}

					RetryCount++;
				}
				else
				{
					g_cMsgBox.OkMsgBox(_T("Failed. Pattern Align!"));
					return;
				}
			}

			g_cMsgBox.OkMsgBox(_T("Failed. Pattern Align!"));
			return;
		}
		else
		{
			g_cMsgBox.OkMsgBox(_T("Failed. Pattern Align!"));
			return;
		}
	}
	else
	{
		g_cMsgBox.OkMsgBox(_T("Please create a Align pattern."));
		return;
	}
}

void CDialogTeachAlign::ClickBtnenhctrlApply()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	ApplyParameter();
}

BOOL	CDialogTeachAlign::Live(BOOL bOn)
{
	return	TRUE;
}

BOOL	CDialogTeachAlign::DisplayPattern(int Idx)
{
	LONGLONG	PatternID;
	Hobject		PtnCont;
	POINT		pt;

	pt.x = m_pAlign->GetCamWidth() / 2;
	pt.y = m_pAlign->GetCamHeight() / 2;

	if (m_pAlign->IsPatternLoaded(Idx))
	{
		m_pAlign->GetPatternID(Idx, &PatternID);
		set_shape_model_origin(PatternID, (Hlong)-m_AlignParam.m_PtnCenterY[Idx], (Hlong)-m_AlignParam.m_PtnCenterX[Idx]);
		get_shape_model_contours(&PtnCont, (Hlong)PatternID, 1);
		set_shape_model_origin(PatternID, 0, 0);

		RECT	FitArea;
		HTuple	Left, Top, Right, Bottom;
		double	dblLeft, dblTop, dblRight, dblBottom;
		smallest_rectangle1_xld(PtnCont, &Top, &Left, &Bottom, &Right);
		tuple_min(Left, &dblLeft);
		tuple_min(Top, &dblTop);
		tuple_max(Right, &dblRight);
		tuple_max(Bottom, &dblBottom);

		FitArea.left = (LONG)dblLeft - 10;
		FitArea.top = (LONG)dblTop - 10;
		FitArea.right = (LONG)dblRight + 10;
		FitArea.bottom = (LONG)dblBottom + 10;

		m_WndDispHalcon[Idx].SetDisplayArea(&FitArea);
		m_WndDispHalcon[Idx].ClearDisplay();
		m_WndDispHalcon[Idx].DisplayCross(pt, 0, RGB(0, 255, 0), FALSE);
		m_WndDispHalcon[Idx].DisplayObject(&PtnCont, RGB(0, 0, 255), FALSE);
		pt.x = m_AlignParam.m_PtnCenterX[Idx];
		pt.y = m_AlignParam.m_PtnCenterY[Idx];
		m_WndDispHalcon[Idx].DisplayCross(pt, 50, RGB(255, 0, 0), FALSE);
		m_bBuildPtn[Idx] = TRUE;
	}
	else
	{
		m_WndDispHalcon[Idx].SetDisplayArea();
		m_WndDispHalcon[Idx].ClearDisplay();
		m_WndDispHalcon[Idx].DisplayCross(pt, 0, RGB(0, 255, 0), FALSE);
		m_bBuildPtn[Idx] = FALSE;
	}

	return	TRUE;
}


void CDialogTeachAlign::MouseDownBtnenhctrlAlignCamUp(short Button, short Shift, long x, long y)
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (!m_pDlgMotion->GetMotAxis(AXIS_1)->isOriginReturn())
		return;

	if (m_SpeedType != TEACHALIGN_MOTION_SPEED_STEP)
		m_pDlgMotion->AxisVMove(AXIS_1, m_pDlgMotion->GetMotAxis(AXIS_1)->GetJogVel() * -m_dJogSpeed, m_pDlgMotion->GetMotAxis(AXIS_1)->GetJogAcc(), m_pDlgMotion->GetMotAxis(AXIS_1)->GetJogDec());
}

void CDialogTeachAlign::MouseDownBtnenhctrlAlignCamDown(short Button, short Shift, long x, long y)
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (!m_pDlgMotion->GetMotAxis(AXIS_1)->isOriginReturn())
		return;

	if (m_SpeedType != TEACHALIGN_MOTION_SPEED_STEP)
		m_pDlgMotion->AxisVMove(AXIS_1, m_pDlgMotion->GetMotAxis(AXIS_1)->GetJogVel() * m_dJogSpeed, m_pDlgMotion->GetMotAxis(AXIS_1)->GetJogAcc(), m_pDlgMotion->GetMotAxis(AXIS_1)->GetJogDec());
}

void CDialogTeachAlign::MouseDownBtnenhctrlAlignCamLeft(short Button, short Shift, long x, long y)
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (!m_pDlgMotion->GetMotAxis(AXIS_0)->isOriginReturn())
		return;

	if (m_SpeedType != TEACHALIGN_MOTION_SPEED_STEP)
		m_pDlgMotion->AxisVMove(AXIS_0, m_pDlgMotion->GetMotAxis(AXIS_0)->GetJogVel() * m_dJogSpeed, m_pDlgMotion->GetMotAxis(AXIS_0)->GetJogAcc(), m_pDlgMotion->GetMotAxis(AXIS_0)->GetJogDec());
}

void CDialogTeachAlign::MouseDownBtnenhctrlAlignCamRight(short Button, short Shift, long x, long y)
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (!m_pDlgMotion->GetMotAxis(AXIS_0)->isOriginReturn())
		return;

	if (m_SpeedType != TEACHALIGN_MOTION_SPEED_STEP)
		m_pDlgMotion->AxisVMove(AXIS_0, m_pDlgMotion->GetMotAxis(AXIS_0)->GetJogVel() * -m_dJogSpeed, m_pDlgMotion->GetMotAxis(AXIS_0)->GetJogAcc(), m_pDlgMotion->GetMotAxis(AXIS_0)->GetJogDec());
}

void CDialogTeachAlign::MouseDownBtnenhctrlAlignCamCw(short Button, short Shift, long x, long y)
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (!m_pDlgMotion->GetMotAxis(AXIS_2)->isOriginReturn())
		return;

	if (m_SpeedType != TEACHALIGN_MOTION_SPEED_STEP)
		m_pDlgMotion->AxisVMove(AXIS_2, m_pDlgMotion->GetMotAxis(AXIS_2)->GetJogVel() * m_dJogSpeed, m_pDlgMotion->GetMotAxis(AXIS_2)->GetJogAcc(), m_pDlgMotion->GetMotAxis(AXIS_2)->GetJogDec());
}

void CDialogTeachAlign::MouseDownBtnenhctrlAlignCamCcw(short Button, short Shift, long x, long y)
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (!m_pDlgMotion->GetMotAxis(AXIS_2)->isOriginReturn())
		return;

	if (m_SpeedType != TEACHALIGN_MOTION_SPEED_STEP)
		m_pDlgMotion->AxisVMove(AXIS_2, m_pDlgMotion->GetMotAxis(AXIS_2)->GetJogVel() * -m_dJogSpeed, m_pDlgMotion->GetMotAxis(AXIS_2)->GetJogAcc(), m_pDlgMotion->GetMotAxis(AXIS_2)->GetJogDec());
}

void CDialogTeachAlign::MouseUpBtnenhctrlAlignCamUp(short Button, short Shift, long x, long y)
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (m_SpeedType != TEACHALIGN_MOTION_SPEED_STEP)
		m_pDlgMotion->GetMotAxis(AXIS_1)->Stop();
}

void CDialogTeachAlign::MouseUpBtnenhctrlAlignCamDown(short Button, short Shift, long x, long y)
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (m_SpeedType != TEACHALIGN_MOTION_SPEED_STEP)
		m_pDlgMotion->GetMotAxis(AXIS_1)->Stop();
}

void CDialogTeachAlign::MouseUpBtnenhctrlAlignCamLeft(short Button, short Shift, long x, long y)
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (m_SpeedType != TEACHALIGN_MOTION_SPEED_STEP)
		m_pDlgMotion->GetMotAxis(AXIS_0)->Stop();
}

void CDialogTeachAlign::MouseUpBtnenhctrlAlignCamRight(short Button, short Shift, long x, long y)
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (m_SpeedType != TEACHALIGN_MOTION_SPEED_STEP)
		m_pDlgMotion->GetMotAxis(AXIS_0)->Stop();
}

void CDialogTeachAlign::MouseUpBtnenhctrlAlignCamCw(short Button, short Shift, long x, long y)
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (m_SpeedType != TEACHALIGN_MOTION_SPEED_STEP)
		m_pDlgMotion->GetMotAxis(AXIS_2)->Stop();
}

void CDialogTeachAlign::MouseUpBtnenhctrlAlignCamCcw(short Button, short Shift, long x, long y)
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (m_SpeedType != TEACHALIGN_MOTION_SPEED_STEP)
		m_pDlgMotion->GetMotAxis(AXIS_2)->Stop();
}



void CDialogTeachAlign::OnMove(int x, int y)
{
	CDialogEx::OnMove(x, y);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (m_pWndDisplay != NULL)
	{
		CRect rect;
		GetDlgItem(IDC_STATIC_LIVE)->GetWindowRect(&rect);
		m_pWndDisplay->MoveWindow(rect.left, rect.top, rect.Width(), rect.Height(), TRUE);
	}
}
