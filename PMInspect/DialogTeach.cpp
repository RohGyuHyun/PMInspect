// DialogSetup.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "PMInspect.h"
#include "PMInspectDlg.h"
#include "DialogTeach.h"
#include "afxdialogex.h"
#include "DialogDispAlign.h"


// CDialogTeach 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDialogTeach, CDialogEx)

CDialogTeach::CDialogTeach(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_TEACH, pParent)
	, m_edit_nRectXIdx(1)
	, m_edit_nRectYIdx(1)
	, m_edit_byMainLightVal(0)
	, m_chk_InspImgSave(FALSE)
	, m_chk_InspFilterView(FALSE)
	, m_chk_InspImageView(FALSE)
	, m_chk_AutoInspRectTeaching(FALSE)
	, m_chk_bInspection(FALSE)
{
	for (int i = 0; i < MAX_INSP_DOUBLE_PARA_CNT; i++)
		m_edit_dInsPara[i] = 0.;


#ifdef ENABLE_LIGHT
		m_pSerialComm = new CSerialPort;
#endif
		m_pDlgTeachAlign = new	CDialogTeachAlign;
}

CDialogTeach::~CDialogTeach()
{
#ifdef ENABLE_LIGHT
	if (NULL != m_pSerialComm)
	{
		m_pSerialComm->Close();

		delete m_pSerialComm;
		m_pSerialComm = NULL;
	}
#endif
	if (m_pDlgTeachAlign != NULL)
	{
		delete	m_pDlgTeachAlign;
		m_pDlgTeachAlign = NULL;
	}
}

void CDialogTeach::DoDataExchange(CDataExchange* pDX)
{

	for (int i = 0; i < MAX_INSP_DOUBLE_PARA_CNT - 6; i++)
		DDX_Text(pDX, IDC_EDIT_INSPECTION_PARA0 + i, m_edit_dInsPara[i]);

	DDX_Text(pDX, IDC_EDIT_INSPECTION_PARA22, m_edit_dInsPara[MAX_INSP_DOUBLE_PARA_CNT - 6]);
	DDX_Text(pDX, IDC_EDIT_INSPECTION_PARA23, m_edit_dInsPara[MAX_INSP_DOUBLE_PARA_CNT - 5]);
	DDX_Text(pDX, IDC_EDIT_INSPECTION_PARA24, m_edit_dInsPara[MAX_INSP_DOUBLE_PARA_CNT - 4]);
	DDX_Text(pDX, IDC_EDIT_INSPECTION_PARA25, m_edit_dInsPara[MAX_INSP_DOUBLE_PARA_CNT - 3]);
	DDX_Text(pDX, IDC_EDIT_INSPECTION_PARA26, m_edit_dInsPara[MAX_INSP_DOUBLE_PARA_CNT - 2]);
	DDX_Text(pDX, IDC_EDIT_INSPECTION_PARA27, m_edit_dInsPara[MAX_INSP_DOUBLE_PARA_CNT - 1]);

	DDX_Text(pDX, IDC_EDIT_INSPECTION_RECT_X_IDX, m_edit_nRectXIdx);
	DDX_Text(pDX, IDC_EDIT_INSPECTION_RECT_Y_IDX, m_edit_nRectYIdx);
	DDX_Text(pDX, IDC_EDIT_MAIN_LIGHT, m_edit_byMainLightVal);
	DDX_Control(pDX, IDC_SLIDER_MAIN_LIGHT, m_slider_main_light);
	CDialogEx::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_PARA_0, m_chk_InspImgSave);
	DDX_Check(pDX, IDC_CHECK_PARA_1, m_chk_InspFilterView);
	DDX_Check(pDX, IDC_CHECK_PARA_2, m_chk_InspImageView);
	DDX_Control(pDX, IDC_COMBO_PM_TYPE, m_combo_PMType);
	DDX_Check(pDX, IDC_CHECK_PARA_3, m_chk_AutoInspRectTeaching);
	DDX_Check(pDX, IDC_CHECK_PARA_4, m_chk_bInspection);
}


