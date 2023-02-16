// DialogSystem.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "PMInspect.h"
#include "DialogSystem.h"
#include "afxdialogex.h"

#include "PMInspectDlg.h"

// CDialogSystem 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDialogSystem, CDialogEx)

CDialogSystem::CDialogSystem(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_SYSTEM, pParent)
{
	m_Paramanager.Create();
	
	ZeroMemory(&m_TrgPara, sizeof(TRIGGER_PARAMETER));
}

CDialogSystem::~CDialogSystem()
{
	m_Paramanager.Release();
}

void CDialogSystem::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_BTNENHCTRL_SYSTEM_LABEL_TRIGGER_IP_ADDRESS1, m_btn_IP_Address);
	DDX_Control(pDX, IDC_BTNENHCTRL_SYSTEM_LABEL_TRIGGER_PORT1, m_btn_Port);
	DDX_Control(pDX, IDC_BTNENHCTRL_SYSTEM_LABEL_TRIGGER_VER1, m_btn_Version);
	DDX_Control(pDX, IDC_BTNENHCTRL_SYSTEM_LABEL_TRIGGER_XUDCNT1, m_btn_XudCnt);
	DDX_Control(pDX, IDC_BTNENHCTRL_SYSTEM_LABEL_TRIGGER_TRGCNT1, m_btn_TrgCnt);
	DDX_Control(pDX, IDC_BTNENHCTRL_SYSTEM_LABEL_TRIGGER_TRGTIME1, m_btn_TrgTime);
	DDX_Control(pDX, IDC_BTNENHCTRL_SYSTEM_LABEL_TRIGGER_TRGENBL1, m_btn_TrgEnbl);
	DDX_Control(pDX, IDC_BTNENHCTRL_SYSTEM_LABEL_TRIGGER_XLCYCLE1, m_btn_Cycle);
	DDX_Control(pDX, IDC_BTNENHCTRL_SYSTEM_LABEL_TRIGGER_XLTRGNO1, m_btn_TrgNo);
	DDX_Control(pDX, IDC_BTNENHCTRL_SYSTEM_LABEL_TRIGGER_XLTRGSTART1, m_btn_TrgStart);
	DDX_Control(pDX, IDC_BTNENHCTRL_SYSTEM_LABEL_TRIGGER_XLTRGEND1, m_btn_TrgEnd);
	DDX_Control(pDX, IDC_BTNENHCTRL_SYSTEM_BUTTON_READDATA, m_btn_ReadData);
	
}

