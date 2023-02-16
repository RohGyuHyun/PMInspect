// DialogMotion.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "PMInspect.h"
#include "DialogMotion.h"
#include "afxdialogex.h"

#include "PMInspectDlg.h"

// CDialogMotion ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CDialogMotion, CDialogEx)

CDialogMotion::CDialogMotion(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_MOTION, pParent)
	, m_edit_nStepVal(0)
{
	
	int i = 0;

	m_pMotion = NULL;
	m_pMotion = new CMotion();
	
	for(i = 0; i < MAX_INPUT_NUM; i++)
		m_bOldInput[i] = FALSE;

	for (i = 0; i < MAX_OUTPUT_NUM; i++)
		m_bOldOutput[i] = FALSE;


	for (int i = 0; i < MAX_AXIS_NUM; i++) {
		for (int j = 0; j < MAX_POSITION_NUM; j++) {
			m_dAxisPosition[i][j] = 0.;
		}
	}
	
	m_nRadioMoveType = 0;			// 0 : Jog 1: Step
	m_nRadioAxis = 0;
	m_nRadioPos = 0;
	m_nRadioAxisName = 0;

	for (int i = 0; i < MAX_AXIS_NUM; i++)
		m_bAmpOnFlag[i] = FALSE;

	m_Paramanager.Create();

	m_pDlgAxisManager = NULL;
	m_pDlgAxisManager = new CDialogAxisManage;

	m_dJogSpeed = 0.;
	m_nProcessMoveIdx = 0;
}

CDialogMotion::~CDialogMotion()
{
#ifdef ENABLE_MOTION
	DestroyAJINX();
#endif

	if (NULL != m_pMotion) {
		delete m_pMotion;
		m_pMotion = NULL;
	}

	if (NULL != m_pDlgAxisManager) {
		delete m_pDlgAxisManager;
		m_pDlgAxisManager = NULL;
	}

	m_Paramanager.Release();
}

void CDialogMotion::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	int i = 0, j = 0, idx = 0;

	for (int i = 0; i < MAX_INPUT_NUM; i++)
		DDX_Control(pDX, IDC_BTNENHCTRL_BUTTON_IN_000 + i, m_CBtnInput[i]);

	for (int i = 0; i < MAX_OUTPUT_NUM; i++)
		DDX_Control(pDX, IDC_BTNENHCTRL_BUTTON_OUT_000 + i, m_CBtnOutput[i]);

	for (int i = 0; i < MAX_AXIS_NUM; i++) {
		DDX_Control(pDX, IDC_BTNENHCTRL_LABEL_AXIS_NAME1 + i, m_CBtnAxisName[i]);
		DDX_Control(pDX, IDC_BTNENHCTRL_LABEL_AXIS_PULSE1 + i, m_CBtnAxisPulse[i]);
		DDX_Control(pDX, IDC_BTNENHCTRL_LABEL_AXIS_SENSOR_P1 + i, m_CBtnAxisSensorP[i]);
		DDX_Control(pDX, IDC_BTNENHCTRL_LABEL_AXIS_SENSOR_H1 + i, m_CBtnAxisSensorH[i]);
		DDX_Control(pDX, IDC_BTNENHCTRL_LABEL_AXIS_SENSOR_N1 + i, m_CBtnAxisSensorN[i]);
		DDX_Control(pDX, IDC_BTNENHCTRL_LABEL_AXIS_AMP1 + i, m_CBtnAxisAmp[i]);
	}

	for (int i = 0; i < MAX_AXIS_NUM; i++) {
		for (int j = 0; j < MAX_POSITION_NUM; j++) {
			DDX_Control(pDX, IDC_BTNENHCTRL_LABEL_AXIS1_POS1 + (j + idx), m_CBtnAxisPosition[i][j]);
		}

		idx += MAX_POSITION_NUM;
	}

	DDX_Text(pDX, IDC_EDIT_STEP_VALUE, m_edit_nStepVal);
	DDX_Control(pDX, IDC_COMBO_JOG_SPEED, m_combo_jog_speed);

	
	DDX_Control(pDX, IDC_BTNENHCTRL_LABEL_AXIS8, m_CBtnLabelAxis8);
	DDX_Control(pDX, IDC_BTNENHCTRL_LABEL_AXIS_NAME4, m_CBtnLabelAxisName4);

	DDX_Control(pDX, IDC_BTNENHCTRL_BUTTON_JOG_TYPE1, m_CBtnJog);
	DDX_Control(pDX, IDC_BTNENHCTRL_BUTTON_AMPON, m_CBtnAmpON);
	DDX_Control(pDX, IDC_BTNENHCTRL_BUTTON_AMPOFF, m_CBtnAmpOff);

	DDX_Control(pDX, IDC_BTNENHCTRL_PROGRESS_ORIGIN, m_ctrl_progress);
}


BEGIN_MESSAGE_MAP(CDialogMotion, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_WM_TIMER()
	ON_WM_PAINT()
	ON_CBN_SELCHANGE(IDC_COMBO_JOG_SPEED, &CDialogMotion::OnCbnSelchangeComboJogSpeed)
END_MESSAGE_MAP()


// CDialogMotion �޽��� ó�����Դϴ�.
BOOL CDialogMotion::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

#ifdef ENABLE_MOTION
	//���� ���̺귯�� �ʱ�ȭ
	InitializeAJINX();

	//��� �ʱ�ȭ
	if (MotInitialize())
	{
		TRACE("MotionInitialize Success");
	}
	else
	{
		AfxMessageBox(_T("MotionInitialize Failed"));
	}
#endif

	SetLinkPara();

	for (int i = 0; i < MAX_AXIS_NUM; i++)
	{
		if (AxisIsHomeFinished(i)) {
			if (AxisGetAmpFaultState(i))
				m_CBtnAxisName[i].SetBackColor(GREEN);
			else
				// Amp Fault �����̸� ��ư ���� Red�� ����
				m_CBtnAxisName[i].SetValue(FALSE);
		}
		else {
			m_CBtnAxisName[i].SetBackColor(0x00FFFFFF);
			m_CBtnAxisName[i].SetValue(FALSE);
		}
	}

	for (int i = 0; i < MAX_AXIS_NUM; i++)
	{
		if (!AxisGetAmpOnState(i))
		{
			m_bAmpOnFlag[i] = FALSE;
			break;
		}
		else
			m_bAmpOnFlag[i] = TRUE;
	}

	m_CBtnAxisName[0].SetValue(TRUE);	
	m_CBtnJog.SetValue(TRUE);
	m_CBtnLabelAxis8.SetValue(TRUE);
	m_CBtnLabelAxisName4.SetValue(TRUE);

	m_pDlgAxisManager->Create(IDD_DIALOG_AXIS_MANAGER, this);
	m_pDlgAxisManager->ShowWindow(SW_HIDE);

	m_combo_jog_speed.AddString(_T("LOW"));
	m_combo_jog_speed.AddString(_T("MID"));
	m_combo_jog_speed.AddString(_T("HIGH"));
	m_combo_jog_speed.SetCurSel(0);

	m_ctrl_progress.SetProgressBarMin(0);
	m_ctrl_progress.SetProgressBarMax(100);
	m_ctrl_progress.SetProgressBarValue(m_nProcessMoveIdx);
	m_ctrl_progress.SetCaption(_T(" "));
	m_ctrl_progress.ShowWindow(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}


//���� I/O Motion ���� ���̺귯�� �ʱ�ȭ
BOOL CDialogMotion::InitializeAJINX()
{
	BOOL bRet(TRUE);
	// Initialize library 
	DWORD Code = AxlOpenNoReset(7);

	if (Code == AXT_RT_SUCCESS)
		TRACE("Library is initialized .\n");
	else
	{
		AfxMessageBox(_T("AJINX Failed initialization."));
		return FALSE;
	}

	return TRUE;
}

BOOL CDialogMotion::DestroyAJINX()
{
	BOOL bRet(TRUE);

	MotAmpDisableAll();

	bRet = AxlClose();

	if (bRet)
		TRACE("Library is Close OK!! .\n");
	else
		TRACE("Failed AXL Library Close!!.");

	return bRet;
}

void CDialogMotion::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
#ifdef ENABLE_MOTION
	if (bShow) {
		//LoadData
		SetTimer(TIMER_IO_MONITORING, 300, NULL);		// I/O Monitoring

		m_Paramanager.LoadPara();
		UpdatePara(TRUE);
	}
	else {
		KillTimer(TIMER_IO_MONITORING);
	}
#endif

}

