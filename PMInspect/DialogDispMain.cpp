// DialogDispMain.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "PMInspect.h"
#include "PMInspectDlg.h"
#include "DialogDispMain.h"
#include "afxdialogex.h"


// CDialogDispMain 대화 상자입니다.

//#define	ENABLE_CAMERA

IMPLEMENT_DYNAMIC(CDialogDispMain, CDialogEx)

CDialogDispMain::CDialogDispMain(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_DISP_MAIN, pParent)
{
	m_pDlgImgRect = NULL;
	ZeroMemory(&m_ImageInfo, sizeof(MILCTRL_IMGINFO));
	m_strModelPath.Empty();
	m_bTestGrabStart = FALSE;
}

CDialogDispMain::~CDialogDispMain()
{
	if (m_ImageInfo.m_pData != NULL)
	{
		delete[]	m_ImageInfo.m_pData;
		ZeroMemory(&m_ImageInfo, sizeof(MILCTRL_IMG));
	}

	if (NULL != m_pDlgImgRect)
	{
		delete m_pDlgImgRect;
		m_pDlgImgRect = NULL;
	}

	g_MilCtrl.Release();

	m_PrsPtnFinder.Release();
	m_ParaManager.Release();

	delete m_pVision;
}

void CDialogDispMain::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDialogDispMain, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_WM_TIMER()
	ON_MESSAGE(USER_MSG_IMG_RECT, OnImageRectMessage)
END_MESSAGE_MAP()


// CDialogDispMain 메시지 처리기입니다.
BOOL CDialogDispMain::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	g_MilCtrl.Create(NULL, NULL, NULL, _T("D:\\DATABASE\\SYSTEM\\vt-9k7c-h80_8t8_10000.dcf"), 1, FALSE, 0, 0, 0, FALSE, 1, TRUE);	//	차후 전역 객체 관리자에서 초기화 및 해제하도록 수정 예정

	m_bLive = FALSE;
	InitDialog();


	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CDialogDispMain::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (bShow)
	{
		if (m_pDlgImgRect != NULL)
			m_pDlgImgRect->ShowWindow(SW_SHOW);
	}
	else
	{
		if (m_pDlgImgRect != NULL)
			m_pDlgImgRect->ShowWindow(SW_HIDE);
	}
}

void CDialogDispMain::SetGrab()
{
	m_bLive = FALSE;
#ifdef ENABLE_LIGHT
	//((CPMInspectDlg*)GetParent())->m_pDlgTeach->SetMainLight(m_pVision->GetLightVal(m_pVision->GetInspIdx()));;
#endif
	g_MilCtrl.GrabBufClear();
	Sleep(10);
	clear_window((Hlong)GetDisplay());
	SetTimer(1, 1, NULL);
	m_pVision->SetGrabEnd(FALSE);
	g_MilCtrl.SetGrab();
}

void CDialogDispMain::SetInspection(char *cSaveImagePath)
{
	m_pVision->SetSaveImagePath(cSaveImagePath);
	/*m_pVision->SetGrabEnd(TRUE);
	m_pVision->SetInspectionEnd(FALSE);
	m_pVision->SetInspectionStart(TRUE);*/
	g_MilCtrl.GetImage(&m_ImageInfo);
	if (!ConvImgToHalBuf(&m_HalGrabImage, &m_ImageInfo))
		return;

	copy_image(m_HalGrabImage, m_pVision->GetDispImage());
	copy_image(m_HalGrabImage, m_pVision->GetGrabImage());

	m_pVision->SetInspectionStart();
}

