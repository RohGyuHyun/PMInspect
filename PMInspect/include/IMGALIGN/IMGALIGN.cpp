#include "stdafx.h"
#include "IMGALIGN.h"


IMGALIGN::IMGALIGN()
{
	m_Status = IMGALIGN_STATUS_NOT_CREATED;
	ZeroMemory(m_PatternID, sizeof(LONGLONG)*IMGALIGN_NUM_MAX_PTN);
	ZeroMemory(m_PatternDiff, sizeof(IMGALIGN_PTNDIFF)*IMGALIGN_NUM_MAX_PTN);
	ZeroMemory(m_CamImage, sizeof(PYLONCTRL_IMG)*IMGALIGN_NUM_MAX_PTN);
	ZeroMemory(m_pWndDisplay, sizeof(CDialogDispAlign*)*IMGALIGN_NUM_MAX_PTN);
	ZeroMemory(m_bIsPtnLoaded, sizeof(BOOL)*IMGALIGN_NUM_MAX_PTN);

#ifdef ENABLE_CAMERA
	g_PylonCtrl.Initialize();	//	차후 전역 객체 관리자에서 초기화 및 해제하도록 수정 예정
#endif
	for (int i = 0; i < IMGALIGN_NUM_MAX_PTN; i++)
	{
		m_CamImage[i].m_Width = 1280;
		m_CamImage[i].m_Height = 960;
	}
}


IMGALIGN::~IMGALIGN()
{
	Release();
}

VOID	IMGALIGN::Release()
{
	if (m_Status == IMGALIGN_STATUS_NOT_CREATED)
		return;

	for (int i = 0; i < IMGALIGN_NUM_MAX_PTN; i++)
	{
		if (m_bIsPtnLoaded[i])
		{
			clear_shape_model(m_PatternID[i]);
			m_bIsPtnLoaded[i] = FALSE;
		}

		if (m_CamImage[i].m_pData != NULL)
		{
			delete[]	m_CamImage[i].m_pData;
			m_CamImage[i].m_pData = NULL;
		}

		if (m_pWndDisplay[i] != NULL)
		{
			delete	m_pWndDisplay[i];
			m_pWndDisplay[i] = NULL;
		}
	}

	ZeroMemory(m_PatternID, sizeof(LONGLONG)*IMGALIGN_NUM_MAX_PTN);
	ZeroMemory(m_PatternDiff, sizeof(IMGALIGN_PTNDIFF)*IMGALIGN_NUM_MAX_PTN);
	ZeroMemory(m_CamImage, sizeof(PYLONCTRL_IMG)*IMGALIGN_NUM_MAX_PTN);
	ZeroMemory(m_pWndDisplay, sizeof(CDialogDispAlign*)*IMGALIGN_NUM_MAX_PTN);

#ifdef ENABLE_CAMERA
	g_PylonCtrl.Terminate();	//	차후 전역 객체 관리자에서 초기화 및 해제하도록 수정 예정
#endif

	m_Status = IMGALIGN_STATUS_NOT_CREATED;
}

BOOL	IMGALIGN::SavePattern(int Idx, LPCTSTR FileName)
{
	if (Idx >= IMGALIGN_NUM_MAX_PTN)
		return	FALSE;

	if (!m_bIsPtnLoaded[Idx])
	{
		return	FALSE;
	}

	Herror	Err;

	set_check("~give_error");
	Err = write_shape_model(m_PatternID[Idx], (CT2A)FileName);
	set_check("give_error");

	if (Err != H_MSG_TRUE)
	{
		return	FALSE;
	}

	return	TRUE;
}

BOOL	IMGALIGN::LoadPattern(int Idx, LPCTSTR FileName)
{
	if (Idx > IMGALIGN_NUM_MAX_PTN)
		return	FALSE;

	Herror	Err;
	LONGLONG	PatternID;

	set_check("~give_error");
	Err = read_shape_model((CT2A)FileName, &PatternID);
	set_check("give_error");

	if (Err == H_MSG_TRUE)
	{
		if (Idx == IMGALIGN_NUM_MAX_PTN)
		{
			return	FALSE;
		}
		else
		{
			m_PatternID[Idx] = PatternID;
			m_bIsPtnLoaded[Idx] = TRUE;
		}
	}
	else
	{
		return	FALSE;
	}

	return	TRUE;
}