void CDialogMotion::SetFilePath()
{
	CPMInspectDlg * pdlg = (CPMInspectDlg *)AfxGetApp()->m_pMainWnd;

	CString strTemp;
	strTemp.Format(_T("%S"), pdlg->GetCurrentModelPath());

	m_Paramanager.SetPath(strTemp, _T("MotionPos.ini"));
}

void CDialogMotion::SetLinkPara()
{
	CPMInspectDlg * pdlg = (CPMInspectDlg *)AfxGetApp()->m_pMainWnd;

	CString strName[MAX_AXIS_NUM] = { _T("LINEAR_X"), _T("LINEAR_Y"), _T("THETA_T") };

	CString strTemp;
	strTemp.Format(_T("%S"), pdlg->GetCurrentModelPath());

	m_Paramanager.SetPath(strTemp, _T("MotionPos.ini"));

	m_Paramanager.LinkPara(strName[AXIS_0], _T("POSITION_1"), &m_dAxisPosition[AXIS_0][0], _T("0."));
	m_Paramanager.LinkPara(strName[AXIS_0], _T("POSITION_2"), &m_dAxisPosition[AXIS_0][1], _T("0."));
	m_Paramanager.LinkPara(strName[AXIS_0], _T("POSITION_3"), &m_dAxisPosition[AXIS_0][2], _T("0."));
	m_Paramanager.LinkPara(strName[AXIS_0], _T("POSITION_4"), &m_dAxisPosition[AXIS_0][3], _T("0."));
	m_Paramanager.LinkPara(strName[AXIS_0], _T("POSITION_5"), &m_dAxisPosition[AXIS_0][4], _T("0."));

	m_Paramanager.LinkPara(strName[AXIS_1], _T("POSITION_1"), &m_dAxisPosition[AXIS_1][0], _T("0."));
	m_Paramanager.LinkPara(strName[AXIS_1], _T("POSITION_2"), &m_dAxisPosition[AXIS_1][1], _T("0."));
	m_Paramanager.LinkPara(strName[AXIS_1], _T("POSITION_3"), &m_dAxisPosition[AXIS_1][2], _T("0."));
	m_Paramanager.LinkPara(strName[AXIS_1], _T("POSITION_4"), &m_dAxisPosition[AXIS_1][3], _T("0."));
	m_Paramanager.LinkPara(strName[AXIS_1], _T("POSITION_5"), &m_dAxisPosition[AXIS_1][4], _T("0."));

	m_Paramanager.LinkPara(strName[AXIS_2], _T("POSITION_1"), &m_dAxisPosition[AXIS_2][0], _T("0."));
	m_Paramanager.LinkPara(strName[AXIS_2], _T("POSITION_2"), &m_dAxisPosition[AXIS_2][1], _T("0."));
	m_Paramanager.LinkPara(strName[AXIS_2], _T("POSITION_3"), &m_dAxisPosition[AXIS_2][2], _T("0."));
	m_Paramanager.LinkPara(strName[AXIS_2], _T("POSITION_4"), &m_dAxisPosition[AXIS_2][3], _T("0."));
	m_Paramanager.LinkPara(strName[AXIS_2], _T("POSITION_5"), &m_dAxisPosition[AXIS_2][4], _T("0."));

}

void CDialogMotion::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	DWORD dwRslt;
	CString strKMsg;
	int i, axis_cnt = 0;

	switch ((int)nIDEvent) {
	case TIMER_IO_MONITORING:
		IOMonitoring();
		DispCurPos();
		DispAxisState();
		break;
	case TIMER_ORIGIN_DLG:
		if (IsDlgButtonChecked(IDC_CHECK_ALL_HOME))
			axis_cnt = MAX_AXIS_NUM;
		else
			axis_cnt = 1;

		for (i = 0; i < axis_cnt; i++)
		{
			if (1 == axis_cnt)
			{
				i = m_nRadioAxisName;
			}

			if (!AxisIsHomeFinished(i))
			{
				dwRslt = MotReadOriginResult(i);

				if (dwRslt == HOME_SUCCESS)
				{
					AxisSetOriginReturn(i, TRUE);
					AxisSetPosition(i, 0.0);

					if (m_CBtnAxisName[i].GetBackColorInterior() != GREEN)
						m_CBtnAxisName[i].SetBackColorInterior(GREEN);

					if (1 == axis_cnt)
					{
						KillTimer(TIMER_ORIGIN_DLG);

						m_nProcessMoveIdx = 0;
						m_ctrl_progress.SetProgressBarValue(m_nProcessMoveIdx);
						m_ctrl_progress.ShowWindow(FALSE);

						strKMsg.Format(_T("%s�� ���� �Ϸ�"), AxisGetAxisName(i));
						g_cMsgBox.OkMsgBox(strKMsg);
					}
					break;
				}
				else if (dwRslt == HOME_SEARCHING)
				{
					if (m_nProcessMoveIdx >= 100)
						m_nProcessMoveIdx = 0;

					m_nProcessMoveIdx++;
					m_ctrl_progress.SetProgressBarValue(m_nProcessMoveIdx);
				}
				else if (dwRslt == HOME_ERR_NOT_DETECT)
				{
					KillTimer(TIMER_ORIGIN_DLG);

					m_nProcessMoveIdx = 0;
					m_ctrl_progress.SetProgressBarValue(m_nProcessMoveIdx);
					m_ctrl_progress.ShowWindow(FALSE);

					AxisSetOriginReturn(i, FALSE);

					strKMsg.Format(_T("%s�� HOME_ERR_NOT_DETECT"), AxisGetAxisName(i));
					g_cMsgBox.OkMsgBox(strKMsg);
					break;
				}
				else if (dwRslt == HOME_ERR_NEG_LIMIT)
				{
					KillTimer(TIMER_ORIGIN_DLG);

					m_nProcessMoveIdx = 0;
					m_ctrl_progress.SetProgressBarValue(m_nProcessMoveIdx);
					m_ctrl_progress.ShowWindow(FALSE);

					AxisSetOriginReturn(i, FALSE);

					strKMsg.Format(_T("%s�� HOME_ERR_NEG_LIMIT"), AxisGetAxisName(i));
					g_cMsgBox.OkMsgBox(strKMsg);
					break;
				}
				else if (dwRslt == HOME_ERR_POS_LIMIT)
				{
					KillTimer(TIMER_ORIGIN_DLG);

					m_nProcessMoveIdx = 0;
					m_ctrl_progress.SetProgressBarValue(m_nProcessMoveIdx);
					m_ctrl_progress.ShowWindow(FALSE);

					AxisSetOriginReturn(i, FALSE);

					strKMsg.Format(_T("%s�� HOME_ERR_POS_LIMIT"), AxisGetAxisName(i));
					g_cMsgBox.OkMsgBox(strKMsg);
					break;
				}
				else if (dwRslt == HOME_ERR_AMP_FAULT)
				{
					KillTimer(TIMER_ORIGIN_DLG);

					m_nProcessMoveIdx = 0;
					m_ctrl_progress.SetProgressBarValue(m_nProcessMoveIdx);
					m_ctrl_progress.ShowWindow(FALSE);

					AxisSetOriginReturn(i, FALSE);

					strKMsg.Format(_T("%s�� HOME_ERR_AMP_FAULT"), AxisGetAxisName(i));
					g_cMsgBox.OkMsgBox(strKMsg);
					break;
				}
				else if (dwRslt == HOME_ERR_TIMEOUT)
				{
					KillTimer(TIMER_ORIGIN_DLG);

					m_nProcessMoveIdx = 0;
					m_ctrl_progress.SetProgressBarValue(m_nProcessMoveIdx);
					m_ctrl_progress.ShowWindow(FALSE);

					AxisSetOriginReturn(i, FALSE);

					strKMsg.Format(_T("%s�� HOME_ERR_TIMEOUT"), AxisGetAxisName(i));
					g_cMsgBox.OkMsgBox(strKMsg);
					break;
				}
				else if (dwRslt == HOME_ERR_USER_BREAK)
				{
					KillTimer(TIMER_ORIGIN_DLG);

					m_nProcessMoveIdx = 0;
					m_ctrl_progress.SetProgressBarValue(m_nProcessMoveIdx);
					m_ctrl_progress.ShowWindow(FALSE);

					AxisSetOriginReturn(i, FALSE);

					strKMsg.Format(_T("%s�� HOME_ERR_USER_BREAK"), AxisGetAxisName(i));
					g_cMsgBox.OkMsgBox(strKMsg);
					break;
				}
				else if (dwRslt == HOME_ERR_UNKNOWN)
				{
					KillTimer(TIMER_ORIGIN_DLG);

					m_nProcessMoveIdx = 0;
					m_ctrl_progress.SetProgressBarValue(m_nProcessMoveIdx);
					m_ctrl_progress.ShowWindow(FALSE);

					AxisSetOriginReturn(i, FALSE);

					strKMsg.Format(_T("%s�� HOME_ERR_UNKNOWN"), AxisGetAxisName(i));
					g_cMsgBox.OkMsgBox(strKMsg);
					break;
				}
			}
		}

		if (AxisIsHomeFinished(AXIS_0) && AxisIsHomeFinished(AXIS_1) && AxisIsHomeFinished(AXIS_2))
		{
			KillTimer(TIMER_ORIGIN_DLG);

			m_nProcessMoveIdx = 0;
			m_ctrl_progress.SetProgressBarValue(m_nProcessMoveIdx);
			m_ctrl_progress.ShowWindow(FALSE);

			strKMsg.Format(_T("��� �� ���� �Ϸ�"));
			g_cMsgBox.OkMsgBox(strKMsg);
		}

		break;
	default:

		break;
	}

	CDialogEx::OnTimer(nIDEvent);
}