void CDialogDispMain::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	BOOL isParallelAllInspStartEnd = TRUE;
	switch (nIDEvent)
	{
	case 0:
		break;
	case 1:	//LIVE
#ifdef ENABLE_CAMERA
		if (g_MilCtrl.GetGrabEnd() && isParallelAllInspStartEnd)
		{
			KillTimer(1);
			m_bTestGrabStart = FALSE;
			//g_MilCtrl.GrabSync();
			g_MilCtrl.GrabBufCopy();
			if (m_ImageInfo.m_pData != NULL)
			{
				delete[]	m_ImageInfo.m_pData;
				ZeroMemory(&m_ImageInfo, sizeof(MILCTRL_IMG));
			}

			g_MilCtrl.GetImage(&m_ImageInfo, TRUE);

			if (!ConvImgToHalBuf(&m_HalGrabImage, &m_ImageInfo))
				break;

			//copy_image(m_HalGrabImage, m_pVision->GetDispImage());
			//copy_image(m_HalGrabImage, m_pVision->GetGrabImage());

			disp_obj(m_HalGrabImage, m_pDlgImgRect->GetView());
			m_pDlgImgRect->Display();
			m_pVision->SetGrabEnd(TRUE);

			if (!m_pVision->GetAutoInspection())
			{
				copy_image(m_HalGrabImage, m_pVision->GetDispImage());
				copy_image(m_HalGrabImage, m_pVision->GetGrabImage());
				copy_image(m_HalGrabImage, m_pVision->GetFilterImage());
			}
		}

		
#endif
		break;
	case 2://Insp Rgn Draw
	case 3:
		if (((CPMInspectDlg*)GetParent())->m_pDlgTeach->IsWindowVisible() && m_isWinVisb)
		{
			KillTimer(nIDEvent);
			SendMessage(USER_MSG_IMG_RECT, 0, 0);
		}
		else if (!((CPMInspectDlg*)GetParent())->m_pDlgTeach->IsWindowVisible() && !m_isWinVisb)
		{
			KillTimer(nIDEvent);
			SendMessage(USER_MSG_IMG_RECT, 0, 0);
		}
		break;
	case 4:
		if (this->IsWindowVisible())
		{
			KillTimer(nIDEvent);
			m_pDlgImgRect->ClickBtnenhctrlZoomfit();
			m_pVision->SetParallelInspThreadStart(TRUE);
			Hlong row1, row2, col1, col2;
			get_part(GetDisplay(), &row1, &col1, &row2, &col2);
			m_pVision->SetSetPartHeight(row2);
			m_pVision->SetSetPartSize(col1, row1, col2, row2);
			UpdateInspRgn();
		}
		break;
	default:
		KillTimer(nIDEvent);
		AfxMessageBox(_T("Error : OnTimer Invalid Index!"));
		break;
	}

	CDialogEx::OnTimer(nIDEvent);
}

LRESULT CDialogDispMain::OnImageRectMessage(WPARAM para0, LPARAM para1)
{
	// TODO: Add your control notification handler code here
	HTuple wd, ht;
	HTuple Num;

	//임시
	CFileDialog dlgFile(FALSE, _T("Image"), _T("*.bmp"),
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("BMP Image(*.bmp)|*.bmp|JPEG(*.jpg)|*.jpg||"));;
	CFileDialog dlgLodaFile(TRUE, _T("Image"), _T("*.bmp"),
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("BMP Image(*.bmp)|*.bmp|JPEG(*.jpg)|*.jpg||"));;
	CString strTemp;
	CString tmpPath_Name = _T("");
	wchar_t *tmpfilename = NULL;
	char cTempFile[512];

	PRSPTNFINDER_RESULT	Result;

	switch ((int)para1)
	{
		//Display
	case 0:
#ifndef NOTE_BOOK
		disp_obj(*m_pVision->GetDispImage(), m_pDlgImgRect->GetView());
		//m_pDlgImgRect->Display();
		
		UpdateInspRgn();
		UpdateSelectRect(m_bSelectRectView);
		m_bSelectRectView = FALSE;
#endif
		break;
		//Live on
	case 1:
#ifdef ENABLE_CAMERA
		m_bTestGrabStart = TRUE;
		SetGrab();
#endif
		SetTimer(1, 100, NULL);
		break;
		//Live off
	case 2:
		KillTimer(1);
		break;
		//Image load
	case 3:
		/*if (!Hal_OpenHalImage(m_pVision->GetDispImage(), _T(""), FALSE))
		{
			AfxMessageBox(_T("Image Load Error"));
			return 0;
		}*/
		if (dlgLodaFile.DoModal() != IDOK)
		{
			return 0;
		}
		strTemp = dlgLodaFile.GetPathName();
		g_MilCtrl.LoadImage(strTemp);
		g_MilCtrl.GetImage(&m_ImageInfo);
		if (!ConvImgToHalBuf(&m_HalGrabImage, &m_ImageInfo))
			break;

		copy_image(m_HalGrabImage, m_pVision->GetDispImage());
		copy_image(*m_pVision->GetDispImage(), m_pVision->GetGrabImage());
		copy_image(*m_pVision->GetDispImage(), m_pVision->GetFilterImage());
		disp_obj(*m_pVision->GetDispImage(), m_pDlgImgRect->GetView());
		get_image_pointer1(*m_pVision->GetDispImage(), NULL, NULL, &wd, &ht);
		
		m_pDlgImgRect->SetZoom(0, (long)(Hlong)wd[0], (long)(Hlong)ht[0]);
		m_pDlgImgRect->Display();
		break;
		//Image save
	case 4:
		if (dlgFile.DoModal() != IDOK)
		{
			return 0;
		}
		strTemp = dlgFile.GetPathName();
		sprintf_s(cTempFile, strTemp.GetLength() + 1, "%S", strTemp);
		write_image(*m_pVision->GetDispImage(), "bmp", 0, cTempFile);
		break;
		//Gray value
	case 5:


		break;
		//Region confirm
	case 6:

		break;
		//Test
	case 7:
		g_MilCtrl.GetImage(&m_ImageInfo);
		if (!ConvImgToHalBuf(&m_HalGrabImage, &m_ImageInfo))
			break;

		disp_obj(*m_pVision->GetDispImage(), m_pDlgImgRect->GetView());
		m_pVision->SetGrabImage(&m_HalGrabImage);
		m_pVision->SetGrabEnd(TRUE);
		m_pVision->SetTestInsp(TRUE);
		m_pVision->SetInspectionStart();
		g_MilCtrl.SetGrabLineCnt(1);

		break;
		//결과 출력
	case 8:
		/*
		if (!m_pVision->GetFilterImageView())
		{
			copy_image(*m_pVision->GetFilterImage(), m_pVision->GetDispImage());
			m_pVision->SetFilterImageVIew(TRUE);
		}
		else
		{
			copy_image(*m_pVision->GetGrabImage(), m_pVision->GetDispImage());
			m_pVision->SetFilterImageVIew(FALSE);
		}
		SendMessage(USER_MSG_IMG_RECT, 0, 0);
		*/

		
		//IMSI
		
		if (g_MilCtrl.m_cAtcCtrl->GetConnect())
			g_MilCtrl.m_cAtcCtrl->CloseAFCModoule();
		else
			g_MilCtrl.m_cAtcCtrl->AFCConnect();
		
		
		break;
	case 10:

		break;
	default:
		AfxMessageBox(_T("Error : OnImageRectMessage Invalid Index!"));
		break;
	}

	return 0;
}