BEGIN_MESSAGE_MAP(CDialogTeach, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_WM_PAINT()
	ON_WM_HSCROLL()
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CDialogTeach 메시지 처리기입니다.
BOOL CDialogTeach::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
#ifdef ENABLE_LIGHT
	m_pSerialComm->Open(3);
#endif
	m_slider_main_light.SetRange(0, 255);
	m_slider_main_light.SetRangeMin(0);
	m_slider_main_light.SetRangeMax(255);
	m_slider_main_light.SetPos(0);
	m_slider_main_light.SetTicFreq(10);
	m_slider_main_light.SetLineSize(1);

	m_pDlgTeachAlign->Create(IDD_DIALOG_TEACH_ALIGN, this);
	m_pDlgTeachAlign->ShowWindow(SW_HIDE);

	m_combo_PMType.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}




void CDialogTeach::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	if (bShow)
	{
		UpdatePara(FALSE);
		//20180917 ngh
		//((CPMInspectDlg*)GetParent())->m_pDlgDispMain->InitRslt();
		((CPMInspectDlg*)GetParent())->m_pDlgDispMain->SetUpdateInspRgn(TRUE);
		((CPMInspectDlg*)GetParent())->m_pDlgDispMain->SetSelectRect(m_edit_nRectXIdx - 1, m_edit_nRectYIdx - 1);
	}
	else
	{
		((CPMInspectDlg*)GetParent())->m_pDlgDispMain->InitRslt();
		((CPMInspectDlg*)GetParent())->m_pDlgDispMain->SetUpdateInspRgn(FALSE);
	}

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