BEGIN_EVENTSINK_MAP(CDialogMotion, CDialogEx)
	ON_EVENT(CDialogMotion, IDC_BTNENHCTRL_BUTTON_JOG_TYPE1, DISPID_CLICK, CDialogMotion::ClickBtnenhctrlButtonJogType1, VTS_NONE)
	ON_EVENT(CDialogMotion, IDC_BTNENHCTRL_BUTTON_JOG_TYPE2, DISPID_CLICK, CDialogMotion::ClickBtnenhctrlButtonJogType2, VTS_NONE)

	ON_EVENT(CDialogMotion, IDC_BTNENHCTRL_BUTTON_JOG1, DISPID_CLICK, CDialogMotion::AxisMove, VTS_NONE)
	ON_EVENT(CDialogMotion, IDC_BTNENHCTRL_BUTTON_JOG2, DISPID_CLICK, CDialogMotion::AxisMove, VTS_NONE)
	ON_EVENT(CDialogMotion, IDC_BTNENHCTRL_BUTTON_JOG3, DISPID_CLICK, CDialogMotion::AxisMove, VTS_NONE)
	ON_EVENT(CDialogMotion, IDC_BTNENHCTRL_BUTTON_JOG4, DISPID_CLICK, CDialogMotion::AxisMove, VTS_NONE)
	ON_EVENT(CDialogMotion, IDC_BTNENHCTRL_BUTTON_JOG5, DISPID_CLICK, CDialogMotion::AxisMove, VTS_NONE)
	ON_EVENT(CDialogMotion, IDC_BTNENHCTRL_BUTTON_JOG6, DISPID_CLICK, CDialogMotion::AxisMove, VTS_NONE)

	ON_EVENT(CDialogMotion, IDC_BTNENHCTRL_BUTTON_OUT_000, DISPID_CLICK, CDialogMotion::ClickOutput, VTS_NONE)
	ON_EVENT(CDialogMotion, IDC_BTNENHCTRL_BUTTON_OUT_001, DISPID_CLICK, CDialogMotion::ClickOutput, VTS_NONE)
	ON_EVENT(CDialogMotion, IDC_BTNENHCTRL_BUTTON_OUT_002, DISPID_CLICK, CDialogMotion::ClickOutput, VTS_NONE)
	ON_EVENT(CDialogMotion, IDC_BTNENHCTRL_BUTTON_OUT_003, DISPID_CLICK, CDialogMotion::ClickOutput, VTS_NONE)
	ON_EVENT(CDialogMotion, IDC_BTNENHCTRL_BUTTON_OUT_004, DISPID_CLICK, CDialogMotion::ClickOutput, VTS_NONE)
	ON_EVENT(CDialogMotion, IDC_BTNENHCTRL_BUTTON_OUT_005, DISPID_CLICK, CDialogMotion::ClickOutput, VTS_NONE)
	ON_EVENT(CDialogMotion, IDC_BTNENHCTRL_BUTTON_OUT_006, DISPID_CLICK, CDialogMotion::ClickOutput, VTS_NONE)
	ON_EVENT(CDialogMotion, IDC_BTNENHCTRL_BUTTON_OUT_007, DISPID_CLICK, CDialogMotion::ClickOutput, VTS_NONE)
	ON_EVENT(CDialogMotion, IDC_BTNENHCTRL_BUTTON_OUT_008, DISPID_CLICK, CDialogMotion::ClickOutput, VTS_NONE)
	ON_EVENT(CDialogMotion, IDC_BTNENHCTRL_BUTTON_OUT_009, DISPID_CLICK, CDialogMotion::ClickOutput, VTS_NONE)

	ON_EVENT(CDialogMotion, IDC_BTNENHCTRL_LABEL_AXIS8, DISPID_CLICK, CDialogMotion::ClickBtnenhctrlLabelAxis8, VTS_NONE)
	ON_EVENT(CDialogMotion, IDC_BTNENHCTRL_LABEL_AXIS9, DISPID_CLICK, CDialogMotion::ClickBtnenhctrlLabelAxis9, VTS_NONE)
	ON_EVENT(CDialogMotion, IDC_BTNENHCTRL_LABEL_AXIS10, DISPID_CLICK, CDialogMotion::ClickBtnenhctrlLabelAxis10, VTS_NONE)
	ON_EVENT(CDialogMotion, IDC_BTNENHCTRL_LABEL_AXIS_NAME4, DISPID_CLICK, CDialogMotion::ClickBtnenhctrlLabelAxisName4, VTS_NONE)
	ON_EVENT(CDialogMotion, IDC_BTNENHCTRL_LABEL_AXIS_NAME5, DISPID_CLICK, CDialogMotion::ClickBtnenhctrlLabelAxisName5, VTS_NONE)
	ON_EVENT(CDialogMotion, IDC_BTNENHCTRL_LABEL_AXIS_NAME6, DISPID_CLICK, CDialogMotion::ClickBtnenhctrlLabelAxisName6, VTS_NONE)
	ON_EVENT(CDialogMotion, IDC_BTNENHCTRL_LABEL_AXIS_NAME7, DISPID_CLICK, CDialogMotion::ClickBtnenhctrlLabelAxisName7, VTS_NONE)
	ON_EVENT(CDialogMotion, IDC_BTNENHCTRL_BUTTON_MOVEPOS, DISPID_CLICK, CDialogMotion::ClickBtnenhctrlButtonMovepos, VTS_NONE)
	ON_EVENT(CDialogMotion, IDC_BTNENHCTRL_BUTTON_SETPOS, DISPID_CLICK, CDialogMotion::ClickBtnenhctrlButtonSetpos, VTS_NONE)
	ON_EVENT(CDialogMotion, IDC_BTNENHCTRL_BUTTON_APPLY, DISPID_CLICK, CDialogMotion::ClickBtnenhctrlButtonApply, VTS_NONE)
	ON_EVENT(CDialogMotion, IDC_BTNENHCTRL_BUTTON_OK, DISPID_CLICK, CDialogMotion::ClickBtnenhctrlButtonOk, VTS_NONE)
	ON_EVENT(CDialogMotion, IDC_BTNENHCTRL_BUTTON_HOME, DISPID_CLICK, CDialogMotion::ClickBtnenhctrlButtonHome, VTS_NONE)
	ON_EVENT(CDialogMotion, IDC_BTNENHCTRL_LABEL_AXIS_NAME1, DISPID_CLICK, CDialogMotion::ClickBtnenhctrlLabelAxisName1, VTS_NONE)
	ON_EVENT(CDialogMotion, IDC_BTNENHCTRL_LABEL_AXIS_NAME2, DISPID_CLICK, CDialogMotion::ClickBtnenhctrlLabelAxisName2, VTS_NONE)
	ON_EVENT(CDialogMotion, IDC_BTNENHCTRL_LABEL_AXIS_NAME3, DISPID_CLICK, CDialogMotion::ClickBtnenhctrlLabelAxisName3, VTS_NONE)

	ON_EVENT(CDialogMotion, IDC_BTNENHCTRL_LABEL_AXIS1_POS1, DISPID_DBLCLICK, CDialogMotion::DblClickPosition, VTS_NONE)
	ON_EVENT(CDialogMotion, IDC_BTNENHCTRL_LABEL_AXIS1_POS2, DISPID_DBLCLICK, CDialogMotion::DblClickPosition, VTS_NONE)
	ON_EVENT(CDialogMotion, IDC_BTNENHCTRL_LABEL_AXIS1_POS3, DISPID_DBLCLICK, CDialogMotion::DblClickPosition, VTS_NONE)
	ON_EVENT(CDialogMotion, IDC_BTNENHCTRL_LABEL_AXIS1_POS4, DISPID_DBLCLICK, CDialogMotion::DblClickPosition, VTS_NONE)
	ON_EVENT(CDialogMotion, IDC_BTNENHCTRL_LABEL_AXIS1_POS5, DISPID_DBLCLICK, CDialogMotion::DblClickPosition, VTS_NONE)
	ON_EVENT(CDialogMotion, IDC_BTNENHCTRL_LABEL_AXIS2_POS1, DISPID_DBLCLICK, CDialogMotion::DblClickPosition, VTS_NONE)
	ON_EVENT(CDialogMotion, IDC_BTNENHCTRL_LABEL_AXIS2_POS2, DISPID_DBLCLICK, CDialogMotion::DblClickPosition, VTS_NONE)
	ON_EVENT(CDialogMotion, IDC_BTNENHCTRL_LABEL_AXIS2_POS3, DISPID_DBLCLICK, CDialogMotion::DblClickPosition, VTS_NONE)
	ON_EVENT(CDialogMotion, IDC_BTNENHCTRL_LABEL_AXIS2_POS4, DISPID_DBLCLICK, CDialogMotion::DblClickPosition, VTS_NONE)
	ON_EVENT(CDialogMotion, IDC_BTNENHCTRL_LABEL_AXIS2_POS5, DISPID_DBLCLICK, CDialogMotion::DblClickPosition, VTS_NONE)
	ON_EVENT(CDialogMotion, IDC_BTNENHCTRL_LABEL_AXIS3_POS1, DISPID_DBLCLICK, CDialogMotion::DblClickPosition, VTS_NONE)
	ON_EVENT(CDialogMotion, IDC_BTNENHCTRL_LABEL_AXIS3_POS2, DISPID_DBLCLICK, CDialogMotion::DblClickPosition, VTS_NONE)
	ON_EVENT(CDialogMotion, IDC_BTNENHCTRL_LABEL_AXIS3_POS3, DISPID_DBLCLICK, CDialogMotion::DblClickPosition, VTS_NONE)
	ON_EVENT(CDialogMotion, IDC_BTNENHCTRL_LABEL_AXIS3_POS4, DISPID_DBLCLICK, CDialogMotion::DblClickPosition, VTS_NONE)
	ON_EVENT(CDialogMotion, IDC_BTNENHCTRL_LABEL_AXIS3_POS5, DISPID_DBLCLICK, CDialogMotion::DblClickPosition, VTS_NONE)
	ON_EVENT(CDialogMotion, IDC_BTNENHCTRL_BUTTON_AMPON, DISPID_CLICK, CDialogMotion::ClickBtnenhctrlButtonAmpon, VTS_NONE)
	ON_EVENT(CDialogMotion, IDC_BTNENHCTRL_BUTTON_AMPOFF, DISPID_CLICK, CDialogMotion::ClickBtnenhctrlButtonAmpoff, VTS_NONE)
	ON_EVENT(CDialogMotion, IDC_BTNENHCTRL_BUTTON_EMO, DISPID_CLICK, CDialogMotion::ClickBtnenhctrlButtonEmo, VTS_NONE)
	ON_EVENT(CDialogMotion, IDC_BTNENHCTRL_BUTTON_MOT_PARA, DISPID_CLICK, CDialogMotion::ClickBtnenhctrlButtonMotPara, VTS_NONE)
	ON_EVENT(CDialogMotion, IDC_BTNENHCTRL_BUTTON_MOVEHOME, DISPID_CLICK, CDialogMotion::ClickBtnenhctrlButtonMovehome, VTS_NONE)
	ON_EVENT(CDialogMotion, IDC_BTNENHCTRL_LABEL_AXIS_NAME8, DISPID_CLICK, CDialogMotion::ClickBtnenhctrlLabelAxisName8, VTS_NONE)
	ON_EVENT(CDialogMotion, IDC_BTNENHCTRL_LABEL_AXIS_AMP1, DISPID_CLICK, CDialogMotion::ClickBtnenhctrlLabelAxisAmp1, VTS_NONE)
	ON_EVENT(CDialogMotion, IDC_BTNENHCTRL_LABEL_AXIS_AMP2, DISPID_CLICK, CDialogMotion::ClickBtnenhctrlLabelAxisAmp2, VTS_NONE)
	ON_EVENT(CDialogMotion, IDC_BTNENHCTRL_LABEL_AXIS_AMP3, DISPID_CLICK, CDialogMotion::ClickBtnenhctrlLabelAxisAmp3, VTS_NONE)