BOOL	IMGALIGN::Create(CWnd* pWndParent, LPRECT pRectAlign1, LPRECT pRectAlign2, LPCTSTR strParaPath)
{
	if (pWndParent == NULL || pRectAlign1 == NULL || pRectAlign2 == NULL || strParaPath == NULL)
	{
		return	FALSE;
	}

	if (m_Status != IMGALIGN_STATUS_NOT_CREATED)
	{
		Release();
	}


	m_Status = IMGALIGN_STATUS_IDLE;

	m_strModelPath = strParaPath;
	SetLinkConfigData();
	LoadAlignData();

	m_pWndDisplay[0] = new CDialogDispAlign(0);
	m_pWndDisplay[0]->Create(IDD_DIALOG_DISP_ALIGN, pWndParent);
	m_pWndDisplay[0]->MoveWindow(pRectAlign1->left, pRectAlign1->top,
		pRectAlign1->right - pRectAlign1->left, pRectAlign1->bottom - pRectAlign1->top, FALSE);
	m_pWndDisplay[0]->SetZoomFit();
	m_pWndDisplay[0]->SetActiveTracker(FALSE, &m_Parameter.m_PatternBuildArea[0], &m_Parameter.m_PatternSearchArea[0]);
	m_pWndDisplay[0]->EnableUI(C_CtrlImageRect_UI_LIVE, FALSE);
	m_pWndDisplay[0]->EnableUI(C_CtrlImageRect_UI_COORD, FALSE);
	m_pWndDisplay[0]->EnableUI(C_CtrlImageRect_UI_LABEL, FALSE);
	m_pWndDisplay[0]->EnableUI(C_CtrlImageRect_UI_GRAY, FALSE);
	m_pWndDisplay[0]->EnableUI(C_CtrlImageRect_UI_GRID, FALSE);
	m_pWndDisplay[0]->EnableUI(C_CtrlImageRect_UI_GENERAL, FALSE);
	m_pWndDisplay[0]->ShowUI(C_CtrlImageRect_UI_LIVE, FALSE);
	m_pWndDisplay[0]->ShowUI(C_CtrlImageRect_UI_COORD, FALSE);
	m_pWndDisplay[0]->ShowUI(C_CtrlImageRect_UI_LABEL, FALSE);
	m_pWndDisplay[0]->ShowUI(C_CtrlImageRect_UI_GRAY, FALSE);
	m_pWndDisplay[0]->ShowUI(C_CtrlImageRect_UI_GRID, FALSE);
	m_pWndDisplay[0]->ShowUI(C_CtrlImageRect_UI_GENERAL, FALSE);
	m_pWndDisplay[0]->ShowWindow(SW_SHOW);

	//Align Disp#2
	m_pWndDisplay[1] = new CDialogDispAlign(1);
	m_pWndDisplay[1]->Create(IDD_DIALOG_DISP_ALIGN, pWndParent);
	m_pWndDisplay[1]->MoveWindow(pRectAlign2->left, pRectAlign2->top,
		pRectAlign2->right - pRectAlign2->left, pRectAlign2->bottom - pRectAlign2->top, FALSE);
	m_pWndDisplay[1]->SetZoomFit();
	m_pWndDisplay[1]->SetActiveTracker(FALSE, &m_Parameter.m_PatternBuildArea[1], &m_Parameter.m_PatternSearchArea[1]);
	m_pWndDisplay[1]->EnableUI(C_CtrlImageRect_UI_LIVE, FALSE);
	m_pWndDisplay[1]->EnableUI(C_CtrlImageRect_UI_COORD, FALSE);
	m_pWndDisplay[1]->EnableUI(C_CtrlImageRect_UI_LABEL, FALSE);
	m_pWndDisplay[1]->EnableUI(C_CtrlImageRect_UI_GRAY, FALSE);
	m_pWndDisplay[1]->EnableUI(C_CtrlImageRect_UI_GRID, FALSE);
	m_pWndDisplay[1]->EnableUI(C_CtrlImageRect_UI_GENERAL, FALSE);
	m_pWndDisplay[1]->ShowUI(C_CtrlImageRect_UI_LIVE, FALSE);
	m_pWndDisplay[1]->ShowUI(C_CtrlImageRect_UI_COORD, FALSE);
	m_pWndDisplay[1]->ShowUI(C_CtrlImageRect_UI_LABEL, FALSE);
	m_pWndDisplay[1]->ShowUI(C_CtrlImageRect_UI_GRAY, FALSE);
	m_pWndDisplay[1]->ShowUI(C_CtrlImageRect_UI_GRID, FALSE);
	m_pWndDisplay[1]->ShowUI(C_CtrlImageRect_UI_GENERAL, FALSE);
	m_pWndDisplay[1]->ShowWindow(SW_SHOW);

	return	TRUE;
}

