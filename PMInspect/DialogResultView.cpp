// DialogResultView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "PMInspect.h"
#include "DialogResultView.h"
#include "DialogDispMain.h"
#include "DlgResultList.h"
#include "PMInspectDlg.h"
#include "afxdialogex.h"


// CDialogResultView 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDialogResultView, CDialogEx)

CDialogResultView::CDialogResultView(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_RESULT_VIEW, pParent)
	, m_IdxDispImg(0)
	, m_bDisplayRegion(FALSE)
{
	m_pDlgImgRect = NULL;
	m_SelectedIdx = 0;
	m_ResultData.Release();
}

CDialogResultView::~CDialogResultView()
{
	Release();
}

void CDialogResultView::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_RESULT, m_ListResult);
	DDX_Radio(pDX, IDC_RADIO_RESULT_GRAB, m_IdxDispImg);
	DDX_Check(pDX, IDC_CHECK_RESULT_REGION, m_bDisplayRegion);
	DDX_Control(pDX, IDC_LIST_DESCRIPT_RESULT, m_ListDescResult);
}


BEGIN_MESSAGE_MAP(CDialogResultView, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_WM_MOVE()
	ON_WM_CLOSE()
	ON_MESSAGE(USER_MSG_IMG_RECT, OnImageRectMessage)
	ON_BN_CLICKED(IDC_RADIO_RESULT_GRAB, &CDialogResultView::OnBnClickedRadioResultGrab)
	ON_BN_CLICKED(IDC_RADIO_RESULT_FILTER, &CDialogResultView::OnBnClickedRadioResultFilter)
	ON_BN_CLICKED(IDC_CHECK_RESULT_REGION, &CDialogResultView::OnBnClickedCheckResultRegion)
	ON_BN_CLICKED(IDC_BTN_DLGRESULVIEW_OK, &CDialogResultView::OnBnClickedBtnDlgresulviewOk)
	ON_BN_CLICKED(IDC_BTN_DLGRESULVIEW_OTHER, &CDialogResultView::OnBnClickedBtnDlgresulviewOther)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_RESULT, &CDialogResultView::OnNMDblclkListResult)
	ON_NOTIFY(NM_RETURN, IDC_LIST_RESULT, &CDialogResultView::OnNMReturnListResult)
END_MESSAGE_MAP()


// CDialogResultView 메시지 처리기입니다.


BOOL CDialogResultView::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_ListResult.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_ListResult.InsertColumn(0, _T("Idx"), LVCFMT_LEFT, 100);
	m_ListResult.InsertColumn(1, _T("Result"), LVCFMT_LEFT, 200);

	m_ListDescResult.InsertColumn(0, _T("Item"), LVCFMT_LEFT, 100);
	m_ListDescResult.InsertColumn(1, _T("Descript"), LVCFMT_LEFT, 200);
	m_ListDescResult.InsertColumn(1, _T("Descript"), LVCFMT_LEFT, 200);
	m_ListDescResult.InsertItem(0, _T("Inspect Index"));
	m_ListDescResult.InsertItem(1, _T("Final Result"));
	m_ListDescResult.InsertItem(2, _T("Dent Count"));
	m_ListDescResult.InsertItem(3, _T("Dent Area Shift"));
	m_ListDescResult.InsertItem(4, _T("Dent Distribution"));
	m_ListDescResult.InsertItem(5, _T("Dent Size"));
	Invalidate();

	//image Dialog
	if (m_pDlgImgRect == NULL)
		delete	m_pDlgImgRect;
	m_pDlgImgRect = new C_CtrlImageRect;
	m_pDlgImgRect->Create(this);
	m_pDlgImgRect->SetCamIndex(0, _T("RESULT DISPLAY"), this->m_hWnd);
	m_pDlgImgRect->EnableUI(C_CtrlImageRect_UI_LIVE, FALSE);
	m_pDlgImgRect->EnableUI(C_CtrlImageRect_UI_GRID, FALSE);
	m_pDlgImgRect->EnableUI(C_CtrlImageRect_UI_GENERAL, FALSE);
	m_pDlgImgRect->ShowUI(C_CtrlImageRect_UI_LIVE, FALSE);
	m_pDlgImgRect->ShowUI(C_CtrlImageRect_UI_GRID, FALSE);
	m_pDlgImgRect->ShowUI(C_CtrlImageRect_UI_GENERAL, FALSE);

	CRect rect;
	GetDlgItem(IDC_STATIC_RESULT_DISPLAY)->GetWindowRect(&rect);
	m_pDlgImgRect->MoveWindow(rect.left, rect.top, rect.Width(), rect.Height(), TRUE);

	m_pDlgImgRect->ShowWindow(SW_HIDE);	//IMSI
	//m_pDlgImgRect->ShowWindow(SW_SHOW);

	//	Cam이 아닐 경우에 대한 셋팅도 필요
	m_ResultData.m_Header.m_ImageWidth = g_MilCtrl.GetCameraWidth();
	m_ResultData.m_Header.m_ImageHeight = g_MilCtrl.GetCameraHeight();
	m_pDlgImgRect->SetZoom(0, m_ResultData.m_Header.m_ImageWidth, m_ResultData.m_Header.m_ImageHeight);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

