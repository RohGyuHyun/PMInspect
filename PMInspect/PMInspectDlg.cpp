
// PMInspectDlg.cpp : ���� ����
//

#include "stdafx.h"
#include "PMInspect.h"
#include "PMInspectDlg.h"
#include "afxdialogex.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DWORD _TimerCounter;

//Time
UINT _TimerID;
TIMECAPS _Caps;
BOOL _TimerPost;
void CALLBACK TimerReadFunc(UINT wID, UINT wUser, DWORD dwUser, DWORD dw1, DWORD dw2);

static void CALLBACK TimerReadFunc(UINT wID, UINT wUser, DWORD dwUser, DWORD dw1, DWORD dw2)
{
	if (_TimerPost == FALSE)
	{
		_TimerCounter++;
	}
	else
	{
		AfxMessageBox(_T("Error : high resolution timer event loss!"));
	}
}

BOOL testbool = FALSE;


//////////////////////////////////////////////////////////////////////
//Motion Data
//////////////////////////////////////////////////////////////////////
CMotionData::CMotionData()
{
	memset(&m_ProSts, NULL, sizeof(typeMotionProcessSts) * MOT_STATUS_MAX_NUM * MOT_STATUS_SUB_MAX_NUM);
	memset(&m_Seq, NULL, sizeof(typeMotionSeq) * MOT_STATUS_MAX_NUM);

}

CMotionData::~CMotionData()
{

}

void CMotionData::ResetSeq()
{
	memset(&m_Seq, NULL, sizeof(typeMotionSeq) * MOT_STATUS_MAX_NUM);

}


// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

// �����Դϴ�.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CPMInspectDlg ��ȭ ����



CPMInspectDlg::CPMInspectDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_PMINSPECT_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_pDispAlign		= NULL;
	m_pDlgDispMain		= NULL;
	m_pDlgTeach			= NULL;
	m_pDlgModel			= NULL;
	m_pDlgSystem		= NULL;
	m_pDlgMotion		= NULL;
	m_pDlgAlarm			= NULL;
	m_pDlgResult		= NULL;

	memset(&m_Flag, NULL, sizeof(typeMotionFlag));
	m_Flag.m_bProcessThread = TRUE;

	m_nCountTotal = 0;
	m_nCountGood = 0;
	m_nCountNg = 0;

	m_bFlagInspect = FALSE;
}

CPMInspectDlg::~CPMInspectDlg()
{
	m_Flag.m_bProcessThread = FALSE;

	if (m_pDlgAlarm != NULL) {
		delete m_pDlgAlarm;
		m_pDlgAlarm = NULL;
	}

	if (m_pDlgTeach != NULL) {
		delete m_pDlgTeach;
		m_pDlgTeach = NULL;
	}

	if (m_pDlgModel != NULL) {
		delete m_pDlgModel;
		m_pDlgModel = NULL;
	}

	if (m_pDlgSystem != NULL) {
		delete m_pDlgSystem;
		m_pDlgSystem = NULL;
	}

	if (m_pDlgMotion != NULL) {
		delete m_pDlgMotion;
		m_pDlgMotion = NULL;
	}

	if (m_pDispAlign != NULL) {
		delete m_pDispAlign;
		m_pDispAlign = NULL;
	}

	if (m_pDlgDispMain != NULL)	{
		delete m_pDlgDispMain;
		m_pDlgDispMain = NULL;
	}

	if (m_pDlgResult != NULL) {
		delete	m_pDlgResult;
		m_pDlgResult = NULL;
	}
}

void CPMInspectDlg::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_BTNENHCTRL_TAP0, m_btnTap[0]);
	DDX_Control(pDX, IDC_BTNENHCTRL_TAP1, m_btnTap[1]);
	DDX_Control(pDX, IDC_BTNENHCTRL_TAP2, m_btnTap[2]);
	DDX_Control(pDX, IDC_BTNENHCTRL_TAP3, m_btnTap[3]);
	DDX_Control(pDX, IDC_BTNENHCTRL_TAP4, m_btnTap[4]);
	DDX_Control(pDX, IDC_BTNENHCTRL_TAP5, m_btnTap[5]);

	DDX_Control(pDX, IDC_BTNENHCTRL_LABEL_DATE, m_btnDate);
	DDX_Control(pDX, IDC_BTNENHCTRL_BUTTON_START, m_btnStart);
	DDX_Control(pDX, IDC_BTNENHCTRL_BUTTON_STOP, m_btnStop);
	DDX_Control(pDX, IDC_BTNENHCTRL_BUTTON_RESET, m_btnReset);

	DDX_Control(pDX, IDC_BTNENHCTRL_LABEL_MODEL_NAME, m_btnModelName);
	DDX_Control(pDX, IDC_BTNENHCTRL_LABEL_OKNG, m_btnJudgement);

	DDX_Control(pDX, IDC_BTNENHCTRL_EDIT_TOAL, m_btnCountTotal);
	DDX_Control(pDX, IDC_BTNENHCTRL_EDIT_GOOD, m_btnCountGood);
	DDX_Control(pDX, IDC_BTNENHCTRL_EDIT_NG, m_btnCountNg);
	DDX_Control(pDX, IDC_BTNENHCTRL_LABEL_STATUS, m_btnOpStatus);
	DDX_Control(pDX, IDC_BTNENHCTRL_LABEL_VAC_STATUS, m_btnVacStatus);
	
	DDX_Control(pDX, IDC_LIST_INSPECT, m_ctrl_listInspect);
	DDX_Control(pDX, IDC_LIST_SYSTEM, m_ctrl_listSystem);
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CPMInspectDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_MESSAGE(USER_MSG_MOT, OnMotMessage)
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CPMInspectDlg, CDialogEx)
	ON_EVENT(CPMInspectDlg, IDC_BTNENHCTRL_TAP0, DISPID_CLICK, CPMInspectDlg::ShowDialog, VTS_NONE)
	ON_EVENT(CPMInspectDlg, IDC_BTNENHCTRL_TAP1, DISPID_CLICK, CPMInspectDlg::ShowDialog, VTS_NONE)
	ON_EVENT(CPMInspectDlg, IDC_BTNENHCTRL_TAP2, DISPID_CLICK, CPMInspectDlg::ShowDialog, VTS_NONE)
	ON_EVENT(CPMInspectDlg, IDC_BTNENHCTRL_TAP3, DISPID_CLICK, CPMInspectDlg::ShowDialog, VTS_NONE)
	ON_EVENT(CPMInspectDlg, IDC_BTNENHCTRL_TAP4, DISPID_CLICK, CPMInspectDlg::ShowDialog, VTS_NONE)
	ON_EVENT(CPMInspectDlg, IDC_BTNENHCTRL_TAP5, DISPID_CLICK, CPMInspectDlg::ShowDialog, VTS_NONE)
	ON_EVENT(CPMInspectDlg, IDC_BTNENHCTRL_BUTTON_START, DISPID_CLICK, OnClickAutoRun, VTS_NONE)
	ON_EVENT(CPMInspectDlg, IDC_BTNENHCTRL_BUTTON_STOP, DISPID_CLICK, OnClickAutoRun, VTS_NONE)
	ON_EVENT(CPMInspectDlg, IDC_BTNENHCTRL_BUTTON_RESET, DISPID_CLICK, CPMInspectDlg::ClickBtnenhctrlButtonReset, VTS_NONE)
	ON_EVENT(CPMInspectDlg, IDC_BTNENHCTRL_COUNT_RESET, DISPID_CLICK, CPMInspectDlg::ClickBtnenhctrlCountReset, VTS_NONE)
END_EVENTSINK_MAP()

// CPMInspectDlg �޽��� ó����

BOOL CPMInspectDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// �ý��� �޴��� "����..." �޴� �׸��� �߰��մϴ�.

	// IDM_ABOUTBOX�� �ý��� ��� ������ �־�� �մϴ�.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// �� ��ȭ ������ �������� �����մϴ�.  ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	CString strTemp;

	InitDialog();

	//MAIN SEQUENCE THREAD
#ifdef ENABLE_MOTION
	m_pThread = AfxBeginThread(MotionThread, this, THREAD_PRIORITY_NORMAL);
	if (m_pThread) m_pThread->m_bAutoDelete = TRUE;