void CDialogTeach::UpdatePara(BOOL isUpdate)
{
	//20180914 ngh
	int Ins_idx = 0, Sel_y = 0, i = 0;
	typeInspPara	InspPara;
	Ins_idx = m_edit_nRectXIdx - 1;
	Sel_y = m_edit_nRectYIdx - 1;

	InspPara = ((CPMInspectDlg*)GetParent())->m_pDlgDispMain->GetInspPara();
	if (isUpdate)
	{
		i = 0;
		UpdateData(TRUE);

		InspPara.dPixelSize = m_edit_dInsPara[i++];

		InspPara.InspRectRecipe[Ins_idx].StartRect.lRectTop = (LONG)m_edit_dInsPara[i++];
		InspPara.InspRectRecipe[Ins_idx].StartRect.lRectLeft = (LONG)m_edit_dInsPara[i++];
		InspPara.InspRectRecipe[Ins_idx].StartRect.lRectBottom = InspPara.InspRectRecipe[Ins_idx].StartRect.lRectTop + (LONG)m_edit_dInsPara[i++];// +(LONG)m_edit_dInsPara[19];
		InspPara.InspRectRecipe[Ins_idx].StartRect.lRectRight = InspPara.InspRectRecipe[Ins_idx].StartRect.lRectLeft + (LONG)m_edit_dInsPara[i++];

		InspPara.InspRectRecipe[Ins_idx].EndRect.lRectTop = (LONG)m_edit_dInsPara[i++];
		InspPara.InspRectRecipe[Ins_idx].EndRect.lRectLeft = (LONG)m_edit_dInsPara[i++];
		InspPara.InspRectRecipe[Ins_idx].EndRect.lRectBottom = InspPara.InspRectRecipe[Ins_idx].EndRect.lRectTop + (LONG)m_edit_dInsPara[i++];
		InspPara.InspRectRecipe[Ins_idx].EndRect.lRectRight = InspPara.InspRectRecipe[Ins_idx].EndRect.lRectLeft + (LONG)m_edit_dInsPara[i++];

		InspPara.ImageFilterPara[Ins_idx].nIllumiSize = (LONG)m_edit_dInsPara[i++];
		InspPara.ImageFilterPara[Ins_idx].nMeanSize = (LONG)m_edit_dInsPara[i++];

		InspPara.PMSearchPara[Ins_idx].nNoiseSize[0] = (LONG)m_edit_dInsPara[i++];
		InspPara.PMSearchPara[Ins_idx].nNoiseSize[1] = (LONG)m_edit_dInsPara[i++];
		InspPara.PMSearchPara[Ins_idx].nNoiseSize[2] = (LONG)m_edit_dInsPara[i++];
		InspPara.PMSearchPara[Ins_idx].nMargeDistance = (LONG)m_edit_dInsPara[i++];

		InspPara.PMSearchPara[Ins_idx].nThreshHold[0] = (LONG)m_edit_dInsPara[i++];
		InspPara.PMSearchPara[Ins_idx].nThreshHold[1] = (LONG)m_edit_dInsPara[i++];

		InspPara.nInspTypeCnt = (LONG)m_edit_dInsPara[i++];
		InspPara.InspRectRecipe[Ins_idx].nYRectCnt = (LONG)m_edit_dInsPara[i++];
		InspPara.InspRectRecipe[Ins_idx].nInspYSpace = (LONG)m_edit_dInsPara[i++];

		InspPara.InspRectRecipe[Ins_idx].InspRect[Sel_y].lRectTop = (LONG)m_edit_dInsPara[i++];
		InspPara.InspRectRecipe[Ins_idx].InspRect[Sel_y].lRectLeft = (LONG)m_edit_dInsPara[i++];
		InspPara.InspRectRecipe[Ins_idx].InspRect[Sel_y].lRectBottom = InspPara.InspRectRecipe[Ins_idx].InspRect[Sel_y].lRectTop + (LONG)m_edit_dInsPara[i++];// +(LONG)m_edit_dInsPara[19];
		InspPara.InspRectRecipe[Ins_idx].InspRect[Sel_y].lRectRight = InspPara.InspRectRecipe[Ins_idx].InspRect[Sel_y].lRectLeft + (LONG)m_edit_dInsPara[i++];
		InspPara.InspRectRecipe[Ins_idx].InspRect[Sel_y].lOffsetY = (LONG)m_edit_dInsPara[i++];
		InspPara.InspRectRecipe[Ins_idx].InspRect[Sel_y].lOffsetX = (LONG)m_edit_dInsPara[i++];

		InspPara.InspRectRecipe[Ins_idx].dLaplaceSigma = m_edit_dInsPara[i++];
		InspPara.ImageFilterPara[Ins_idx].dLaplaceSigma = m_edit_dInsPara[i++];

		InspPara.nLightVal[Ins_idx] = m_edit_byMainLightVal;

		InspPara.bInspImgSave = m_chk_InspImgSave;
		InspPara.bFilterImgView = m_chk_InspFilterView;
		InspPara.bInspImgView = m_chk_InspImageView;
		InspPara.bAutoInspRectTeaching = m_chk_AutoInspRectTeaching;
		InspPara.bInspection = m_chk_bInspection;

		InspPara.PMSearchPara[Ins_idx].nPmType = m_combo_PMType.GetCurSel();

		((CPMInspectDlg*)GetParent())->m_pDlgDispMain->SetInspPara(&InspPara);
	}
	else
	{
		i = 0;
		m_edit_dInsPara[i++] = InspPara.dPixelSize;

		m_edit_dInsPara[i++] = InspPara.InspRectRecipe[Ins_idx].StartRect.lRectTop;
		m_edit_dInsPara[i++] = InspPara.InspRectRecipe[Ins_idx].StartRect.lRectLeft;
		m_edit_dInsPara[i++] = InspPara.InspRectRecipe[Ins_idx].StartRect.lRectBottom - InspPara.InspRectRecipe[Ins_idx].StartRect.lRectTop;
		m_edit_dInsPara[i++] = InspPara.InspRectRecipe[Ins_idx].StartRect.lRectRight - InspPara.InspRectRecipe[Ins_idx].StartRect.lRectLeft;

		m_edit_dInsPara[i++] = InspPara.InspRectRecipe[Ins_idx].EndRect.lRectTop;
		m_edit_dInsPara[i++] = InspPara.InspRectRecipe[Ins_idx].EndRect.lRectLeft;
		m_edit_dInsPara[i++] = InspPara.InspRectRecipe[Ins_idx].EndRect.lRectBottom - InspPara.InspRectRecipe[Ins_idx].EndRect.lRectTop;
		m_edit_dInsPara[i++] = InspPara.InspRectRecipe[Ins_idx].EndRect.lRectRight - InspPara.InspRectRecipe[Ins_idx].EndRect.lRectLeft;

		m_edit_dInsPara[i++] = InspPara.ImageFilterPara[Ins_idx].nIllumiSize;
		m_edit_dInsPara[i++] = InspPara.ImageFilterPara[Ins_idx].nMeanSize;

		m_edit_dInsPara[i++] = InspPara.PMSearchPara[Ins_idx].nNoiseSize[0];
		m_edit_dInsPara[i++] = InspPara.PMSearchPara[Ins_idx].nNoiseSize[1];
		m_edit_dInsPara[i++] = InspPara.PMSearchPara[Ins_idx].nNoiseSize[2];
		m_edit_dInsPara[i++] = InspPara.PMSearchPara[Ins_idx].nMargeDistance;

		m_edit_dInsPara[i++] = InspPara.PMSearchPara[Ins_idx].nThreshHold[0];
		m_edit_dInsPara[i++] = InspPara.PMSearchPara[Ins_idx].nThreshHold[1];

		m_edit_dInsPara[i++] = InspPara.nInspTypeCnt;
		m_edit_dInsPara[i++] = InspPara.InspRectRecipe[Ins_idx].nYRectCnt;
		m_edit_dInsPara[i++] = InspPara.InspRectRecipe[Ins_idx].nInspYSpace;

		m_edit_dInsPara[i++] = InspPara.InspRectRecipe[Ins_idx].InspRect[Sel_y].lRectTop;
		m_edit_dInsPara[i++] = InspPara.InspRectRecipe[Ins_idx].InspRect[Sel_y].lRectLeft;
		m_edit_dInsPara[i++] = InspPara.InspRectRecipe[Ins_idx].InspRect[Sel_y].lRectBottom - InspPara.InspRectRecipe[Ins_idx].InspRect[Sel_y].lRectTop;
		m_edit_dInsPara[i++] = InspPara.InspRectRecipe[Ins_idx].InspRect[Sel_y].lRectRight - InspPara.InspRectRecipe[Ins_idx].InspRect[Sel_y].lRectLeft;
		m_edit_dInsPara[i++] = InspPara.InspRectRecipe[Ins_idx].InspRect[Sel_y].lOffsetY;
		m_edit_dInsPara[i++] = InspPara.InspRectRecipe[Ins_idx].InspRect[Sel_y].lOffsetX;

		m_edit_dInsPara[i++] = InspPara.InspRectRecipe[Ins_idx].dLaplaceSigma;
		m_edit_dInsPara[i++] = InspPara.ImageFilterPara[Ins_idx].dLaplaceSigma;

		m_edit_byMainLightVal = InspPara.nLightVal[Ins_idx];

		m_chk_InspImgSave = InspPara.bInspImgSave;
		m_chk_InspFilterView = InspPara.bFilterImgView;
		m_chk_InspImageView = InspPara.bInspImgView;
		m_chk_AutoInspRectTeaching = InspPara.bAutoInspRectTeaching;
		m_chk_bInspection = InspPara.bInspection;

		m_combo_PMType.SetCurSel(InspPara.PMSearchPara[Ins_idx].nPmType);


		UpdateData(FALSE);
	}
}


