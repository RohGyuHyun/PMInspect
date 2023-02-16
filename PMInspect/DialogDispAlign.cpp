// DialogDispAlign.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DialogDispAlign.h"

const	enum DISPALIGN_THREADSTATUS
{
	DISPALIGN_THREADSTATUS_IDLE,
	DISPALIGN_THREADSTATUS_GRABBING,
	DISPALIGN_THREADSTATUS_GRAB_FAILED,
	DISPALIGN_THREADSTATUS_GRAB_COMPLETE
};

typedef	struct DISPALIGN_THREADINFO
{
	BOOL					m_bIsRunning;
	BOOL					m_bBeginNextGrab;
	DISPALIGN_THREADSTATUS	m_Status;
	BOOL					m_bTerminateNow;

}DISPALIGN_THREADINFO, *P_DISPALIGN_THREADINFO;;

DISPALIGN_THREADINFO	g_ThreadInfo;
CCriticalSection		g_CS;

#define	GRABTHREAD_RETURN		WM_USER + 1234

UINT GrabThread(LPVOID pParam)
{
	CDialogDispAlign	*pDlg = (CDialogDispAlign*)pParam;

	g_CS.Lock();
	g_ThreadInfo.m_Status		=	DISPALIGN_THREADSTATUS_IDLE;
	g_ThreadInfo.m_bIsRunning	=	TRUE;
	g_CS.Unlock();

	while (!g_ThreadInfo.m_bTerminateNow)
	{
		if (g_ThreadInfo.m_bBeginNextGrab)
		{
			g_ThreadInfo.m_Status = DISPALIGN_THREADSTATUS_GRABBING;
			if (g_PylonCtrl.Grab())
			{
				g_ThreadInfo.m_Status = DISPALIGN_THREADSTATUS_GRAB_COMPLETE;
			}
			else
			{
				g_ThreadInfo.m_Status = DISPALIGN_THREADSTATUS_GRAB_FAILED;
			}

			g_CS.Lock();
			g_ThreadInfo.m_bBeginNextGrab = FALSE;
			g_CS.Unlock();

			pDlg->PostMessage(GRABTHREAD_RETURN);
			Sleep(100);	//	다른 처리를 위해 대기
		}
	}

	g_CS.Lock();
	g_ThreadInfo.m_Status		=	DISPALIGN_THREADSTATUS_IDLE;
	g_ThreadInfo.m_bIsRunning	=	FALSE;
	g_CS.Unlock();
	return 0;
}

// CDialogDispAlign 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDialogDispAlign, CDialogEx)

CDialogDispAlign::CDialogDispAlign(LONG AlignIdx, CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_DISP_ALIGN, pParent)
{
	ZeroMemory(&g_ThreadInfo, sizeof(DISPALIGN_THREADINFO));
	ZeroMemory(&m_CamImage, sizeof(PYLONCTRL_IMGINFO));
	ZeroMemory(&m_ProcessArea, sizeof(RECT)*2);

	if (!g_PylonCtrl.GetImage(&m_CamImage, TRUE))
	{
		m_CamImage.m_Width = 1280;
		m_CamImage.m_Height = 960;
		m_CamImage.m_Bpp = 8;
		m_CamImage.m_Channel = 1;
		m_CamImage.m_ColorType = PYLONCTRL_COLORTYPE_MONO;
		m_CamImage.m_DataSize = m_CamImage.m_Width * m_CamImage.m_Height;
		m_CamImage.m_pData = new	BYTE[m_CamImage.m_DataSize];
		memset(m_CamImage.m_pData, 0xFF, m_CamImage.m_DataSize);
	}

	ConvImgToHalBuf(&m_HalconImage, &m_CamImage);
	m_pDlgImgRect = NULL;
	m_AlignIdx = AlignIdx;
	m_bIsLive = FALSE;
}

CDialogDispAlign::~CDialogDispAlign()
{
	if (g_ThreadInfo.m_bIsRunning)
	{
		g_ThreadInfo.m_bTerminateNow = TRUE;
		while (g_ThreadInfo.m_bIsRunning)
		{
			Sleep(10);
		}
	}

	if (m_pDlgImgRect != NULL)
	{
		delete m_pDlgImgRect;
		m_pDlgImgRect = NULL;
	}

	m_HalconImage.Reset();

	if (m_CamImage.m_pData != NULL)
	{
		delete[]	m_CamImage.m_pData;
		ZeroMemory(&m_CamImage, sizeof(PYLONCTRL_IMGINFO));
	}
}

void CDialogDispAlign::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDialogDispAlign, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_WM_TIMER()
	ON_MESSAGE(USER_MSG_IMG_RECT, OnImageRectMessage)
	ON_MESSAGE(GRABTHREAD_RETURN, OnGrabReturnMessage)