END_EVENTSINK_MAP()

void CDialogMotion::UpdatePara(BOOL bShow)
{
	CString strTemp;

	if (bShow) {
		for (int i = 0; i < MAX_AXIS_NUM; i++) {
			for (int j = 0; j < MAX_POSITION_NUM; j++) {
				strTemp.Format(_T("%6.3f"), m_dAxisPosition[i][j]);
				m_CBtnAxisPosition[i][j].SetCaption(strTemp);
			}
		}
	}
	else {
		for (int i = 0; i < MAX_AXIS_NUM; i++) {
			for (int j = 0; j < MAX_POSITION_NUM; j++) {
				strTemp = m_CBtnAxisPosition[i][j].GetCaption();
				m_dAxisPosition[i][j] = _ttof(strTemp);
			}
		}
	}
}

void CDialogMotion::IOMonitoring()
{
	//INPUT
	//START BUTTON
	if (m_bOldInput[0] != m_pMotion->ReadInput(AXIS_0, UNIVERSAL_INPUT_X02)) {

		m_bOldInput[0] = m_pMotion->ReadInput(AXIS_0, UNIVERSAL_INPUT_X02);

		if (m_bOldInput[0])
			m_CBtnInput[0].SetBackColor(RGB(0, 235, 0));
		else
			m_CBtnInput[0].SetBackColor(RGB(200, 200, 200));
	}

	//STOP BUTTON
	if (m_bOldInput[1] != m_pMotion->ReadInput(AXIS_0, UNIVERSAL_INPUT_X03)) {

		m_bOldInput[1] = m_pMotion->ReadInput(AXIS_0, UNIVERSAL_INPUT_X03);

		if (m_bOldInput[1])
			m_CBtnInput[1].SetBackColor(RGB(0, 235, 0));
		else
			m_CBtnInput[1].SetBackColor(RGB(200, 200, 200));
	}

	//VAC ON BUTTON
	if (m_bOldInput[2] != m_pMotion->ReadInput(AXIS_1, UNIVERSAL_INPUT_X02)) {

		m_bOldInput[2] = m_pMotion->ReadInput(AXIS_1, UNIVERSAL_INPUT_X02);

		if (m_bOldInput[2])
			m_CBtnInput[2].SetBackColor(RGB(0, 235, 0));
		else
			m_CBtnInput[2].SetBackColor(RGB(200, 200, 200));
	}

	//VAC OFF BUTTON
	if (m_bOldInput[3] != m_pMotion->ReadInput(AXIS_1, UNIVERSAL_INPUT_X03)) {

		m_bOldInput[3] = m_pMotion->ReadInput(AXIS_1, UNIVERSAL_INPUT_X03);

		if (m_bOldInput[3])
			m_CBtnInput[3].SetBackColor(RGB(0, 235, 0));
		else
			m_CBtnInput[3].SetBackColor(RGB(200, 200, 200));
	}

	//EMO BUTTON
	if (m_bOldInput[4] != m_pMotion->ReadInput(AXIS_0, UNIVERSAL_INPUT_X04)) {

		m_bOldInput[4] = m_pMotion->ReadInput(AXIS_0, UNIVERSAL_INPUT_X04);

		if (m_bOldInput[4])
			m_CBtnInput[4].SetBackColor(RGB(0, 235, 0));
		else
			m_CBtnInput[4].SetBackColor(RGB(200, 200, 200));
	}

	//Pressure Signal
	if (m_bOldInput[5] != m_pMotion->ReadInput(AXIS_1, UNIVERSAL_INPUT_X04)) {

		m_bOldInput[5] = m_pMotion->ReadInput(AXIS_1, UNIVERSAL_INPUT_X04);

		if (m_bOldInput[5])
			m_CBtnInput[5].SetBackColor(RGB(0, 235, 0));
		else
			m_CBtnInput[5].SetBackColor(RGB(200, 200, 200));
	}

	//OUTPUT
	//VAC ON
	if (m_bOldOutput[0] != m_pMotion->ReadOutput(AXIS_0, UNIVERSAL_OUTPUT_Y02)) {

		m_bOldOutput[0] = m_pMotion->ReadOutput(AXIS_0, UNIVERSAL_OUTPUT_Y02);

		if (m_bOldOutput[0])
			m_CBtnOutput[0].SetValue(TRUE);
		else
			m_CBtnOutput[0].SetValue(FALSE);
	}

	//VAC Destroy
	if (m_bOldOutput[1] != m_pMotion->ReadOutput(AXIS_0, UNIVERSAL_OUTPUT_Y03)) {

		m_bOldOutput[1] = m_pMotion->ReadOutput(AXIS_0, UNIVERSAL_OUTPUT_Y03);

		if (m_bOldOutput[1])
			m_CBtnOutput[1].SetValue(TRUE);
		else
			m_CBtnOutput[1].SetValue(FALSE);
	}

	//VAC #1
	if (m_bOldOutput[2] != m_pMotion->ReadOutput(AXIS_1, UNIVERSAL_OUTPUT_Y02)) {

		m_bOldOutput[2] = m_pMotion->ReadOutput(AXIS_1, UNIVERSAL_OUTPUT_Y02);

		if (m_bOldOutput[2])
			m_CBtnOutput[2].SetValue(TRUE);
		else
			m_CBtnOutput[2].SetValue(FALSE);
	}

	//VAC #2
	if (m_bOldOutput[3] != m_pMotion->ReadOutput(AXIS_1, UNIVERSAL_OUTPUT_Y03)) {

		m_bOldOutput[3] = m_pMotion->ReadOutput(AXIS_1, UNIVERSAL_OUTPUT_Y03);

		if (m_bOldOutput[3])
			m_CBtnOutput[3].SetValue(TRUE);
		else
			m_CBtnOutput[3].SetValue(FALSE);
	}
}