VOID	CDialogResultView::Release()
{
	ResetData();

	if (m_ListDescResult.m_hWnd != NULL)
		m_ListDescResult.DeleteAllItems();

	if (m_pDlgImgRect != NULL)
	{
		delete	m_pDlgImgRect;
		m_pDlgImgRect = NULL;
	}

	m_ResultData.Release();
	m_strModelDir.Empty();
}

BOOL	CDialogResultView::ChangeModel(LPCTSTR strModelDir)
{
	ResetData();

	if (strModelDir == NULL)
	{
		m_strModelDir.Empty();
		return	FALSE;
	}

	if (_tcslen(strModelDir) == 0)
	{
		m_strModelDir.Empty();
		return	FALSE;
	}

	m_strModelDir = strModelDir;

	return	TRUE;
}

BOOL	CDialogResultView::SaveAndViewResultData(LPSYSTEMTIME pSysTime, LPCTSTR strID/*NULL*/)
{
	if (m_strModelDir.GetLength() <= 0 || pSysTime == NULL)
	{
		AfxMessageBox(_T("Result Data Error!"));
		return	FALSE;
	}

	CString	strDir, strFileName;

	if (strID == NULL)
	{
		strFileName.Format(_T("%02d-%02d-%02d"), pSysTime->wHour, pSysTime->wMinute, pSysTime->wSecond);
	}
	else if (_tcslen(strID) == 0)
	{
		strFileName.Format(_T("%02d-%02d-%02d"), pSysTime->wHour, pSysTime->wMinute, pSysTime->wSecond);
	}
	else
	{
		strFileName = strID;
	}

	strDir.Format(_T("%s\\%s\\%04d\\%02d\\%02d\\%s"),
		m_strModelDir,
		DLGRESULTVIEW_FILEFORMAT_STR_RESULT_FOLDERNAME,
		pSysTime->wYear, pSysTime->wMonth, pSysTime->wDay,
		strFileName);

	if (!MakeDirectories(strDir))
	{
		AfxMessageBox(_T("Result Directory Create Failed!"));
		return	FALSE;
	}

	if (!SaveData(strDir, strFileName))
	{
		RemoveFileSRecurse(strDir, -1);
		RemoveDirectory(strDir);
		AfxMessageBox(_T("Result Data Save Failed!"));
		return	FALSE;
	}

	ShowWindow(SW_SHOW);

	return	TRUE;
}

VOID	CDialogResultView::ResetData()
{
	m_ResultData.m_Header.m_CountResultArea = 0;

	for( int i = 0 ; i < DLGRESULTVIEW_IMG_NUM_TYPE ; i++ )
		m_ImageResult[i].Reset();

	for (int i = 0; i < DLGRESULTVIEW_REGION_NUM_TYPE; i++)
		m_Region[i].Reset();

	if (m_ListResult.m_hWnd != NULL)
		m_ListResult.DeleteAllItems();

	if (m_ListDescResult.m_hWnd != NULL)
	{
		for (int i = 0; i < 6; i++)
			m_ListDescResult.SetItemText(i, 1, NULL);
	}

	m_SelectedIdx = 0;
	m_strID.Empty();
}