BEGIN_EVENTSINK_MAP(CDialogTeach, CDialogEx)
	ON_EVENT(CDialogTeach, IDC_BTNENHCTRL_INSP_REGION_X_LEFT, DISPID_CLICK, CDialogTeach::ClickBtnenhctrlRectXNumLeft, VTS_NONE)
	ON_EVENT(CDialogTeach, IDC_BTNENHCTRL_INSP_REGION_X_RIGHT, DISPID_CLICK, CDialogTeach::ClickBtnenhctrlRectXNumRight, VTS_NONE)
	ON_EVENT(CDialogTeach, IDC_BTNENHCTRL_INSP_REGION_Y_LEFT, DISPID_CLICK, CDialogTeach::ClickBtnenhctrlRectYNumLeft, VTS_NONE)
	ON_EVENT(CDialogTeach, IDC_BTNENHCTRL_INSP_REGION_Y_RIGHT, DISPID_CLICK, CDialogTeach::ClickBtnenhctrlRectYNumRight, VTS_NONE)
	ON_EVENT(CDialogTeach, IDC_BTNENHCTRL_SETUP_APPLY, DISPID_CLICK, CDialogTeach::ClickBtnenhctrlSetupApply, VTS_NONE)
	ON_EVENT(CDialogTeach, IDC_BTNENHCTRL_SETUP_TEST, DISPID_CLICK, CDialogTeach::ClickBtnenhctrlSetupTest, VTS_NONE)
	ON_EVENT(CDialogTeach, IDC_BTNENHCTRL_SETUP_ALIGN_TEST2, DISPID_CLICK, CDialogTeach::ClickBtnenhctrlSetupAlignTest2, VTS_NONE)
	ON_EVENT(CDialogTeach, IDC_BTNENHCTRL_SETUP_TEACHING_AREA, DISPID_CLICK, CDialogTeach::ClickBtnenhctrlSetupTeachingArea, VTS_NONE)