void CDialogMotion::ClickOutput()
{
	int nDialogID;

	CBtnenhctrl *pBtnEnh = (CBtnenhctrl*)GetFocus();
	nDialogID = pBtnEnh->GetDlgCtrlID();

	int a = (DWORD)pBtnEnh->GetValue();

	switch (nDialogID)
	{
	case IDC_BTNENHCTRL_BUTTON_OUT_000:
		m_pMotion->WriteOutput(AXIS_0, UNIVERSAL_OUTPUT_Y02, (DWORD)pBtnEnh->GetValue());
		break;
	case IDC_BTNENHCTRL_BUTTON_OUT_001:
		m_pMotion->WriteOutput(AXIS_0, UNIVERSAL_OUTPUT_Y03, (DWORD)pBtnEnh->GetValue());
		break;
	case IDC_BTNENHCTRL_BUTTON_OUT_002:
		m_pMotion->WriteOutput(AXIS_1, UNIVERSAL_OUTPUT_Y02, (DWORD)pBtnEnh->GetValue());
		break;
	case IDC_BTNENHCTRL_BUTTON_OUT_003:
		m_pMotion->WriteOutput(AXIS_1, UNIVERSAL_OUTPUT_Y03, (DWORD)pBtnEnh->GetValue());
		break;
	case IDC_BTNENHCTRL_BUTTON_OUT_004:
		break;
	case IDC_BTNENHCTRL_BUTTON_OUT_005:
		break;
	case IDC_BTNENHCTRL_BUTTON_OUT_006:
		break;
	case IDC_BTNENHCTRL_BUTTON_OUT_007:
		break;
	case IDC_BTNENHCTRL_BUTTON_OUT_008:
		break;
	case IDC_BTNENHCTRL_BUTTON_OUT_009:
		break;
	}
}


/*
void CDialogAxisJog::ClickAxisJogLeft()
{
// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
if(m_iOpMode != STEP_MODE)
return;

if(m_paxDlgX != NULL)
{
if(!m_paxDlgX->isOriginReturn())
{
//g_cMsgView.OkMsgBox("�ش� ���� ���� ���� �Ǿ� ���� �ʽ��ϴ�.","Axis Not Origin !!","���޷���λ !!");
g_cMsgBox.OkMsgBox(_T("�ش� ���� ���� ���� �Ǿ� ���� �ʽ��ϴ�."));
return;
}
//>> 130331 JSPark
double dCurPos, dTragetPos;
double dNegSWLimit = m_paxDlgX->GetNegSWLimit();
double dRPos = -1.0*m_dStepPitch;
m_paxDlgX->GetPosition(dCurPos);
dTragetPos = dCurPos+dRPos;
if(dTragetPos < dNegSWLimit)
{
//>>20130902 lee
CString strKMsg;
strKMsg.Format(_T("X���� �����̰��� �ϴ� ��ǥ ��ġ:%6.3f�� -SW Limit ���� ����:%6.3f�� ������ϴ�.")
,dTragetPos, dNegSWLimit);

g_cMsgBox.OkMsgBox(strKMsg);
//<<
return;
}
m_paxDlgX->RMove(dRPos, true);
m_bTeachPosXFlag = true;
//m_paxSelectAxis = m_paxDlgX;
//<<
}
*/