BOOL	CDialogResultView::SaveData(LPCTSTR strDir, LPCTSTR strFileName)
{
	if (strDir == NULL || strFileName == NULL)
		return	FALSE;

	if (m_ResultData.m_pResultData == NULL || m_ResultData.m_Header.m_CountResultArea == 0)
		return	FALSE;

	CString	strTmp;

	strTmp.Format(_T("%s\\%s.%s"), strDir, strFileName, DLGRESULTVIEW_FILEFORMAT_STR_RESULT_DATAFORMAT);

	HANDLE	hFile = CreateFile(strTmp, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, NULL, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
		return	FALSE;

	DWORD	dwSize;
	if (!WriteFile(hFile, &m_ResultData.m_Header, sizeof(DLGRESULTVIEW_FILEFORMAT_HEADER), &dwSize, NULL))
	{
		CloseHandle(hFile);
		return	FALSE;
	}

	if( dwSize < sizeof(DLGRESULTVIEW_FILEFORMAT_HEADER) )
	{
		CloseHandle(hFile);
		return	FALSE;
	}

	if (!WriteFile(hFile, m_ResultData.m_pResultData, sizeof(DLGRESULTVIEW_DATA)*m_ResultData.m_Header.m_CountResultArea, &dwSize, NULL))
	{
		CloseHandle(hFile);
		return	FALSE;
	}

	if (dwSize < sizeof(DLGRESULTVIEW_DATA)*m_ResultData.m_Header.m_CountResultArea)
	{
		CloseHandle(hFile);
		return	FALSE;
	}

	CloseHandle(hFile);

	for (int i = 0; i < DLGRESULTVIEW_IMG_NUM_TYPE; i++)
	{
		strTmp.Format(_T("%s\\%s_%d.%s"), strDir, strFileName, i, DLGRESULTVIEW_FILEFORMAT_STR_RESULT_IMGFORMAT);
		write_image(m_ImageResult[i], CT2A(DLGRESULTVIEW_FILEFORMAT_STR_RESULT_IMGFORMAT), 0, CT2A(strTmp));
	}

	for (int i = 0; i < DLGRESULTVIEW_REGION_NUM_TYPE; i++)
	{
		strTmp.Format(_T("%s\\%s_%d.%s"), strDir, strFileName, i, DLGRESULTVIEW_FILEFORMAT_STR_RESULT_RGNFORMAT);
		write_region(m_Region[i], CT2A(strTmp));
	}

	return	TRUE;
}

BOOL	CDialogResultView::LoadData(LPCTSTR strDir/*NULL*/, LPCTSTR strFileName/*NULL*/)
{
	ResetData();

	if (strDir == NULL)	//	Cam으로부터
	{
		m_ResultData.m_Header.m_ImageWidth = g_MilCtrl.GetCameraWidth();
		m_ResultData.m_Header.m_ImageHeight = g_MilCtrl.GetCameraHeight();
		m_pDlgImgRect->SetZoom(0, m_ResultData.m_Header.m_ImageWidth, m_ResultData.m_Header.m_ImageHeight);

		long	InspIdx;

		CVisionModule*	pVisionModule = ((CPMInspectDlg*)(GetParent()))->m_pDlgDispMain->GetVisionModule();
		if (pVisionModule == NULL)
		{
			ResetData();
			return	FALSE;
		}

		InspIdx = pVisionModule->GetInspIdx();

		if (pVisionModule->GetGrabImage() == NULL)
		{
			ResetData();
			return	FALSE;
		}

		copy_image(*pVisionModule->GetGrabImage(), &m_ImageResult[DLGRESULTVIEW_IMG_TYPE_GRAB]);

		if (pVisionModule->GetFilterImage() == NULL)
		{
			ResetData();
			return	FALSE;
		}

		copy_image(*pVisionModule->GetFilterImage(), &m_ImageResult[DLGRESULTVIEW_IMG_TYPE_FILTER]);

		copy_obj(pVisionModule->GetInspRgn(InspIdx), &m_Region[DLGRESULTVIEW_REGION_TYPE_AREA], 1, -1);
		copy_obj(pVisionModule->GetInspRsltRgn(InspIdx), &m_Region[DLGRESULTVIEW_REGION_TYPE_DENT], 1, -1);

		typeInspPara InspParam = pVisionModule->GetInspPara();

		m_ResultData.m_Header.m_CountResultArea = InspParam.InspRectRecipe[InspIdx].nYRectCnt;

		m_ResultData.m_pResultData = new	DLGRESULTVIEW_DATA[m_ResultData.m_Header.m_CountResultArea];
		ZeroMemory(m_ResultData.m_pResultData, sizeof(DLGRESULTVIEW_DATA)*m_ResultData.m_Header.m_CountResultArea);

		typeInsRect	*pInsRect;
		for (int i = 0; i < m_ResultData.m_Header.m_CountResultArea; i++)
		{
			pInsRect = &InspParam.InspRectRecipe[InspIdx].InspRect[i];
			m_ResultData.m_pResultData[i].m_InspArea.left = pInsRect->lRectLeft;
			m_ResultData.m_pResultData[i].m_InspArea.top = pInsRect->lRectTop;
			m_ResultData.m_pResultData[i].m_InspArea.right = pInsRect->lRectRight;
			m_ResultData.m_pResultData[i].m_InspArea.bottom = pInsRect->lRectBottom;

			m_ResultData.m_pResultData[i].m_CntDent = pVisionModule->GetPmEachRsltCnt(i);
			m_ResultData.m_pResultData[i].m_IsGood = FALSE;
			m_ResultData.m_pResultData[i].m_Result = pVisionModule->GetPMEachRslt(i);
			if (m_ResultData.m_pResultData[i].m_Result == 0 )
			{
				m_ResultData.m_pResultData[i].m_IsGood = TRUE;
			}
		}
	}
	else
	{
		if (strFileName == NULL)
			return	FALSE;
		if (_tcslen(strDir) <= 0 || _tcslen(strFileName) <= 0)
			return	FALSE;

		CString	strTmp;
		strTmp.Format(_T("%s\\%s.%s"), strDir, strFileName, DLGRESULTVIEW_FILEFORMAT_STR_RESULT_DATAFORMAT);

		HANDLE	hFile = CreateFile(strTmp, GENERIC_READ, 0, NULL, OPEN_EXISTING, NULL, NULL);

		if (hFile == INVALID_HANDLE_VALUE)
			return	FALSE;

		DWORD	dwSize;
		if (!ReadFile(hFile, &m_ResultData.m_Header, sizeof(DLGRESULTVIEW_FILEFORMAT_HEADER), &dwSize, NULL))
		{
			CloseHandle(hFile);
			ResetData();
			return	FALSE;
		}

		if (dwSize < sizeof(DLGRESULTVIEW_FILEFORMAT_HEADER))
		{
			CloseHandle(hFile);
			ResetData();
			return	FALSE;
		}

		m_ResultData.m_pResultData = new	DLGRESULTVIEW_DATA[m_ResultData.m_Header.m_CountResultArea];
		ZeroMemory(m_ResultData.m_pResultData, sizeof(DLGRESULTVIEW_DATA)*m_ResultData.m_Header.m_CountResultArea);

		if (!ReadFile(hFile, m_ResultData.m_pResultData, sizeof(DLGRESULTVIEW_DATA)*m_ResultData.m_Header.m_CountResultArea, &dwSize, NULL))
		{
			CloseHandle(hFile);
			ResetData();
			return	FALSE;
		}

		if (dwSize < sizeof(DLGRESULTVIEW_DATA)*m_ResultData.m_Header.m_CountResultArea)
		{
			CloseHandle(hFile);
			ResetData();
			return	FALSE;
		}

		CloseHandle(hFile);

		for (int i = 0; i < DLGRESULTVIEW_IMG_NUM_TYPE; i++)
		{
			strTmp.Format(_T("%s\\%s_%d.%s"), strDir, strFileName, i, DLGRESULTVIEW_FILEFORMAT_STR_RESULT_IMGFORMAT);
			read_image(&m_ImageResult[i], CT2A(strTmp));
		}

		for (int i = 0; i < DLGRESULTVIEW_REGION_NUM_TYPE; i++)
		{
			strTmp.Format(_T("%s\\%s_%d.%s"), strDir, strFileName, i, DLGRESULTVIEW_FILEFORMAT_STR_RESULT_RGNFORMAT);
			read_region(&m_Region[i], CT2A(strTmp));
		}
	}

	return	TRUE;
}

VOID	CDialogResultView::RefreshData()
{
	UpdateData(TRUE);

	SetRedraw(FALSE);
	m_ListResult.SetItemCount(m_ResultData.m_Header.m_CountResultArea);

	CString	strTmp;
	for (int i = 0; i < m_ResultData.m_Header.m_CountResultArea; i++)
	{
		strTmp.Format(_T("%d"), i+1);
		m_ListResult.InsertItem(i, strTmp);
		if (m_ResultData.m_pResultData[i].m_IsGood)
			strTmp.Format(_T("Good"));
		else
			strTmp.Format(_T("NG"));
		m_ListResult.SetItemText(i, 1, strTmp);
	}

	SetRedraw(TRUE);

	m_ListResult.Invalidate();
	DisplayImage();
}

VOID	CDialogResultView::ResultSelected(LONG Idx)
{
	if (Idx < 0 || Idx >= m_ResultData.m_Header.m_CountResultArea)
	{
		for (int i = 0; i < 6; i++)
			m_ListDescResult.SetItemText(i, 1, NULL);

		return;
	}

	CString	strTmp;
	int	Iter = 0;
	strTmp.Format(_T("%d"), Idx);
	m_ListDescResult.SetItemText(Iter++, 1, strTmp);

	if (m_ResultData.m_pResultData[Idx].m_IsGood)
		strTmp.Format(_T("GOOD"));
	else
		strTmp.Format(_T("NG"));
	m_ListDescResult.SetItemText(Iter++, 1, strTmp);

	strTmp.Format(_T("%d"), m_ResultData.m_pResultData[Idx].m_CntDent);
	if (m_ResultData.m_pResultData[Idx].m_Result & (0x01 << DLGRESULTVIEW_RESULT_TYPE_LESSCNT))
		strTmp += _T(",NG");
	else
		strTmp += _T(",GOOD");
	m_ListDescResult.SetItemText(Iter++, 1, strTmp);

	strTmp.Empty();
	if (m_ResultData.m_pResultData[Idx].m_Result & (0x01 << DLGRESULTVIEW_RESULT_TYPE_LSHIFT))
		strTmp = _T("Left");
	if (m_ResultData.m_pResultData[Idx].m_Result & (0x01 << DLGRESULTVIEW_RESULT_TYPE_RSHIFT))
	{
		if (strTmp.GetLength() > 0)
			strTmp += _T(",");
		strTmp += _T("Right");
	}
	if (m_ResultData.m_pResultData[Idx].m_Result & (0x01 << DLGRESULTVIEW_RESULT_TYPE_USHIFT))
	{
		if (strTmp.GetLength() > 0)
			strTmp += _T(",");
		strTmp += _T("Up");
	}
	if (m_ResultData.m_pResultData[Idx].m_Result & (0x01 << DLGRESULTVIEW_RESULT_TYPE_DSHIFT))
	{
		if (strTmp.GetLength() > 0)
			strTmp += _T(",");
		strTmp += _T("Down");
	}
	if (strTmp.GetLength() == 0)
		strTmp = _T("GOOD");
	m_ListDescResult.SetItemText(Iter++, 1, strTmp);

	if (m_ResultData.m_pResultData[Idx].m_Result & (0x01 << DLGRESULTVIEW_RESULT_TYPE_DISTRBT))
		strTmp.Format(_T("NG"));
	else
		strTmp.Format(_T("GOOD"));
	m_ListDescResult.SetItemText(Iter++, 1, strTmp);

	if (m_ResultData.m_pResultData[Idx].m_Result & (0x01 << DLGRESULTVIEW_RESULT_TYPE_SIZE))
		strTmp.Format(_T("NG"));
	else
		strTmp.Format(_T("GOOD"));
	m_ListDescResult.SetItemText(Iter++, 1, strTmp);

	//	이미지 렉트 위치 센터로 이동
	m_pDlgImgRect->SetPos(&m_ResultData.m_pResultData[Idx].m_InspArea);
	m_SelectedIdx = Idx;
//	DisplayImage();
//	m_pDlgImgRect->SetZoom(0, m_ImgWidth, m_ImgHeight);
}

VOID	CDialogResultView::DisplayImage()
{
	if (m_pDlgImgRect != NULL)
	{
		disp_obj(m_ImageResult[m_IdxDispImg], m_pDlgImgRect->GetView());

		set_draw(m_pDlgImgRect->GetView(), HTuple("margin"));
		if (m_bDisplayRegion)
		{
			set_color(m_pDlgImgRect->GetView(), HTuple("yellow"));
			disp_obj(m_Region[DLGRESULTVIEW_REGION_TYPE_AREA], m_pDlgImgRect->GetView());
			disp_obj(m_Region[DLGRESULTVIEW_REGION_TYPE_DENT], m_pDlgImgRect->GetView());

			char result0[20];
			for (int i = 0; i < m_ResultData.m_Header.m_CountResultArea; i++)
			{
				set_font(m_pDlgImgRect->GetView(), "-Arial-12-*-*-*-*-*-1-");
				set_draw(m_pDlgImgRect->GetView(), HTuple("margin"));

				sprintf(result0, "PM %d Cnt : %d", i+1, m_ResultData.m_pResultData[i].m_CntDent);

				if (m_ResultData.m_pResultData[i].m_IsGood)
				{
					set_color(m_pDlgImgRect->GetView(), HTuple("green"));
				}
				else
				{
					set_color(m_pDlgImgRect->GetView(), HTuple("red"));
				}
				disp_rectangle1(m_pDlgImgRect->GetView(),
					m_ResultData.m_pResultData[i].m_InspArea.top,
					m_ResultData.m_pResultData[i].m_InspArea.left,
					m_ResultData.m_pResultData[i].m_InspArea.bottom,
					m_ResultData.m_pResultData[i].m_InspArea.right);

				set_tposition(m_pDlgImgRect->GetView(),
					m_ResultData.m_pResultData[i].m_InspArea.bottom + 5,
					m_ResultData.m_pResultData[i].m_InspArea.left + 5);

				write_string(m_pDlgImgRect->GetView(), HTuple(result0));
			}

			set_color(m_pDlgImgRect->GetView(), HTuple("blue"));
			disp_rectangle1(m_pDlgImgRect->GetView(),
				m_ResultData.m_pResultData[m_SelectedIdx].m_InspArea.top - 2,
				m_ResultData.m_pResultData[m_SelectedIdx].m_InspArea.left - 2,
				m_ResultData.m_pResultData[m_SelectedIdx].m_InspArea.bottom + 2,
				m_ResultData.m_pResultData[m_SelectedIdx].m_InspArea.right + 2);
		}

		m_pDlgImgRect->Display();
	}
}

void CDialogResultView::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (bShow)
	{
		LoadData();
		RefreshData();
		if (m_pDlgImgRect != NULL)
			m_pDlgImgRect->ShowWindow(SW_SHOW);
		Invalidate(TRUE);
	}
	else
	{
		if (m_pDlgImgRect != NULL)
			m_pDlgImgRect->ShowWindow(SW_HIDE);
	}
}