#endif

	_TimerPost = FALSE;
	timeGetDevCaps(&_Caps, sizeof(_Caps));
	_Caps.wPeriodMin = 1;
	timeBeginPeriod(_Caps.wPeriodMin);
	_TimerID = timeSetEvent(_Caps.wPeriodMin, _Caps.wPeriodMin, (LPTIMECALLBACK)&TimerReadFunc, 0, (UINT)TIME_PERIODIC);

	if (!_TimerID)
	{
		AfxMessageBox(_T("Error : high resolution timer set fail!"));
	}

	strTemp.Format(_T("%S"), m_pDlgModel->GetName());
	m_btnModelName.SetCaption(strTemp);
	m_btnStop.SetValue(TRUE);

	MakeDefaultDir();

	SetTimer(TIMER_DATE, 1000, NULL);
	//SetTimer(TIMER_COUNT, 500, NULL);

	LV_COLUMN m_lvColumn;

	m_lvColumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	m_lvColumn.fmt = LVCFMT_LEFT;
	m_lvColumn.pszText = _T("INSPECT LOG");
	m_lvColumn.iSubItem = 0;
	m_lvColumn.cx = 1000;
	m_ctrl_listInspect.InsertColumn(0, &m_lvColumn);

	m_lvColumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	m_lvColumn.fmt = LVCFMT_LEFT;
	m_lvColumn.pszText = _T("SYSTEM LOG");
	m_lvColumn.iSubItem = 0;
	m_lvColumn.cx = 1000;
	m_ctrl_listSystem.InsertColumn(0, &m_lvColumn);

	SetOpStatus(OP_STATUS_STOP);

	SystemListDisplay(TRUE, _T("Initialize complete!"));

	//Set Button
	//m_btnStart.SetEnabled(TRUE);
	//m_btnStart.SetValue(TRUE);
	//m_btnStop.SetEnabled(FALSE);
	//m_btnStop.SetValue(TRUE);
	//m_btnReset.SetEnabled(FALSE);
	//m_btnReset.SetValue(FALSE);

	//((CButton*)GetDlgItem(IDC_BTNENHCTRL_BUTTON_RESET))->EnableWindow(FALSE);

	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}

void CPMInspectDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�.  ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CPMInspectDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ�Դϴ�.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CRect rect;
		CPen pen, *oldPen;
		CBrush brush, *oldBrush;

		CPaintDC dc(this);

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

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
HCURSOR CPMInspectDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

int CPMInspectDlg::HandleAllMessage()
{
	int returnValue;
	MSG Mess;

	do {
		returnValue = ::PeekMessage(&Mess, NULL, 0, 0, PM_REMOVE);
		if (returnValue)
		{
			::TranslateMessage(&Mess);
			::DispatchMessage(&Mess);
		}
	} while (returnValue);

	return returnValue;
}

void CPMInspectDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	switch (nIDEvent)
	{
	case TIMER_DATE:	//TIME
	{
		SYSTEMTIME lpTime;
		CString strTemp;
		GetLocalTime(&lpTime);

		strTemp.Format(_T("%04d.%02d.%02d / %02d:%02d:%02d"), lpTime.wYear, lpTime.wMonth, lpTime.wDay, lpTime.wHour, lpTime.wMinute, lpTime.wSecond);
		m_btnDate.SetCaption(strTemp);
		
		//Update Count
		SetCount();

		//Update Vac Status
		if (m_pDlgMotion->ReadInput(AXIS_1, UNIVERSAL_INPUT_X04))
		{
			if(GREEN != m_btnVacStatus.GetBackColor())
				m_btnVacStatus.SetBackColor(GREEN);
		}
		else
		{
			if (RED != m_btnVacStatus.GetBackColor())
				m_btnVacStatus.SetBackColor(RED);
		}

		break;
	}
	//case TIMER_COUNT:
		
	//	break;
	default:
		KillTimer(nIDEvent);
		AfxMessageBox(_T("Error : OnTimer Invalid Index!"));
		break;
	}

	CDialogEx::OnTimer(nIDEvent);
}

BOOL CPMInspectDlg::InitDialog()
{
	BOOL rslt = TRUE;
	CRect sys_rect, rect;

	SystemParametersInfo(SPI_GETWORKAREA, NULL, &sys_rect, NULL);
	MoveWindow(0, 0, sys_rect.Width(), sys_rect.Height(), TRUE);

	//Child Dialog
	m_pDlgModel = new CDialogModel;
	m_pDlgModel->Create(IDD_DIALOG_MODEL, this);
	GetDlgItem(IDC_STATIC_DLG)->GetWindowRect(&rect);
	m_pDlgModel->MoveWindow(rect.left, rect.top, rect.Width(), rect.Height(), FALSE);
	m_pDlgModel->ShowWindow(SW_HIDE);

	m_pDlgTeach = new CDialogTeach;
	m_pDlgTeach->Create(IDD_DIALOG_TEACH, this);
	GetDlgItem(IDC_STATIC_DLG)->GetWindowRect(&rect);
	m_pDlgTeach->MoveWindow(rect.left, rect.top, rect.Width(), rect.Height(), FALSE);
	m_pDlgTeach->ShowWindow(SW_HIDE);

	m_pDlgSystem = new CDialogSystem;
	m_pDlgSystem->Create(IDD_DIALOG_SYSTEM, this);
	GetDlgItem(IDC_STATIC_DLG)->GetWindowRect(&rect);
	m_pDlgSystem->MoveWindow(rect.left, rect.top, rect.Width(), rect.Height(), FALSE);
	m_pDlgSystem->ShowWindow(SW_HIDE);

	m_pDlgMotion = new CDialogMotion;
	m_pDlgMotion->Create(IDD_DIALOG_MOTION, this);
	GetDlgItem(IDC_STATIC_DLG)->GetWindowRect(&rect);
	m_pDlgMotion->MoveWindow(rect.left, rect.top, rect.Width(), rect.Height(), FALSE);
	m_pDlgMotion->ShowWindow(SW_HIDE);

	//Align Disp#1
	m_pDispAlign = new	IMGALIGN;
	GetDlgItem(IDC_STATIC_DISP_ALIGN)->GetWindowRect(&sys_rect);
	GetDlgItem(IDC_STATIC_DISP_ALIGN2)->GetWindowRect(&rect);
	m_pDispAlign->Create((CWnd*)this, &sys_rect, &rect, CA2CT(m_pDlgModel->GetPath()));
	m_pDispAlign->LoadAlignData((CA2CT)m_pDlgModel->GetPath());
//	m_pDispAlign->LoadAlignData(_T("D:\\DATABASE"));	//	IMSI

	//Main Disp
	m_pDlgDispMain = new CDialogDispMain;
	m_pDlgDispMain->Create(IDD_DIALOG_DISP_MAIN, this);
	GetDlgItem(IDC_STATIC_DISP_MAIN)->GetWindowRect(&rect);
	m_pDlgDispMain->LoadModelData(m_pDlgModel->GetPath());
	m_pDlgDispMain->MoveWindow(rect.left, rect.top, rect.Width(), rect.Height(), FALSE);
	m_pDlgDispMain->ShowWindow(SW_SHOW);

	m_pDlgAlarm = new CDialogAlarm;
	m_pDlgAlarm->Create(IDD_DIALOG_MSG_ALARM, this);
	//GetDlgItem(IDC_STATIC_DLG)->GetWindowRect(&rect);
	//m_pDlgModel->MoveWindow(rect.left, rect.top, rect.Width(), rect.Height(), FALSE);
	m_pDlgAlarm->ShowWindow(SW_HIDE);

	m_pDlgResult = new	CDialogResultView;
	m_pDlgResult->Create(IDD_DIALOG_RESULT_VIEW, this);
	m_pDlgResult->ChangeModel(CA2CT(m_pDlgModel->GetPath()));
	
	m_pDlgResult->ShowWindow(SW_HIDE);
	//m_pDlgResult->ShowWindow(SW_SHOW);		IMSI

	m_nTapID = MAIN_DIALOG;
	return rslt;
}

void CPMInspectDlg::Display()
{
	SendMessage(USER_MSG_IMG_RECT, 0, 0);
}