void CDialogMotion::AxisMove()
{
	UpdateData(TRUE);

	if (m_nRadioMoveType != STEP_MODE)
		return;

	int nDialogID;
	double dRPos;

	CBtnenhctrl *pBtnEnh = (CBtnenhctrl*)GetFocus();
	nDialogID = pBtnEnh->GetDlgCtrlID();

	switch (nDialogID)
	{
	case IDC_BTNENHCTRL_BUTTON_JOG1:		//X +
		if (!GetMotAxis(AXIS_0)->isOriginReturn()) {
			g_cMsgBox.OkMsgBox(_T("Failed. Run Origin !"));
			return;	
		}

		dRPos = 1.0 * m_edit_nStepVal;
		//GetMotAxis(AXIS_0)->GetPosition(dCurPos);
		//dTragetPos = dCurPos + dRPos;

		AxisRMove(AXIS_0, dRPos, true);
		
		break;
	case IDC_BTNENHCTRL_BUTTON_JOG2:		//X -
		if (!GetMotAxis(AXIS_0)->isOriginReturn()) {
			g_cMsgBox.OkMsgBox(_T("Failed. Run Origin !"));
			return;
		}

		dRPos = -1.0 * m_edit_nStepVal;
		//GetMotAxis(AXIS_0)->GetPosition(dCurPos);
		//dTragetPos = dCurPos + dRPos;

		AxisRMove(AXIS_0, dRPos, true);
		break;
	case IDC_BTNENHCTRL_BUTTON_JOG3:		//Y +
		if (!GetMotAxis(AXIS_1)->isOriginReturn()) {
			g_cMsgBox.OkMsgBox(_T("Failed. Run Origin !"));
			return;
		}

		dRPos = 1.0 * m_edit_nStepVal;
		//GetMotAxis(AXIS_1)->GetPosition(dCurPos);
		//dTragetPos = dCurPos + dRPos;

		AxisRMove(AXIS_1, dRPos, true);
		break;
	case IDC_BTNENHCTRL_BUTTON_JOG4:		//Y - 
		if (!GetMotAxis(AXIS_1)->isOriginReturn()) {
			g_cMsgBox.OkMsgBox(_T("Failed. Run Origin !"));
			return;
		}

		dRPos = -1.0 * m_edit_nStepVal;
		//GetMotAxis(AXIS_1)->GetPosition(dCurPos);
		//dTragetPos = dCurPos + dRPos;

		AxisRMove(AXIS_1, dRPos, true);
		break;
	case IDC_BTNENHCTRL_BUTTON_JOG5:		//T +
		if (!GetMotAxis(AXIS_2)->isOriginReturn()) {
			g_cMsgBox.OkMsgBox(_T("Failed. Run Origin !"));
			return;
		}

		dRPos = 1.0 * m_edit_nStepVal;
		//GetMotAxis(AXIS_2)->GetPosition(dCurPos);
		//dTragetPos = dCurPos + dRPos;

		AxisRMove(AXIS_2, dRPos, true);
		break;
	case IDC_BTNENHCTRL_BUTTON_JOG6:		//T -
		if (!GetMotAxis(AXIS_2)->isOriginReturn()) {
			g_cMsgBox.OkMsgBox(_T("Failed. Run Origin !"));
			return;
		}

		dRPos = -1.0 * m_edit_nStepVal;
		//GetMotAxis(AXIS_2)->GetPosition(dCurPos);
		//dTragetPos = dCurPos + dRPos;

		AxisRMove(AXIS_2, dRPos, true);
		break;
	}
}

void CDialogMotion::DblClickPosition()
{
	int nDialogID;

	CBtnenhctrl *pBtnEnh = (CBtnenhctrl*)GetFocus();
	nDialogID = pBtnEnh->GetDlgCtrlID();

	CDialogNumber_Box dlg;

	if (dlg.DoModal() == IDOK) {
		m_CBtnAxisPosition[m_nRadioAxis][m_nRadioPos].SetCaption(dlg.GetstrNum());

		UpdatePara(FALSE);
	}
	else {
		return;
	}
}

void CDialogMotion::ClickBtnenhctrlButtonMovepos()
{
	if (m_nRadioPos == 3)
	{
		AxisMove(AXIS_0, m_dAxisPosition[AXIS_0][m_nRadioPos], 1.0, FALSE);
		AxisMove(AXIS_1, m_dAxisPosition[AXIS_1][m_nRadioPos], 1.0, FALSE);
		AxisMove(AXIS_2, m_dAxisPosition[AXIS_2][m_nRadioPos], 1.0, FALSE);
	}
	else
	{
		//AxisMove(AXIS_0, m_dAxisPosition[AXIS_0][m_nRadioPos], MOTION_VEL_MAGNIFICATION, FALSE);
		AxisMove(AXIS_0, m_dAxisPosition[AXIS_0][m_nRadioPos], 10, FALSE);
		AxisMove(AXIS_1, m_dAxisPosition[AXIS_1][m_nRadioPos], MOTION_VEL_MAGNIFICATION, FALSE);
		AxisMove(AXIS_2, m_dAxisPosition[AXIS_2][m_nRadioPos], MOTION_VEL_MAGNIFICATION, FALSE);
	}
}

void CDialogMotion::ClickBtnenhctrlButtonSetpos()
{
	m_CBtnAxisPosition[AXIS_0][m_nRadioPos].SetCaption(m_CBtnAxisPulse[AXIS_0].GetCaption());
	m_CBtnAxisPosition[AXIS_1][m_nRadioPos].SetCaption(m_CBtnAxisPulse[AXIS_1].GetCaption());
	m_CBtnAxisPosition[AXIS_2][m_nRadioPos].SetCaption(m_CBtnAxisPulse[AXIS_2].GetCaption());

	UpdatePara(FALSE);
}

void CDialogMotion::DispCurPos()
{
	CString strPos = _T("");
	double dCurPos = 0.0;
	int j = 1;

	for (int i = 0; i < MAX_AXIS_NUM; i++)
	{
		AxisGetPosition(i, dCurPos);
		if(i == AXIS_0)
			strPos.Format(_T("%.4f"), dCurPos);
		else
			strPos.Format(_T("%.3f"), dCurPos);

		if(strPos.Compare(m_CBtnAxisPulse[i].GetCaption()))
			m_CBtnAxisPulse[i].SetCaption(strPos);
	}

	for (int i = 0; i < MAX_AXIS_NUM; i++)
	{
		for (int j = 0; j < MAX_POSITION_NUM; j++)
		{
			if (i == AXIS_0)
				strPos.Format(_T("%.4f"), m_dAxisPosition[i][j]);
			else
				strPos.Format(_T("%.3f"), m_dAxisPosition[i][j]);

			if(strPos.Compare(m_CBtnAxisPosition[i][j].GetCaption()))
				m_CBtnAxisPosition[i][j].SetCaption(strPos);
		}
	}
}

void CDialogMotion::DispAxisState()
{
	int j = 1;
	long level;

	for (int i = 0; i < MAX_AXIS_NUM; i++)
	{
		level = 0;

		//POS LIMIT
		m_pMotion->AxisGetPosLimitLevel(i, level);

		if (m_pMotion->AxisGetPosLimitSensor(i))
		{
			if (m_CBtnAxisSensorP[i].GetBackColor() != RED)
				m_CBtnAxisSensorP[i].SetBackColor(RED);
		}
		else
		{
			if (m_CBtnAxisSensorP[i].GetBackColor() != (LTYELLOW))
				m_CBtnAxisSensorP[i].SetBackColor(LTYELLOW);
		}

		//NEG LIMIT
		m_pMotion->AxisGetNegLimitLevel(i, level);

		if (m_pMotion->AxisGetNegLimitSensor(i))
		{
			if (m_CBtnAxisSensorN[i].GetBackColor() != (RED))
				m_CBtnAxisSensorN[i].SetBackColor(RED);
		}
		else
		{
			if (m_CBtnAxisSensorN[i].GetBackColor() != (LTYELLOW))
				m_CBtnAxisSensorN[i].SetBackColor(LTYELLOW);
		}

		//AMP
		if(AxisGetAmpOnState(i))// || m_pMotion->AxisGetAmpFaultState(i))
		{
			if (m_CBtnAxisAmp[i].GetBackColor() != GREEN)
			{
				m_CBtnAxisAmp[i].SetCaption(_T("Normal"));
				m_CBtnAxisAmp[i].SetBackColor(GREEN);
				m_CBtnAxisAmp[i].SetForeColor(BLACK);

				m_bAmpOnFlag[i] = TRUE;
			}
		}
		else
		{
			if (m_CBtnAxisAmp[i].GetBackColor() != LTYELLOW)
			{
				m_CBtnAxisAmp[i].SetCaption(_T("Fault"));
				m_CBtnAxisAmp[i].SetBackColor(LTYELLOW);
				m_CBtnAxisAmp[i].SetForeColor(RED);

				m_bAmpOnFlag[i] = FALSE;
			}
		}

		/*
		if (!m_pMotion->AxisGetAmpFaultState(i))
		{
			if (m_CBtnAxisAmp[i].GetBackColor() != LTYELLOW)
			{
				m_CBtnAxisAmp[i].SetCaption(_T("Fault"));
				m_CBtnAxisAmp[i].SetBackColor(LTYELLOW);
				m_CBtnAxisAmp[i].SetForeColor(RED);

				m_bAmpOnFlag[i] = FALSE;
			}
		}
		*/

		if (m_bAmpOnFlag[i])
		{
			if (m_CBtnAmpON.GetBackColor() != YELLOW)
			{
				m_CBtnAmpON.SetBackColor(YELLOW);
				m_CBtnAmpOff.SetBackColor(0x00FFFFFF);
			}
		}
		else
		{
			if (m_CBtnAmpOff.GetBackColor() != YELLOW)
			{
				m_CBtnAmpON.SetBackColor(0x00FFFFFF);
				m_CBtnAmpOff.SetBackColor(YELLOW);
			}
		}

		// Origin Check -> Green
		if (!AxisIsHomeFinished(i))
		{
			if (m_CBtnAxisName[i].GetBackColor() != RED)
				m_CBtnAxisName[i].SetBackColor(RED);
		}
		// ETC
		else
		{
			if (m_CBtnAxisName[i].GetBackColor() != GREEN)
				m_CBtnAxisName[i].SetBackColor(GREEN);
		}
	}
}