void CDialogResultView::OnMove(int x, int y)
{
	CDialogEx::OnMove(x, y);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (m_pDlgImgRect != NULL)
	{
		CRect rect;
		GetDlgItem(IDC_STATIC_RESULT_DISPLAY)->GetWindowRect(&rect);
		m_pDlgImgRect->MoveWindow(rect.left, rect.top, rect.Width(), rect.Height(), TRUE);
	}
}


void CDialogResultView::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (m_pDlgImgRect != NULL)
		m_pDlgImgRect->ShowWindow(SW_HIDE);

	CDialogEx::OnClose();
}

LRESULT CDialogResultView::OnImageRectMessage(WPARAM para0, LPARAM para1)
{
	// TODO: Add your control notification handler code here
	switch ((int)para1)
	{
		//Display
	case 0:
		DisplayImage();
		break;
		//Live on
	case 1:
		break;
		//Live off
	case 2:
		break;
		//Image load
	case 3:
		break;
		//Image save
	case 4:
		break;
		//Gray value
	case 5:
		break;
		//Region confirm
	case 6:
		break;
		//Test
	case 7:
		break;
		//결과 출력
	case 10:

		break;
	default:
		AfxMessageBox(_T("Error : OnImageRectMessage Invalid Index!"));
		break;
	}

	return 0;
}