BOOL CPMInspectDlg::ShowDialog()
{
	if (m_Flag.m_bAutoStart) {
		g_cMsgBox.OkMsgBox(_T("AUTO START MODE, Can't switch menu!"));
		return TRUE;
	}

	HTuple wd, ht;
	BOOL rslt = TRUE;
	int nDialogID;
	CBtnenhctrl *pBtnEnh = (CBtnenhctrl*)GetFocus();
	nDialogID = pBtnEnh->GetDlgCtrlID();

	CButton *btn;

	m_nTapID = nDialogID;

	switch (nDialogID)
	{
	case MAIN_DIALOG:
		if (m_pDlgModel->IsWindowVisible())
			m_pDlgModel->ShowWindow(SW_HIDE);
		if (m_pDlgTeach->IsWindowVisible())
			m_pDlgTeach->ShowWindow(SW_HIDE);
		if (m_pDlgSystem->IsWindowVisible())
			m_pDlgSystem->ShowWindow(SW_HIDE);
		if (m_pDlgMotion->IsWindowVisible())
			m_pDlgMotion->ShowWindow(SW_HIDE);

		btn = (CButton*)GetDlgItem(IDC_BTNENHCTRL_TAP1);
		if(!btn->GetCheck()) btn->SetCheck(FALSE);
		btn = (CButton*)GetDlgItem(IDC_BTNENHCTRL_TAP2);
		if (!btn->GetCheck()) btn->SetCheck(FALSE);
		btn = (CButton*)GetDlgItem(IDC_BTNENHCTRL_TAP3);
		if (!btn->GetCheck()) btn->SetCheck(FALSE);
		btn = (CButton*)GetDlgItem(IDC_BTNENHCTRL_TAP4);

		if (!btn->GetCheck()) btn->SetCheck(FALSE);
		break;
	case SETUP_DIALOG:
		if (m_pDlgModel->IsWindowVisible())
			m_pDlgModel->ShowWindow(SW_HIDE);

		if (m_pDlgSystem->IsWindowVisible())
			m_pDlgSystem->ShowWindow(SW_HIDE);

		//if (m_pDlgMotion->IsWindowVisible())
		//	m_pDlgMotion->ShowWindow(SW_HIDE);

		m_pDlgTeach->ShowWindow(SW_SHOW);

		m_btnTap[0].SetValue(FALSE);
		SetFirstStart(FALSE);
		break;
	case SYSTEM_DIALOG:
		if (m_pDlgModel->IsWindowVisible())
			m_pDlgModel->ShowWindow(SW_HIDE);

		if (m_pDlgTeach->IsWindowVisible())
			m_pDlgTeach->ShowWindow(SW_HIDE);

		if (m_pDlgMotion->IsWindowVisible())
			m_pDlgMotion->ShowWindow(SW_HIDE);

		m_pDlgSystem->ShowWindow(SW_SHOW);

		m_btnTap[0].SetValue(FALSE);
		SetFirstStart(FALSE);
		break;
	case MODEL_DIALOG:
		if(m_pDlgTeach->IsWindowVisible())
			m_pDlgTeach->ShowWindow(SW_HIDE);

		if (m_pDlgSystem->IsWindowVisible())
			m_pDlgSystem->ShowWindow(SW_HIDE);

		if (m_pDlgMotion->IsWindowVisible())
			m_pDlgMotion->ShowWindow(SW_HIDE);

		m_pDlgModel->ShowWindow(SW_SHOW);

		m_btnTap[0].SetValue(FALSE);
		SetFirstStart(FALSE);
		break;
	case MOTION_DIALOG:
		//if (m_pDlgTeach->IsWindowVisible())
		//	m_pDlgTeach->ShowWindow(SW_HIDE);

		if (m_pDlgModel->IsWindowVisible())
			m_pDlgModel->ShowWindow(SW_HIDE);

		if (m_pDlgSystem->IsWindowVisible())
			m_pDlgSystem->ShowWindow(SW_HIDE);

		m_pDlgMotion->ShowWindow(SW_SHOW);

		m_btnTap[0].SetValue(FALSE);
		SetFirstStart(FALSE);
		break;
	case EXIT_DIALOG:
		if (IDYES == AfxMessageBox(_T("EXIT The Program ?"), MB_YESNO)) {
			m_pDlgMotion->WriteOutput(AXIS_0, UNIVERSAL_OUTPUT_Y02, FALSE);
			m_pDlgMotion->WriteOutput(AXIS_0, UNIVERSAL_OUTPUT_Y03, TRUE);
			Delay(100, TRUE);
			m_pDlgMotion->WriteOutput(AXIS_0, UNIVERSAL_OUTPUT_Y03, FALSE);

			WriteLog(_T("Exit the Program!"), SYSTEM_LOG);

			EndDialog(0);
		}
		break;
	default:
		AfxMessageBox(_T("Error : ShowDialog Invalid Index!"));
		break;
	}

	if (nDialogID != EXIT_DIALOG)
	{
		Display();
	}

	return rslt;
}

////////////////////////////////////////////////////////////////////////////////////
//Main Sequence
LRESULT CPMInspectDlg::OnMotMessage(WPARAM para0, LPARAM para1)
{
	// TODO: Add your control notification handler code here
	int i, j;
	CString strTemp, strTemp2;

	switch (para0)
	{
	//Alarm
	case 0:
		for (i = 0; i < MOT_PROCESS_MAX_NUM; i++)
		{
			for (j = 0; j < MOT_STATUS_MAX_NUM; j++)
			{
				if (m_MDat[i].m_Seq[j].m_bAlarm)
				{
					if (0 < m_MDat[i].m_Seq[j].m_bOpSts)
					{
						switch (m_MDat[i].m_Seq[j].m_bOpSts)
						{
						case 1:			// �˻�Ϸ�
							AutoReset();
							AutoStart();
							break;
						case 2:

							break;
						default:

							break;
						}
						break;
					}
					else
					{
						//EMO
						if (!strcmp(m_MDat[i].m_ProSts[j][m_MDat[i].m_Seq[j].m_iSubSeqIdx].m_cNGCode, ALARM_CODE_EMO)) {
							//��� ����
							m_pDlgMotion->AxisStop(AXIS_0);
							m_pDlgMotion->AxisStop(AXIS_1);
							m_pDlgMotion->AxisStop(AXIS_2);
							//Ȩ, amp ����
							m_pDlgMotion->MotAmpDisableAll();
						}
						//STOP
						else if (!strcmp(m_MDat[i].m_ProSts[j][m_MDat[i].m_Seq[j].m_iSubSeqIdx].m_cNGCode, ALARM_CODE_STOP))
						{
							AutoStop();
							m_btnStop.SetValue(TRUE);

						}
						//VACCUM
						else if (!strcmp(m_MDat[i].m_ProSts[j][m_MDat[i].m_Seq[j].m_iSubSeqIdx].m_cNGCode, ALARM_CODE_VACCUM))
						{
							AutoStop();
							m_btnStop.SetValue(TRUE);
						}
						else {
			
						}
						
						m_pDlgAlarm->SetMessage(m_MDat[i].m_ProSts[j][m_MDat[i].m_Seq[j].m_iSubSeqIdx].m_cNGCode,
												m_MDat[i].m_ProSts[j][m_MDat[i].m_Seq[j].m_iSubSeqIdx].m_cNGStatus);

						m_pDlgAlarm->ShowWindow(SW_SHOW);


						//ALARM LOG
						strTemp.Format(_T("%s"), CA2T(m_MDat[i].m_ProSts[j][m_MDat[i].m_Seq[j].m_iSubSeqIdx].m_cNGStatus));
						WriteLog(strTemp, SYSTEM_LOG);

						//m_pDlgManual->SetLampError();
						//pDlgManual->WriteOutput(RESET_SWITCH_LAMP, TRUE);
						break;
					}
				}
			}
		}
		break;
	default:
		AfxMessageBox(_T("Error : invalid index!(CEMIControlDlg::OnMotMessage)"));
		break;
	}

	return 0;
}

int CPMInspectDlg::AutoStart()
{
	int i, j;

	//First start
	if (!m_Flag.m_bFirstStart)
	{
		ClickBtnenhctrlButtonReset();
	}
	
	for (i = 0; i < MOT_PROCESS_MAX_NUM; i++)
	{
		for (j = 0; j < MOT_STATUS_MAX_NUM; j++)
		{
			m_Flag.m_bAutoStart = TRUE;
			m_MDat[i].m_Seq[j].m_bAutoStart = m_Flag.m_bAutoStart;
		}
	}

	SetOpStatus(OP_STATUS_AUTO);
	SystemListDisplay(TRUE, _T("Switch to Auto mode!"));

	((CButton*)GetDlgItem(IDC_BTNENHCTRL_BUTTON_RESET))->EnableWindow(FALSE);

	return 0;
}

void CPMInspectDlg::AutoStop()
{
	int i, j;

	for (i = 0; i < MOT_PROCESS_MAX_NUM; i++)
	{
		for (j = 0; j < MOT_STATUS_MAX_NUM; j++)
		{
			m_Flag.m_bAutoStart = FALSE;
			m_MDat[i].m_Seq[j].m_bAutoStart = m_Flag.m_bAutoStart;
		}
	}

	SetOpStatus(OP_STATUS_STOP);
	SystemListDisplay(TRUE, _T("Switch to Stop mode!"));

	((CButton*)GetDlgItem(IDC_BTNENHCTRL_BUTTON_RESET))->EnableWindow(TRUE);
}