BOOL	IMGALIGN::BuildPattern(int Idx, CDialogDispAlign* pDlgDispAlign/*NULL*/)
{
	DeletePattern(Idx);

	if (Idx > IMGALIGN_NUM_MAX_PTN)
		return	FALSE;

	if (m_Status == IMGALIGN_STATUS_NOT_CREATED)
	{
		return	FALSE;
	}

	HTuple wd, ht;
	Herror herr;

	RECT		rtPattrenArea, rtSearchArea;
	DOUBLE		Threshold;
	Hobject*	pImage;
	Hlong		view;
	DOUBLE		PatternCX, PatternCY;

	if (pDlgDispAlign == NULL)
	{
		pImage = m_pWndDisplay[Idx]->GetImage();
		view = m_pWndDisplay[Idx]->GetView();
		m_pWndDisplay[Idx]->GetTrackerArea(&rtPattrenArea, &rtSearchArea);
	}
	else
	{
		pImage = pDlgDispAlign->GetImage();
		view = pDlgDispAlign->GetView();
		pDlgDispAlign->GetTrackerArea(&rtPattrenArea, &rtSearchArea);
	}
	Threshold = 30;

	set_check("~give_error");
	herr = get_image_pointer1(*pImage, NULL, NULL, &wd, &ht);
	set_check("give_error");
	if (H_MSG_TRUE == herr)
	{
	}
	else
	{
		AfxMessageBox(_T("Halcon Error Occured at get_image_pointer1!"));
		return	FALSE;
	}

	Hobject modimage, cropimage, Region;
	Hlong	lLeft, lTop, lRight, lBottom;
	RECT	FitArea;

	gray_dilation_rect(*pImage, &modimage, 59, 59);
	gray_erosion_rect(modimage, &modimage, 59, 59);
	threshold(modimage, &Region, m_Parameter.m_Threshold[Idx], 255);
	region_to_bin(Region, &modimage, 255, 0, wd, ht);

	crop_rectangle1(modimage, &cropimage, rtPattrenArea.top, rtPattrenArea.left, rtPattrenArea.bottom, rtPattrenArea.right);
	threshold(cropimage, &Region, 255, 255);
	smallest_rectangle1(Region, &lTop, &lLeft, &lBottom, &lRight);
	//	edges_sub_pix(modimage, &Edges, "sobel_fast", 1, 20, 40);
	//	smallest_rectangle1_xld(Edges, &dblTop, &dblLeft, &dblBottom, &dblRight);
	FitArea.left = rtPattrenArea.left + lLeft - 10;
	FitArea.top = rtPattrenArea.top + lTop - 10;
	FitArea.right = rtPattrenArea.left + lRight + 10;
	FitArea.bottom = rtPattrenArea.top + lBottom + 10;

	PatternCX = PatternCY = 0.0;

	Hlong	PatternID;
	//Teach////////////////////////////////////////////////////////////////
	if (Hal_ModelTeachingRect1(view,
		&modimage,
		&FitArea,
		(Hlong)wd[0],
		(Hlong)ht[0],
		m_Parameter.m_PatPara[Idx],
		Threshold,								  //threshold
		(int)(Threshold * 0.5),					  //min threshold
		&PatternCX,
		&PatternCY,
		&cropimage,
		&PatternID)
		)
	{
		m_PatternID[Idx] = PatternID;
		m_bIsPtnLoaded[Idx] = TRUE;
		m_PatternDiff[Idx].m_XCenter = PatternCX;
		m_PatternDiff[Idx].m_YCenter = PatternCY;
		m_PatternDiff[Idx].m_Angle = 0.0;
		m_PatternDiff[Idx].m_Score = 1.0;
		m_PatternDiff[Idx].m_Scale = 1.0;

	}
	else
	{
		return	FALSE;
	}

	memcpy(&m_Parameter.m_PatternBuildArea[Idx], &rtPattrenArea, sizeof(RECT));
	memcpy(&m_Parameter.m_PatternSearchArea[Idx], &rtSearchArea, sizeof(RECT));
	m_Parameter.m_PtnCenterX[Idx] = PatternCX;
	m_Parameter.m_PtnCenterY[Idx] = PatternCY;
//	m_Parameter.m_Threshold[Idx] = Threshold;

	return	TRUE;
}