BEGIN_MESSAGE_MAP(CDialogSystem, CDialogEx)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// CDialogSystem 메시지 처리기입니다.
BOOL CDialogSystem::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	if (!AfxSocketInit())
	{
		AfxMessageBox(_T("SYSTEM : Failed to Initialize Sockets"));
		return FALSE;
	}

	m_btn_IP_Address.SetCaption(_T("172.68.0.2"));
	m_btn_Port.SetCaption(_T("5000"));

	SetLinkPara();

	m_Paramanager.LoadPara();
	UpdatePara(TRUE);

	ClickBtnenhctrlSystemButtonParameterSet();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CDialogSystem::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 여기에 메시지 처리기 코드를 추가합니다.
					   // 그리기 메시지에 대해서는 CDialogEx::OnPaint()을(를) 호출하지 마십시오.

	if (IsIconic())
	{

	}
	else
	{
		CRect rect;
		CPen pen, *oldPen;
		CBrush brush, *oldBrush;

		GetClientRect(&rect);

		//빈공간 채우기
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

void CDialogSystem::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	switch ((int)nIDEvent)
	{
	case 0:
		{
			CSocket s;

			int port;
			CString url;
			CString _tmp;
			int nByteRead;

			struct data_form
			{
				WORD readwrite;     // read : 0x5555, write : 0xAAAA
				WORD address;       // address : 0 ~ 65535
				DWORD dataA;
			};

			struct data_form send_data_form, receive_data_form;

			_tmp = m_btn_Port.GetCaption();
			port = _ttoi(_tmp);

			url = m_btn_IP_Address.GetCaption();

			send_data_form.readwrite = 0x5555;
			send_data_form.address = X_ENCODER_COUNTER; // Address = X Encoder Counter Read
														//	send_data_form.dataA = 0;

			if (!s.Create())
			{
				AfxMessageBox(_T("It is failed to create Socket"));
				return;
			}

			if (!s.Connect(url, port))
			{
				AfxMessageBox(_T("It is failed to connect to Server."));
				return;
			}

			send_data_form.address = TRIGER_VERSION_READ;
			s.Send(&send_data_form, sizeof(struct data_form), 0);
			nByteRead = s.Receive(&receive_data_form, sizeof(struct data_form), 0);
			SetDlgItemInt(IDC_BTNENHCTRL_SYSTEM_LABEL_TRIGGER_VER1, receive_data_form.dataA);
			ZeroMemory(&receive_data_form, sizeof(struct data_form)); // 메모리를 비운다.


			send_data_form.address = X_ENCODER_COUNTER;
			s.Send(&send_data_form, sizeof(struct data_form), 0); // 전송 구조체를 통째로 전송한다.
			nByteRead = s.Receive(&receive_data_form, sizeof(struct data_form), 0); // FPGA로 부터 날아온 데이터를 구조체에 저장한다.
																					// nByteRead : 몇개의 바이트가 수신되었는지 표시한다
			SetDlgItemInt(IDC_BTNENHCTRL_SYSTEM_LABEL_TRIGGER_XUDCNT1, receive_data_form.dataA); // FPGA로 부터 되돌아온 Read/Write 상태를 표시한다.
			ZeroMemory(&receive_data_form, sizeof(struct data_form)); // 메모리를 비운다.

			send_data_form.address = XL_TRIGGER_COUNTER; // Address = X Trigger Counter Read
			s.Send(&send_data_form, sizeof(struct data_form), 0);
			nByteRead = s.Receive(&receive_data_form, sizeof(struct data_form), 0);
			SetDlgItemInt(IDC_BTNENHCTRL_SYSTEM_LABEL_TRIGGER_TRGCNT1, receive_data_form.dataA);
			ZeroMemory(&receive_data_form, sizeof(struct data_form));

			/*
			// X축 트리거 사이클 읽기
			send_data_form.address = XL_SET_TRG_CYCLE1_READ; // 트리거 사이클 읽기
			s.Send(&send_data_form, sizeof(struct data_form), 0);
			nByteRead = s.Receive(&receive_data_form, sizeof(struct data_form), 0);
			SetDlgItemInt(IDC_BTNENHCTRL_SYSTEM_LABEL_TRIGGER_XLCYCLE1, 0x000FFFFF & receive_data_form.dataA);
			ZeroMemory(&receive_data_form, sizeof(struct data_form));

			// X축 트리거 수량 읽기
			send_data_form.address = XL_SET_TRG_NUMBER1_READ; // 트리거 출력 수량 읽기
			s.Send(&send_data_form, sizeof(struct data_form), 0);
			nByteRead = s.Receive(&receive_data_form, sizeof(struct data_form), 0);
			SetDlgItemInt(IDC_BTNENHCTRL_SYSTEM_LABEL_TRIGGER_XLTRGNO1, receive_data_form.dataA);
			ZeroMemory(&receive_data_form, sizeof(struct data_form));

			// X축 트리거 스타트 읽기
			send_data_form.address = XL_SET_TRG1_START_POS_READ; // 트리거 스타트 읽기
			s.Send(&send_data_form, sizeof(struct data_form), 0);
			nByteRead = s.Receive(&receive_data_form, sizeof(struct data_form), 0);
			SetDlgItemInt(IDC_BTNENHCTRL_SYSTEM_LABEL_TRIGGER_XLTRGSTART1, receive_data_form.dataA);
			ZeroMemory(&receive_data_form, sizeof(struct data_form));

			// X축 트리거 엔드 읽기
			send_data_form.address = XL_SET_TRG1_END_POS_READ; // 트리거 엔드 읽기
			s.Send(&send_data_form, sizeof(struct data_form), 0);
			nByteRead = s.Receive(&receive_data_form, sizeof(struct data_form), 0);
			SetDlgItemInt(IDC_BTNENHCTRL_SYSTEM_LABEL_TRIGGER_XLTRGEND1, receive_data_form.dataA);
			ZeroMemory(&receive_data_form, sizeof(struct data_form));
			*/
		}
		break;

	default:

		break;
	}

	CDialogEx::OnTimer(nIDEvent);
}


void CDialogSystem::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (bShow)
	{
		m_Paramanager.LoadPara();
		UpdatePara(TRUE);

		//SetTimer(0, 10, NULL);
	}
	else
	{
		KillTimer(0);
	}
}