int CPMInspectDlg::AutoReset()
{
	int i, j;
	int prc_idx, seq_idx, sub_seq_idx;
	prc_idx = 0;
	seq_idx = 0;
	sub_seq_idx = 0;

	for (i = 0; i < MOT_PROCESS_MAX_NUM; i++)
	{
		for (j = 0; j < MOT_STATUS_MAX_NUM; j++)
		{
			if (m_MDat[i].m_Seq[j].m_bAlarm)
			{
				prc_idx = i;
				seq_idx = j;
				sub_seq_idx = m_MDat[i].m_Seq[j].m_iSubSeqIdx;

			}
			m_MDat[i].m_Seq[j].m_bAlarm = FALSE;
		}
	}

	m_Flag.m_bAlarm = FALSE;

	switch (seq_idx)
	{
	case 0:

		break;
	case 1:

		break;
	case 2:
		switch (sub_seq_idx)
		{
		case 110:	//VAC ��ư �Է� ���� �̵�
			m_MDat[0].m_Seq[seq_idx].m_iSubSeqIdx = 100;	
			break;
		case 140:	//ALIGN #1 �׷� ����
		case 141:
			m_MDat[0].m_Seq[seq_idx].m_iSubSeqIdx = 140;		
			break;
		case 180:	//ALIGN #2 �׷� ����
		case 181:
			m_MDat[0].m_Seq[seq_idx].m_iSubSeqIdx = 180;
			break;
		case 263: //LINESCAN �׷� ����
			m_MDat[0].m_Seq[seq_idx].m_iSubSeqIdx = 100;
			break;
		case 480:	//�׷� ���� ����
			//m_MDat[0].m_Seq[seq_idx].m_iSubSeqIdx = 430;		//�׷� ���� ��ġ�� �̵�
			break;

		default:

			break;
		}
		break;
	}


	m_pDlgAlarm->ShowWindow(SW_HIDE);

	m_Flag.m_bAutoStart = FALSE;

	//m_pDlgManual->SetLampWait();
	//m_pDlgManual->WriteOutput(RESET_SWITCH_LAMP, FALSE);

	//Buzzer off
	//m_pDlgManual->WriteOutput(BUZZER_ERROR_SIGNAL, FALSE);

	return 0;
}

UINT CPMInspectDlg::MotionThread(LPVOID pParam)
{
	CPMInspectDlg *pdlg = (CPMInspectDlg *)pParam;

	pdlg->MotionThread();

	return 0;
}

void CPMInspectDlg::MotionThread()
{
	int rslt[MOT_PROCESS_MAX_NUM][MOT_STATUS_MAX_NUM];
	int i, j, prc_idx, seq_idx;

	BOOL chk = FALSE;

	while (m_Flag.m_bProcessThread)
	{
		//������ ��ƾ�� �� ������ m_bAutoStart�� üũ�Ѵ�.
		chk = TRUE;
#ifndef NOTE_BOOK
		for (i = 0; i < MOT_PROCESS_MAX_NUM; i++)
		{
			for (j = 0; j < MOT_STATUS_MAX_NUM; j++)
			{
				if (!m_MDat[i].m_Seq[j].m_bAutoStart)
				{
					chk = FALSE;
				}
			}
		}

		if (chk)		//Auto mode
		{
			//Proccessing
			for (i = 0; i < MOT_PROCESS_MAX_NUM; i++)
			{
				for (j = 0; j < MOT_STATUS_MAX_NUM; j++)
				{
					rslt[i][j] = MotionProcess(i, j);

					if (1 == rslt[i][j])
					{
						//Auto status
					}
					else if (0 == rslt[i][j])
					{
						//Error

						//����
						AutoStop();

						m_MDat[i].m_Seq[j].m_bAlarm = TRUE;
						m_Flag.m_bAlarm = TRUE; // Display Status ������

						//���� ������ ������ ���� ó���Ѵ�.
						::SendMessage(this->m_hWnd, USER_MSG_MOT, 0, 0);

						//���½� ������ ���ۿ���
						m_MDat[i].m_Seq[j].m_iSubSeqIdx = m_MDat[i].m_Seq[j].m_iNextSubSeqIdx;
						break;
					}
				}
			}
		}
		else
		{
			//Stop Mode
			if (1 == StopModeOperation(&prc_idx, &seq_idx))
			{

			}
			else
			{
				//����
				AutoStop();
				m_btnStop.SetValue(TRUE);

				//Error
				m_MDat[prc_idx].m_Seq[seq_idx].m_bAlarm = TRUE;
				m_Flag.m_bAlarm = TRUE; // Display Status ������

				//���� ������ ������ ���� ó���Ѵ�.
				::SendMessage(this->m_hWnd, USER_MSG_MOT, 0, 0);
			}

			Sleep(1);
		}
#endif
		Sleep(1);
	}

	AfxEndThread(0);
}