BOOL CDialogDispMain::InitDialog()
{
	BOOL rslt = TRUE;
	CRect rect;

	//image Dialog
	if (m_pDlgImgRect == NULL)
		delete	m_pDlgImgRect;
	m_pDlgImgRect = new C_CtrlImageRect;
	m_pDlgImgRect->Create(this);
	m_pDlgImgRect->SetCamIndex(0, _T("MAIN DISPLAY"), this->m_hWnd);
	m_pDlgImgRect->ShowWindow(SW_SHOW);
	m_pDlgImgRect->ShowWindow(SW_HIDE);

	if (m_ImageInfo.m_pData != NULL)
	{
		delete[]	m_ImageInfo.m_pData;
		ZeroMemory(&m_ImageInfo, sizeof(MILCTRL_IMG));
	}

	//	g_MilCtrl대응토록 수정
	g_MilCtrl.GetImage(&m_ImageInfo, TRUE);
#ifdef NOTE_BOOK
	m_ImageInfo.m_Width = 2448;
	m_ImageInfo.m_Height = 65000;
#endif
	//20180913 ngh
	m_pVision = new CVisionModule(m_ImageInfo.m_Width, m_ImageInfo.m_Height);

	if (m_ImageInfo.m_Bpp == 8 && m_ImageInfo.m_Channel == 1)
	{
		gen_image_const(&m_HalGrabImage, HTuple("byte"), m_ImageInfo.m_Width, m_ImageInfo.m_Height);
	}

	if (m_ImageInfo.m_Bpp <= 8 && m_ImageInfo.m_Channel == 1)
	{
		m_HalGrabImage.Reset();
		gen_image_const(&m_HalGrabImage, "byte", m_ImageInfo.m_Width, m_ImageInfo.m_Height);
	}
	else if (m_ImageInfo.m_Bpp <= 16 && m_ImageInfo.m_Channel == 1)
	{
		m_HalGrabImage.Reset();
		gen_image_const(&m_HalGrabImage, "uint2", m_ImageInfo.m_Width, m_ImageInfo.m_Height);
	}
	else
	{
		return	FALSE;
	}

	m_pDlgImgRect->SetZoom(1, m_ImageInfo.m_Width, m_ImageInfo.m_Height);
	SetTimer(4, 100, NULL);

	//ngh
	m_pVision->SetDisp(GetDisplay());
	g_MilCtrl.SetHalDisp(GetDisplay());

	m_pVision->SetGrabHeight(g_MilCtrl.GetGrabHeight());
	//m_pVision->SetGrabHeight(128);

	set_line_width(GetDisplay(), 2);

	return TRUE;
}