END_EVENTSINK_MAP()


void CDialogTeach::ClickBtnenhctrlRectXNumLeft()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	//int i = INSP_RECT_START_IDX;
	UpdateData(TRUE);

	m_edit_nRectXIdx--;
	RectValDisplay(m_edit_nRectXIdx, m_edit_nRectYIdx);

	UpdateData(FALSE);
}


void CDialogTeach::ClickBtnenhctrlRectXNumRight()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	//int i = INSP_RECT_START_IDX;

	UpdateData(TRUE);
	m_edit_nRectXIdx++;
	RectValDisplay(m_edit_nRectXIdx, m_edit_nRectYIdx);
	UpdateData(FALSE);
}


void CDialogTeach::ClickBtnenhctrlRectYNumLeft()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	//int i = INSP_RECT_START_IDX;
	UpdateData(TRUE);

	m_edit_nRectYIdx--;
	RectValDisplay(m_edit_nRectXIdx, m_edit_nRectYIdx);

	UpdateData(FALSE);
}


void CDialogTeach::ClickBtnenhctrlRectYNumRight()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	//int i = INSP_RECT_START_IDX;

	UpdateData(TRUE);
	m_edit_nRectYIdx++;
	RectValDisplay(m_edit_nRectXIdx, m_edit_nRectYIdx);

	UpdateData(FALSE);
}


void CDialogTeach::ClickBtnenhctrlSetupApply()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CPMInspectDlg *pdlg = (CPMInspectDlg*)AfxGetApp()->m_pMainWnd;
	UpdatePara(TRUE);
	((CPMInspectDlg*)GetParent())->m_pDlgDispMain->SaveModelData();

	if (m_pDlgTeachAlign->IsWindowVisible())
	{
		m_pDlgTeachAlign->ApplyParameter();
	}
}


void CDialogTeach::ClickBtnenhctrlSetupTest()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CPMInspectDlg *pdlg = (CPMInspectDlg*)AfxGetApp()->m_pMainWnd;

	((CPMInspectDlg*)GetParent())->m_pDlgDispMain->SetInspRgn(m_edit_nRectXIdx - 1);

}

void CDialogTeach::OnPaint()
{
	// device context for painting
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// 그리기 메시지에 대해서는 CDialogEx::OnPaint()을(를) 호출하지 마십시오.
	CPaintDC dc(this);

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


void CDialogTeach::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (pScrollBar)
	{

		// 어떤 슬라이더인지 검사
		if (pScrollBar == (CScrollBar*)&m_slider_main_light)
		{
#ifdef ENABLE_LIGHT
			WritePacket(LIGHT_SOURCE_MAIN, m_slider_main_light.GetPos());
#endif
			m_edit_byMainLightVal = m_slider_main_light.GetPos();
		}
		UpdateData(FALSE);
	}

	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CDialogTeach::WritePacket(int iType, BYTE byVal)
{
	char sPacket[MAX_PATH];
	memset(sPacket, 0, sizeof(char) * MAX_PATH);

	switch (iType)
	{
		case	LIGHT_SOURCE_MAIN:	sprintf_s(sPacket, "C1%02X%c%c\0", byVal, 0x0D, 0x0A);	break;
		case	LIGHT_SOURCE_ALIGN	:	sprintf_s(sPacket, "C2%02X%c%c\0", byVal, 0x0D, 0x0A);	break;
		default	:	return;
	}

	m_pSerialComm->Write(sPacket, (DWORD)strlen(sPacket));

	/*
		sprintf_s(m_sPacket,  "H%dON%c%c\0", ch, 0x0D,0x0A);	//ON
		sprintf_s(m_sPacket,  "H%dOF%c%c\0", s,ch, 0x0D,0x0A);	//OFF
	*/
}
	


void CDialogTeach::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CDialogEx::OnClose();
}


void CDialogTeach::ClickBtnenhctrlSetupAlignTest2()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_pDlgTeachAlign->ShowWindow(SW_SHOW);
	return;

	IMGALIGN_PTNDIFF	Diff;
	P_IMGALIGN			pAlign;

	pAlign = ((CPMInspectDlg*)GetParent())->m_pDispAlign;
	if (!pAlign->PatternMatch(1))
		return;