END_MESSAGE_MAP()


// CDialogDispAlign 메시지 처리기입니다.


BOOL CDialogDispAlign::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	InitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CDialogDispAlign::OnShowWindow(BOOL bShow, UINT nStatus)
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


void CDialogDispAlign::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	switch (nIDEvent)
	{
	case 0:
		break;
	case 1:
/*
#ifdef ENABLE_CAMERA
		g_PylonCtrl.Grab();
		g_PylonCtrl.GetImage(&m_CamImage);
		if (!ConvImgToHalBuf(&m_HalconImage, &m_CamImage))
			break;
#endif
		disp_obj(m_HalconImage, m_pDlgImgRect->GetView());
		m_pDlgImgRect->Display();
		DisplayCross();
*/
		KillTimer(nIDEvent);
		break;
	default:
		KillTimer(nIDEvent);
		AfxMessageBox(_T("Error : OnTimer Invalid Index!"));
		break;
	}

	CDialogEx::OnTimer(nIDEvent);
}

LRESULT CDialogDispAlign::OnImageRectMessage(WPARAM para0, LPARAM para1)
{
	// TODO: Add your control notification handler code here
	HTuple wd, ht;
	HTuple Num;

	//임시
	CFileDialog dlgFile(FALSE, _T("Image"), _T("*.bmp"),
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("BMP Image(*.bmp)|*.bmp|JPEG(*.jpg)|*.jpg||"));;
	CString strTemp;
	CString tmpPath_Name = _T("");
	wchar_t *tmpfilename = NULL;
	char cTempFile[512];


	switch ((int)para1)
	{
		//Display
	case 0:
/*
#ifdef ENABLE_CAMERA
		g_PylonCtrl.GetImage(&m_CamImage);
		if (!ConvImgToHalBuf(&m_HalconImage, &m_CamImage))
			break;
#endif
*/
		disp_obj(m_HalconImage, m_pDlgImgRect->GetView());
		m_pDlgImgRect->Display();
		DisplayCross();
		break;
		//Live on
	case 1:
		GrabContinuous(TRUE);
//		SetTimer(1, 100, NULL);
		break;
		//Live off
	case 2:
		GrabContinuous(FALSE);
//		KillTimer(1);
		break;
		//Image load
	case 3:
		if (!Hal_OpenHalImage(&m_HalconImage, _T(""), FALSE))
		{
			AfxMessageBox(_T("Image Load Error"));
			return 0;
		}

		get_image_pointer1(m_HalconImage, NULL, NULL, &wd, &ht);
		disp_obj(m_HalconImage, m_pDlgImgRect->GetView());
		m_pDlgImgRect->SetZoom(0, (long)(Hlong)wd[0], (long)(Hlong)ht[0]);
		m_pDlgImgRect->Display();
		DisplayCross();
		break;
		//Image save
	case 4:
		if (dlgFile.DoModal() != IDOK)
		{
			return 0;
		}
		strTemp = dlgFile.GetPathName();
		sprintf_s(cTempFile, strTemp.GetLength() + 1, "%S", strTemp);
		write_image(m_HalconImage, "bmp", 0, cTempFile);
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

LRESULT CDialogDispAlign::OnGrabReturnMessage(WPARAM para0, LPARAM para1)
{
	g_PylonCtrl.GetImage(&m_CamImage);
	ConvImgToHalBuf(&m_HalconImage, &m_CamImage);
	disp_obj(m_HalconImage, m_pDlgImgRect->GetView());
	m_pDlgImgRect->Display();
	DisplayCross();

	if (m_bIsLive)
	{
		g_CS.Lock();
		g_ThreadInfo.m_bBeginNextGrab = TRUE;
		g_CS.Unlock();
	}
	else
	{
		g_CS.Lock();
		g_ThreadInfo.m_bTerminateNow = TRUE;
		g_CS.Unlock();

		while (g_ThreadInfo.m_bIsRunning)
		{
			Sleep(10);
		}
	}

	return	0;
}

BOOL CDialogDispAlign::InitDialog()
{
	BOOL rslt = TRUE;
	CRect rect;
	CString	strTmp;

	//image Dialog
	if (m_pDlgImgRect == NULL)
		delete	m_pDlgImgRect;
	m_pDlgImgRect = new C_CtrlImageRect;
	m_pDlgImgRect->Create(this);
	strTmp.Format(_T("ALIGN DISPLAY #%d"), m_AlignIdx+1);
	m_pDlgImgRect->SetCamIndex(0, strTmp, this->m_hWnd);
	m_pDlgImgRect->ShowWindow(SW_SHOW);
	m_pDlgImgRect->SetZoom(0, m_CamImage.m_Width, m_CamImage.m_Height);
	m_pDlgImgRect->ShowWindow(SW_HIDE);

	DisplayCross();

	return TRUE;
}

BOOL	CDialogDispAlign::ConvImgToHalBuf(Hobject *pHalBuf, P_PYLONCTRL_IMGINFO pImg)
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

BOOL	CDialogDispAlign::DisplayCross()
{
	set_color(m_pDlgImgRect->GetView(), "green");
	set_draw(m_pDlgImgRect->GetView(), HTuple("margin"));
	disp_line(m_pDlgImgRect->GetView(), m_CamImage.m_Height / 2, 0, m_CamImage.m_Height / 2, m_CamImage.m_Width);
	disp_line(m_pDlgImgRect->GetView(), 0, m_CamImage.m_Width / 2, m_CamImage.m_Height, m_CamImage.m_Width / 2);

	return	TRUE;
}

BOOL	CDialogDispAlign::SetActiveTracker(BOOL bActive, LPRECT pPatternArea/*NULL*/, LPRECT pSearchArea/*NULL*/)
{
	if (m_pDlgImgRect == NULL)
		return	FALSE;

	if (pPatternArea != NULL)
		memcpy(&m_ProcessArea[0], pPatternArea, sizeof(RECT));

	if (pSearchArea != NULL)
		memcpy(&m_ProcessArea[1], pSearchArea, sizeof(RECT));

	if (bActive)
	{
		m_pDlgImgRect->SetActiveTracker(bActive);

		m_pDlgImgRect->DeleteAll();	//	이미지 패턴 트랙커 삭제

		//	이미지 패턴 트랙커 추가
		m_pDlgImgRect->AddTracker(0,
			m_ProcessArea[0].left,
			m_ProcessArea[0].top,
			m_ProcessArea[0].right,
			m_ProcessArea[0].bottom,
			RGB(128, 128, 128), "ALIGN-PATTERN REGION");

		m_pDlgImgRect->AddTracker(0,
			m_ProcessArea[1].left,
			m_ProcessArea[1].top,
			m_ProcessArea[1].right,
			m_ProcessArea[1].bottom,
			RGB(0, 0, 255), "ALIGN-SEARCH REGION");
	}
	else
	{
		m_pDlgImgRect->DeleteAll();	//	이미지 패턴 트랙커 삭제
		m_pDlgImgRect->SetActiveTracker(bActive);
	}

	disp_obj(m_HalconImage, m_pDlgImgRect->GetView());
	m_pDlgImgRect->Display();
	DisplayCross();
	return	TRUE;
}

BOOL	CDialogDispAlign::GetTrackerArea(LPRECT pPatternArea, LPRECT pSearchArea)
{
	if (pPatternArea == NULL || pSearchArea == NULL)
		return	FALSE;

	for( int i = 0 ; i < 2 ; i++ )
		m_pDlgImgRect->GetTracker(i, &m_ProcessArea[i].left, &m_ProcessArea[i].top, &m_ProcessArea[i].right, &m_ProcessArea[i].bottom);

	memcpy(pPatternArea, &m_ProcessArea[0], sizeof(RECT));
	memcpy(pSearchArea, &m_ProcessArea[1], sizeof(RECT));

	return	TRUE;
}

/*
BOOL	CDialogDispAlign::GetAlignDiff(P_ALIGNVIEW_PTNDIFF pDiff)
{
	if (pDiff == NULL || m_AlignIdx < 0)
		return	FALSE;

	ZeroMemory(&m_PtnDiff, sizeof(ALIGNVIEW_PTNDIFF));

	IMGALIGN_PTNINFO	PTNInfo;
	if (!m_ImageAlign.FindPatternPos(0, m_pDlgImgRect->GetView(), &m_HalconImage, &m_Param.m_SearchArea, m_Param.m_Threshold, &PTNInfo))
		return	FALSE;

	m_PtnDiff.m_DiffCX = m_Param.m_PatternCX - PTNInfo.m_XCenter;
	m_PtnDiff.m_DiffCY = m_Param.m_PatternCY - PTNInfo.m_YCenter;
	m_PtnDiff.m_Distance = sqrt(pow(abs(m_PtnDiff.m_DiffCX), 2) + pow(abs(m_PtnDiff.m_DiffCY), 2));
	m_PtnDiff.m_DiffAngle = PTNInfo.m_Angle;
	m_PtnDiff.m_Scale = PTNInfo.m_Scale;
	m_PtnDiff.m_Score = PTNInfo.m_Score;

	memcpy(pDiff, &m_PtnDiff, sizeof(ALIGNVIEW_PTNDIFF));

	return	TRUE;
}
*/

void CDialogDispAlign::MoveWindow(int x, int y, int nWidth, int nHeight, BOOL bRepaint /*TRUE*/)
{
	CDialogEx::MoveWindow(x, y, nWidth, nHeight, bRepaint);

	if (m_pDlgImgRect != NULL)
	{
		m_pDlgImgRect->MoveWindow(x, y, nWidth, nHeight, TRUE);
	}
}

void CDialogDispAlign::MoveWindow(LPCRECT lpRect, BOOL bRepaint /*TRUE*/)
{
	if (lpRect == NULL)
		return;

	MoveWindow(lpRect->left, lpRect->top, lpRect->right - lpRect->left, lpRect->bottom - lpRect->top, bRepaint);
}

void	CDialogDispAlign::SetZoomFit()
{
	if (m_pDlgImgRect != NULL)
	{
		m_pDlgImgRect->SetZoom(0, m_CamImage.m_Width, m_CamImage.m_Height);
	}
}

void	CDialogDispAlign::SetZoomOrg()
{
	if (m_pDlgImgRect != NULL)
	{
		m_pDlgImgRect->SetZoom(1, m_CamImage.m_Width, m_CamImage.m_Height);
	}
}

BOOL	CDialogDispAlign::GrabContinuous(BOOL bOn)
{
	g_ThreadInfo.m_bTerminateNow = TRUE;
	while (g_ThreadInfo.m_bIsRunning)
		Sleep(10);

	m_bIsLive = bOn;
	if (m_bIsLive)
	{
		GreabAsync();
	}

	return	TRUE;
}

BOOL	CDialogDispAlign::Live(BOOL bOn)
{
	if (bOn)
	{
		if (!m_pDlgImgRect->GetActiveLive())
		{
			m_pDlgImgRect->SetActiveLive(TRUE);
			m_pDlgImgRect->ClickBtnenhctrlLive();
		}
	}
	else
	{
		if (m_pDlgImgRect->GetActiveLive())
		{
			m_pDlgImgRect->SetActiveLive(FALSE);
			m_pDlgImgRect->ClickBtnenhctrlLive();
		}
	}

	return	TRUE;
}

BOOL	CDialogDispAlign::GrabSync()
{
	if (IsGrabbing())
		return	FALSE;

	if (g_ThreadInfo.m_bIsRunning)
	{
		g_ThreadInfo.m_bTerminateNow = TRUE;
		while (g_ThreadInfo.m_bIsRunning)
			Sleep(10);
	}

	if (g_PylonCtrl.Grab())
	{
		g_PylonCtrl.GetImage(&m_CamImage);
		ConvImgToHalBuf(&m_HalconImage, &m_CamImage);
		disp_obj(m_HalconImage, m_pDlgImgRect->GetView());
		m_pDlgImgRect->Display();
		DisplayCross();

		return	TRUE;
	}

	return	FALSE;
}

BOOL	CDialogDispAlign::GreabAsync()
{
	if (g_ThreadInfo.m_bIsRunning)
	{
		g_ThreadInfo.m_bTerminateNow = TRUE;
		while (g_ThreadInfo.m_bIsRunning)
			Sleep(10);
	}


	g_ThreadInfo.m_bBeginNextGrab = TRUE;
	g_ThreadInfo.m_bTerminateNow = FALSE;

	CWinThread	*pThread;
	pThread = AfxBeginThread(GrabThread, this, THREAD_PRIORITY_NORMAL);
	if (pThread)
		pThread->m_bAutoDelete = TRUE;

	return	TRUE;
}

BOOL	CDialogDispAlign::IsGrabbing()
{
	if (g_ThreadInfo.m_bIsRunning)
	{
		if (g_ThreadInfo.m_Status == DISPALIGN_THREADSTATUS_IDLE ||
			g_ThreadInfo.m_Status == DISPALIGN_THREADSTATUS_GRABBING)
		{
			return	TRUE;
		}
	}

	return	FALSE;
}

BOOL	CDialogDispAlign::ForceDisplay()
{
	g_PylonCtrl.GetImage(&m_CamImage);
	ConvImgToHalBuf(&m_HalconImage, &m_CamImage);
	disp_obj(m_HalconImage, m_pDlgImgRect->GetView());
	m_pDlgImgRect->Display();
	DisplayCross();

	return	TRUE;
}

void	CDialogDispAlign::SetTitle(LPCTSTR strTitle)
{
	if (strTitle != NULL)
	{
		m_pDlgImgRect->SetCamIndex(0, strTitle, this->m_hWnd);
	}
}