void CDialogMotion::ClickBtnenhctrlButtonApply()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	m_Paramanager.SavePara();
}


void CDialogMotion::ClickBtnenhctrlButtonOk()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	//EndDialog(0);
	ShowWindow(SW_HIDE);
}


void CDialogMotion::ClickBtnenhctrlButtonHome()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.

	//m_CBtnAxisName
	//m_nRadioAxisName
	CString strTemp;
	CString strMsg;
	int i;

	if(!m_bAmpOnFlag[m_nRadioAxisName]) {
		strTemp.Format(_T("AMP OFF ������ ���� �ֽ��ϴ�."), m_nRadioAxisName);
		AfxMessageBox(strTemp);	return;
	}

	if (IsDlgButtonChecked(IDC_CHECK_ALL_HOME))
	{
		//��� �� ����
		strTemp.Format(_T("ALL Axis, Start to Origin?"));

		//���� ����
		if (AfxMessageBox(strTemp, MB_YESNO) == IDNO)
			return;

		for (i = 0; i < MAX_AXIS_NUM; i++)
		{
			if (!AxisGetAmpFaultState(i))
			{
				strMsg.Format(_T("The %s-axis is amp fault"), AxisGetAxisName(i));
				AfxMessageBox(strMsg);
				return;
			}
		}

		for (i = 0; i < MAX_AXIS_NUM; i++)
		{
			MotSetOriginMethod(i);
			MotStartOrigin(i);
		}

		strTemp.Format(_T("Now homming ALL Axis. Please wait.."));
	}
	else
	{
		//���� ����
		strTemp.Format(_T("%s Axis, Start to Origin?"), AxisGetAxisName(m_nRadioAxisName));

		//���� ����
		if (AfxMessageBox(strTemp, MB_YESNO) == IDNO)
			return;


		if (!AxisGetAmpFaultState(m_nRadioAxisName))
		{
			strMsg.Format(_T("The %s-axis is amp fault"), AxisGetAxisName(m_nRadioAxisName));
			AfxMessageBox(strMsg);
			return;
		}

		MotSetOriginMethod(m_nRadioAxisName);
		MotStartOrigin(m_nRadioAxisName);

		strTemp.Format(_T("Now homming %s Axis. Please wait.."), AxisGetAxisName(m_nRadioAxisName));
	}

	//���α׷��� ����
	m_ctrl_progress.SetCaption(strTemp);
	m_ctrl_progress.ShowWindow(TRUE);

	SetTimer(TIMER_ORIGIN_DLG, 300, NULL);
}



void CDialogMotion::ClickBtnenhctrlButtonAmpon()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	
	if (MotAmpEnableAll()) {
		if (MotInitialize()) {
			for(int i = 0; i < MAX_AXIS_NUM; i++)
				m_bAmpOnFlag[i] = TRUE;
		}
	}
}


void CDialogMotion::ClickBtnenhctrlButtonAmpoff()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	CString strTemp;

	strTemp.Format(_T("Off the %d Axis Amp"), m_nRadioAxisName + 1);

	if(AfxMessageBox(strTemp, MB_YESNO) == IDNO)
		return;
	/*
	AxisAmpDisable(m_nRadioAxisName);
	AxisSetHomeFinished(m_nRadioAxisName, FALSE);
	AxisSetOriginReturn(m_nRadioAxisName, FALSE);

	m_CBtnAxisAmp[m_nRadioAxisName].SetCaption(_T("Fault"));
	m_CBtnAxisAmp[m_nRadioAxisName].SetBackColor(LTYELLOW);
	m_CBtnAxisAmp[m_nRadioAxisName].SetForeColor(RED);
	*/
	
	MotAmpDisableAll();
	
	for (int i = 0; i < MAX_AXIS_NUM; i++){
		AxisSetHomeFinished(i, FALSE);
		AxisSetOriginReturn(i, FALSE);
		AxisAmpDisable(i);
	
		m_CBtnAxisAmp[i].SetCaption(_T("Fault"));
		m_CBtnAxisAmp[i].SetBackColor(LTYELLOW);
		m_CBtnAxisAmp[i].SetForeColor(RED);

		m_bAmpOnFlag[i] = FALSE;
	}
}

void CDialogMotion::ClickBtnenhctrlLabelAxisName1()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	m_nRadioAxisName = 0;
}


void CDialogMotion::ClickBtnenhctrlLabelAxisName2()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	m_nRadioAxisName = 1;
}

void CDialogMotion::ClickBtnenhctrlLabelAxisName3()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	m_nRadioAxisName = 2;
}


void CDialogMotion::ClickBtnenhctrlLabelAxis8()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	m_nRadioAxis = 0;
}


void CDialogMotion::ClickBtnenhctrlLabelAxis9()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	m_nRadioAxis = 1;
}


void CDialogMotion::ClickBtnenhctrlLabelAxis10()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	m_nRadioAxis = 2;
}


void CDialogMotion::ClickBtnenhctrlLabelAxisName4()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	m_nRadioPos = 0;
}


void CDialogMotion::ClickBtnenhctrlLabelAxisName5()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	m_nRadioPos = 1;
}


void CDialogMotion::ClickBtnenhctrlLabelAxisName6()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	m_nRadioPos = 2;
}


void CDialogMotion::ClickBtnenhctrlLabelAxisName7()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	m_nRadioPos = 3;
}

void CDialogMotion::ClickBtnenhctrlLabelAxisName8()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	m_nRadioPos = 4;
}


void CDialogMotion::ClickBtnenhctrlButtonJogType1()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	m_nRadioMoveType = 0;
}


void CDialogMotion::ClickBtnenhctrlButtonJogType2()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	UpdateData(TRUE);

	m_nRadioMoveType = 1;
	//m_edit_nStepVal;
}