BOOL	IMGALIGN::PatternMatch(int Idx, CDialogDispAlign* pDlgDispAlign/*NULL*/)
{
	if (Idx > IMGALIGN_NUM_MAX_PTN)
		return	FALSE;

	if (m_Status == IMGALIGN_STATUS_NOT_CREATED || !m_bIsPtnLoaded[Idx])
	{
		return	FALSE;
	}

	HTuple wd, ht;
	Herror herr;
	CRect rect;

	RECT		rtPattrenArea, rtSearchArea;
	DOUBLE		Threshold;
	Hobject*	pImage;
	Hlong		view;

	if (pDlgDispAlign == NULL)
	{
		pImage = m_pWndDisplay[Idx]->GetImage();
		view = m_pWndDisplay[Idx]->GetView();
		m_pWndDisplay[Idx]->GetTrackerArea(&rtPattrenArea, &rtSearchArea);
	}
	else
	{
		pImage = pDlgDispAlign->GetImage();
		view = pDlgDispAlign->GetView();
		pDlgDispAlign->GetTrackerArea(&rtPattrenArea, &rtSearchArea);
	}
	Threshold = 30;

	double rx, ry, ra, rs, rscore;
	Hlong rn;
	Hobject modimage, shape;

	double pat_score;
	pat_score = 0.5;

	set_check("~give_error");
	herr = get_image_pointer1(*pImage, NULL, NULL, &wd, &ht);
	set_check("give_error");
	if (H_MSG_TRUE == herr)
	{
	}
	else
	{
		AfxMessageBox(_T("Halcon Error Occured at get_image_pointer1!"));
		return	FALSE;
	}

	Hobject Edges, Region;

	gray_dilation_rect(*pImage, &modimage, 59, 59);
	gray_erosion_rect(modimage, &modimage, 59, 59);
	threshold(modimage, &Region, m_Parameter.m_Threshold[Idx], 255);
	region_to_bin(Region, &modimage, 255, 0, wd, ht);

	Hal_ShapePatternFind(view,
		&modimage,
		m_PatternID[Idx],
		m_Parameter.m_PatPara[Idx],
		rtSearchArea,
		(Hlong)wd[0],
		(Hlong)ht[0],
		0,
		0,
		pat_score,//Score
		&Edges,
		&shape,
		&rx,
		&ry,
		&ra,
		&rs,
		&rscore,
		&rn);

	if (rn)
	{
		m_PatternDiff[Idx].m_XCenter = rx;
		m_PatternDiff[Idx].m_YCenter = ry;
		m_PatternDiff[Idx].m_Angle = ra;
		m_PatternDiff[Idx].m_Score = rscore;
		m_PatternDiff[Idx].m_Scale = rs;

		if (view)
		{
			set_color(view, "green");
			set_draw(view, HTuple("margin"));
			set_tposition(view, 10, 10);
			write_string(view, HTuple(HTuple("SCORE : ") + HTuple(rscore * 100)));
			set_tposition(view, 40, 10);
			write_string(view, HTuple(HTuple("X : ") + HTuple(rx)));
			set_tposition(view, 70, 10);
			write_string(view, HTuple(HTuple("Y : ") + HTuple(ry)));
			set_tposition(view, 100, 10);
			write_string(view, HTuple(HTuple("ANGLE : ") + HTuple(ra)));
		}
	}
	else
	{
		if (view)
		{
			set_color(view, "red");
			set_draw(view, HTuple("margin"));
			set_draw(view, HTuple("margin"));
			set_tposition(view, 10, 10);
			write_string(view, HTuple(HTuple("FAIL : can't find model!")));
		}

		return	FALSE;
	}

	return	TRUE;
}

BOOL	IMGALIGN::DeletePattern(int Idx)
{
	if (Idx > IMGALIGN_NUM_MAX_PTN)
		return	FALSE;

	if (m_Status == IMGALIGN_STATUS_NOT_CREATED || !m_bIsPtnLoaded[Idx])
	{
		return	FALSE;
	}

	clear_shape_model(m_PatternID[Idx]);
	m_bIsPtnLoaded[Idx] = FALSE;
	m_PatternDiff[Idx].m_XCenter = 0;
	m_PatternDiff[Idx].m_YCenter = 0;
	m_PatternDiff[Idx].m_Angle = 0.0;
	m_PatternDiff[Idx].m_Score = 0.0;
	m_PatternDiff[Idx].m_Scale = 0.0;
	m_Parameter.m_PtnCenterX[Idx] = 0;
	m_Parameter.m_PtnCenterY[Idx] = 0;

	return	TRUE;
}