BOOL CDialogSystem::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CDialogEx::PreTranslateMessage(pMsg);
}
BEGIN_EVENTSINK_MAP(CDialogSystem, CDialogEx)
	ON_EVENT(CDialogSystem, IDC_BTNENHCTRL_SYSTEM_BUTTON_CNT_RESET, DISPID_CLICK, CDialogSystem::ClickBtnenhctrlSystemButtonCntReset, VTS_NONE)
	ON_EVENT(CDialogSystem, IDC_BTNENHCTRL_SYSTEM_BUTTON_PARAMETER_SET, DISPID_CLICK, CDialogSystem::ClickBtnenhctrlSystemButtonParameterSet, VTS_NONE)
	ON_EVENT(CDialogSystem, IDC_BTNENHCTRL_SYSTEM_LABEL_TRIGGER_TRGTIME1, DISPID_DBLCLICK, CDialogSystem::DblClickBtnenhctrlSystemLabelTriggerTrgtime1, VTS_NONE)
	ON_EVENT(CDialogSystem, IDC_BTNENHCTRL_SYSTEM_LABEL_TRIGGER_TRGENBL1, DISPID_DBLCLICK, CDialogSystem::DblClickBtnenhctrlSystemLabelTriggerTrgenbl1, VTS_NONE)
	ON_EVENT(CDialogSystem, IDC_BTNENHCTRL_SYSTEM_LABEL_TRIGGER_XLCYCLE1, DISPID_DBLCLICK, CDialogSystem::DblClickBtnenhctrlSystemLabelTriggerXlcycle1, VTS_NONE)
	ON_EVENT(CDialogSystem, IDC_BTNENHCTRL_SYSTEM_LABEL_TRIGGER_XLTRGNO1, DISPID_DBLCLICK, CDialogSystem::DblClickBtnenhctrlSystemLabelTriggerXltrgno1, VTS_NONE)
	ON_EVENT(CDialogSystem, IDC_BTNENHCTRL_SYSTEM_LABEL_TRIGGER_XLTRGSTART1, DISPID_DBLCLICK, CDialogSystem::DblClickBtnenhctrlSystemLabelTriggerXltrgstart1, VTS_NONE)
	ON_EVENT(CDialogSystem, IDC_BTNENHCTRL_SYSTEM_LABEL_TRIGGER_XLTRGEND1, DISPID_DBLCLICK, CDialogSystem::DblClickBtnenhctrlSystemLabelTriggerXltrgend1, VTS_NONE)
	ON_EVENT(CDialogSystem, IDC_BTNENHCTRL_SYSTEM_BUTTON_APPLY, DISPID_CLICK, CDialogSystem::ClickBtnenhctrlSystemButtonApply, VTS_NONE)
	ON_EVENT(CDialogSystem, IDC_BTNENHCTRL_SYSTEM_BUTTON_OK, DISPID_CLICK, CDialogSystem::ClickBtnenhctrlSystemButtonOk, VTS_NONE)
	ON_EVENT(CDialogSystem, IDC_BTNENHCTRL_SYSTEM_BUTTON_READDATA, DISPID_CLICK, CDialogSystem::ClickBtnenhctrlSystemButtonReaddata, VTS_NONE)
	ON_EVENT(CDialogSystem, IDC_BTNENHCTRL_SYSTEM_BUTTON_CNT_RESET2, DISPID_CLICK, CDialogSystem::ClickBtnenhctrlSystemButtonCntReset2, VTS_NONE)
END_EVENTSINK_MAP()