BOOL	CDialogDispMain::ConvImgToHalBuf(Hobject *pHalBuf, P_MILCTRL_IMGINFO pImg)
{
	if (pHalBuf == NULL || pImg == NULL)
		return	FALSE;

	if (pImg->m_pData == NULL)
		return	FALSE;

	if (pImg->m_Bpp <= 8 && pImg->m_Channel == 1)
	{
		pHalBuf->Reset();
		gen_image1_extern(pHalBuf, "byte", pImg->m_Width, pImg->m_Height, (Hlong)pImg->m_pData, (Hlong)0);
	}
	else if (pImg->m_Bpp <= 16 && pImg->m_Channel == 1)
	{
		pHalBuf->Reset();
		gen_image1_extern(pHalBuf, "uint2", pImg->m_Width, pImg->m_Height, (Hlong)pImg->m_pData, (Hlong)0);
	}
	else
	{
		return	FALSE;
	}

	return	TRUE;
}

//20180913 ngh
BOOL CDialogDispMain::LoadModelData(char *strModelPath)
{
	m_strModelPath.Format(_T("%S"), (strModelPath));
#ifdef NOTE_BOOK
	m_strModelPath.Format(_T("d:\\Database\\MODEL\\Test"));
#endif
	BOOL rslt = m_pVision->LoadInspPara(strModelPath);
	//m_pVision->SetInspRgn(0);

	return rslt;
}

BOOL	CDialogDispMain::LoadModelData(LPCTSTR strModelPath)
{
	BOOL	IsDlgVisible = m_pDlgImgRect->IsWindowVisible();
	CString	strTmp;

	m_ParaManager.SetPath(strModelPath, INSP_STR_PARAFILENAME);
	m_ParaManager.LoadPara();

	if (IsDlgVisible)
	{
/*
		m_pDlgImgRect->SetActiveTracker(TRUE);

		m_pDlgImgRect->DeleteAll();	//	이미지 패턴 트랙커 삭제

									//	이미지 패턴 트랙커 추가
		m_pDlgImgRect->AddTracker(0,
			m_Param.m_PatternArea[0].left,
			m_Param.m_PatternArea[0].top,
			m_Param.m_PatternArea[0].right,
			m_Param.m_PatternArea[0].bottom,
			RGB(128, 128, 128), "ALIGN-PATTERN REGION");

		m_pDlgImgRect->AddTracker(0,
			m_Param.m_SearchArea[0].left,
			m_Param.m_SearchArea[0].top,
			m_Param.m_SearchArea[0].right,
			m_Param.m_SearchArea[0].bottom,
			RGB(0, 0, 255), "ALIGN-SEARCH REGION");
*/
	}
	m_strModelPath = strModelPath;
	return	TRUE;
}

BOOL	CDialogDispMain::SetActiveTracker(BOOL bActive)
{
	if (m_pDlgImgRect == NULL)
		return	FALSE;
/*
	if (bActive)
	{
		m_pDlgImgRect->SetActiveTracker(bActive);

		m_pDlgImgRect->DeleteAll();	//	이미지 패턴 트랙커 삭제

									//	이미지 패턴 트랙커 추가
		m_pDlgImgRect->AddTracker(0,
			m_Param.m_PatternArea[0].left,
			m_Param.m_PatternArea[0].top,
			m_Param.m_PatternArea[0].right,
			m_Param.m_PatternArea[0].bottom,
			RGB(128, 128, 128), "ALIGN-PATTERN REGION");

		m_pDlgImgRect->AddTracker(0,
			m_Param.m_SearchArea[0].left,
			m_Param.m_SearchArea[0].top,
			m_Param.m_SearchArea[0].right,
			m_Param.m_SearchArea[0].bottom,
			RGB(0, 0, 255), "ALIGN-SEARCH REGION");
	}
	else
	{
		m_pDlgImgRect->DeleteAll();	//	이미지 패턴 트랙커 삭제
		m_pDlgImgRect->SetActiveTracker(bActive);
	}
*/
	disp_obj(m_HalGrabImage, m_pDlgImgRect->GetView());
	m_pDlgImgRect->Display();
	return	TRUE;
}