BOOL	IMGALIGN::GetAlignCompPos(DOUBLE LinearPosX0, DOUBLE LinearPosX1, DOUBLE LinearPosY0, DOUBLE LinearPosY1, DOUBLE *pAngle, DOUBLE *pX, DOUBLE *pY, DOUBLE *pDistance/*NULL*/, BOOL IsIgnoreAngle/*FALSE*/, BOOL IsTeaching/*FALSE*/)
{
	DOUBLE	AngleDiffRad, DistCtoC, DistX, DistY, RotX, RotY, TransX, TransY, CosT, SinT;
//	LONG	Temp;

	if (IsIgnoreAngle)
	{
		*pX = m_PatternDiff[1].m_XCenter;
		*pY = m_PatternDiff[1].m_YCenter;
		*pAngle = 0.0;
		if( pDistance != NULL )
			*pDistance = 0.0;
	}
	else
	{
		//	um 단위로 계산
		DistX = ((m_PatternDiff[0].m_XCenter - m_PatternDiff[1].m_XCenter) * m_Parameter.m_umPerPixelX) + ((LinearPosX1 - LinearPosX0) * m_Parameter.m_umPerPulseX);
		DistY = ((m_PatternDiff[0].m_YCenter - m_PatternDiff[1].m_YCenter) * m_Parameter.m_umPerPixelY) + ((LinearPosY0 - LinearPosY1) * m_Parameter.m_umPerPulseY);;

		//	각도 계산
		AngleDiffRad = atan2(DistY, DistX);
		DistCtoC = sqrt(pow(DistX,2) + pow(DistY,2));

		if (!IsTeaching)	//	기존 패턴의 각과 차이만큼 빼준
		{
			DistX = ((m_Parameter.m_PtnCenterX[0] - m_Parameter.m_PtnCenterX[1]) * m_Parameter.m_umPerPixelX) + ((LinearPosX1 - LinearPosX0) * m_Parameter.m_umPerPulseX);
			DistY = ((m_Parameter.m_PtnCenterY[0] - m_Parameter.m_PtnCenterY[1]) * m_Parameter.m_umPerPixelY) + ((LinearPosY0 - LinearPosY1) * m_Parameter.m_umPerPulseY);

			AngleDiffRad -= atan2(DistY, DistX);
		}

		*pAngle = -(AngleDiffRad * (180 / PI));

		//	회전축 기준으로 해당각도만큼 회전 시 틀어질 X, Y축 계산
		//	펄스 단위를 미크론 단위로 변경하여 거리를 구하고 다시 픽셀단위로 변경
		RotX = ((m_Parameter.m_DistRotX - (LinearPosX1*m_Parameter.m_umPerPulseX)) / m_Parameter.m_umPerPixelX) + (m_CamImage[1].m_Width / 2);
		RotY = ((m_Parameter.m_DistRotY - (LinearPosY1*m_Parameter.m_umPerPulseY)) / m_Parameter.m_umPerPixelY) + (m_CamImage[1].m_Height / 2);

		//	회전 후 X, Y좌표를 구함
		TransX = m_PatternDiff[1].m_XCenter - RotX;
		TransY = m_PatternDiff[1].m_YCenter - RotY;
		CosT = cos(AngleDiffRad);
		SinT = sin(AngleDiffRad);
		if (AngleDiffRad <= 0)
		{
			*pX = (TransX * CosT) + (TransY * SinT);
		}
		else
		{
			*pX = (TransX * CosT) + (TransY * SinT);
		}
		*pY = (TransX * SinT) + (TransY * CosT);

		*pX += RotX;
		*pY += RotY;
/*
		if (AngleDiffRad >= 0.0)
		{
			*pX = (TransX * CosT) - (TransY * SinT) - RotX;
			*pY = (TransX * SinT) + (TransY * CosT) - RotY;
		}
		else
		{
			*pX = (TransX * CosT) - (TransY * SinT) - RotX;
			*pY = (TransX * SinT) + (TransY * CosT) - RotY;
		}
*/
	}

	//	패턴 센터 기준으로 차이 정리
	if (IsTeaching)
	{
		//	카메라 센터 기준으로 차이 정리
		*pX -= (m_CamImage[1].m_Width / 2);
		*pY -= (m_CamImage[1].m_Height / 2);
	}
	else
	{
		*pX -= m_Parameter.m_PtnCenterX[1];
		*pY	-= m_Parameter.m_PtnCenterY[1];
	}

	//	펄스 좌표계 값으로 변경
	*pAngle	/=	m_Parameter.m_AnglePerPulse;
	*pX		*=	-(m_Parameter.m_umPerPixelX / m_Parameter.m_umPerPulseX);
	*pY		*=	m_Parameter.m_umPerPixelY / m_Parameter.m_umPerPulseY;
	if (pDistance != NULL)
		*pDistance = DistCtoC / m_Parameter.m_umPerPulseX;
/*
	Temp = (LONG)(abs(*pAngle) + 0.5);
	(*pAngle < 0) ? *pAngle = -Temp : *pAngle = Temp;
	Temp = (LONG)(abs(*pX) + 0.5);
	(*pX < 0) ? *pX = -Temp : *pX = Temp;
	Temp = (LONG)(abs(*pY) + 0.5);
	(*pY < 0) ? *pY = -Temp : *pY = Temp;
*/
	return	TRUE;
}

BOOL	IMGALIGN::Grab(int Idx)
{
	if (Idx > IMGALIGN_NUM_MAX_PTN)
		return	FALSE;

	if (m_Status == IMGALIGN_STATUS_NOT_CREATED )
	{
		return	FALSE;
	}

	return	m_pWndDisplay[Idx]->GrabSync();
}

BOOL	IMGALIGN::Live(int Idx, BOOL bOn)
{
	if (Idx > IMGALIGN_NUM_MAX_PTN)
		return	FALSE;

	if (m_Status == IMGALIGN_STATUS_NOT_CREATED)
	{
		return	FALSE;
	}

	return	m_pWndDisplay[Idx]->Live(bOn);
}

LONG	IMGALIGN::GetCamWidth()
{
	return	m_CamImage[0].m_Width;
}

LONG	IMGALIGN::GetCamHeight()
{
	return	m_CamImage[0].m_Height;
}