int CPMInspectDlg::MotionProcess(int prc_idx, int seq_idx)
{
	int rslt = 1;
	int sub_seq_idx;
	BOOL chk_timeout = TRUE;
	BOOL not_delay = TRUE;
	double dCurPos = 0.;
	CString SendPacket, strTemp;

	//	Align Test
//	static	ALIGNVIEW_PTNDIFF	AlignDiff[2];
	double						dblXTmp, dblYTmp, dblTTmp;
	static	double				dblXCorrection = 0.0, dblYCorrection = 0.0, dblTCorrection = 0.0;
	static	BOOL				IsAlignCheck = FALSE;
	static	double				TMoveCnt;
	SYSTEMTIME	st;

	char status_type[20];
	memset(status_type, NULL, sizeof(status_type));
	//CTime CurTime;
	//CurTime = CTime::GetCurrentTime();

	switch (seq_idx)
	{
	//Stop mode check
	case 0:
		chk_timeout = FALSE;
		break;
	//CASE 1: Safe and limit sensor(inter lock)
	case 1:
		sub_seq_idx = m_MDat[prc_idx].m_Seq[seq_idx].m_iSubSeqIdx;

		switch (sub_seq_idx)
		{
		case 100:	//OP EMS üũ
			m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_dTimeout = TIMEOUT_SAFE_SENSOR;//ms
			sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGStatus, "Detected EMO !(1:100)");
			sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGCode, ALARM_CODE_EMO);

			if (m_pDlgMotion->ReadInput(AXIS_0, UNIVERSAL_INPUT_X04))
				SetSeqIdx(prc_idx, seq_idx, 110);
			break;
		case 110:	//STOP ��ư üũ
			m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_dTimeout = TIMEOUT_SAFE_SENSOR;//ms
			sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGStatus, "Detected STOP !(1:110)");
			sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGCode, ALARM_CODE_STOP);

			if (!m_pDlgMotion->ReadInput(AXIS_0, UNIVERSAL_INPUT_X03))
				SetSeqIdx(prc_idx, seq_idx, 120);
			break;
		case 120:		//Main AIr Check
			if (m_bFlagInspect)			{
				m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_dTimeout = TIMEOUT_SAFE_SENSOR;//ms
				sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGStatus, "No products or Error VACCUM Sensor !(1:120)");
				sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGCode, ALARM_CODE_VACCUM);

				if (m_pDlgMotion->ReadInput(AXIS_1, UNIVERSAL_INPUT_X04))
					SetSeqIdx(prc_idx, seq_idx, 200);
			}
			else {
				SetSeqIdx(prc_idx, seq_idx, 200);
			}
			
			break;
		case 200:
			SetSeqIdx(prc_idx, seq_idx, 100);
			break;
		}
		break;
	//CASE 2: Main sequence
	case 2:
		sub_seq_idx = m_MDat[prc_idx].m_Seq[seq_idx].m_iSubSeqIdx;
		
		switch (sub_seq_idx)
		{
		case 100:	//�ʱ�ȭ ��ƾ
			SystemListDisplay(TRUE, _T("Enther a Vaccum button!"));
			m_bFlagInspect = FALSE;

			SetSeqIdx(prc_idx, seq_idx, 105);
			break;
		case 105:	//VACCUM ��ư �Է�
			m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_dTimeout = 0;// ���Ѵ��
			sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGStatus, "Time over VACCUM input !(2:105)");
			sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGCode, ALARM_CODE_READY_TIMEOUT);

			if (m_pDlgMotion->ReadInput(AXIS_1, UNIVERSAL_INPUT_X02) && !m_pDlgMotion->ReadInput(AXIS_1, UNIVERSAL_INPUT_X03))
			{
				m_pDlgMotion->WriteOutput(AXIS_0, UNIVERSAL_OUTPUT_Y03, FALSE);
				m_pDlgMotion->WriteOutput(AXIS_0, UNIVERSAL_OUTPUT_Y02, TRUE);

				SetSeqIdx(prc_idx, seq_idx, 110);
			}
			break;
		case 110:	//VACCUM ��ư �Է� Ȯ��
			m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_dTimeout = TIMEOUT_VAC_SENSOR;//ms
			sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGStatus, "No products or Error VACCUM Sensor !(2:110)");
			sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGCode, ALARM_CODE_SOL);

			if (m_pDlgMotion->ReadInput(AXIS_1, UNIVERSAL_INPUT_X04))
			{
				SystemListDisplay(TRUE, _T("Start Inspection! (VACCUM OK)"));
				
				m_bFlagInspect = TRUE;

				IsAlignCheck = FALSE;
				TMoveCnt = 0;
				dblXCorrection = dblYCorrection = dblTCorrection = 0.0;

				//20181114 ngh
				m_pDlgDispMain->SetATFHomming();

				if (m_pDlgResult->IsWindowVisible())
				{
					m_pDlgResult->ShowWindow(SW_HIDE);
				}

				SetSeqIdx(prc_idx, seq_idx, 120);
			}
			break;

		//ALIGN #1
		case 120://Align#1 ��ġ�� �̵�
			m_pDlgMotion->AxisMove(AXIS_0, m_pDlgMotion->GetAxisPos(AXIS_0, 0) + dblXCorrection, MOTION_VEL_MAGNIFICATION, FALSE);
			m_pDlgMotion->AxisMove(AXIS_1, m_pDlgMotion->GetAxisPos(AXIS_1, 0) + dblYCorrection, MOTION_VEL_MAGNIFICATION, FALSE);
			m_pDlgMotion->AxisMove(AXIS_2, m_pDlgMotion->GetAxisPos(AXIS_2, 0) + dblTCorrection, MOTION_VEL_MAGNIFICATION, FALSE);

			SetSeqIdx(prc_idx, seq_idx, 130);
			break;
		case 130://Align#1 ��ġ üũ
			m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_dTimeout = TIMEOUT_IN_MOTION;//ms
			sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGStatus, "Time over X-Axis Move !(2:130)");
			sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGCode, ALARM_CODE_INMOTION);

			if(m_pDlgMotion->AxisMoveDone(AXIS_0))
				SetSeqIdx(prc_idx, seq_idx, 131);
			break;
		case 131://Align#1 ��ġ üũ
			m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_dTimeout = TIMEOUT_IN_MOTION;//ms
			sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGStatus, "Time over Y-Axis Move !(2:131)");
			sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGCode, ALARM_CODE_INMOTION);

			if (m_pDlgMotion->AxisMoveDone(AXIS_1))
				SetSeqIdx(prc_idx, seq_idx, 132);
			break;
		case 132://Align#1 ��ġ üũ
			m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_dTimeout = TIMEOUT_IN_MOTION;//ms
			sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGStatus, "Time over T-Axis Move !(2:132)");
			sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGCode, ALARM_CODE_INMOTION);

			if (m_pDlgMotion->AxisMoveDone(AXIS_2))
				SetSeqIdx(prc_idx, seq_idx, 133);
			break;
		case 133:
			not_delay = CheckDelay(prc_idx, seq_idx, 100);

			if (not_delay)
				SetSeqIdx(prc_idx, seq_idx, 140);
			break;
		case 140://Align #1 GRAB ����
			m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_dTimeout = TIMEOUT_ALIGN_GRAB;//ms
			sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGStatus, "Time over Align#1 Grab !(2:140)");
			sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGCode, ALARM_CODE_GRAB);
			
			if (m_pDispAlign->Grab(0))
				SetSeqIdx(prc_idx, seq_idx, 141);
			break;
		case 141://Align #1 GRAB ����
			m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_dTimeout = TIMEOUT_ALIGN_GRAB;//ms
			sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGStatus, "Time over Align#1 Grab !(2:141)");
			sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGCode, ALARM_CODE_GRAB);

			if (!m_pDispAlign->IsGrabbing(0))
				SetSeqIdx(prc_idx, seq_idx, 150);
			break;
		case 150://Align #1 Inspection ����
			m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_dTimeout = TIMEOUT_ALIGN_INSPECTION;//ms
			sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGStatus, "Failed Align#1 Inspection !(2:150)");
			sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGCode, ALARM_CODE_GRAB);

			if (m_pDispAlign->PatternMatch(0))
				SetSeqIdx(prc_idx, seq_idx, 151);
			break;
		case 151://Align #1 Inspection ����
			m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_dTimeout = TIMEOUT_ALIGN_INSPECTION;//ms
			sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGStatus, "Time over Align#1 Inspection !(2:151)");
			sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGCode, ALARM_CODE_INSPECTION);

			if (TRUE)	//�˻� ���� �÷���
				SetSeqIdx(prc_idx, seq_idx, 160);
			break;
			
		//ALIGN #2
		case 160://Align#2 ��ġ�� �̵�
			m_pDlgMotion->AxisMove(AXIS_0, m_pDlgMotion->GetAxisPos(AXIS_0, 1) + dblXCorrection, MOTION_VEL_MAGNIFICATION, FALSE);
			m_pDlgMotion->AxisMove(AXIS_1, m_pDlgMotion->GetAxisPos(AXIS_1, 1) + dblYCorrection, MOTION_VEL_MAGNIFICATION, FALSE);
			m_pDlgMotion->AxisMove(AXIS_2, m_pDlgMotion->GetAxisPos(AXIS_2, 1) + dblTCorrection, MOTION_VEL_MAGNIFICATION, FALSE);

			SetSeqIdx(prc_idx, seq_idx, 170);
			break;
		case 170://Align#2 ��ġ üũ
			m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_dTimeout = TIMEOUT_IN_MOTION;//ms
			sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGStatus, "Time over X-Axis Move !(2:170)");
			sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGCode, ALARM_CODE_INMOTION);

			if (m_pDlgMotion->AxisMoveDone(AXIS_0))
				SetSeqIdx(prc_idx, seq_idx, 171);
			break;
		case 171://Align#2 ��ġ üũ
			m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_dTimeout = TIMEOUT_IN_MOTION;//ms
			sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGStatus, "Time over Y-Axis Move !(2:171)");
			sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGCode, ALARM_CODE_INMOTION);

			if (m_pDlgMotion->AxisMoveDone(AXIS_1))
				SetSeqIdx(prc_idx, seq_idx, 172);
			break;
		case 172://Align#2 ��ġ üũ
			m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_dTimeout = TIMEOUT_IN_MOTION;//ms
			sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGStatus, "Time over T-Axis Move !(2:172)");
			sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGCode, ALARM_CODE_INMOTION);

			if (m_pDlgMotion->AxisMoveDone(AXIS_2))
				SetSeqIdx(prc_idx, seq_idx, 173);
			break;
		case 173:
			not_delay = CheckDelay(prc_idx, seq_idx, 100);

			if (not_delay)
				SetSeqIdx(prc_idx, seq_idx, 180);
			break;
		case 180://Align #2 GRAB ����
			m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_dTimeout = TIMEOUT_ALIGN_GRAB;//ms
			sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGStatus, "Time over Align#2 Grab !(2:180)");
			sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGCode, ALARM_CODE_GRAB);

			if (m_pDispAlign->Grab(1))
				SetSeqIdx(prc_idx, seq_idx, 181);
			break;
		case 181://Align #2 GRAB ����
			m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_dTimeout = TIMEOUT_ALIGN_GRAB;//ms
			sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGStatus, "Time over Align#2 Grab !(2:181)");
			sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGCode, ALARM_CODE_GRAB);

			if (!m_pDispAlign->IsGrabbing(1))
				SetSeqIdx(prc_idx, seq_idx, 190);
			break;
		case 190://Align #2 Inspection ����
			//m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_dTimeout = TIMEOUT_ALIGN_INSPECTION;//ms
			//sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGStatus, "ALIGN #2 �˻� �ð� �ʰ�!(2:190)");
			//sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGCode, ALARM_CODE_INSPECTION);

			if (!m_pDispAlign->PatternMatch(1))
			{
				AfxMessageBox(_T("Pattern Match Error"));
				SetSeqIdx(prc_idx, seq_idx, 100);
				break;
			}

			if (!IsAlignCheck)
			{
				if (TMoveCnt == 0)
				{
					if (m_pDispAlign->GetAlignCompPos(
						m_pDlgMotion->GetAxisPos(AXIS_0, 0) + dblXCorrection,
						m_pDlgMotion->GetAxisPos(AXIS_0, 1) + dblXCorrection,
						m_pDlgMotion->GetAxisPos(AXIS_1, 0) + dblYCorrection,
						m_pDlgMotion->GetAxisPos(AXIS_1, 1) + dblYCorrection,
						&dblTTmp, &dblXTmp, &dblYTmp))
					{
						dblXCorrection += dblXTmp;
						dblYCorrection += dblYTmp;
						dblTCorrection += dblTTmp;
						TMoveCnt++;
						SetSeqIdx(prc_idx, seq_idx, 160);
					}
					else
					{
						SetSeqIdx(prc_idx, seq_idx, 100);
					}
				}
				else if(TMoveCnt < 5)
				{
					if (m_pDispAlign->GetAlignCompPos(
						m_pDlgMotion->GetAxisPos(AXIS_0, 0) + dblXCorrection,
						m_pDlgMotion->GetAxisPos(AXIS_0, 1) + dblXCorrection,
						m_pDlgMotion->GetAxisPos(AXIS_1, 0) + dblYCorrection,
						m_pDlgMotion->GetAxisPos(AXIS_1, 1) + dblYCorrection,
						&dblTTmp, &dblXTmp, &dblYTmp, NULL, TRUE))
					{
						if (abs(dblXTmp) > 20 || abs(dblYTmp) > 2)	//	2��ũ�� �̻� ���� �� ��츸 �ٽ� ����
						{
							dblXCorrection += dblXTmp;
							dblYCorrection += dblYTmp;
							TMoveCnt++;
							SetSeqIdx(prc_idx, seq_idx, 160);
						}
						else
						{
							if (m_pDispAlign->IsCheckAllCam())
							{
								IsAlignCheck = TRUE;
								SetSeqIdx(prc_idx, seq_idx, 120);
							}
							else
							{
								SetSeqIdx(prc_idx, seq_idx, 191);
							}
						}
					}
					else
					{
						SetSeqIdx(prc_idx, seq_idx, 100);
					}
				}
				else
				{
					SetSeqIdx(prc_idx, seq_idx, 100);
				}

				break;
			}

			SetSeqIdx(prc_idx, seq_idx, 191);
			break;
		case 191://Align #2 Inspection ����
			m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_dTimeout = TIMEOUT_ALIGN_INSPECTION;//ms
			sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGStatus, "Time over Align#2 Inspection !(2:191)");
			sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGCode, ALARM_CODE_INSPECTION);

			if (TRUE)	//�˻� ���� �÷���
				SetSeqIdx(prc_idx, seq_idx, 200);
			break;
		case 200://ALIGN ��ġ üũ
			m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_dTimeout = TIMEOUT_IN_MOTION;//ms
			sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGStatus, "Time over X-Axis Move !(2:200)");
			sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGCode, ALARM_CODE_INMOTION);

			if (m_pDlgMotion->AxisMoveDone(AXIS_0))
				SetSeqIdx(prc_idx, seq_idx, 201);
			break;
		case 201://ALIGN ��ġ üũ
			m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_dTimeout = TIMEOUT_IN_MOTION;//ms
			sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGStatus, "Time over Y-Axis Move !(2:201)");
			sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGCode, ALARM_CODE_INMOTION);

			if (m_pDlgMotion->AxisMoveDone(AXIS_1))
				SetSeqIdx(prc_idx, seq_idx, 202);
			break;
		case 202://ALIGN ��ġ üũ
			m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_dTimeout = TIMEOUT_IN_MOTION;//ms
			sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGStatus, "Time over T-Axis Move !(2:202)");
			sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGCode, ALARM_CODE_INMOTION);

			if (m_pDlgMotion->AxisMoveDone(AXIS_2))
				SetSeqIdx(prc_idx, seq_idx, 210);
			break;

		//LINESCAN
		case 210://Linescan GRAB ������ġ �̵�
			m_pDlgMotion->AxisMove(AXIS_0, m_pDlgMotion->GetAxisPos(AXIS_0, 2) + dblXCorrection, MOTION_VEL_MAGNIFICATION, FALSE);
			m_pDlgMotion->AxisMove(AXIS_1, m_pDlgMotion->GetAxisPos(AXIS_1, 2) + dblYCorrection, MOTION_VEL_MAGNIFICATION, FALSE);
			m_pDlgMotion->AxisMove(AXIS_2, m_pDlgMotion->GetAxisPos(AXIS_2, 2) + dblTCorrection, MOTION_VEL_MAGNIFICATION, FALSE);

			SetSeqIdx(prc_idx, seq_idx, 220);
			break;
		case 220://Linescan Start ��ġ üũ
			m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_dTimeout = TIMEOUT_IN_MOTION;//ms
			sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGStatus, "Time over X-Axis Move !(2:220)");
			sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGCode, ALARM_CODE_INMOTION);

			if (m_pDlgMotion->AxisMoveDone(AXIS_0))
				SetSeqIdx(prc_idx, seq_idx, 221);
			break;
		case 221://Linescan Start ��ġ üũ
			m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_dTimeout = TIMEOUT_IN_MOTION;//ms
			sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGStatus, "Time over Y-Axis Move !(2:221)");
			sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGCode, ALARM_CODE_INMOTION);

			if (m_pDlgMotion->AxisMoveDone(AXIS_1))
				SetSeqIdx(prc_idx, seq_idx, 222);
			break;
		case 222://Linescan Start ��ġ üũ
			m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_dTimeout = TIMEOUT_IN_MOTION;//ms
			sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGStatus, "Time over T-Axis Move !(2:222)");
			sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGCode, ALARM_CODE_INMOTION);

			if (m_pDlgMotion->AxisMoveDone(AXIS_2))
				SetSeqIdx(prc_idx, seq_idx, 225);// 240);
			break;
		case 225:
			not_delay = CheckDelay(prc_idx, seq_idx, 100);

			if (not_delay)
				SetSeqIdx(prc_idx, seq_idx, 230);
			break;
		case 230://test triger init
			m_pDlgSystem->SetTriggerInit();

			SetSeqIdx(prc_idx, seq_idx, 240);
			break;

		case 240://Linescan Grab Ready & Start
			//20180912 ngh
			m_pDlgDispMain->SetGrab();

			SetSeqIdx(prc_idx, seq_idx, 241);
			break;
		case 241:
			m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_dTimeout = TIMEOUT_LINESCAN_GRAB;//ms
			sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGStatus, "Time over Grab Linescan !(2:241)");
			sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGCode, ALARM_CODE_GRAB);

			if(m_pDlgDispMain->GetGrabWait())
				SetSeqIdx(prc_idx, seq_idx, 250);
			break;
		case 250://LLinescan GRAB End
			m_pDlgMotion->AxisMove(AXIS_0, m_pDlgMotion->GetAxisPos(AXIS_0, 3) + dblXCorrection, 1.0, FALSE);
			m_pDlgMotion->AxisMove(AXIS_1, m_pDlgMotion->GetAxisPos(AXIS_1, 3) + dblYCorrection, 1.0, FALSE);
			m_pDlgMotion->AxisMove(AXIS_2, m_pDlgMotion->GetAxisPos(AXIS_2, 3) + dblTCorrection, 1.0, FALSE);

			//20181112 ngh
			m_pDlgDispMain->SetInspection(m_pDlgModel->GetPath());//	IMSI

			SetSeqIdx(prc_idx, seq_idx, 260);
			break;
		case 260://Linescan End ��ġ üũ
			m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_dTimeout = TIMEOUT_IN_MOTION;//ms
			sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGStatus, "Time over X-Axis Move !(2:260)");
			sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGCode, ALARM_CODE_INMOTION);

			if (m_pDlgMotion->AxisMoveDone(AXIS_0))
				SetSeqIdx(prc_idx, seq_idx, 261);
			break;
		case 261://Linescan End ��ġ üũ
			m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_dTimeout = TIMEOUT_IN_MOTION;//ms
			sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGStatus, "Time over Y-Axis Move !(2:261)");
			sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGCode, ALARM_CODE_INMOTION);

			if (m_pDlgMotion->AxisMoveDone(AXIS_1))
				SetSeqIdx(prc_idx, seq_idx, 262);
			break;
		case 262://Linescan End ��ġ üũ
			m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_dTimeout = TIMEOUT_IN_MOTION;//ms
			sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGStatus, "Time over T-Axis Move !(2:262)");
			sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGCode, ALARM_CODE_INMOTION);

			if (m_pDlgMotion->AxisMoveDone(AXIS_2))
				SetSeqIdx(prc_idx, seq_idx, 263);
			break;
		case 263:
			m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_dTimeout = TIMEOUT_LINESCAN_GRAB;//ms
			sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGStatus, "Grab End Time Out!(2:263)");
			sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGCode, ALARM_CODE_INMOTION);

			//20181112 ngh
			//if (m_pDlgDispMain->GetGrabEnd())
			//20181114 ngh
			m_pDlgDispMain->SetGrabEnd(TRUE);
				SetSeqIdx(prc_idx, seq_idx, 270);
			break;
		case 270://���� �˻� ����
			//20180912 ngh
			//m_pDlgDispMain->SetInspection(m_pDlgModel->GetPath());//	IMSI
			

			//IMSI
			if(testbool)
				SetSeqIdx(prc_idx, seq_idx, 210);
			else
				SetSeqIdx(prc_idx, seq_idx, 280);
			
			break;
		case 280://��� ��ġ�� �̵�
			m_pDlgMotion->AxisMove(AXIS_0, m_pDlgMotion->GetAxisPos(AXIS_0, 4), MOTION_VEL_MAGNIFICATION, FALSE);
			m_pDlgMotion->AxisMove(AXIS_1, m_pDlgMotion->GetAxisPos(AXIS_1, 4), MOTION_VEL_MAGNIFICATION, FALSE);
			m_pDlgMotion->AxisMove(AXIS_2, m_pDlgMotion->GetAxisPos(AXIS_2, 4), MOTION_VEL_MAGNIFICATION, FALSE);

			SetSeqIdx(prc_idx, seq_idx, 290);
			break;
		case 290://��� ��ġ üũ
			m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_dTimeout = TIMEOUT_IN_MOTION;//ms
			sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGStatus, "Time over X-Axis Move !(2:290)");
			sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGCode, ALARM_CODE_INMOTION);

			if (m_pDlgMotion->AxisMoveDone(AXIS_0))
				SetSeqIdx(prc_idx, seq_idx, 291);
			break;
		case 291://��� ��ġ üũ
			m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_dTimeout = TIMEOUT_IN_MOTION;//ms
			sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGStatus, "Time over Y-Axis Move !(2:291)");
			sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGCode, ALARM_CODE_INMOTION);

			if (m_pDlgMotion->AxisMoveDone(AXIS_1))
				SetSeqIdx(prc_idx, seq_idx, 292);
			break;
		case 292://��� ��ġ üũ
			m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_dTimeout = TIMEOUT_IN_MOTION;//ms
			sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGStatus, "Time over T-Axis Move !(2:292)");
			sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGCode, ALARM_CODE_INMOTION);

			if (m_pDlgMotion->AxisMoveDone(AXIS_2))
				SetSeqIdx(prc_idx, seq_idx, 300);	//IMSI
				//SetSeqIdx(prc_idx, seq_idx, 310);
			break;
		case 300://�˻��� ���
			m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_dTimeout = TIMEOUT_MAIN_INSPECTION;//ms
			sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGStatus, "Time over Main Inspection !(2:300)");
			sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGCode, ALARM_CODE_INMOTION);

			if (m_pDlgDispMain->GetInspEnd())
			{
				if (m_pDlgDispMain->GetInspRslt(0)) {	//Inspect result 
					m_nCountGood++;
					m_btnJudgement.SetCaption(_T("GOOD"));
					m_btnJudgement.SetBackColor(GREEN);
				}
				else {
					m_nCountNg++;
					m_btnJudgement.SetCaption(_T("NG"));
					m_btnJudgement.SetBackColor(RED);
				}

				SystemListDisplay(TRUE, _T("Release a Vaccum button!"));
				GetLocalTime(&st);
				
				//m_pDlgResult->SaveAndViewResultData(&st);	//	bjs	IMSI
				
				//SetSeqIdx(prc_idx, seq_idx, 310);	IMSI
				AfxMessageBox(_T("NEXT...."));
				SetSeqIdx(prc_idx, seq_idx, 110);
			};
			break;
		case 310://VAC ��ư ������
			m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_dTimeout = 0;// ���� ���
			sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGStatus, "Time over VACCUM release !(2:310)");
			sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][sub_seq_idx].m_cNGCode, ALARM_CODE_SOL);

			if (!m_pDlgMotion->ReadInput(AXIS_1, UNIVERSAL_INPUT_X02) && m_pDlgMotion->ReadInput(AXIS_1, UNIVERSAL_INPUT_X03))
			{
				m_pDlgMotion->WriteOutput(AXIS_0, UNIVERSAL_OUTPUT_Y02, FALSE);
				m_pDlgMotion->WriteOutput(AXIS_0, UNIVERSAL_OUTPUT_Y03, TRUE);
				Delay(100, TRUE);
				m_pDlgMotion->WriteOutput(AXIS_0, UNIVERSAL_OUTPUT_Y03, FALSE);

				SystemListDisplay(TRUE, _T("End of Inspect!"));

				m_bFlagInspect = FALSE;

				SetSeqIdx(prc_idx, seq_idx, 500);
			}
			break;
		case 500:
			SetSeqIdx(prc_idx, seq_idx, 100);
			break;
		}

		break;
	}

	if (chk_timeout)
		rslt = CheckTimeout(prc_idx, seq_idx, m_MDat[prc_idx].m_Seq[seq_idx].m_iSubSeqIdx, status_type);

	return rslt;
}