void CDialogSystem::ClickBtnenhctrlSystemButtonCntReset()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	int nByteRead;
	int port;
	CString url;

	struct data_form
	{
		WORD readwrite;     // read : 0x5555, write : 0xAAAA
		WORD address;       // address : 0 ~ 65535
		DWORD dataA;
	};

	struct data_form send_data_form, receive_data_form;

	CString _tmp;
	_tmp = m_btn_Port.GetCaption();
	port = _ttoi(_tmp);

	url = m_btn_IP_Address.GetCaption();

	send_data_form.readwrite = 0xAAAA;
	send_data_form.address = X_COUNTER_RESET;
	send_data_form.dataA = 1; // 리셋을 걸고


	CSocket s;

	if (!s.Create())
	{
		AfxMessageBox(_T("It is failed to create Socket"));
		return;
	}

	if (!s.Connect(url, port))
	{
		AfxMessageBox(_T("It is failed to connect to Server."));
		return;
	}

	s.Send(&send_data_form, sizeof(struct data_form), 0); // 전송 구조체를 통째로 전송한다. Reset
	nByteRead = s.Receive(&receive_data_form, sizeof(struct data_form), 0); // FPGA로 부터 날아온 데이터를 구조체에 저장한다.
																			// nByteRead : 몇개의 바이트가 수신되었는지 표시한다.
	ZeroMemory(&receive_data_form, sizeof(struct data_form)); // 메모리를 비운다.

	Sleep(1);

	send_data_form.dataA = 0; // 리셋 해제
	s.Send(&send_data_form, sizeof(struct data_form), 0); // 전송 구조체를 통째로 전송한다. Reset 해제

	nByteRead = s.Receive(&receive_data_form, sizeof(struct data_form), 0); // FPGA로 부터 날아온 데이터를 구조체에 저장한다.
																			// nByteRead : 몇개의 바이트가 수신되었는지 표시한다.
	ZeroMemory(&receive_data_form, sizeof(struct data_form)); // 메모리를 비운다


	s.Close();
}


void CDialogSystem::ClickBtnenhctrlSystemButtonParameterSet()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	int port;
	CString url;
	int nByteRead;

	//int xl_strb_even = (GetDlgItemInt(IDC_EDIT_XL_STROBE_EVEN)) << 16 & 0xffff0000;
	//int xl_strb_odd = (GetDlgItemInt(IDC_EDIT_XL_STROBE_ODD)) & 0x0000ffff;

	struct data_form
	{
		WORD readwrite;     // read : 0x5555, write : 0xAAAA
		WORD address;       // address : 0 ~ 65535
		DWORD dataA;
	};

	struct data_form send_data_form, receive_data_form;

	CString _tmp;
	_tmp = m_btn_Port.GetCaption();
	port = _ttoi(_tmp);

	url = m_btn_IP_Address.GetCaption();

	send_data_form.readwrite = 0xAAAA;

	CSocket s;

	if (!s.Create())
	{
		AfxMessageBox(_T("It is failed to create Socket"));
		return;
	}

	if (!s.Connect(url, port))
	{
		AfxMessageBox(_T("It is failed to connect to Server."));
		return;
	}

	send_data_form.address = XL_SET_TRG_CYCLE;
	send_data_form.dataA = (DWORD)_ttoi(m_btn_Cycle.GetCaption());
	s.Send(&send_data_form, sizeof(struct data_form), 0);
	nByteRead = s.Receive(&receive_data_form, sizeof(struct data_form), 0);
	ZeroMemory(&receive_data_form, sizeof(struct data_form));

	send_data_form.address = XL_SET_TRG_ON_TIME;
	send_data_form.dataA = (DWORD)_ttoi(m_btn_TrgTime.GetCaption());
	s.Send(&send_data_form, sizeof(struct data_form), 0);
	nByteRead = s.Receive(&receive_data_form, sizeof(struct data_form), 0);
	ZeroMemory(&receive_data_form, sizeof(struct data_form));

	send_data_form.address = XL_SET_TRG_ENABLE;
	send_data_form.dataA = (DWORD)_ttoi(m_btn_TrgEnbl.GetCaption());
	s.Send(&send_data_form, sizeof(struct data_form), 0);
	nByteRead = s.Receive(&receive_data_form, sizeof(struct data_form), 0);
	ZeroMemory(&receive_data_form, sizeof(struct data_form));

	send_data_form.address = XL_SET_TRG_NUMBER;
	send_data_form.dataA = (DWORD)_ttoi(m_btn_TrgNo.GetCaption());
	s.Send(&send_data_form, sizeof(struct data_form), 0);
	nByteRead = s.Receive(&receive_data_form, sizeof(struct data_form), 0);
	ZeroMemory(&receive_data_form, sizeof(struct data_form));

	send_data_form.address = XL_SET_TRG1_START_POS;
	send_data_form.dataA = (DWORD)_ttoi(m_btn_TrgStart.GetCaption());
	s.Send(&send_data_form, sizeof(struct data_form), 0);
	nByteRead = s.Receive(&receive_data_form, sizeof(struct data_form), 0);
	ZeroMemory(&receive_data_form, sizeof(struct data_form));

	send_data_form.address = XL_SET_TRG1_END_POS; // Trigger 1 End Position
	send_data_form.dataA = (DWORD)_ttoi(m_btn_TrgEnd.GetCaption());
	s.Send(&send_data_form, sizeof(struct data_form), 0);
	nByteRead = s.Receive(&receive_data_form, sizeof(struct data_form), 0);
	ZeroMemory(&receive_data_form, sizeof(struct data_form));

	s.Close();
}