//20180913 ngh
BOOL	CDialogDispMain::SaveParam()
{
	CString	strTmp = m_strModelPath + _T("\\") + INSP_STR_PARAFILENAME[0];
	m_Param = m_pVision->GetInspPara();
	m_ParaManager.SavePara();
	return	TRUE;
}

void CDialogDispMain::SetInspRgn(int nInspType)
{
	m_pVision->SetInspIdx(nInspType);
	m_pVision->SetGrabImage(m_pVision->GetDispImage());
	m_pVision->SetInspRgn(INSPECTION_SPACE_PM_COUNT_TEMP);
	//m_pVision->SetInspRgn(INSPECTION_SPACE_PM_SHIFT);
	//m_pVision->SetInspRgn(INSPECTION_SPACE_DISTRIBUTION);
}

//20180914 ngh
BOOL CDialogDispMain::SaveModelData()
{
	char strPath[512];
	sprintf_s(strPath, m_strModelPath.GetLength() + 1, "%S", m_strModelPath);
	return m_pVision->SaveInspPara(strPath);
}

void CDialogDispMain::MoveWindow(int x, int y, int nWidth, int nHeight, BOOL bRepaint /*TRUE*/)
{
	CDialogEx::MoveWindow(x, y, nWidth, nHeight, bRepaint);

	if (m_pDlgImgRect != NULL)
	{
		m_pDlgImgRect->MoveWindow(x, y, nWidth, nHeight, TRUE);
		m_pDlgImgRect->SetZoom(0, m_ImageInfo.m_Width, m_ImageInfo.m_Height);
	}
}

void CDialogDispMain::MoveWindow(LPCRECT lpRect, BOOL bRepaint /*TRUE*/)
{
	if (lpRect == NULL)
		return;

	MoveWindow(lpRect->left, lpRect->top, lpRect->right - lpRect->left, lpRect->bottom - lpRect->top, bRepaint);
}

//ngh
void CDialogDispMain::SetUpdateInspRgn(BOOL isWinVisb)
{
	m_isWinVisb = isWinVisb;
	SetTimer(2, 100, NULL);
	SetTimer(3, 100, NULL);
}

void CDialogDispMain::UpdateInspRgn()
{
	BOOL isVisb = ((CPMInspectDlg*)GetParent())->m_pDlgTeach->IsWindowVisible();
	
	//m_pDlgImgRect->Display();
	set_color(GetDisplay(), HTuple("yellow"));
	set_draw(GetDisplay(), HTuple("margin"));
	
	if (isVisb)
	{
		m_pVision->UpdateInspRgn();
	}
	else
	{
		HTuple Num;
		count_obj(m_pVision->GetInspRgn(m_pVision->GetInspIdx(), 0), &Num);
		if ((long)(Hlong)Num[0] > 0)
		{
			disp_obj(m_pVision->GetInspRgn(m_pVision->GetInspIdx(), 0), GetDisplay());
		}

		count_obj(m_pVision->GetInspRgn(m_pVision->GetInspIdx(), 1), &Num);
		if ((long)(Hlong)Num[0] > 0)
		{
			disp_obj(m_pVision->GetInspRgn(m_pVision->GetInspIdx(), 1), GetDisplay());
		}
	}	

	/*count_obj(m_pVision->GetInspRgn(m_pVision->GetInspIdx(), 1), &Num);
	if ((long)(Hlong)Num[0] > 0)
	{
		disp_obj(m_pVision->GetInspRgn(m_pVision->GetInspIdx(), 1), GetDisplay());
	}*/

	//if (isVisb || m_pVision->GetRsltView())
	{
		m_pVision->DisplayRslt();
	}
}

void CDialogDispMain::UpdateSelectRect(BOOL isSelect)
{
	BOOL isVisb = ((CPMInspectDlg*)GetParent())->m_pDlgTeach->IsWindowVisible();

	if (isVisb)
	{
		m_pVision->SetStartEndInspRectView(m_nXIdx);
		m_pVision->SetSelectRect(m_nXIdx, m_nYIdx, isSelect);
	}
}

typeInspPara CDialogDispMain::GetInspPara()
{ 
	return  m_pVision->GetInspPara();
}

void CDialogDispMain::SetInspPara(typeInspPara *pInspPara)
{
	m_pVision->SetInspPara(pInspPara);
	SetUpdateInspRgn(TRUE);
}

void CDialogDispMain::SetATFHomming()
{
	g_MilCtrl.m_cAtcCtrl->ATFHome();
}