//������ �ε��� �ֱ�(������ �������� ����)
void CPMInspectDlg::SetSeqIdx(int prc_idx, int seq_idx, int seq_sub_idx)
{
	m_MDat[prc_idx].m_ProSts[seq_idx][seq_sub_idx].m_iNGSeqIdx = 0;
	m_MDat[prc_idx].m_Seq[seq_idx].m_iSubSeqIdx = seq_sub_idx;
	m_MDat[prc_idx].m_Seq[seq_idx].m_iDispSubSeqIdx = seq_sub_idx;
	m_MDat[prc_idx].m_Seq[seq_idx].m_iNextSubSeqIdx = seq_sub_idx;
	m_MDat[prc_idx].m_Seq[seq_idx].m_bOpSts = 0;
	m_MDat[prc_idx].m_Seq[seq_idx].m_iDeviceIdx = 0;
}

//������ �ε��� �ֱ�(�����ϸ� ������������ �Ѿ)
void CPMInspectDlg::SetSeqIdx(int prc_idx, int seq_idx, int seq_sub_idx, int next_seq_sub_idx, int type)
{
	m_MDat[prc_idx].m_ProSts[seq_idx][seq_sub_idx].m_iNGSeqIdx = 0;
	m_MDat[prc_idx].m_Seq[seq_idx].m_iSubSeqIdx = seq_sub_idx;
	m_MDat[prc_idx].m_Seq[seq_idx].m_iDispSubSeqIdx = seq_sub_idx;
	m_MDat[prc_idx].m_Seq[seq_idx].m_iNextSubSeqIdx = next_seq_sub_idx;
	m_MDat[prc_idx].m_Seq[seq_idx].m_bOpSts = type;
	m_MDat[prc_idx].m_Seq[seq_idx].m_iDeviceIdx = 0;
}