void CDialogResultView::OnBnClickedRadioResultGrab()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	DisplayImage();
}


void CDialogResultView::OnBnClickedRadioResultFilter()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	DisplayImage();
}


void CDialogResultView::OnBnClickedCheckResultRegion()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	DisplayImage();
}


void CDialogResultView::OnBnClickedBtnDlgresulviewOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	ShowWindow(SW_HIDE);
}


void CDialogResultView::OnBnClickedBtnDlgresulviewOther()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDlgResultList	ResultList;
	CString			strResultPath;

	strResultPath.Format(_T("%s\\%s"),
		m_strModelDir,
		DLGRESULTVIEW_FILEFORMAT_STR_RESULT_FOLDERNAME);

	ResultList.SetRootDir(strResultPath);
	if (ResultList.DoModal() == IDOK)
	{
		if (ResultList.IsSelected())
		{
			LoadData(ResultList.GetSelectedDir(), ResultList.GetSelectedID());
			RefreshData();
		}
	}
}


void CDialogResultView::OnNMDblclkListResult(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int	Idx = pNMItemActivate->iItem;
	ResultSelected(Idx);
	*pResult = 0;
}


void CDialogResultView::OnNMReturnListResult(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_ListResult.GetFirstSelectedItemPosition();
	int	Idx = m_ListResult.GetNextItem(-1, LVNI_SELECTED);
	ResultSelected(Idx);
	*pResult = 0;
}


BOOL CDialogResultView::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
			return	FALSE;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}