BOOL CDialogMotion::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	if (pMsg->wParam != 0)
	{
		if (pMsg->wParam == VK_F4)
			return TRUE;

		if (pMsg->wParam == VK_ESCAPE)
			return TRUE;

		if (pMsg->wParam == VK_RETURN)
			return TRUE;
	}

	UpdateData(TRUE);

	switch (pMsg->message)
	{
	case WM_LBUTTONDOWN:
		if (pMsg->hwnd == GetDlgItem(IDC_BTNENHCTRL_BUTTON_JOG1)->GetSafeHwnd())
		{
			if (!GetMotAxis(AXIS_0)->isOriginReturn()) {
				g_cMsgBox.OkMsgBox(_T("Failed. Run Origin !"));
				return TRUE;
			}

			if (m_nRadioMoveType == JOG_MODE)
				AxisVMove(AXIS_0, GetMotAxis(AXIS_0)->GetJogVel() * m_dJogSpeed, GetMotAxis(AXIS_0)->GetJogAcc(), GetMotAxis(AXIS_0)->GetJogDec());
		}
		else if (pMsg->hwnd == GetDlgItem(IDC_BTNENHCTRL_BUTTON_JOG2)->GetSafeHwnd())	
		{
			if (!GetMotAxis(AXIS_0)->isOriginReturn()) {
				g_cMsgBox.OkMsgBox(_T("Failed. Run Origin !"));
				return TRUE;
			}

			if (m_nRadioMoveType == JOG_MODE)
				AxisVMove(AXIS_0, -1.0 * GetMotAxis(AXIS_0)->GetJogVel() * m_dJogSpeed, GetMotAxis(AXIS_0)->GetJogAcc(), GetMotAxis(AXIS_0)->GetJogDec());
		}
		else if (pMsg->hwnd == GetDlgItem(IDC_BTNENHCTRL_BUTTON_JOG3)->GetSafeHwnd())
		{
			if (!GetMotAxis(AXIS_1)->isOriginReturn()) {
				g_cMsgBox.OkMsgBox(_T("Failed. Run Origin !"));
				return TRUE;
			}

			if (m_nRadioMoveType == JOG_MODE)
				AxisVMove(AXIS_1, GetMotAxis(AXIS_1)->GetJogVel() * m_dJogSpeed, GetMotAxis(AXIS_1)->GetJogAcc(), GetMotAxis(AXIS_1)->GetJogDec());
		}
		else if (pMsg->hwnd == GetDlgItem(IDC_BTNENHCTRL_BUTTON_JOG4)->GetSafeHwnd())
		{
			
			if (!GetMotAxis(AXIS_1)->isOriginReturn()) {
				g_cMsgBox.OkMsgBox(_T("Failed. Run Origin !"));
				return TRUE;
			}

			if (m_nRadioMoveType == JOG_MODE)
				AxisVMove(AXIS_1 , -1.0 * GetMotAxis(AXIS_1)->GetJogVel() * m_dJogSpeed, GetMotAxis(AXIS_1)->GetJogAcc(), GetMotAxis(AXIS_1)->GetJogDec());
		}
		else if (pMsg->hwnd == GetDlgItem(IDC_BTNENHCTRL_BUTTON_JOG5)->GetSafeHwnd())
		{
			if (!GetMotAxis(AXIS_2)->isOriginReturn())
			{
				g_cMsgBox.OkMsgBox(_T("Failed. Run Origin !"));
				return TRUE;
			}

			if (m_nRadioMoveType == JOG_MODE)
				AxisVMove(AXIS_2, GetMotAxis(AXIS_2)->GetJogVel() * m_dJogSpeed, GetMotAxis(AXIS_2)->GetJogAcc(), GetMotAxis(AXIS_2)->GetJogDec());
		}
		else if (pMsg->hwnd == GetDlgItem(IDC_BTNENHCTRL_BUTTON_JOG6)->GetSafeHwnd())
		{
			
			if (!GetMotAxis(AXIS_2)->isOriginReturn())
			{
				g_cMsgBox.OkMsgBox(_T("Failed. Run Origin !"));
				return TRUE;
			}

			if (m_nRadioMoveType == JOG_MODE)
				AxisVMove(AXIS_2 , -1.0 * GetMotAxis(AXIS_2)->GetJogVel() * m_dJogSpeed, GetMotAxis(AXIS_2)->GetJogAcc(), GetMotAxis(AXIS_2)->GetJogDec());
		}
		break;

	case WM_LBUTTONUP:
		if (pMsg->hwnd == GetDlgItem(IDC_BTNENHCTRL_BUTTON_JOG1)->GetSafeHwnd() || pMsg->hwnd == GetDlgItem(IDC_BTNENHCTRL_BUTTON_JOG2)->GetSafeHwnd())
		{
			if (m_nRadioMoveType == JOG_MODE)
				GetMotAxis(AXIS_0)->Stop();
			else
				Delay(10, TRUE);

		}
		else if (pMsg->hwnd == GetDlgItem(IDC_BTNENHCTRL_BUTTON_JOG3)->GetSafeHwnd() || pMsg->hwnd == GetDlgItem(IDC_BTNENHCTRL_BUTTON_JOG4)->GetSafeHwnd())
		{
			if (m_nRadioMoveType == JOG_MODE)
				GetMotAxis(AXIS_1)->Stop();
			else
				Delay(10, TRUE);
		}
		else if (pMsg->hwnd == GetDlgItem(IDC_BTNENHCTRL_BUTTON_JOG5)->GetSafeHwnd() || pMsg->hwnd == GetDlgItem(IDC_BTNENHCTRL_BUTTON_JOG6)->GetSafeHwnd())
		{
			if (m_nRadioMoveType == JOG_MODE)
			{
				GetMotAxis(AXIS_2)->Stop();
			}
			else
				Delay(10, TRUE);
		}

		break;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CDialogMotion::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
					   // �׸��� �޽����� ���ؼ��� CDialogEx::OnPaint()��(��) ȣ������ ���ʽÿ�.
	if (IsIconic())
	{

	}
	else
	{
		CRect rect;
		CPen pen, *oldPen;
		CBrush brush, *oldBrush;

		GetClientRect(&rect);

		//����� ä���
		pen.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
		oldPen = dc.SelectObject(&pen);
		brush.CreateSolidBrush(RGB(0, 0, 0));
		oldBrush = dc.SelectObject(&brush);

		dc.Rectangle(&rect);

		dc.SelectObject(oldPen);
		pen.DeleteObject();
		dc.SelectObject(oldBrush);
		brush.DeleteObject();

		CDialog::OnPaint();
	}
}

void CDialogMotion::ClickBtnenhctrlButtonEmo()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	AxisStop(AXIS_0);
	AxisStop(AXIS_1);
	AxisStop(AXIS_2);

	if (m_ctrl_progress.IsWindowVisible()) {
		m_nProcessMoveIdx = 0;
		m_ctrl_progress.SetProgressBarValue(m_nProcessMoveIdx);
		m_ctrl_progress.ShowWindow(FALSE);
	}
}

void CDialogMotion::ClickBtnenhctrlButtonMotPara()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	m_pDlgAxisManager->ShowWindow(SW_SHOW);
}


void CDialogMotion::OnCbnSelchangeComboJogSpeed()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	switch (m_combo_jog_speed.GetCurSel())
	{
	case 0:
		m_dJogSpeed = 0.1;
		break;
	case 1:
		m_dJogSpeed = 1;
		break;
	case 2:
		m_dJogSpeed = 2;
		break;
	}
}


void CDialogMotion::ClickBtnenhctrlButtonMovehome()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	AxisMove(AXIS_0, 0, MOTION_VEL_MAGNIFICATION, FALSE);
	AxisMove(AXIS_1, 0, MOTION_VEL_MAGNIFICATION, FALSE);
	AxisMove(AXIS_2, 0, MOTION_VEL_MAGNIFICATION, FALSE);
}

void CDialogMotion::ClickBtnenhctrlLabelAxisAmp1()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	if(!m_bAmpOnFlag[AXIS_0])
		if (AxisAmpEnable(AXIS_0))
			m_bAmpOnFlag[AXIS_0] = TRUE;
}


void CDialogMotion::ClickBtnenhctrlLabelAxisAmp2()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	if (!m_bAmpOnFlag[AXIS_1])
		if (AxisAmpEnable(AXIS_1))
			m_bAmpOnFlag[AXIS_1] = TRUE;
}


void CDialogMotion::ClickBtnenhctrlLabelAxisAmp3()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	if (!m_bAmpOnFlag[AXIS_2])
		if (AxisAmpEnable(AXIS_2))
			m_bAmpOnFlag[AXIS_2] = TRUE;
}