void CPMInspectDlg::SetSeqIdx(int prc_idx, int seq_idx, int seq_sub_idx, int next_seq_sub_idx, int type, int device_idx)
{
	m_MDat[prc_idx].m_ProSts[seq_idx][seq_sub_idx].m_iNGSeqIdx = 0;
	m_MDat[prc_idx].m_Seq[seq_idx].m_iSubSeqIdx = seq_sub_idx;
	m_MDat[prc_idx].m_Seq[seq_idx].m_iDispSubSeqIdx = seq_sub_idx;
	m_MDat[prc_idx].m_Seq[seq_idx].m_iNextSubSeqIdx = next_seq_sub_idx;
	m_MDat[prc_idx].m_Seq[seq_idx].m_bOpSts = type;
	m_MDat[prc_idx].m_Seq[seq_idx].m_iDeviceIdx = device_idx;
}

//Check timeout
int CPMInspectDlg::CheckTimeout(int prc_idx, int seq_idx, int seq_sub_idx, char* status_type)
{

	if (0 == m_MDat[prc_idx].m_ProSts[seq_idx][seq_sub_idx].m_iNGSeqIdx)
	{
		m_MDat[prc_idx].m_ProSts[seq_idx][seq_sub_idx].m_iNGSeqIdx++;
		m_MDat[prc_idx].m_ProSts[seq_idx][seq_sub_idx].m_dTimeoutS = _TimerCounter;
	}
	else
	{
		//Compare timeout
		if (0 <m_MDat[prc_idx].m_ProSts[seq_idx][seq_sub_idx].m_dTimeout)
		{
			m_MDat[prc_idx].m_ProSts[seq_idx][seq_sub_idx].m_dTimeoutE = _TimerCounter;
			if (m_MDat[prc_idx].m_ProSts[seq_idx][seq_sub_idx].m_dTimeout < (m_MDat[prc_idx].m_ProSts[seq_idx][seq_sub_idx].m_dTimeoutE - m_MDat[prc_idx].m_ProSts[seq_idx][seq_sub_idx].m_dTimeoutS))
			{
				return 0;
			}
		}
	}

	sprintf(m_MDat[prc_idx].m_ProSts[seq_idx][seq_sub_idx].m_iTypeStatus, "%s", status_type);//���ȣ

	return 1;
}