/*
	IMGALIGN_STATUS	Status;
	while (TRUE)
	{
		Status = pAlign->GetStatus();
		if (Status == IMGALIGN_STATUS_PATTERN_MATCH_FAILED)
			return;
		else if (Status == IMGALIGN_STATUS_PATTERN_MATCH_COMPLETE)
			break;
	}
*/
	if (pAlign->GetPtnDiff(1, &Diff))
	{
		CString	strTemp;
		strTemp.Format(_T("X_Diff = %.2f\nY_Diff = %.2f\nAngle_Diff = %.2f\nScale_Diff = %.2f\nScore = %.2f"),
			Diff.m_XCenter,
			Diff.m_YCenter,
			Diff.m_Angle,
			Diff.m_Scale,
			Diff.m_Score);
		AfxMessageBox(strTemp);
	}

	if (AfxMessageBox(_T("Test Calc Align?"), MB_YESNO) == IDYES)
	{
		DOUBLE	Angle, XDiff, YDiff;
		if (AfxMessageBox(_T("XY Only?"), MB_YESNO) == IDYES)
		{
			if (pAlign->GetAlignCompPos(
				((CPMInspectDlg*)GetParent())->m_pDlgMotion->GetAxisPos(AXIS_0, 0),
				((CPMInspectDlg*)GetParent())->m_pDlgMotion->GetAxisPos(AXIS_0, 1),
				((CPMInspectDlg*)GetParent())->m_pDlgMotion->GetAxisPos(AXIS_1, 0),
				((CPMInspectDlg*)GetParent())->m_pDlgMotion->GetAxisPos(AXIS_1, 1),
				&Angle, &XDiff, &YDiff, NULL, TRUE, TRUE))
			{
				CString	strTemp;
				strTemp.Format(_T("Comp_Angle = %.2f, Comp_X = %.2f, Comp_Y = %.2f"),
					Angle,
					XDiff,
					YDiff);
				AfxMessageBox(strTemp);
			}
		}
		else
		{
			if (pAlign->GetAlignCompPos(
				((CPMInspectDlg*)GetParent())->m_pDlgMotion->GetAxisPos(AXIS_0, 0),
				((CPMInspectDlg*)GetParent())->m_pDlgMotion->GetAxisPos(AXIS_0, 1),
				((CPMInspectDlg*)GetParent())->m_pDlgMotion->GetAxisPos(AXIS_1, 0),
				((CPMInspectDlg*)GetParent())->m_pDlgMotion->GetAxisPos(AXIS_1, 1),
				&Angle, &XDiff, &YDiff, NULL, FALSE, TRUE))
			{
				CString	strTemp;
				strTemp.Format(_T("Comp_Angle = %.2f, Comp_X = %.2f, Comp_Y = %.2f"),
					Angle,
					XDiff,
					YDiff);
				AfxMessageBox(strTemp);
			}
		}
	}
}