void CDialogSystem::DblClickBtnenhctrlSystemLabelTriggerTrgtime1()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CDialogNumber_Box dlg;

	if (dlg.DoModal() == IDOK) {
		m_btn_TrgTime.SetCaption(dlg.GetstrNum());
		UpdatePara(FALSE);
	}
	else {
		return;
	}
}

void CDialogSystem::DblClickBtnenhctrlSystemLabelTriggerTrgenbl1()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CDialogNumber_Box dlg;

	if (dlg.DoModal() == IDOK) {
		m_btn_TrgEnbl.SetCaption(dlg.GetstrNum());
		UpdatePara(FALSE);
	}
	else {
		return;
	}
}

void CDialogSystem::DblClickBtnenhctrlSystemLabelTriggerXlcycle1()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CDialogNumber_Box dlg;

	if (dlg.DoModal() == IDOK) {
		m_btn_Cycle.SetCaption(dlg.GetstrNum());
		UpdatePara(FALSE);
	}
	else {
		return;
	}
}

void CDialogSystem::DblClickBtnenhctrlSystemLabelTriggerXltrgno1()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CDialogNumber_Box dlg;

	if (dlg.DoModal() == IDOK) {
		m_btn_TrgNo.SetCaption(dlg.GetstrNum());
		UpdatePara(FALSE);
	}
	else {
		return;
	}
}

void CDialogSystem::DblClickBtnenhctrlSystemLabelTriggerXltrgstart1()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CDialogNumber_Box dlg;

	if (dlg.DoModal() == IDOK) {
		m_btn_TrgStart.SetCaption(dlg.GetstrNum());
		UpdatePara(FALSE);
	}
	else {
		return;
	}
}

void CDialogSystem::DblClickBtnenhctrlSystemLabelTriggerXltrgend1()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CDialogNumber_Box dlg;

	if (dlg.DoModal() == IDOK) {
		m_btn_TrgEnd.SetCaption(dlg.GetstrNum());
		UpdatePara(FALSE);
	}
	else {
		return;
	}
}

void CDialogSystem::SetFilePath()
{
	//CPMInspectDlg * pdlg = (CPMInspectDlg *)AfxGetApp()->m_pMainWnd;

	//CString strTemp;
	//strTemp.Format(_T("%S"), pdlg->GetCurrentModelPath());

	m_Paramanager.SetPath(DATA_SYSTEM_PATH, _T("Trigger.ini"));
}

void CDialogSystem::SetLinkPara()
{
	//CPMInspectDlg * pdlg = (CPMInspectDlg *)AfxGetApp()->m_pMainWnd;

	//CString strTemp;
	//strTemp.Format(_T("%S"), pdlg->GetCurrentModelPath());

	m_Paramanager.SetPath(DATA_SYSTEM_PATH, _T("Trigger.ini"));

	m_Paramanager.LinkPara(_T("TRIGGER_PARAMETER"), _T("CYCLE"), &m_TrgPara.m_nCycle, _T("0"));
	m_Paramanager.LinkPara(_T("TRIGGER_PARAMETER"), _T("ENABLE"), &m_TrgPara.m_nEnable, _T("0"));
	m_Paramanager.LinkPara(_T("TRIGGER_PARAMETER"), _T("TIME"), &m_TrgPara.m_nTime, _T("0"));
	m_Paramanager.LinkPara(_T("TRIGGER_PARAMETER"), _T("NUMBER"), &m_TrgPara.m_nNumber, _T("0"));
	m_Paramanager.LinkPara(_T("TRIGGER_PARAMETER"), _T("START"), &m_TrgPara.m_nStart, _T("0"));
	m_Paramanager.LinkPara(_T("TRIGGER_PARAMETER"), _T("END"), &m_TrgPara.m_nEnd, _T("0"));

}