BOOL	IMGALIGN::GetParameter(P_IMGALIGN_PARAM pParam)
{
	if (pParam == NULL)
		return	FALSE;

	memcpy(pParam, &m_Parameter, sizeof(IMGALIGN_PARAM));
	return	TRUE;
}

BOOL	IMGALIGN::SetParameter(P_IMGALIGN_PARAM pParam)
{
	if (pParam == NULL)
		return	FALSE;

	m_Parameter.m_umPerPixelX		= pParam->m_umPerPixelX;
	m_Parameter.m_umPerPixelY		= pParam->m_umPerPixelY;
	m_Parameter.m_umPerPulseX		= pParam->m_umPerPulseX;
	m_Parameter.m_umPerPulseY		= pParam->m_umPerPulseY;
	m_Parameter.m_AnglePerPulse		= pParam->m_AnglePerPulse;
	m_Parameter.m_DistRotX			= pParam->m_DistRotX;
	m_Parameter.m_DistRotY			= pParam->m_DistRotY;
	m_Parameter.m_bAlignCheckAllCam	= pParam->m_bAlignCheckAllCam;

	for (int i = 0; i < IMGALIGN_NUM_MAX_PTN; i++)
	{
		m_Parameter.m_MotionXPos[i] = pParam->m_MotionXPos[i];	//	리니어 X축 위치
		m_Parameter.m_MotionYPos[i] = pParam->m_MotionYPos[i];	//	리니어 Y축 위치
		m_Parameter.m_MotionTPos[i] = pParam->m_MotionTPos[i];	//	리니어 T축 위치
		m_Parameter.m_LightLevel[i] = pParam->m_LightLevel[i];	//	조명 밝기 값
		m_Parameter.m_Threshold[i] = pParam->m_Threshold[i];	//	패턴 검색시 사용되는 쓰레숄드

		memcpy(m_Parameter.m_PatPara, pParam->m_PatPara, sizeof(typePatPara)*IMGALIGN_NUM_MAX_PTN);
	}

	return	TRUE;
}

VOID	IMGALIGN::SetActiveTracker(BOOL bActivate)
{
	for (int i = 0; i < IMGALIGN_NUM_MAX_PTN; i++)
	{
		m_pWndDisplay[i]->SetActiveTracker(bActivate);
	}
}

VOID	IMGALIGN::SetTrackerPos(int Idx, LPRECT pRect)
{
	if (Idx > IMGALIGN_NUM_MAX_PTN)
		return;

	if (m_Status == IMGALIGN_STATUS_NOT_CREATED)
	{
		return;
	}

	m_pWndDisplay[Idx]->SetActiveTracker(TRUE, pRect);
}

BOOL	IMGALIGN::SaveAlignData()
{
	CString	strTmp;
	for (int i = 0; i < IMGALIGN_NUM_MAX_PTN; i++)
	{
		if (m_bIsPtnLoaded[i])
		{
			strTmp.Format(_T("%s\\%s"), m_strModelPath, IMGALIGN_STR_PTNFILENAME[i]);
			SavePattern(i, strTmp);
		}
	}

	m_ParaManager.SavePara();

	return	TRUE;
}

BOOL	IMGALIGN::LoadAlignData(LPCTSTR strParaPath/*NULL*/)
{
	if (strParaPath == NULL)
	{
		if (m_strModelPath.GetLength() == 0)
			return	FALSE;
	}
	else
	{
		m_strModelPath.Empty();
		m_strModelPath = strParaPath;
		SetLinkConfigData();
	}

	CString	strTmp;
	for (int i = 0; i < IMGALIGN_NUM_MAX_PTN; i++)
	{
		if (m_bIsPtnLoaded[i])
		{
			clear_shape_model(m_PatternID[i]);
			m_bIsPtnLoaded[i] = FALSE;
		}

		strTmp.Format(_T("%s\\%s"), m_strModelPath, IMGALIGN_STR_PTNFILENAME[i]);
		if (LoadPattern(i, strTmp))
		{
			m_bIsPtnLoaded[i] = TRUE;
		}
	}

	m_ParaManager.LoadPara();

	return	TRUE;
}

BOOL	IMGALIGN::GetPtnDiff(int Idx, P_IMGALIGN_PTNDIFF pPtnDiff)
{
	if (Idx > IMGALIGN_NUM_MAX_PTN || pPtnDiff == NULL )
		return	FALSE;

	if (m_Status == IMGALIGN_STATUS_NOT_CREATED)
	{
		return	FALSE;
	}

	memcpy(pPtnDiff, &m_PatternDiff[Idx], sizeof(IMGALIGN_PTNDIFF));


	return	TRUE;
}

IMGALIGN_STATUS	IMGALIGN::GetStatus()
{
	return	m_Status;
}