//Check timeout
int CPMInspectDlg::CheckDelay(int prc_idx, int seq_idx, int delay)
{
	if (0 == m_MDat[prc_idx].m_ProSts[seq_idx][m_MDat[prc_idx].m_Seq[seq_idx].m_iSubSeqIdx].m_iNGSeqIdx)
	{
		m_MDat[prc_idx].m_ProSts[seq_idx][m_MDat[prc_idx].m_Seq[seq_idx].m_iSubSeqIdx].m_iNGSeqIdx++;
		m_MDat[prc_idx].m_ProSts[seq_idx][m_MDat[prc_idx].m_Seq[seq_idx].m_iSubSeqIdx].m_dTimeoutS = _TimerCounter;
	}
	else
	{
		//Compare timeout
		if (0 < delay)
		{
			m_MDat[prc_idx].m_ProSts[seq_idx][m_MDat[prc_idx].m_Seq[seq_idx].m_iSubSeqIdx].m_dTimeoutE = _TimerCounter;
			if (delay < (m_MDat[prc_idx].m_ProSts[seq_idx][m_MDat[prc_idx].m_Seq[seq_idx].m_iSubSeqIdx].m_dTimeoutE - m_MDat[prc_idx].m_ProSts[seq_idx][m_MDat[prc_idx].m_Seq[seq_idx].m_iSubSeqIdx].m_dTimeoutS))
			{
				return 1;
			}
		}
		else
		{
			return 1;
		}
	}

	return 0;
}

int CPMInspectDlg::StopModeOperation(int *prc_idx, int *seq_idx)
{
	BOOL rslt = TRUE;;

	*prc_idx = 0;
	*seq_idx = 0;

	if (m_MDat[*prc_idx].m_Seq[*seq_idx].m_bAlarm)
	{
		return 1;
	}

	//Safe and limit sensor check
	if (!m_pDlgMotion->ReadInput(AXIS_0, UNIVERSAL_INPUT_X04))
	{
		m_MDat[*prc_idx].m_Seq[*seq_idx].m_iSubSeqIdx = OP_EMS_CHECK;
		m_MDat[*prc_idx].m_ProSts[*seq_idx][m_MDat[*prc_idx].m_Seq[*seq_idx].m_iSubSeqIdx].m_dTimeout = TIMEOUT_SAFE_SENSOR;//ms
		sprintf(m_MDat[*prc_idx].m_ProSts[*seq_idx][m_MDat[*prc_idx].m_Seq[*seq_idx].m_iSubSeqIdx].m_cNGStatus, "Detected EMO !");
		sprintf(m_MDat[*prc_idx].m_ProSts[*seq_idx][m_MDat[*prc_idx].m_Seq[*seq_idx].m_iSubSeqIdx].m_cNGCode, ALARM_CODE_EMO);

		return 0;
	}

	if (m_pDlgMotion->ReadInput(AXIS_0, UNIVERSAL_INPUT_X02))
	{
		AutoStart();
		m_btnStart.SetValue(TRUE);
	}

	return TRUE;
}

void CPMInspectDlg::OnClickAutoRun()
{
	CBtnenhctrl *pBtnEnh = (CBtnenhctrl*)GetFocus();
	BOOL rslt = TRUE;

	switch (pBtnEnh->GetDlgCtrlID())
	{
	case IDC_BTNENHCTRL_BUTTON_START:
		if (g_cMsgBox.YesNoMsgBox(_T("Auto Run Start?")) == IDYES)
		{
			for (int i = 0; i < MAX_AXIS_NUM; i++)
			{
				if (!m_pDlgMotion->AxisIsHomeFinished(i))
				{
					rslt = FALSE;
				}
			}

			if (!rslt)
			{
				g_cMsgBox.OkMsgBox(_T("Failed. Run Origin !"));
				return;
			}

			AutoStart();

			//m_pDlgManual->SetLampRun();
		}
		break;
	case IDC_BTNENHCTRL_BUTTON_STOP:
	{
		AutoStop();

		//m_pDlgManual->SetLampWait();
		break;
	}

	default:
		return;
	}
}

//20180911 by Add
void CPMInspectDlg::MakeDefaultDir()
{

	MakeDirectories(DATA_MODEL_PATH);
	MakeDirectories(DATA_SYSTEM_PATH);

}

void CPMInspectDlg::ClickBtnenhctrlButtonReset()
{
	if (g_cMsgBox.YesNoMsgBox(_T("Initialize sequence ?")) == IDYES)
	{
		while (TRUE)
		{
			if (m_pDlgMotion->ReadInput(AXIS_1, UNIVERSAL_INPUT_X02) && !m_pDlgMotion->ReadInput(AXIS_1, UNIVERSAL_INPUT_X03))
				g_cMsgBox.OkMsgBox(_T("VAC Button is pressed!"));
			else
				break;

			Delay(1, TRUE);
		}

		//VAC �ʱ�ȭ
		m_pDlgMotion->WriteOutput(AXIS_0, UNIVERSAL_OUTPUT_Y02, FALSE);
		m_pDlgMotion->WriteOutput(AXIS_0, UNIVERSAL_OUTPUT_Y03, TRUE);
		Delay(100, TRUE);
		m_pDlgMotion->WriteOutput(AXIS_0, UNIVERSAL_OUTPUT_Y03, FALSE);

		//��� ��� ��ġ��
		m_pDlgMotion->AxisMove(AXIS_0, m_pDlgMotion->GetAxisPos(AXIS_0, 4), MOTION_VEL_MAGNIFICATION, FALSE);
		m_pDlgMotion->AxisMove(AXIS_1, m_pDlgMotion->GetAxisPos(AXIS_1, 4), MOTION_VEL_MAGNIFICATION, FALSE);
		m_pDlgMotion->AxisMove(AXIS_2, m_pDlgMotion->GetAxisPos(AXIS_2, 4), MOTION_VEL_MAGNIFICATION, FALSE);

		//������ �ʱ�ȭ
		m_bFlagInspect = FALSE;

		m_Flag.m_bFirstStart = TRUE;

		//�˻����
		m_MDat[0].m_Seq[1].m_iSubSeqIdx = 100;
		m_MDat[0].m_Seq[2].m_iSubSeqIdx = 100;
	}
}

void CPMInspectDlg::SetCount()
{
	CString strTemp;


	if (m_nCountTotal != _ttoi(m_btnCountTotal.GetCaption())) {
		strTemp.Format(_T("%d"), m_nCountTotal);
		m_btnCountTotal.SetCaption(strTemp);
	}

	if (m_nCountGood != _ttoi(m_btnCountGood.GetCaption())) {
		strTemp.Format(_T("%d"), m_nCountGood);
		m_btnCountGood.SetCaption(strTemp);
	}

	if (m_nCountNg != _ttoi(m_btnCountNg.GetCaption())) {
		strTemp.Format(_T("%d"), m_nCountNg);
		m_btnCountNg.SetCaption(strTemp);
	}
}

void CPMInspectDlg::SetChangeModel()
{
	CString strTemp;

	strTemp.Format(_T("%S"), GetCurrentModelName());
	m_btnModelName.SetCaption(strTemp);

	m_pDlgMotion->SetFilePath();
	m_pDlgSystem->SetFilePath();

	m_pDlgDispMain->LoadModelData(GetCurrentModelName());
	m_pDispAlign->LoadAlignData(strTemp);
	m_pDlgResult->ChangeModel(strTemp);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

void CPMInspectDlg::ClickBtnenhctrlCountReset()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.

	//imsi
	if (testbool)
		testbool = FALSE;
	else
		testbool = TRUE;

	return; 

	if (g_cMsgBox.YesNoMsgBox(_T("Reset count ?")) == IDYES)
	{
		m_nCountTotal = 0;
		m_nCountGood = 0;
		m_nCountNg = 0;
	}
}

void CPMInspectDlg::SetOpStatus(CString str)
{
	m_btnOpStatus.SetCaption(str);
}

void CPMInspectDlg::InspectListDisplay(BOOL save, CString data)
{
	if (LIST_MAX_NUM <= m_ctrl_listInspect.GetItemCount())
	{
		m_ctrl_listInspect.DeleteItem(LIST_MAX_NUM - 1);
	}

	m_ctrl_listInspect.InsertItem(0, data);
}

void CPMInspectDlg::SystemListDisplay(BOOL save, CString data)
{
	CString strTemp;
	SYSTEMTIME	lpSystemTime;
	GetLocalTime(&lpSystemTime);

	strTemp.Format(_T("[%04d%02d%02d_%02d%02d%02d]%s"), lpSystemTime.wYear, lpSystemTime.wMonth, lpSystemTime.wDay,
																lpSystemTime.wHour, lpSystemTime.wMinute, lpSystemTime.wSecond,
																data);

	if (LIST_MAX_NUM <= m_ctrl_listSystem.GetItemCount())
	{
		m_ctrl_listSystem.DeleteItem(LIST_MAX_NUM - 1);
	}

	m_ctrl_listSystem.InsertItem(0, strTemp);
	
	if (save)
		WriteLog(data, SYSTEM_LOG);
}