//20180914 ngh
void CDialogTeach::RectValDisplay(int Insp_idx, int y_idx)
{
	typeInspPara InspPara;
	InspPara = ((CPMInspectDlg*)GetParent())->m_pDlgDispMain->GetInspPara();

	if (Insp_idx >= InspPara.nInspTypeCnt)
	{
		Insp_idx = InspPara.nInspTypeCnt;
		m_edit_nRectXIdx = Insp_idx;
	}

	if (Insp_idx <= 1)
	{
		Insp_idx = 1;
		m_edit_nRectXIdx = Insp_idx;
	}

	if (y_idx >= InspPara.InspRectRecipe[Insp_idx - 1].nYRectCnt)
	{
		y_idx = InspPara.InspRectRecipe[Insp_idx - 1].nYRectCnt;
		m_edit_nRectYIdx = y_idx;
	}

	if (y_idx <= 1)
	{
		y_idx = 1;
		m_edit_nRectYIdx = y_idx;
	}

	int i = 1;
	m_edit_dInsPara[i++] = InspPara.InspRectRecipe[Insp_idx - 1].StartRect.lRectTop;
	m_edit_dInsPara[i++] = InspPara.InspRectRecipe[Insp_idx - 1].StartRect.lRectLeft;
	m_edit_dInsPara[i++] = InspPara.InspRectRecipe[Insp_idx - 1].StartRect.lRectBottom - InspPara.InspRectRecipe[Insp_idx - 1].StartRect.lRectTop;
	m_edit_dInsPara[i++] = InspPara.InspRectRecipe[Insp_idx - 1].StartRect.lRectRight - InspPara.InspRectRecipe[Insp_idx - 1].StartRect.lRectLeft;

	m_edit_dInsPara[i++] = InspPara.InspRectRecipe[Insp_idx - 1].EndRect.lRectTop;
	m_edit_dInsPara[i++] = InspPara.InspRectRecipe[Insp_idx - 1].EndRect.lRectLeft;
	m_edit_dInsPara[i++] = InspPara.InspRectRecipe[Insp_idx - 1].EndRect.lRectBottom - InspPara.InspRectRecipe[Insp_idx - 1].EndRect.lRectTop;
	m_edit_dInsPara[i++] = InspPara.InspRectRecipe[Insp_idx - 1].EndRect.lRectRight - InspPara.InspRectRecipe[Insp_idx - 1].EndRect.lRectLeft;

	m_edit_dInsPara[i++] = InspPara.ImageFilterPara[Insp_idx - 1].nIllumiSize;
	m_edit_dInsPara[i++] = InspPara.ImageFilterPara[Insp_idx - 1].nMeanSize;
	m_edit_dInsPara[i++] = InspPara.PMSearchPara[Insp_idx - 1].nNoiseSize[0];
	m_edit_dInsPara[i++] = InspPara.PMSearchPara[Insp_idx - 1].nNoiseSize[1];
	m_edit_dInsPara[i++] = InspPara.PMSearchPara[Insp_idx - 1].nNoiseSize[2];
	m_edit_dInsPara[i++] = InspPara.PMSearchPara[Insp_idx - 1].nMargeDistance;
	m_edit_dInsPara[i++] = InspPara.PMSearchPara[Insp_idx - 1].nThreshHold[0];
	m_edit_dInsPara[i++] = InspPara.PMSearchPara[Insp_idx - 1].nThreshHold[1];
	i++;
	m_edit_dInsPara[i++] = InspPara.InspRectRecipe[Insp_idx - 1].nYRectCnt;
	m_edit_dInsPara[i++] = InspPara.InspRectRecipe[Insp_idx - 1].nInspYSpace;
	m_edit_dInsPara[i++] = InspPara.InspRectRecipe[Insp_idx - 1].InspRect[y_idx - 1].lRectTop;
	m_edit_dInsPara[i++] = InspPara.InspRectRecipe[Insp_idx - 1].InspRect[y_idx - 1].lRectLeft;
	m_edit_dInsPara[i++] = InspPara.InspRectRecipe[Insp_idx - 1].InspRect[y_idx - 1].lRectBottom - InspPara.InspRectRecipe[Insp_idx - 1].InspRect[y_idx - 1].lRectTop;
	m_edit_dInsPara[i++] = InspPara.InspRectRecipe[Insp_idx - 1].InspRect[y_idx - 1].lRectRight - InspPara.InspRectRecipe[Insp_idx - 1].InspRect[y_idx - 1].lRectLeft;
	m_edit_dInsPara[i++] = InspPara.InspRectRecipe[Insp_idx - 1].InspRect[y_idx - 1].lOffsetY;
	m_edit_dInsPara[i++] = InspPara.InspRectRecipe[Insp_idx - 1].InspRect[y_idx - 1].lOffsetX;

	UpdateData(FALSE);

	((CPMInspectDlg*)GetParent())->m_pDlgDispMain->SetInspType(Insp_idx - 1);
	((CPMInspectDlg*)GetParent())->m_pDlgDispMain->SetUpdateInspRgn(TRUE);
	((CPMInspectDlg*)GetParent())->m_pDlgDispMain->SetUpdateSelectRect(TRUE);
	((CPMInspectDlg*)GetParent())->m_pDlgDispMain->SetSelectRect(Insp_idx - 1, y_idx - 1);
}

//20180917 ngh
void CDialogTeach::SetMainLight(int nVal)
{
	WritePacket(LIGHT_SOURCE_MAIN, nVal);
}

void CDialogTeach::ClickBtnenhctrlSetupTeachingArea()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	((CPMInspectDlg*)GetParent())->m_pDlgDispMain->SetInspRgn(m_edit_nRectXIdx - 1);
	UpdatePara(FALSE);
}