VOID	IMGALIGN::SetLinkConfigData()
{
	CString	strTmp;
	m_ParaManager.Create();
	m_ParaManager.SetPath(m_strModelPath, IMGALIGN_STR_PARAFILENAME);

	//m_Paramanager.LinkPara(섹션이름, 키이름, 값, 기본값)
	m_ParaManager.LinkPara(IMGALIGN_STR_SECTIONNAME, _T("MICRON_PER_PIXEL_X"), &m_Parameter.m_umPerPixelX, _T("1.9"));
	m_ParaManager.LinkPara(IMGALIGN_STR_SECTIONNAME, _T("MICRON_PER_PIXEL_Y"), &m_Parameter.m_umPerPixelY, _T("1.9"));
	m_ParaManager.LinkPara(IMGALIGN_STR_SECTIONNAME, _T("MICRON_PER_PULSE_X"), &m_Parameter.m_umPerPulseX, _T("0.1"));
	m_ParaManager.LinkPara(IMGALIGN_STR_SECTIONNAME, _T("MICRON_PER_PULSE_Y"), &m_Parameter.m_umPerPulseY, _T("1.0"));
	m_ParaManager.LinkPara(IMGALIGN_STR_SECTIONNAME, _T("MICRON_PER_PULSE_DEGREE"), &m_Parameter.m_AnglePerPulse, _T("0.001"));
	m_ParaManager.LinkPara(IMGALIGN_STR_SECTIONNAME, _T("MICRON_DISTANCE_ALIGNCAM_TO_ROT_CT_X"), &m_Parameter.m_DistRotX, _T("451793.0"));
	m_ParaManager.LinkPara(IMGALIGN_STR_SECTIONNAME, _T("MICRON_DISTANCE_ALIGNCAM_TO_ROT_CT_Y"), &m_Parameter.m_DistRotY, _T("-350160.0"));
	m_ParaManager.LinkPara(IMGALIGN_STR_SECTIONNAME, _T("ALIGN_CHECK_ALL_CAM"), &m_Parameter.m_bAlignCheckAllCam, _T("0"));
	

	for (int i = 0; i < IMGALIGN_NUM_MAX_PTN; i++)
	{
		strTmp.Format(_T("MOTION_X_POS_%d"), i + 1);
		m_ParaManager.LinkPara(IMGALIGN_STR_SECTIONNAME, strTmp, &m_Parameter.m_MotionXPos[i], _T("0"));
		strTmp.Format(_T("MOTION_Y_POS_%d"), i + 1);
		m_ParaManager.LinkPara(IMGALIGN_STR_SECTIONNAME, strTmp, &m_Parameter.m_MotionYPos[i], _T("0"));
		strTmp.Format(_T("MOTION_T_POS_%d"), i + 1);
		m_ParaManager.LinkPara(IMGALIGN_STR_SECTIONNAME, strTmp, &m_Parameter.m_MotionTPos[i], _T("0"));

		strTmp.Format(_T("LIGHT_LEVEL_%d"), i + 1);
		m_ParaManager.LinkPara(IMGALIGN_STR_SECTIONNAME, strTmp, &m_Parameter.m_LightLevel[i], _T("10"));
		strTmp.Format(_T("THRESHOLD_%d"), i + 1);
		m_ParaManager.LinkPara(IMGALIGN_STR_SECTIONNAME, strTmp, &m_Parameter.m_Threshold[i], _T("250"));

		strTmp.Format(_T("SHAPE_TYPE_%d"), i + 1);
		m_ParaManager.LinkPara(IMGALIGN_STR_SECTIONNAME, strTmp, &m_Parameter.m_PatPara[i].m_shType, _T("1"));	//	VISIONHALFUNC_MOD_RECT1
		strTmp.Format(_T("ANGLE_START_%d"), i + 1);
		m_ParaManager.LinkPara(IMGALIGN_STR_SECTIONNAME, strTmp, &m_Parameter.m_PatPara[i].m_AngleStart, _T("-0.39"));
		strTmp.Format(_T("ANGLE_EXTEND_%d"), i + 1);
		m_ParaManager.LinkPara(IMGALIGN_STR_SECTIONNAME, strTmp, &m_Parameter.m_PatPara[i].m_AngleExtend, _T("0.79"));
		strTmp.Format(_T("ANGLE_STEP_%d"), i + 1);
		m_ParaManager.LinkPara(IMGALIGN_STR_SECTIONNAME, strTmp, &m_Parameter.m_PatPara[i].m_AngleStep, _T("0.00175"));

		strTmp.Format(_T("OPTIMIZE_%d"), i + 1);
		m_ParaManager.LinkParaMBS(IMGALIGN_STR_SECTIONNAME, strTmp, m_Parameter.m_PatPara[i].m_Optimize, _T("none"));
		strTmp.Format(_T("MATRIC_%d"), i + 1);
		m_ParaManager.LinkParaMBS(IMGALIGN_STR_SECTIONNAME, strTmp, m_Parameter.m_PatPara[i].m_Matric, _T("ignore_global_polarity"));
		strTmp.Format(_T("SUBPIX_%d"), i + 1);
		m_ParaManager.LinkParaMBS(IMGALIGN_STR_SECTIONNAME, strTmp, m_Parameter.m_PatPara[i].m_SupPix, _T("interpolation"));

		strTmp.Format(_T("OVERLAP_%d"), i + 1);
		m_ParaManager.LinkPara(IMGALIGN_STR_SECTIONNAME, strTmp, &m_Parameter.m_PatPara[i].m_Overlap, _T("0"));
		strTmp.Format(_T("MATCHNUM_%d"), i + 1);
		m_ParaManager.LinkPara(IMGALIGN_STR_SECTIONNAME, strTmp, &m_Parameter.m_PatPara[i].m_MatchNum, _T("1"));
		strTmp.Format(_T("GREED_%d"), i + 1);
		m_ParaManager.LinkPara(IMGALIGN_STR_SECTIONNAME, strTmp, &m_Parameter.m_PatPara[i].m_Greed, _T("0.5"));
		strTmp.Format(_T("SCALE_MIN_%d"), i + 1);
		m_ParaManager.LinkPara(IMGALIGN_STR_SECTIONNAME, strTmp, &m_Parameter.m_PatPara[i].m_ScaleMin, _T("0.9"));
		strTmp.Format(_T("SCALE_MAX_%d"), i + 1);
		m_ParaManager.LinkPara(IMGALIGN_STR_SECTIONNAME, strTmp, &m_Parameter.m_PatPara[i].m_ScaleMax, _T("1.1"));
		strTmp.Format(_T("SCALE_STEP_%d"), i + 1);
		m_ParaManager.LinkPara(IMGALIGN_STR_SECTIONNAME, strTmp, &m_Parameter.m_PatPara[i].m_ScaleStep, _T("0.01"));


		strTmp.Format(_T("PATTERN_BUILD_AREA_LEFT_%d"), i + 1);
		m_ParaManager.LinkPara(IMGALIGN_STR_SECTIONNAME, strTmp, &m_Parameter.m_PatternBuildArea[i].left, _T("440"));
		strTmp.Format(_T("PATTERN_BUILD_AREA_TOP_%d"), i + 1);
		m_ParaManager.LinkPara(IMGALIGN_STR_SECTIONNAME, strTmp, &m_Parameter.m_PatternBuildArea[i].top, _T("280"));
		strTmp.Format(_T("PATTERN_BUILD_AREA_RIGHT_%d"), i + 1);
		m_ParaManager.LinkPara(IMGALIGN_STR_SECTIONNAME, strTmp, &m_Parameter.m_PatternBuildArea[i].right, _T("840"));
		strTmp.Format(_T("PATTERN_BUILD_AREA_BOTTOM_%d"), i + 1);
		m_ParaManager.LinkPara(IMGALIGN_STR_SECTIONNAME, strTmp, &m_Parameter.m_PatternBuildArea[i].bottom, _T("680"));

		strTmp.Format(_T("PATTERN_SEARCH_AREA_LEFT_%d"), i + 1);
		m_ParaManager.LinkPara(IMGALIGN_STR_SECTIONNAME, strTmp, &m_Parameter.m_PatternSearchArea[i].left, _T("100"));
		strTmp.Format(_T("PATTERN_SEARCH_AREA_TOP_%d"), i + 1);
		m_ParaManager.LinkPara(IMGALIGN_STR_SECTIONNAME, strTmp, &m_Parameter.m_PatternSearchArea[i].top, _T("100"));
		strTmp.Format(_T("PATTERN_SEARCH_AREA_RIGHT_%d"), i + 1);
		m_ParaManager.LinkPara(IMGALIGN_STR_SECTIONNAME, strTmp, &m_Parameter.m_PatternSearchArea[i].right, _T("1180"));
		strTmp.Format(_T("PATTERN_SEARCH_AREA_BOTTOM_%d"), i + 1);
		m_ParaManager.LinkPara(IMGALIGN_STR_SECTIONNAME, strTmp, &m_Parameter.m_PatternSearchArea[i].bottom, _T("860"));

		strTmp.Format(_T("PATTERN_CENTER_X_%d"), i + 1);
		m_ParaManager.LinkPara(IMGALIGN_STR_SECTIONNAME, strTmp, &m_Parameter.m_PtnCenterX[i], _T("0"));
		strTmp.Format(_T("PATTERN_CENTER_Y_%d"), i + 1);
		m_ParaManager.LinkPara(IMGALIGN_STR_SECTIONNAME, strTmp, &m_Parameter.m_PtnCenterY[i], _T("0"));
	}
}

BOOL	IMGALIGN::GetPatternID(int Idx, LONGLONG* pPatternID)
{
	if (Idx < 0 || Idx > IMGALIGN_NUM_MAX_PTN)
		return	FALSE;

	*pPatternID = m_PatternID[Idx];

	return	TRUE;
}