void CDialogSystem::UpdatePara(BOOL bShow)
{
	CString strTemp;

	if (bShow)
	{
		strTemp.Format(_T("%d"), m_TrgPara.m_nCycle);
		m_btn_Cycle.SetCaption(strTemp);

		strTemp.Format(_T("%d"), m_TrgPara.m_nTime);
		m_btn_TrgTime.SetCaption(strTemp);

		strTemp.Format(_T("%d"), m_TrgPara.m_nEnable);
		m_btn_TrgEnbl.SetCaption(strTemp);

		strTemp.Format(_T("%d"), m_TrgPara.m_nNumber);
		m_btn_TrgNo.SetCaption(strTemp);

		strTemp.Format(_T("%d"), m_TrgPara.m_nStart);
		m_btn_TrgStart.SetCaption(strTemp);

		strTemp.Format(_T("%d"), m_TrgPara.m_nEnd);
		m_btn_TrgEnd.SetCaption(strTemp);
	}
	else
	{
		strTemp = m_btn_Cycle.GetCaption();
		m_TrgPara.m_nCycle = _ttoi(strTemp);

		strTemp = m_btn_TrgTime.GetCaption();
		m_TrgPara.m_nTime = _ttoi(strTemp);

		strTemp = m_btn_TrgEnbl.GetCaption();
		m_TrgPara.m_nEnable = _ttoi(strTemp);

		strTemp = m_btn_TrgNo.GetCaption();
		m_TrgPara.m_nNumber = _ttoi(strTemp);

		strTemp = m_btn_TrgStart.GetCaption();
		m_TrgPara.m_nStart = _ttoi(strTemp);

		strTemp = m_btn_TrgEnd.GetCaption();
		m_TrgPara.m_nEnd = _ttoi(strTemp);
	}
}

void CDialogSystem::ClickBtnenhctrlSystemButtonApply()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_Paramanager.SavePara();
}


void CDialogSystem::ClickBtnenhctrlSystemButtonOk()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	EndDialog(0);
}



void CDialogSystem::ClickBtnenhctrlSystemButtonReaddata()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (m_btn_ReadData.GetValue())
	{
		SetTimer(0, 10, NULL);
	}
	else
	{
		KillTimer(0);
	}
}


void CDialogSystem::ClickBtnenhctrlSystemButtonCntReset2()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	int port;
	CString url;

	struct data_form
	{
		WORD readwrite;     // read : 0x5555, write : 0xAAAA
		WORD address;       // address : 0 ~ 65535
		DWORD dataA;
	};

	struct data_form send_data_form, receive_data_form;

	CString _tmp;
	_tmp = m_btn_Port.GetCaption();
	port = _ttoi(_tmp);

	url = m_btn_IP_Address.GetCaption();

	send_data_form.readwrite = 0xAAAA;
	send_data_form.address = X_COUNTER_RESET;
	send_data_form.dataA = 2; // 리셋을 걸고

	CSocket s;

	if (!s.Create())
	{
		AfxMessageBox(_T("It is failed to create Socket"));
		return;
	}

	if (!s.Connect(url, port))
	{
		AfxMessageBox(_T("It is failed to connect to Server."));
		return;
	}

	s.Send(&send_data_form, sizeof(struct data_form), 0); // 전송 구조체를 통째로 전송한다. Reset
	int nByteRead;
	nByteRead = s.Receive(&receive_data_form, sizeof(struct data_form), 0); // FPGA로 부터 날아온 데이터를 구조체에 저장한다.
																			// nByteRead : 몇개의 바이트가 수신되었는지 표시한다.
	ZeroMemory(&receive_data_form, sizeof(struct data_form)); // 메모리를 비운다.

	Sleep(1);
	send_data_form.dataA = 0; // 리셋 해제
	s.Send(&send_data_form, sizeof(struct data_form), 0); // 전송 구조체를 통째로 전송한다. Reset 해제

	nByteRead = s.Receive(&receive_data_form, sizeof(struct data_form), 0); // FPGA로 부터 날아온 데이터를 구조체에 저장한다.
																			// nByteRead : 몇개의 바이트가 수신되었는지 표시한다.
	ZeroMemory(&receive_data_form, sizeof(struct data_form)); // 메모리를 비운다
}

void CDialogSystem::SetTriggerInit()
{
	ClickBtnenhctrlSystemButtonCntReset();
	ClickBtnenhctrlSystemButtonCntReset2();
}