#include "stdAfx.h"
#include <omp.h>
#include "PMInspect.h"
#include "PMInspectDlg.h"
#include "afxdialogex.h"
#include "VisionModule/VisionModule.h"

#ifdef  _UNICODE
#define	_tsprintf	wsprintf
#define _tfopen		_wfopen
#define _tfputs		fputws
#else
#define	_tsprintf	sprintf
#define _tfopen		fopen
#define _tfputs		fputs
#endif


CVisionModule::CVisionModule(long lWidth, long lHeight, int nInspType)
{
	m_nMainInspType = 1;// nInspType;
	//m_RsltRgn = NULL;
	m_bInit = InitVision(lWidth, lHeight);
}


CVisionModule::~CVisionModule(void)
{
	CloseVision();
}

int CVisionModule::limite(int nVal)
{
	int nRslt = 0;

	if (nVal > MAX_VAL)
	{
		nRslt = MAX_VAL;
	}
	else if (nVal < 0)
	{
		nRslt = 0;
	}
	else
	{
		nRslt = nVal;
	}

	return nRslt;
}


BOOL CVisionModule::InitThread()
{
	BOOL rslt = TRUE;
	CString strLog;

	m_bInspThreadStart = TRUE;
	m_bInspThreadEnd = FALSE;

	if (m_nMainInspType == INSPECTION_TYPE_PARALLEL)
	{
		m_bParallelMainInspThreadStart = FALSE;
		m_bParalleMainlInspThreadEnd = FALSE;

		for (int i = 0; i < MAX_PARALLEL_THREAD; i++)
		{
			m_bParallelUnderInspection[i] = FALSE;
		}

		m_pThreadParallelMainInsp = AfxBeginThread(ParallelMainInspThread, this, THREAD_PRIORITY_NORMAL);
		m_pThreadParallelMainInsp->m_bAutoDelete = TRUE;

		if (m_pThreadParallelMainInsp == NULL)
		{
			strLog.Format(_T("Parallel Main Inspection Thread Init Error"));
			rslt = FALSE;
		}
#ifdef THREAD_TEST
		m_pThreadParallelInspRsltSum = NULL;
		m_pThreadParallelInspRsltSum = AfxBeginThread(ParallelInspRsltSumThread, this, THREAD_PRIORITY_NORMAL);
		m_pThreadParallelInspRsltSum->m_bAutoDelete = TRUE;

		if (m_pThreadParallelInspRsltSum == NULL)
		{
			strLog.Format(_T("Parallel Inspection Rslt Sum Thread Init Error"));
			rslt = FALSE;
		}
#endif
	}
	else
	{
		m_pThreadInspection = AfxBeginThread(InspectionThread, this, THREAD_PRIORITY_TIME_CRITICAL);
		m_pThreadInspection->m_bAutoDelete = TRUE;

		SetThreadPriority(m_pThreadInspection->m_hThread, THREAD_PRIORITY_TIME_CRITICAL);

		if (m_pThreadInspection == NULL)
		{
			strLog.Format(_T("Inspection Thread Init Error"));
			rslt = FALSE;
		}
	}

	if (rslt)
	{
		m_pThreadImageSave = AfxBeginThread(ImageSaveThread, this, THREAD_PRIORITY_TIME_CRITICAL);
		m_pThreadImageSave->m_bAutoDelete = TRUE;

		if (m_pThreadImageSave == NULL)
		{
			strLog.Format(_T("Image Save Thread Init Error"));
			rslt = FALSE;
		}
	}


	return rslt;
}

void CVisionModule::InitRslt()
{
	m_bRsltView = FALSE;
	m_bFilterImageView = FALSE;
	m_InspRslt[m_nInsp_Idx].lRslt_num = 0;
	for (int i = 0; i < MAX_INS_Y_RECT_CNT; i++)
	{
		m_nRsltCnt[m_nInsp_Idx][i] = 0;
		m_InspRslt[m_nInsp_Idx].wShiftInspRslt[m_nInsp_Idx][i] = 0;
		m_nInspEnd[m_nInsp_Idx][i] = FALSE;
	}
	//gen_empty_region(&m_InspRslt[m_nInsp_Idx].hShiftInsp_rgn);
	//gen_empty_region(&m_InspRslt[m_nInsp_Idx].hInsp_rgn);
	gen_empty_region(&m_InspRslt[m_nInsp_Idx].hRslt_rgn);

	if (m_nMainInspType == 1)
	{
		m_nLastInspRectY = m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[m_InspPara.InspRectRecipe[m_nInsp_Idx].nYRectCnt - 1].lRectBottom;
	}
	else
	{
		m_nLastInspRectY = m_InspPara.InspRectRecipe[m_nInsp_Idx].nYRectCnt;
	}

	if (m_RsltRgn != NULL)
	{
		m_RsltRgn->clear();

		delete m_RsltRgn;
		m_RsltRgn = NULL;
		m_RsltRgn = new deque<Hobject>;
	}
	else
	{
		m_RsltRgn = new deque<Hobject>;
	}

	if (m_RsltFilterImg != NULL)
	{
		m_RsltFilterImg->clear();

		delete m_RsltFilterImg;
		m_RsltFilterImg = NULL;
		m_RsltFilterImg = new deque<Hobject>;
	}
	else
	{
		m_RsltFilterImg = new deque<Hobject>;
	}

	if (m_RsltImgRect != NULL)
	{
		m_RsltImgRect->clear();
		delete m_RsltImgRect;
		m_RsltImgRect = NULL;
		m_RsltImgRect = new deque<CRect>;
	}
	else
	{
		m_RsltImgRect = new deque<CRect>;
	}

	for (int i = 0; i < m_InspPara.InspRectRecipe[i].nYRectCnt; i++)
	{
		memset(m_InspRslt[i].cRsltText[i], NULL, sizeof(char) * 20);
	}

	for (int i = 0; i < m_PmCntRslt.size(); i++)
	{
		m_PmCntRslt[i].PMRslt.clear();
	}
	m_PmCntRslt.clear();

	memset(m_PmEachRslt, NULL, sizeof(typePMEachRslt) * MAX_INS_Y_RECT_CNT);

#ifdef THREAD_TEST
	for (int i = 0; i < MAX_PARALLEL_INSP_THREAD_CNT; i++)
	{
		if (m_Inspection[i] != NULL)
			delete m_Inspection[i];

		m_Inspection[i] = new CInspection();
	}
#endif

}

BOOL CVisionModule::InitVision(long lWidth, long lHeight)
{
	BOOL rslt = TRUE;
	m_bFilterImageView = FALSE;

	memset(&m_InspPara, NULL, sizeof(typeInspPara));
	memset(&m_InspRslt, NULL, sizeof(typeInspRslt));

	m_lWidth = lWidth;
	m_lHeight = lHeight;

	InitImage();
	InitRslt();

	m_bGrabEnd = FALSE;

	m_bInspectionEnd = TRUE;
	m_bInspectionStart = FALSE;

	rslt = InitThread();

	m_bInit = rslt;

	m_strModelPath.Format(_T("d:\\Database\\MODEL\\Test"));

	if (m_ImageInfo.m_pData != NULL)
	{
		delete[]	m_ImageInfo.m_pData;
		ZeroMemory(&m_ImageInfo, sizeof(MILCTRL_IMG));
	}

	g_MilCtrl.GetImage(&m_ImageInfo, TRUE);	

#ifdef THREAD_TEST
	for (int i = 0; i < MAX_PARALLEL_INSP_THREAD_CNT; i++)
	{
		m_Inspection[i] = NULL;
	}
#endif
	return rslt;
}

BOOL CVisionModule::CloseVision()
{
	BOOL rslt = TRUE;

	m_bGrabEnd = FALSE;

	m_bInspThreadStart = FALSE;
	m_bInspThreadEnd = TRUE;
	m_bImageSaveThreadStart = FALSE;
	m_bImageSaveThreadEnd = TRUE;
	m_bParallelInspRsltSumThreadStart = FALSE;
	m_bParallelInspRsltSumThreadEnd = TRUE;
	Sleep(100);

	if (m_nMainInspType == INSPECTION_TYPE_PARALLEL)
	{
		m_bParallelMainInspThreadStart = FALSE;
		m_bParalleMainlInspThreadEnd = TRUE;

		Sleep(100);
		for (int i = 0; i < MAX_PARALLEL_THREAD; i++)
		{
			if(m_bParallelUnderInspection[i])
			{
				DWORD dwRet = WaitForSingleObject(m_pThreadParallelInsp[i]->m_hThread, 1000);

				if (m_pThreadParallelInsp[i])
				{
					delete m_pThreadParallelInsp[i];
					m_pThreadParallelInsp[i] = NULL;
				}
			}
		}
	}
	else
	{
		if (m_pThreadInspection == NULL)
		{
			delete m_pThreadInspection;
		}
	}

#ifdef THREAD_TEST
	for (int i = 0; i < MAX_PARALLEL_INSP_THREAD_CNT; i++)
	{
		if (m_Inspection[i] != NULL)
		{
			if (m_Inspection[i]->GetUnderInspection())
			{
				if (m_Inspection[i]->WaitEndThread())
					delete m_Inspection[i];
			}
			else
			{
				delete m_Inspection[i];
			}
		}
	}
#endif
	

	for (int i = 0; i < MAX_INS_CNT; i++)
	{
		delete m_nRsltCnt[i];
		delete m_nInspEnd[i];
	}

	if (m_ImageInfo.m_pData != NULL)
	{
		delete[]	m_ImageInfo.m_pData;
		ZeroMemory(&m_ImageInfo, sizeof(MILCTRL_IMG));
	}

	return rslt;
}

UINT CVisionModule::InspectionThread(LPVOID pParam)
{
	CVisionModule *pdlg = (CVisionModule *)pParam;

	while(TRUE)
	{
		if(pdlg->m_bInspThreadStart)
		{
			if(pdlg->GetGrabEnd() && !pdlg->GetInspectionEnd())
			{
				pdlg->SetInspection();
			}
		}

		if(pdlg->m_bInspThreadEnd)
		{
			break;
		}

		Sleep(1);
	}

	return 0;
}

BOOL CVisionModule::InitImage()
{
	BOOL rslt = TRUE;
	Herror herr;
	HTuple wd, ht;

	gen_image_const(&m_HalGrabImage, HTuple("byte"), m_lHeight, m_lWidth);
	
	set_check("~give_error");
	herr = get_image_pointer1(m_HalGrabImage, NULL, NULL, &wd, &ht);
	if(H_MSG_TRUE == herr)
	{

	}
	else
	{
		AfxMessageBox(_T("Halcon Error Occured at get_image_pointer1_00!"));
		rslt = FALSE;
	}

	gen_image_const(&m_HalDispImage, HTuple("byte"), m_lHeight, m_lWidth);

	set_check("~give_error");
	herr = get_image_pointer1(m_HalDispImage, NULL, NULL, &wd, &ht);
	if (H_MSG_TRUE == herr)
	{

	}
	else
	{
		AfxMessageBox(_T("Halcon Error Occured at get_image_pointer1_00!"));
		rslt = FALSE;
	}
	set_check("give_error");

	gen_image_const(&m_HalDispFilterImage, HTuple("byte"), m_lHeight, m_lWidth);

	set_check("~give_error");
	herr = get_image_pointer1(m_HalDispFilterImage, NULL, NULL, &wd, &ht);
	if (H_MSG_TRUE == herr)
	{

	}
	else
	{
		AfxMessageBox(_T("Halcon Error Occured at get_image_pointer1_00!"));
		rslt = FALSE;
	}
	set_check("give_error");

	for (int i = 0; i < MAX_INS_CNT; i++)
	{
		m_nRsltCnt[i] = new int[MAX_INS_Y_RECT_CNT];
		m_nInspEnd[i] = new BOOL[MAX_INS_Y_RECT_CNT];
	}

	return rslt;
}

void CVisionModule::SetInspectionStart()
{
	Hlong row1, row2, col1, col2;
	get_part(m_Disp, &row1, &col1, &row2, &col2);
	SetSetPartHeight(row2);
	SetSetPartSize(col1, row1, col2, row2);

	if (m_InspPara.bInspection)
	{
		SetInspectionStart(TRUE);
		SetInspectionEnd(FALSE);
		InitRslt();
		/*
		if (m_nMainInspType == INSPECTION_TYPE_PARALLEL)
		{
			g_MilCtrl.SetGrabLineCnt(1);
			//SetInspectionStart(FALSE);
			//SetInspectionEnd(TRUE);
		}
		else
		{
			//SetGrabEnd(TRUE);

		}*/
	}

	/*int i = 14000, j = 1000, k = 0, l = 0, n = 0;
	m_nMaxInspRectCnt = 0;
	CRect rect[1000];
	while (TRUE)
	{
		m_nStartInspRectIdx[k] = 0;
		m_nEndInspRectIdx[k] = 0;
		SetInspImageSize(i, i + j, k);

		for (int m = m_nStartInspRectIdx[k]; m < m_nEndInspRectIdx[k]; m++)
		{
			rect[n].left = m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[m].lRectLeft + m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[m].lOffsetX;
			rect[n].right = m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[m].lRectRight + m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[m].lOffsetX;
			rect[n].top = m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[m].lRectTop + m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[m].lOffsetY;
			rect[n].bottom = m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[m].lRectBottom + m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[m].lOffsetY;
			n++;
		}

		i = i + j;
		k++;
		if (k == 5)
		{
			k = 0;
		}

		if (i > 20000)
		{
			break;
		}



	}*/


	//SetGrabEnd(TRUE);
	//SetInspectionStart(TRUE);
	//SetInspectionEnd(FALSE);
}

void CVisionModule::SetInspection()
{
	CString strText, strText1, strText2;
	m_bInspectionStart = TRUE;
	m_bInspectionEnd = FALSE;
	SetGrabEnd(FALSE);
	double start_time, end_time;
	CTime NowTime;
	start_time = GetCurrentTime();
	//ngh
	copy_image(m_HalGrabImage, &m_HalDispImage);
	copy_image(m_HalGrabImage, &m_HalDispFilterImage);
	//PMCntInspection(m_Disp, &m_HalGrabImage, &m_InspRslt[m_nInsp_Idx].lRslt_num, &m_InspRslt[m_nInsp_Idx].hRslt_rgn, m_InspPara.bInspImgSave);
	PMCntInspection(m_Disp, &m_HalGrabImage, m_InspPara.bFilterImgView, m_InspPara.bInspImgSave, TRUE);
	//m_InspRslt[m_nInsp_Idx].hShiftInsp_rgn = SelectInspectionSpace(INSPECTION_SPACE_PM_SHIFT);
	//PMShiftInspection(m_Disp, &m_HalDispFilterImage, m_InspPara.bInspImgSave);
	
	SetGrabEnd(FALSE);
	m_bInspectionEnd = TRUE;
	m_bInspectionStart = FALSE;
	end_time = GetCurrentTime();
	strText.Format(_T("%.3f sec,"), (end_time - start_time) / 1000.0);

	DisplayRslt();
	NowTime = CTime::GetCurrentTime();
	strText1.Format(_T("%02d:%02d:%02d,%s"), NowTime.GetHour(), NowTime.GetMinute(), NowTime.GetSecond(), strText);
	for (int z = 0; z < m_InspPara.InspRectRecipe[m_nInsp_Idx].nYRectCnt; z++)
	{
		strText2.Format(_T("%S"), m_InspRslt[m_nInsp_Idx].cRsltText[z]);
		strText.Format(_T("%s,%s"), strText1, strText2.Mid(9, strText2.GetLength() - 9));
		strText1.Format(_T("%s"), strText);
	}
	WriteRslt(strText);

	//AfxMessageBox(strText);
}

BOOL CVisionModule::SelectInspectionSpace(int nSpaceType)
{
	BOOL rslt = TRUE;
	CString strText;
	int k = 0;
	typeInspPara insPara = GetInspPara();
	Hobject ins_rgn, sub_ins_rgn;
	Hobject hInspImage;
	RECT rect;
	CRect standard_rect;
	CRect frist_rect, last_rect, select_rect, temp_rect, temp_rect2;
	Hlong width = 0, height = 0;
	Hobject hCrop_image, hIlluminate, hThreshold, hRectangle, hEmphasize, hLaplace, hScale, hCrop_image2, hDistritionRgn;
	Hlong lRectCnt;
	HTuple Num, row0, col0, row1, col1;
	Hobject hSelectRect;
	double s_time = 0., e_time = 0., tack_time = 0.;
	long lwidth, lheight, lspace;
	BYTE *bTempFilter2;
	char typ[128];
	long *Histo;
	Histo = new long[sizeof(long) * MAX_VAL];
	memset(Histo, NULL, sizeof(long) * MAX_VAL);
	int nVal = 0;
	//Herror	herr;
	int z = 0;
	BOOL bSelect = FALSE;
	typeInsRect InspRect[2000];
	memset(InspRect, NULL, sizeof(typeInsRect) * 2000);
	//for (int i = 0; i < insPara.nInspTypeCnt; i++)


	int i = m_nInsp_Idx;
	{
		switch (nSpaceType)
		{
		case INSPECTION_SPACE_PM_COUNT_TEMP:
			memset(insPara.InspRectRecipe[i].InspRect, NULL, sizeof(typeInsRect) * MAX_INS_Y_RECT_CNT);
			gen_empty_obj(&ins_rgn);
			gen_empty_obj(&m_InspRslt[i].hInsp_rgn);

			standard_rect.left = (LONG)insPara.InspRectRecipe[i].InspRect[0].lRectLeft;
			standard_rect.top = (LONG)insPara.InspRectRecipe[i].InspRect[0].lRectTop;
			standard_rect.right = (LONG)insPara.InspRectRecipe[i].InspRect[0].lRectRight;
			standard_rect.bottom = (LONG)insPara.InspRectRecipe[i].InspRect[0].lRectBottom;

			width = (standard_rect.right - standard_rect.left);
			height = (standard_rect.bottom - standard_rect.top);

			s_time = GetCurrentTime();
			frist_rect.left = (LONG)insPara.InspRectRecipe[i].StartRect.lRectLeft;
			frist_rect.top = (LONG)insPara.InspRectRecipe[i].StartRect.lRectTop;
			frist_rect.right = (LONG)insPara.InspRectRecipe[i].StartRect.lRectRight;
			frist_rect.bottom = (LONG)insPara.InspRectRecipe[i].StartRect.lRectBottom;

			last_rect.left = (LONG)insPara.InspRectRecipe[i].EndRect.lRectLeft;
			last_rect.top = (LONG)insPara.InspRectRecipe[i].EndRect.lRectTop;
			last_rect.right = (LONG)insPara.InspRectRecipe[i].EndRect.lRectRight;
			last_rect.bottom = (LONG)insPara.InspRectRecipe[i].EndRect.lRectBottom;

			lwidth = frist_rect.right - frist_rect.left;
			lheight = frist_rect.bottom - frist_rect.top;
			lspace = insPara.InspRectRecipe[i].nInspYSpace;

			//crop_rectangle1(m_HalGrabImage, &hCrop_image, (Hlong)frist_rect.top - 20, (Hlong)frist_rect.left, (Hlong)last_rect.bottom + 20, (Hlong)last_rect.right);			
			gen_rectangle1(&hRectangle, (Hlong)frist_rect.top - 50, (Hlong)frist_rect.left, (Hlong)last_rect.bottom + 50, (Hlong)last_rect.right);
			reduce_domain(m_HalGrabImage, hRectangle, &hCrop_image);
			//illuminate(hCrop_image, &hIlluminate, frist_rect.bottom - frist_rect.top, frist_rect.bottom - frist_rect.top, 0.5);
			//emphasize(hCrop_image, &hEmphasize, 10, 10, 2);
			if (mean_image(hCrop_image, &hEmphasize, insPara.InspRectRecipe[i].dLaplaceSigma * 2, insPara.InspRectRecipe[i].dLaplaceSigma * 2) != H_MSG_TRUE)
			{
				break;
			}
			if (laplace_of_gauss(hEmphasize, &hLaplace, insPara.InspRectRecipe[i].dLaplaceSigma) != H_MSG_TRUE)
			{
				break;
			}
			if (scale_image_max(hLaplace, &hScale) != H_MSG_TRUE)
			{
				break;
			}
			//illuminate(hCrop_image2, &hScale, frist_rect.bottom - frist_rect.top, frist_rect.bottom - frist_rect.top, 0.5);

			disp_obj(hScale, m_Disp);

			//SaveImage(&hScale, "d:\\abcd.bmp");

			get_image_pointer1(hScale, (Hlong*)&bTempFilter2, typ, &width, &height);
			
			GetHistgram(bTempFilter2, width, height, Histo);

			nVal = GetHistoMax(Histo);

			if (nVal < 1)
			{
				nVal = 5;
			}

			if (nVal > 250)
			{
				nVal = 245;
			}

			threshold(hScale, &hThreshold, nVal, 255);
			connection(hThreshold, &hThreshold);
			closing_rectangle1(hThreshold, &hThreshold, 3, 3);
			//opening_rectangle1(hThreshold, &hThreshold, 5, 5);
			lRectCnt = 0;
			count_obj(hThreshold, &Num);
			lRectCnt = (long)(Hlong)Num[0];
			lheight -= 5;
			select_shape(hThreshold, &hThreshold, "area", "and", lspace - 5, "max");
			select_shape(hThreshold, &hThreshold, "width", "and", lspace - 5, "max");
			select_shape(hThreshold, &hThreshold, "height", "and", lspace - 5, "max");

			//dilation_rectangle1(hThreshold, &hThreshold, (Hlong)(height / 3), (Hlong)(height / 3));
		
			//erosion_rectangle1(hThreshold, &hThreshold, (Hlong)5, (Hlong)5);
			//closing_rectangle1(hThreshold, &hThreshold, (Hlong)(lheight / 3), (Hlong)((frist_rect.bottom - frist_rect.top) / 3));

			union1(hThreshold, &hThreshold);
			connection(hThreshold, &hThreshold);
			
			lRectCnt = 0;
			count_obj(hThreshold, &Num);
			lRectCnt = (long)(Hlong)Num[0];
			insPara.InspRectRecipe[i].nYRectCnt = lRectCnt;

			lheight += 5;

			for (int y = 0; y < lRectCnt; y++)
			{
				select_obj(hThreshold, &hSelectRect, y + 1);
				smallest_rectangle1(hSelectRect, &row0, &col0, &row1, &col1);

				select_rect.SetRect(frist_rect.left, (long)(Hlong)row0[0] + 0, frist_rect.right, (long)(Hlong)row1[0] - 12);

				if (y > 0)
				{
					if ((select_rect.top - insPara.InspRectRecipe[i].InspRect[y - 1].lRectBottom) <= lspace)
					{
						select_rect.SetRect(frist_rect.left, insPara.InspRectRecipe[i].InspRect[y - 1].lRectBottom + lspace + 5, frist_rect.right, insPara.InspRectRecipe[i].InspRect[y - 1].lRectBottom + lspace + select_rect.Height() - 12);
					}
				}
				InspRect[y].lRectLeft = select_rect.left;
				InspRect[y].lRectTop = select_rect.top;
				InspRect[y].lRectRight = select_rect.right;
				InspRect[y].lRectBottom = select_rect.top + (frist_rect.bottom - frist_rect.top);
			}

			z = 0;
			select_rect.SetRect(InspRect[z].lRectLeft, InspRect[z].lRectTop, InspRect[z].lRectRight, InspRect[z].lRectBottom);
			gen_rectangle1(&hRectangle, (Hlong)select_rect.top, (Hlong)select_rect.left, (Hlong)select_rect.bottom, (Hlong)select_rect.right);
			union2(ins_rgn, hRectangle, &ins_rgn);

			insPara.InspRectRecipe[i].InspRect[z].lRectLeft = InspRect[z].lRectLeft;
			insPara.InspRectRecipe[i].InspRect[z].lRectTop = InspRect[z].lRectTop;
			insPara.InspRectRecipe[i].InspRect[z].lRectRight = InspRect[z].lRectRight;
			insPara.InspRectRecipe[i].InspRect[z].lRectBottom = InspRect[z].lRectBottom;
			z++;
			for (int y = 1; y < 1999; y++)
			{
				if (InspRect[y].lRectLeft == 0 && InspRect[y].lRectTop == 0 && InspRect[y].lRectRight == 0 && InspRect[y].lRectBottom == 0)
				{
					break;
				}
				select_rect.SetRect(InspRect[y].lRectLeft, InspRect[y].lRectTop, InspRect[y].lRectRight, InspRect[y].lRectBottom);
				//temp_rect.SetRect(InspRect[y - 1].lRectLeft, InspRect[y - 1].lRectTop, InspRect[y - 1].lRectRight, InspRect[y - 1].lRectBottom);
				temp_rect.SetRect(insPara.InspRectRecipe[i].InspRect[z - 1].lRectLeft,
					insPara.InspRectRecipe[i].InspRect[z - 1].lRectTop,
					insPara.InspRectRecipe[i].InspRect[z - 1].lRectRight,
					insPara.InspRectRecipe[i].InspRect[z - 1].lRectBottom);

				if (select_rect.Height() < frist_rect.Height())
				{
					continue;
				}

				if (select_rect.IntersectRect(select_rect, temp_rect) == 1)
				{
					continue;
				}

				select_rect.SetRect(InspRect[y].lRectLeft, InspRect[y].lRectTop, InspRect[y].lRectRight, InspRect[y].lRectBottom);
				temp_rect.SetRect(insPara.InspRectRecipe[i].InspRect[z - 1].lRectLeft, 
								insPara.InspRectRecipe[i].InspRect[z - 1].lRectTop,
								insPara.InspRectRecipe[i].InspRect[z - 1].lRectRight, 
								insPara.InspRectRecipe[i].InspRect[z - 1].lRectBottom);
				temp_rect2 = select_rect;

				if (z == 32)
				{
					int abcd = 0;
				}

				if (select_rect.top - temp_rect.bottom > lspace + 10)
				{
					temp_rect2.SetRect(temp_rect.left, temp_rect.bottom + lspace, temp_rect.right, temp_rect.bottom + lspace + frist_rect.Height() - 3);
				}

				if (select_rect.top - temp_rect.bottom < (lspace / 2) + 10)
				{
					temp_rect2.SetRect(temp_rect.left, temp_rect.bottom + lspace, temp_rect.right, temp_rect.bottom + lspace + frist_rect.Height() - 3);
				}

				if (select_rect.Height() > frist_rect.Height() + 5)
				{
					temp_rect2.SetRect(temp_rect.left, temp_rect.bottom + lspace, temp_rect.right, temp_rect.bottom + lspace + frist_rect.Height() - 3);
				}

				if (select_rect.top <= temp_rect.bottom + 5)
				{
					temp_rect2.SetRect(temp_rect.left, temp_rect.bottom + lspace, temp_rect.right, temp_rect.bottom + lspace + frist_rect.Height() - 3);
				}

				gen_rectangle1(&hRectangle, (Hlong)temp_rect2.top, (Hlong)temp_rect2.left, (Hlong)temp_rect2.bottom, (Hlong)temp_rect2.right);
				union2(ins_rgn, hRectangle, &ins_rgn);

				insPara.InspRectRecipe[i].InspRect[z].lRectLeft = temp_rect2.left;
				insPara.InspRectRecipe[i].InspRect[z].lRectTop = temp_rect2.top;
				insPara.InspRectRecipe[i].InspRect[z].lRectRight = temp_rect2.right;
				insPara.InspRectRecipe[i].InspRect[z].lRectBottom = temp_rect2.bottom;
				z++;
			}
			insPara.InspRectRecipe[i].nYRectCnt = z;
			
			copy_obj(ins_rgn, &m_InspRslt[i].hInsp_rgn, 1, -1);
			disp_obj(ins_rgn, m_Disp);
			//SetInspPara(&insPara);
			e_time = GetCurrentTime();
			tack_time = (e_time - s_time) / 1000.0;
			strText.Format(_T("%.4f sec"), tack_time);
			if (m_nMainInspType == 1)
			{
				m_nLastInspRectY = insPara.InspRectRecipe[i].InspRect[insPara.InspRectRecipe[i].nYRectCnt - 1].lRectBottom;
			}
			//AfxMessageBox(strText);
			m_InspPara = insPara;
			break;
		case INSPECTION_SPACE_PM_COUNT:
			for (int j = 0; j < insPara.InspRectRecipe[i].nYRectCnt; j++)
			{
				if (j == 0)
				{
					rect.top = standard_rect.top;
					rect.bottom = standard_rect.bottom;
				}
				else
				{
					rect.top = standard_rect.top + (height * j) + (insPara.InspRectRecipe[i].nInspYSpace * j);
					rect.bottom = standard_rect.bottom + (height * j) + (insPara.InspRectRecipe[i].nInspYSpace * j);
				}
				rect.left = standard_rect.left;
				rect.right = standard_rect.right;

				insPara.InspRectRecipe[i].InspRect[j].lRectLeft = rect.left;
				insPara.InspRectRecipe[i].InspRect[j].lRectTop = rect.top;
				insPara.InspRectRecipe[i].InspRect[j].lRectRight = rect.right;
				insPara.InspRectRecipe[i].InspRect[j].lRectBottom = rect.bottom;

				rect.left += insPara.InspRectRecipe[i].InspRect[j].lOffsetX;
				rect.right += insPara.InspRectRecipe[i].InspRect[j].lOffsetX;
				rect.top += insPara.InspRectRecipe[i].InspRect[j].lOffsetY;
				rect.bottom += insPara.InspRectRecipe[i].InspRect[j].lOffsetY;

				gen_rectangle1(&sub_ins_rgn, rect.top, rect.left, rect.bottom, rect.right);
				union2(sub_ins_rgn, ins_rgn, &ins_rgn);
			}

			copy_obj(ins_rgn, &m_InspRslt[i].hInsp_rgn, 1, -1);
			break;
		case INSPECTION_SPACE_PM_SHIFT:
			char typ[128];
			long lwidth, lheight;
			Hlong width, height;
			BYTE *bFilterRslt;
			copy_image(m_HalGrabImage, &hInspImage);
			get_image_pointer1(hInspImage, (Hlong*)&bFilterRslt, typ, &width, &height);
			lwidth = width;
			lheight = height;

			m_InspPara.InspRectRecipe[m_nInsp_Idx].nShiftInspSpaceX = 10;
			m_InspPara.InspRectRecipe[m_nInsp_Idx].nShiftInspSpaceY = 10;

			for (int j = 0; j < insPara.InspRectRecipe[i].nYRectCnt; j++)
			{
				Hobject hShiftImage[4], hStandardImage;
				BYTE *bShift_ptr[4], *bStandard_ptr;
				BYTE bShiftAvgVal[4], bStandardAvgVal;
				CRect rShiftRect[4];
				int nInspSpace = ((m_InspPara.InspRectRecipe[m_nInsp_Idx].nInspYSpace - (m_InspPara.InspRectRecipe[m_nInsp_Idx].nShiftInspSpaceY * 3)) / 2);

				//기준 이미지
				rect.left = m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[j].lRectLeft + m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[j].lOffsetX;
				rect.right = m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[j].lRectRight + m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[j].lOffsetX;
				rect.top = m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[j].lRectTop + m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[j].lOffsetY;
				rect.bottom = m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[j].lRectBottom + m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[j].lOffsetY;

				if (crop_rectangle1(hInspImage, &hStandardImage, (Hlong)rect.top, (Hlong)rect.left, (Hlong)rect.bottom, (Hlong)rect.right) != H_MSG_TRUE)
				{
					continue;
				}

				width = 0;
				height = 0;
				if (get_image_pointer1(hStandardImage, (Hlong*)&bStandard_ptr, typ, &width, &height) != H_MSG_TRUE)
				{
					continue;
				}

				bStandardAvgVal = GetImageAvgVal(bStandard_ptr, width, height);

				//기준 이미지 왼쪽
				rShiftRect[0].left = m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[j].lRectLeft - m_InspPara.InspRectRecipe[m_nInsp_Idx].nShiftInspSpaceX - (nInspSpace * 2);
				rShiftRect[0].right = m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[j].lRectLeft - m_InspPara.InspRectRecipe[m_nInsp_Idx].nShiftInspSpaceX;
				rShiftRect[0].top = m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[j].lRectTop + m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[j].lOffsetY;
				rShiftRect[0].bottom = m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[j].lRectBottom + m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[j].lOffsetY;

				if (crop_rectangle1(hInspImage, &hShiftImage[0], (Hlong)rShiftRect[0].top, (Hlong)rShiftRect[0].left, (Hlong)rShiftRect[0].bottom, (Hlong)rShiftRect[0].right) != H_MSG_TRUE)
				{
					continue;
				}

				width = 0;
				height = 0;
				if (get_image_pointer1(hShiftImage[0], (Hlong*)&bShift_ptr[0], typ, &width, &height) != H_MSG_TRUE)
				{
					continue;
				}

				bShiftAvgVal[0] = GetImageAvgVal(bShift_ptr[0], width, height);

				//기준 이미지 오른쪽
				rShiftRect[1].left = m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[j].lRectRight + m_InspPara.InspRectRecipe[m_nInsp_Idx].nShiftInspSpaceX;
				rShiftRect[1].right = m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[j].lRectRight + m_InspPara.InspRectRecipe[m_nInsp_Idx].nShiftInspSpaceX + (nInspSpace * 2);
				rShiftRect[1].top = m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[j].lRectTop + m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[j].lOffsetY;
				rShiftRect[1].bottom = m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[j].lRectBottom + m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[j].lOffsetY;

				if (crop_rectangle1(hInspImage, &hShiftImage[1], (Hlong)rShiftRect[1].top, (Hlong)rShiftRect[1].left, (Hlong)rShiftRect[1].bottom, (Hlong)rShiftRect[1].right) != H_MSG_TRUE)
				{
					continue;
				}

				width = 0;
				height = 0;
				if (get_image_pointer1(hShiftImage[1], (Hlong*)&bShift_ptr[1], typ, &width, &height) != H_MSG_TRUE)
				{
					continue;
				}

				bShiftAvgVal[1] = GetImageAvgVal(bShift_ptr[1], width, height);

				//기준 이미지 상단
				rShiftRect[2].left = m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[j].lRectLeft + m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[j].lOffsetX;
				rShiftRect[2].right = m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[j].lRectRight + m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[j].lOffsetX;
				rShiftRect[2].top = m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[j].lRectTop - m_InspPara.InspRectRecipe[m_nInsp_Idx].nShiftInspSpaceY - nInspSpace;
				rShiftRect[2].bottom = m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[j].lRectTop - m_InspPara.InspRectRecipe[m_nInsp_Idx].nShiftInspSpaceY;

				if (crop_rectangle1(hInspImage, &hShiftImage[2], (Hlong)rShiftRect[2].top, (Hlong)rShiftRect[2].left, (Hlong)rShiftRect[2].bottom, (Hlong)rShiftRect[2].right) != H_MSG_TRUE)
				{
					continue;
				}

				width = 0;
				height = 0;
				if (get_image_pointer1(hShiftImage[2], (Hlong*)&bShift_ptr[2], typ, &width, &height) != H_MSG_TRUE)
				{
					continue;
				}

				bShiftAvgVal[2] = GetImageAvgVal(bShift_ptr[2], width, height);

				//기준 이미지 하단
				rShiftRect[3].left = m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[j].lRectLeft + m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[j].lOffsetX;
				rShiftRect[3].right = m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[j].lRectRight + m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[j].lOffsetX;
				rShiftRect[3].top = m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[j].lRectBottom + m_InspPara.InspRectRecipe[m_nInsp_Idx].nShiftInspSpaceY;
				rShiftRect[3].bottom = m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[j].lRectBottom + m_InspPara.InspRectRecipe[m_nInsp_Idx].nShiftInspSpaceY + nInspSpace;

				if (crop_rectangle1(hInspImage, &hShiftImage[3], (Hlong)rShiftRect[3].top, (Hlong)rShiftRect[3].left, (Hlong)rShiftRect[3].bottom, (Hlong)rShiftRect[3].right) != H_MSG_TRUE)
				{
					continue;
				}

				width = 0;
				height = 0;
				if (get_image_pointer1(hShiftImage[3], (Hlong*)&bShift_ptr[3], typ, &width, &height) != H_MSG_TRUE)
				{
					continue;
				}

				bShiftAvgVal[3] = GetImageAvgVal(bShift_ptr[3], width, height);

				for (int k = 0; k < MAX_SHIFT_INSP_CNT; k++)
				{
					m_InspPara.ShiftInspRect[k][j].isInsp = FALSE;
					m_InspPara.ShiftInspRect[k][j].lRectLeft = 0;
					m_InspPara.ShiftInspRect[k][j].lRectRight = 0;
					m_InspPara.ShiftInspRect[k][j].lRectTop = 0;
					m_InspPara.ShiftInspRect[k][j].lRectBottom = 0;
					if (((bStandardAvgVal + SHIFT_IMAGE_AVG_VAL_RANGE) >= bShiftAvgVal[k]) && (bShiftAvgVal[k] >= bStandardAvgVal - SHIFT_IMAGE_AVG_VAL_RANGE))
					{
						Hobject hRectangle, hInspRgn, hShiftReduceImage;

						gen_rectangle1(&hRectangle, (Hlong)rShiftRect[k].top, (Hlong)rShiftRect[k].left, (Hlong)rShiftRect[k].bottom, (Hlong)rShiftRect[k].right);

						copy_obj(hRectangle, &hInspRgn, 1, -1);
						union2(ins_rgn, hInspRgn, &ins_rgn);

						m_InspPara.ShiftInspRect[k][j].isInsp = TRUE;
						m_InspPara.ShiftInspRect[k][j].lRectLeft = rShiftRect[k].left;
						m_InspPara.ShiftInspRect[k][j].lRectRight = rShiftRect[k].right;
						m_InspPara.ShiftInspRect[k][j].lRectTop = rShiftRect[k].top;
						m_InspPara.ShiftInspRect[k][j].lRectBottom = rShiftRect[k].bottom;
					}
				}
			}
			disp_obj(ins_rgn, m_Disp);
			copy_obj(ins_rgn, &m_InspRslt[i].hShiftInsp_rgn, 1, -1);
			break;
		case 3:
			insPara.PMDistributionInspPara.nDistributionXCnt = 4;
			insPara.PMDistributionInspPara.nDistributionYCnt = 2;
			for (int i = 0; i < insPara.InspRectRecipe[m_nInsp_Idx].nYRectCnt; i++)
			{
				CRect rect;
				int nInspWidth, nInspHeight, nXcnt, nYcnt;
				rect.left = insPara.InspRectRecipe[m_nInsp_Idx].InspRect[i].lRectLeft + insPara.InspRectRecipe[m_nInsp_Idx].InspRect[i].lOffsetX;
				rect.right = insPara.InspRectRecipe[m_nInsp_Idx].InspRect[i].lRectRight + insPara.InspRectRecipe[m_nInsp_Idx].InspRect[i].lOffsetX;
				rect.top = insPara.InspRectRecipe[m_nInsp_Idx].InspRect[i].lRectTop + insPara.InspRectRecipe[m_nInsp_Idx].InspRect[i].lOffsetY;
				rect.bottom = insPara.InspRectRecipe[m_nInsp_Idx].InspRect[i].lRectBottom + insPara.InspRectRecipe[m_nInsp_Idx].InspRect[i].lOffsetY;

				nXcnt = insPara.PMDistributionInspPara.nDistributionXCnt;
				nYcnt = insPara.PMDistributionInspPara.nDistributionYCnt;

				nInspWidth = rect.Width() / nXcnt;
				nInspHeight = rect.Height() / nYcnt;

				for (int y = 0; y < nYcnt; y++)
				{
					for (int x = 0; x < nXcnt; x++)
					{
						int nStartX, nStartY, nEndX, nEndY;
						nStartX = x * (nInspWidth) + rect.left;
						nStartY = y * (nInspHeight) + rect.top;

						if(x == 0)
						{
							nStartX = rect.left;
						}
						if (y == 0)
						{
							nStartY = rect.top;
						}

						nEndX = nStartX + nInspWidth - 2;
						nEndY = nStartY + nInspHeight - 2;
						if (x == (nXcnt - 1))
						{
							nEndX = rect.right;
						}

						if (y == (nYcnt - 1))
						{
							nEndY = rect.bottom;
						}

						gen_rectangle1(&hRectangle, nStartY, nStartX, nEndY, nEndX);
						union2(hDistritionRgn, hRectangle, &hDistritionRgn);
					}
				}
			}
			disp_obj(hDistritionRgn, m_Disp);
			copy_obj(hDistritionRgn, &m_InspRslt[i].hDistributionInspRgn, 1, -1);
			break;
		default:
			AfxMessageBox(_T("Error : SelectInspectionSpace Invalid Index!"));
			break;
		}
	}

	delete Histo;

	return TRUE;
}

BOOL CVisionModule::ClipImage(BYTE *src_image, Hobject *rslt_image, CRect rect)
{
	BOOL rslt = TRUE;
	BYTE *by_rslt;
	long lcrop_top = 0, lcrop_left = 0;
	Hlong width, height, lwidth, lheight;
	char typ[128];
	width = rect.Width();
	height = rect.Height();
	Herror	herr;

	set_check("~give_error");
	herr = get_image_pointer1(*rslt_image, (Hlong*)&by_rslt, typ, &lwidth, &lheight);
	set_check("give_error");
	if (H_MSG_TRUE == herr)
	{
	}
	else
	{
		rslt = FALSE;
	}

	if (rslt)
	{
		lcrop_top = rect.top;
		for (int h = 0; h < height; h++)
		{
			lcrop_left = rect.left;
			for (int w = 0; w < width; w++)
			{
				by_rslt[(lcrop_top * lwidth) + lcrop_left] = src_image[(h * width) + w];
				lcrop_left++;
			}
			lcrop_top++;
		}
		set_check("~give_error");
		herr = gen_image1(rslt_image, "byte", lwidth, lheight, (Hlong)by_rslt);
		set_check("give_error");
		if (H_MSG_TRUE == herr)
		{
		}
		else
		{
			rslt = FALSE;
		}
	}

	return rslt;
}

BOOL CVisionModule::ClipImage(Hobject *src_image, Hobject *rslt_image, CRect rect)
{
	/*
	BOOL rslt = TRUE;
	Herror	herr;
	BYTE *by_src, *by_rslt;
	long lcrop_top = 0, lcrop_left = 0;
	Hlong width, height, lwidth, lheight;
	char typ[128];
	width = rect.Width();
	height = rect.Height();
	set_check("~give_error");
	herr = get_image_pointer1(*src_image, (Hlong*)&by_src, typ, &width, &height);
	set_check("give_error");
	if (H_MSG_TRUE == herr)
	{
	}
	else
	{
		rslt = FALSE;
	}

	if (rslt)
	{
		set_check("~give_error");
		herr = get_image_pointer1(*rslt_image, (Hlong*)&by_rslt, typ, &lwidth, &lheight);
		set_check("give_error");
		if (H_MSG_TRUE == herr)
		{
		}
		else
		{
			rslt = FALSE;
		}
	}

	if (rslt)
	{
		lcrop_top = rect.top;
		for (int h = 0; h < rect.Height(); h++)
		{
			lcrop_top = rect.top + h;

			lcrop_left = rect.left;
			for (int w = 0; w < rect.Width(); w++)
			{
				by_rslt[(lcrop_top * lwidth) + lcrop_left] = by_src[(h * width) + w];
				lcrop_left++;
			}
		}
		set_check("~give_error");
		gen_image1(rslt_image, "byte", lwidth, lheight, (Hlong)by_rslt);
		set_check("give_error");
		if (H_MSG_TRUE == herr)
		{
		}
		else
		{
			rslt = FALSE;
		}
	}
	*/
	BOOL rslt = TRUE;
	BYTE *by_rslt;
	long lcrop_top = 0, lcrop_left = 0;
	Hobject temp_image, hRectangle, hReduceImage;
	Hlong width, height, lwidth, lheight;
	char typ[128];
	width = rect.Width();
	height = rect.Height();

	set_check("~give_error");
	if (get_image_pointer1(*rslt_image, (Hlong*)&by_rslt, typ, &lwidth, &lheight) != H_MSG_TRUE)
	{
		set_check("give_error");
		return FALSE;
	}

	if (tile_images_offset(*src_image, &temp_image, rect.top, rect.left, 0, 0, rect.Height(), rect.Width(), lwidth, lheight) != H_MSG_TRUE)
	{
		set_check("give_error");
		return FALSE;
	}

	if (gen_rectangle1(&hRectangle, rect.top, rect.left, rect.bottom, rect.right) != H_MSG_TRUE)
	{
		set_check("give_error");
		return FALSE;
	}

	if (reduce_domain(temp_image, hRectangle, &hReduceImage) != H_MSG_TRUE)
	{
		set_check("give_error");
		return FALSE;
	}

	if (paint_gray(hReduceImage, *rslt_image, rslt_image) != H_MSG_TRUE)
	{
		set_check("give_error");
		return FALSE;
	}
	set_check("give_error");
	return rslt;
}

void CVisionModule::SetImageFilter(long disp, Hobject *image, int nInspIdx, BOOL isDisp, BOOL isSave)
{
	long *Histo;
	long lcrop_top = 0, lcrop_left = 0;
	Hlong width, height, lwidth, lheight;
	Hobject hRsltImage;
	CRect rect;
	char typ[128], cText[512];
	CString strText;
	BYTE *bFilterRslt, *bTempFilter;
	Histo = new long[sizeof(long) * MAX_VAL];
	copy_image(*image, &hRsltImage);
	get_image_pointer1(hRsltImage, (Hlong*)&bFilterRslt, typ, &width, &height);

	lwidth = width;
	lheight = height;

	for (int i = 0; i < m_InspPara.InspRectRecipe[nInspIdx].nYRectCnt; i++)
	{
		Hobject hFilterImage, hIlluminate, hIlluminate1, hFilterImage1, hFilterImage2;
		Hobject hMeanImage, hEmphaImage;
		BYTE *bFilter_ptr;
		int nMaskWidth, nMaskHeight;

		rect.left = m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[i].lRectLeft + m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[i].lOffsetX;
		rect.right = m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[i].lRectRight + m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[i].lOffsetX;
		rect.top = m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[i].lRectTop + m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[i].lOffsetY;
		rect.bottom = m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[i].lRectBottom + m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[i].lOffsetY;

		crop_rectangle1(*image, &hFilterImage, (Hlong)rect.top, (Hlong)rect.left, (Hlong)rect.bottom, (Hlong)rect.right);

		width = 0;
		height = 0;
		get_image_pointer1(hFilterImage, (Hlong*)&bFilter_ptr, typ, &width, &height);

		memset(Histo, NULL, sizeof(long) * MAX_VAL);
		GetHistgram(bFilter_ptr, width, height, Histo);

		m_nTestVal = 0;

		strText.Format(_T("d:\\%d_original.bmp"), m_nTestVal++);
		sprintf_s(cText, strText.GetLength() + 1, "%S", strText);
		SaveImage(&hFilterImage, cText);

		illuminate(hFilterImage, &hIlluminate, 20, 20, 1);
		strText.Format(_T("d:\\%d_illuminate.bmp"), m_nTestVal++);
		sprintf_s(cText, strText.GetLength() + 1, "%S", strText);
		SaveImage(&hIlluminate, cText);

		mean_image(hIlluminate, &hFilterImage1, 3, 3);
		strText.Format(_T("d:\\%d_mean_image.bmp"), m_nTestVal++);
		sprintf_s(cText, strText.GetLength() + 1, "%S", strText);
		SaveImage(&hFilterImage1, cText);

		nMaskWidth = rect.Width();
		nMaskHeight = rect.Width();
		if (rect.Width() > rect.Height())
		{
			nMaskWidth = rect.Height();
			nMaskHeight = rect.Height();
		}

		/*
		ImageFilter(disp, &hFilterImage1, FILTER_TYPE_LAPLACIAN);
		//ImageFilter(disp, &hFilterImage1, FILTER_TYPE_DIFFUSION);
		ImageFilter(disp, &hFilterImage1, FILTER_TYPE_GAMMA_CORRECTION);
		ImageFilter(disp, &hFilterImage1, FILTER_TYPE_CONTRAST_CORRECTION);
		//ImageFilter(disp, &hFilterImage1, FILTER_TYPE_DIFFUSION);

		

		get_image_pointer1(hFilterImage1, (Hlong*)&bTempFilter_ptr, typ, &width, &height);

		memset(Histo, NULL, sizeof(long) * MAX_VAL);
		GetHistgram(bTempFilter_ptr, width, height, Histo);

		illuminate(hFilterImage1, &hIlluminate1, nMaskWidth, nMaskHeight, 1);// GetilluminateFactorCal(Histo));
		strText.Format(_T("d:\\%d_illuminate.bmp"), m_nTestVal++);
		sprintf_s(cText, strText.GetLength() + 1, "%S", strText);
		SaveImage(&hIlluminate1, cText);

		mean_image(hIlluminate1, &hMeanImage, (nMaskWidth / 10), (nMaskHeight / 10));
		strText.Format(_T("d:\\%d_mean_image.bmp"), m_nTestVal++);
		sprintf_s(cText, strText.GetLength() + 1, "%S", strText);
		SaveImage(&hMeanImage, cText);

		emphasize(hMeanImage, &hEmphaImage, (nMaskWidth / 10), (nMaskHeight / 10), 1);
		strText.Format(_T("d:\\%d_emphasize.bmp"), m_nTestVal++);
		sprintf_s(cText, strText.GetLength() + 1, "%S", strText);
		SaveImage(&hEmphaImage, cText);*/

		emphasize(hFilterImage1, &hEmphaImage, (nMaskWidth / 10), (nMaskHeight / 10), 1);
		strText.Format(_T("d:\\%d_empha_image.bmp"), m_nTestVal++);
		sprintf_s(cText, strText.GetLength() + 1, "%S", strText);
		SaveImage(&hEmphaImage, cText);

		laplace_of_gauss(hEmphaImage, &hIlluminate1, 2);
		strText.Format(_T("d:\\%d_laplace_ofgauss_image.bmp"), m_nTestVal++);
		sprintf_s(cText, strText.GetLength() + 1, "%S", strText);
		SaveImage(&hIlluminate1, cText);

		scale_image_max(hIlluminate1, &hFilterImage2);

		get_image_pointer1(hFilterImage2, (Hlong*)&bTempFilter, typ, &width, &height);

		memset(Histo, NULL, sizeof(long) * MAX_VAL);
		GetHistgram(bTempFilter, width, height, Histo);

		int nVal = GetHistoMax(Histo);

		ClipImage(&hFilterImage2, &hRsltImage, rect);
	}

	if (isDisp)
	{
		disp_obj(hRsltImage, disp);
	}

	if (isSave)
	{
		char cPath[512];
		size_t len1 = strlen("ImageFilter.bmp");
		size_t len2 = strlen(m_cSavePath);
		sprintf_s(cPath, len1 + len2 + 1, "%sImageFilter.bmp", m_cSavePath);

		SaveImage(&hRsltImage, cPath);
	}

	if (TRUE)
	{
		char cPath[512];
		size_t len1 = strlen("d:\\ImageFilter.bmp");
		sprintf_s(cPath, len1 + 1, "d:\\ImageFilter.bmp");

		SaveImage(&hRsltImage, cPath);
	}

	delete Histo;
	copy_image(hRsltImage, image);
}

void CVisionModule::SetSaveImagePath(char *cPath)
{
	size_t len = strlen(cPath) + 1;
	sprintf_s(m_cSavePath, len, "%s", cPath);
}

void CVisionModule::ImageFilter(long disp, Hobject *image, int nFilterType)
{
	CString strText;
	Hlong width, height;
	BYTE *by_source;
	Hobject hFilterImage;
	char typ[128];
	double para1 = 0., para2 = 0., para3 = 0.;
	int para4 = 0, para5 = 0;

	copy_image(*image, &hFilterImage);
	get_image_pointer1(hFilterImage, (Hlong*)&by_source, typ, &width, &height);

	switch (nFilterType)
	{
	case FILTER_TYPE_AVERAGE_FILL:
		strText.Format(_T("d:\\%d_FILTER_TYPE_AVERAGE_FILL.bmp"), m_nTestVal);
		para4 = m_InspPara.ImageFilterPara[m_nInsp_Idx].nRangeVal;
		para5 = m_InspPara.ImageFilterPara[m_nInsp_Idx].nFillVal;
		AverageFill(by_source, width, height, para4, para5);
		break;
	case FILTER_TYPE_GAMMA_CORRECTION:
		strText.Format(_T("d:\\%d_FILTER_TYPE_GAMMA_CORRECTION.bmp"), m_nTestVal);
		para1 = 1.f / (float)m_InspPara.ImageFilterPara[m_nInsp_Idx].dGamma;
		GammaCorrection(by_source, width, height, para1);
		break;
	case FILTER_TYPE_CONTRAST_CORRECTION:
		strText.Format(_T("d:\\%d_FILTER_TYPE_CONTRAST_CORRECTION.bmp"), m_nTestVal);
		para4 = GetImageAvgVal(by_source, width, height);;
		para5 = m_InspPara.ImageFilterPara[m_nInsp_Idx].nContrastVal;
		ContrastCorrection(by_source, width, height, para4, para5);
		break;
	case FILTER_TYPE_LAPLACIAN:
		strText.Format(_T("d:\\%d_FILTER_TYPE_LAPLACIAN.bmp"), m_nTestVal);
		para1 = m_InspPara.ImageFilterPara[m_nInsp_Idx].dLaplAlpha = 2;
		LaplacianFilter(by_source, width, height, para1);
		break;
	case FILTER_TYPE_DIFFUSION:
		strText.Format(_T("d:\\%d_FILTER_TYPE_DIFFUSION.bmp"), m_nTestVal);
		para1 = m_InspPara.ImageFilterPara[m_nInsp_Idx].dDiffLambda = 0.1;
		para2 = m_InspPara.ImageFilterPara[m_nInsp_Idx].dDiffK = 4.0;
		para4 = m_InspPara.ImageFilterPara[m_nInsp_Idx].nDiffIter = 10;
		DiffusionFilter(by_source, width, height, para1, para2, para4);
		break;
	case FILTER_TYPE_BLUR:
		strText.Format(_T("d:\\%d_FILTER_TYPE_BLUR.bmp"), m_nTestVal);
		para4 = 1;
		BlurImage(by_source, width, height, para4);
		break;
	}
	m_nTestVal++;
	gen_image1(&hFilterImage, "byte", width, height, (Hlong)by_source);
	copy_image(hFilterImage, image);

	if (FALSE)
	{
		char cPath[512];
		sprintf_s(cPath, strText.GetLength() + 1, "%S", strText);

		SaveImage(&hFilterImage, cPath);
	}
}

void CVisionModule::SetSetPartSize(long x1, long y1, long x2, long y2)
{
	m_PartSize.SetRect(x1, y1, x2, y2);
}

BOOL CVisionModule::PMCntInspection(long disp, Hobject *image, typeThreadInsp ThreadInspPara, BOOL bFilterDisp, BOOL bSave, BOOL bInspImageDisp)
{
	BOOL rslt = TRUE;
	int nRgnOffsetY = ThreadInspPara.InspImageSize.top;
	int nInspIdx = m_nInsp_Idx;
	CRect rect, image_rect;
	Hlong width, height;
	char typ[128];
	long *Histo;
	Histo = new long[MAX_VAL];
	CString strText;
	char cText[512], result0[20];
	Hobject hRsltImage, ins_rgn, hRslt_rgn, hInspImage, hReduce_rect;
	typePMCntRslt PmCntRslt;

	m_InspRslt[m_nInsp_Idx].lRslt_num = 0;

	image_rect = ThreadInspPara.InspImageSize;

	image_rect.top = (ThreadInspPara.InspImageSize.top + ThreadInspPara.InspRect[0].top) - (ThreadInspPara.nInspYSpace / 2);

	if (image_rect.top >= ThreadInspPara.InspImageSize.top)
	{
		image_rect.top -= ThreadInspPara.InspImageSize.top;
		image_rect.bottom -= ThreadInspPara.InspImageSize.top;
		gen_rectangle1(&hReduce_rect, image_rect.top, image_rect.left, image_rect.bottom, image_rect.right - 1);
		reduce_domain(*image, hReduce_rect, &hInspImage);
	}
	else
	{
		//rect = ThreadInspPara.InspImageSize;
		//gen_rectangle1(&hReduce_rect, rect.top - ThreadInspPara.InspImageSize.top, rect.left, rect.bottom - ThreadInspPara.InspImageSize.top, rect.right - 1);
		//reduce_domain(*image, hReduce_rect, &hInspImage);
		//image_rect = rect;
		image_rect.SetRect(ThreadInspPara.InspImageSize.left, (ThreadInspPara.InspImageSize.top + ThreadInspPara.InspRect[0].top) - (ThreadInspPara.nInspYSpace / 2) - nRgnOffsetY, ThreadInspPara.InspImageSize.right, ThreadInspPara.InspImageSize.Height());
		gen_rectangle1(&hReduce_rect, image_rect.top, image_rect.left, image_rect.bottom, image_rect.right - 1);
		reduce_domain(*image, hReduce_rect, &hInspImage);
		//copy_image(*image, &hInspImage);
	}
	image_rect.right -= 1;

	copy_image(hInspImage, &hRsltImage);
	m_nTestVal = 0;
	PmCntRslt.lRsltAllCnt = 0;
	
	for (int i = 0; i < ThreadInspPara.nYRectCnt; i++)
	{
		PmCntRslt.lRsltCnt[i] = 0;
		Hobject hFilterImage, hIlluminate, hIlluminate1, hFilterImage1, hFilterImage2;
		Hobject hMeanImage, hEmphaImage;
		BYTE *bTempFilter;
		int nMaskWidth, nMaskHeight, nMaxNoiseSize;

		rect.left = ThreadInspPara.InspRect[i].left;
		rect.right = ThreadInspPara.InspRect[i].right;
		rect.top = ThreadInspPara.InspRect[i].top;
		rect.bottom = ThreadInspPara.InspRect[i].bottom;

		crop_rectangle1(hInspImage, &hFilterImage, (Hlong)rect.top, (Hlong)rect.left, (Hlong)rect.bottom, (Hlong)rect.right);

		
		if (bSave)
		{
			strText.Format(_T("d:\\%d_original.bmp"), m_nTestVal++);
			sprintf_s(cText, strText.GetLength() + 1, "%S", strText);
			SaveImage(&hFilterImage, cText);
		}

		illuminate(hFilterImage, &hIlluminate, ThreadInspPara.ImageFilterPara.nIllumiSize, ThreadInspPara.ImageFilterPara.nIllumiSize, 1.);
		if (bSave)
		{
			strText.Format(_T("d:\\%d_illuminate.bmp"), m_nTestVal++);
			sprintf_s(cText, strText.GetLength() + 1, "%S", strText);
			SaveImage(&hIlluminate, cText);
		}

		mean_image(hIlluminate, &hFilterImage1, ThreadInspPara.ImageFilterPara.nMeanSize, ThreadInspPara.ImageFilterPara.nMeanSize);
		if (bSave)
		{
			strText.Format(_T("d:\\%d_mean_image.bmp"), m_nTestVal++);
			sprintf_s(cText, strText.GetLength() + 1, "%S", strText);
			SaveImage(&hFilterImage1, cText);
		}

		nMaskWidth = rect.Width();
		nMaskHeight = rect.Width();
		if (rect.Width() > rect.Height())
		{
			nMaskWidth = rect.Height();
			nMaskHeight = rect.Height();
		}

		emphasize(hFilterImage1, &hEmphaImage, (nMaskWidth / 10), (nMaskHeight / 10), 1);
		if (bSave)
		{
			strText.Format(_T("d:\\%d_empha_image.bmp"), m_nTestVal++);
			sprintf_s(cText, strText.GetLength() + 1, "%S", strText);
			SaveImage(&hEmphaImage, cText);
		}

		laplace_of_gauss(hEmphaImage, &hIlluminate1, ThreadInspPara.ImageFilterPara.dLaplaceSigma);
		scale_image_max(hIlluminate1, &hFilterImage2);
		if (bSave)
		{
			strText.Format(_T("d:\\%d_laplace_ofgauss_image.bmp"), m_nTestVal++);
			sprintf_s(cText, strText.GetLength() + 1, "%S", strText);
			SaveImage(&hFilterImage2, cText);
		}

		get_image_pointer1(hFilterImage2, (Hlong*)&bTempFilter, typ, &width, &height);

		memset(Histo, NULL, sizeof(long) * MAX_VAL);
		GetHistgram(bTempFilter, width, height, Histo);

		int nVal = GetHistoMax(Histo);

		ClipImage(&hFilterImage2, &hRsltImage, rect);

		/*if (bSave)
		{
			strText.Format(_T("d:\\%d_filter_image.bmp"), m_nTestVal++);
			sprintf_s(cText, strText.GetLength() + 1, "%S", strText);
			SaveImage(&hRsltImage, cText);
		}*/

		Hobject hRectangle, hRectangle1, hFilterRslt, ImageReduced, hCropImage, Region, filter_rslt_rgn_black, filter_rslt_rgn_white;
		HTuple Num;
		int nThresh = 0;

		gen_rectangle1(&hRectangle, (Hlong)rect.top, (Hlong)rect.left, (Hlong)rect.bottom, (Hlong)rect.right);
		copy_obj(hRectangle, &ins_rgn, 1, -1);

		nMaskWidth = rect.Width() / 10;
		nMaskHeight = rect.Width() / 10;
		nMaxNoiseSize = rect.Width();
		if (rect.Width() > rect.Height())
		{
			nMaskWidth = rect.Height() / 10;
			nMaskHeight = rect.Height() / 10;
			nMaxNoiseSize = rect.Height();
		}

		gen_rectangle1(&hRectangle1, (Hlong)(rect.top + 1), (Hlong)(rect.left + 1), (Hlong)(rect.bottom - 1), (Hlong)(rect.right - 1));
		reduce_domain(hRsltImage, hRectangle1, &ImageReduced);
		
		nThresh = ((long)nVal - ThreadInspPara.PMSearchPara.nThreshHold[0]);
		if (nThresh <= 0)
		{
			nThresh = 1;
		}
		threshold(ImageReduced, &Region, 0, nThresh);
		connection(Region, &filter_rslt_rgn_black);

		nThresh = ((long)nVal + ThreadInspPara.PMSearchPara.nThreshHold[1]);
		if (nThresh >= 255)
		{
			nThresh = 254;
		}
		threshold(ImageReduced, &Region, nThresh, 255);
		connection(Region, &filter_rslt_rgn_white);

		long black_cnt = 0, white_cnt = 0;
		count_obj(filter_rslt_rgn_black, &Num);
		black_cnt = (long)(Hlong)Num[0];
		count_obj(filter_rslt_rgn_white, &Num);
		white_cnt = (long)(Hlong)Num[0];

		/*erosion_circle(filter_rslt_rgn_black, &filter_rslt_rgn_black, HTuple(1));
		erosion_circle(filter_rslt_rgn_white, &filter_rslt_rgn_white, HTuple(1));*/

		if (m_InspPara.PMSearchPara[m_nInsp_Idx].nMargeDistance > 0)
		{
			dilation_circle(filter_rslt_rgn_black, &filter_rslt_rgn_black, HTuple(ThreadInspPara.PMSearchPara.nMargeDistance));

			dilation_circle(filter_rslt_rgn_white, &filter_rslt_rgn_white, HTuple(ThreadInspPara.PMSearchPara.nMargeDistance));
		}

		union1(filter_rslt_rgn_black, &filter_rslt_rgn_black);
		union1(filter_rslt_rgn_white, &filter_rslt_rgn_white);

		connection(filter_rslt_rgn_black, &filter_rslt_rgn_black);
		connection(filter_rslt_rgn_white, &filter_rslt_rgn_white);

		if (m_InspPara.PMSearchPara[m_nInsp_Idx].nMargeDistance > 0)
		{
			closing_circle(filter_rslt_rgn_black, &filter_rslt_rgn_black, HTuple(ThreadInspPara.PMSearchPara.nMargeDistance));

			closing_circle(filter_rslt_rgn_white, &filter_rslt_rgn_white, HTuple(ThreadInspPara.PMSearchPara.nMargeDistance));
		}

		select_shape(filter_rslt_rgn_black, &filter_rslt_rgn_black, HTuple("area"), HTuple("and"), HTuple(2), HTuple('max'));
		select_shape(filter_rslt_rgn_black, &filter_rslt_rgn_black, HTuple("width"), HTuple("and"), HTuple(2), HTuple('max'));
		select_shape(filter_rslt_rgn_black, &filter_rslt_rgn_black, HTuple("height"), HTuple("and"), HTuple(2), HTuple('max'));

		select_shape(filter_rslt_rgn_white, &filter_rslt_rgn_white, HTuple("area"), HTuple("and"), HTuple(2), HTuple('max'));
		select_shape(filter_rslt_rgn_white, &filter_rslt_rgn_white, HTuple("width"), HTuple("and"), HTuple(2), HTuple('max'));
		select_shape(filter_rslt_rgn_white, &filter_rslt_rgn_white, HTuple("height"), HTuple("and"), HTuple(2), HTuple('max'));

		black_cnt = 0, white_cnt = 0;
		count_obj(filter_rslt_rgn_black, &Num);
		black_cnt = (long)(Hlong)Num[0];
		count_obj(filter_rslt_rgn_white, &Num);
		white_cnt = (long)(Hlong)Num[0];

		/*union1(filter_rslt_rgn_black, &filter_rslt_rgn_black);
		union1(filter_rslt_rgn_white, &filter_rslt_rgn_white);
		set_color(disp, HTuple("yellow"));
		disp_obj(filter_rslt_rgn_black, disp);
		set_color(disp, HTuple("blue"));
		disp_obj(filter_rslt_rgn_white, disp);
		connection(filter_rslt_rgn_black, &filter_rslt_rgn_black);
		connection(filter_rslt_rgn_white, &filter_rslt_rgn_white);*/

		Hobject temp_rgn, sel_black_rgn, sel_white_rgn, inter_rgn, sub_tot_rgn, temp_rgn1;
		HTuple Area, row, col, row0, col0, row1, col1;
		CRect temp_rect, rslt_rect;
		BOOL bSum = FALSE;
		long rslt_cnt;
		long nRgn_Cnt = i;

		//black & white 붙어있는 것만 고르기
		////////////////////////////////////////////////////////////////////
		gen_empty_obj(&temp_rgn);
		rslt_cnt = 0;
		m_nRsltCnt[m_nInsp_Idx][ThreadInspPara.nInspRectIdx[i]] = 0;
		if (black_cnt > 0 && white_cnt > 0)
		{
			for (int i = 0; i < white_cnt; i++)
			{
				int nBlackY, nWhiteY1, nWhiteY2, nBlackX, nWhiteX1, nWhiteX2;
				gen_empty_obj(&sel_white_rgn);
				select_obj(filter_rslt_rgn_white, &sel_white_rgn, i + 1);
				dilation_circle(sel_white_rgn, &sel_white_rgn, 10);
				area_center(sel_white_rgn, &Area, &row, &col);
				smallest_rectangle1(sel_white_rgn, &row0, &col0, &row1, &col1);
				temp_rect = CRect((Hlong)col0[0], (Hlong)row0[0], (Hlong)col1[0], (Hlong)row1[0]);
				nWhiteY1 = temp_rect.top;
				nWhiteY2 = temp_rect.bottom;
				nWhiteX1 = temp_rect.left;
				nWhiteX2 = temp_rect.right;
				bSum = FALSE;

				if (rslt_rect.IntersectRect(rect, temp_rect) == 0)
				{
					continue;
				}

				for (int j = 0; j < black_cnt; j++)
				{
					gen_empty_obj(&sel_black_rgn);
					select_obj(filter_rslt_rgn_black, &sel_black_rgn, j + 1);
					dilation_circle(sel_black_rgn, &sel_black_rgn, 10);
					intersection(sel_white_rgn, sel_black_rgn, &inter_rgn);
					area_center(inter_rgn, &Area, &row, &col);

					smallest_rectangle1(sel_black_rgn, &row0, &col0, &row1, &col1);

					//if(ThreadInspPara.PMSearchPara.nPmType == INSPECTION_PM_TYPE_W_DOWN_B_UP)

					switch (ThreadInspPara.PMSearchPara.nPmType)
					{
					case INSPECTION_PM_TYPE_W_UP_B_DOWN:

						break;
					case INSPECTION_PM_TYPE_W_DOWN_B_UP:
						nBlackY = ((Hlong)row0[0] + (Hlong)row1[0]) / 2;
						nBlackX = ((Hlong)col0[0] + (Hlong)col1[0]) / 2;
						nWhiteY1 = temp_rect.CenterPoint().y;
						nWhiteY2 = temp_rect.bottom + 10;
						break;
					case INSPECTION_PM_TYPE_W_LEFT_B_RIGHT:

						break;
					case INSPECTION_PM_TYPE_W_RIGHT_B_LEFT:

						break;
					}


					if (1 < (Hlong)Area[0]
						&& (nWhiteY1 < nBlackY && nBlackY < nWhiteY2)
						&& (nWhiteX1 + 5 <= nBlackX && nBlackX <= nWhiteX2 - 5))
					{
						bSum = TRUE;
						gen_empty_obj(&sub_tot_rgn);
						union1(sel_black_rgn, &sel_black_rgn);
						union1(sel_white_rgn, &sel_white_rgn);
						union2(sel_black_rgn, sel_white_rgn, &sub_tot_rgn);

						erosion_circle(sub_tot_rgn, &sub_tot_rgn, 10);

						union1(temp_rgn, &temp_rgn);
						union2(temp_rgn, sub_tot_rgn, &sub_tot_rgn);
						union1(sub_tot_rgn, &temp_rgn);

						obj_diff(filter_rslt_rgn_black, sel_black_rgn, &filter_rslt_rgn_black);
						connection(filter_rslt_rgn_black, &filter_rslt_rgn_black);
						obj_diff(filter_rslt_rgn_white, sel_white_rgn, &filter_rslt_rgn_white);
						count_obj(filter_rslt_rgn_black, &Num);
						black_cnt = (long)(Hlong)Num[0];
						break;
					}
				}

				if (bSum)
				{
					union1(temp_rgn, &temp_rgn);
				}
			}
		}
		////////////////////////////////////////////////////////////////////
		connection(temp_rgn, &temp_rgn);

		count_obj(temp_rgn, &Num);
		rslt_cnt = 0;
		rslt_cnt = (long)(Hlong)Num[0];

		Hobject sel_rslt_rgn, Ellipses;
		HTuple Ra, Rb, Phi, row_temp1;
		Hlong temp_row;
		long temp_Ra, temp_Rb;


		gen_empty_obj(&temp_rgn1);
		typePMRslt PmRslt;
		int nRsltIdx = 0;
		for (int j = 0; j < rslt_cnt; j++)
		{
			Hobject sel_rslt_rgn, Ellipses;
			gen_empty_obj(&Ellipses);
			gen_empty_obj(&sel_rslt_rgn);
			select_obj(temp_rgn, &sel_rslt_rgn, j + 1);
			area_center(sel_rslt_rgn, &Area, &row, &col);
			elliptic_axis(sel_rslt_rgn, &Ra, &Rb, &Phi);

			temp_Ra = (Hlong)Ra[0];
			temp_Rb = (Hlong)Rb[0];

			if ((temp_Ra < ThreadInspPara.PMSearchPara.nNoiseSize[1]) || (temp_Rb < ThreadInspPara.PMSearchPara.nNoiseSize[2]))
			{
				continue;
			}

			temp_row = (Hlong)(row[0]) + nRgnOffsetY;
			tuple_gen_const(1, temp_row, &row_temp1);
			gen_ellipse(&Ellipses, row_temp1, col, Phi, Ra, Rb);
			concat_obj(temp_rgn1, Ellipses, &temp_rgn1);
		}

		connection(temp_rgn1, &temp_rgn1);

		//작은 노이즈 없에기
		select_shape(temp_rgn1, &temp_rgn1, HTuple("area"), HTuple("and"), HTuple(ThreadInspPara.PMSearchPara.nNoiseSize[0]), HTuple("max"));
		select_shape(temp_rgn1, &temp_rgn1, HTuple("ra"), HTuple("and"), HTuple(ThreadInspPara.PMSearchPara.nNoiseSize[1]), HTuple("max"));
		select_shape(temp_rgn1, &temp_rgn1, HTuple("rb"), HTuple("and"), HTuple(ThreadInspPara.PMSearchPara.nNoiseSize[2]), HTuple("max"));

		count_obj(temp_rgn1, &Num);
		rslt_cnt = 0;
		rslt_cnt = (long)(Hlong)Num[0];

		nRsltIdx = 0;
		for (int j = 0; j < rslt_cnt; j++)
		{
			Hobject sel_rslt_rgn, Ellipses;
			gen_empty_obj(&Ellipses);
			gen_empty_obj(&sel_rslt_rgn);
			select_obj(temp_rgn1, &sel_rslt_rgn, j + 1);
			area_center(sel_rslt_rgn, &Area, &row, &col);
			elliptic_axis(sel_rslt_rgn, &Ra, &Rb, &Phi);

			PmCntRslt.lRsltAllCnt++;
			PmCntRslt.lRsltCnt[i]++;
			PmRslt.dPhi = (double)Phi[0];
			PmRslt.lLongLenght = (double)Ra[0] * ThreadInspPara.dPixelSize;
			PmRslt.lShortLenght = (double)Rb[0] * ThreadInspPara.dPixelSize;
			PmRslt.pCenter.SetPoint((Hlong)col[0], (Hlong)row[0]);
			PmRslt.lLeadNum = ThreadInspPara.nInspRectIdx[i];
			PmRslt.lRsltIdx = nRsltIdx++;
			PmCntRslt.PMRslt.push_back(PmRslt);
		}

		count_obj(temp_rgn1, &Num);
		rslt_cnt = 0;
		rslt_cnt = (long)(Hlong)Num[0];

		m_nRsltCnt[m_nInsp_Idx][ThreadInspPara.nInspRectIdx[i]] = rslt_cnt;

		union1(temp_rgn1, &temp_rgn1);
		union2(hRslt_rgn, temp_rgn1, &hRslt_rgn);

		if (rslt_cnt >= 10)
		{
			PmCntRslt.bRslt[i] = TRUE;
		}
		else
		{
			PmCntRslt.bRslt[i] = FALSE;
		}

		memset(result0, NULL, 20);
		sprintf(result0, "PM Cnt : %d", (int)rslt_cnt);
		sprintf_s(m_InspRslt[nInspIdx].cRsltText[ThreadInspPara.nInspRectIdx[i]], strlen(result0) + 1, "%s", result0);
		m_nInspEnd[m_nInsp_Idx][ThreadInspPara.nInspRectIdx[i]] = TRUE;
	}
	
	delete Histo;

	
	rect.left = ThreadInspPara.InspRect[0].left;
	rect.right = ThreadInspPara.InspRect[0].right - 1;
	rect.top = ThreadInspPara.InspRect[0].top;
	rect.bottom = ThreadInspPara.InspRect[ThreadInspPara.nYRectCnt - 1].bottom;
	Hobject hRsltImage2;
	crop_rectangle1(hRsltImage, &hRsltImage2, rect.top, rect.left, rect.bottom, rect.right);

	rect.top = ThreadInspPara.InspRect[0].top + nRgnOffsetY;
	rect.bottom = ThreadInspPara.InspRect[ThreadInspPara.nYRectCnt - 1].bottom + nRgnOffsetY - 1;

	PmCntRslt.nStartLeadIdx = ThreadInspPara.nInspRectIdx[0];
	PmCntRslt.nEndLeadIdx = ThreadInspPara.nInspRectIdx[ThreadInspPara.nYRectCnt - 1];

	m_VisionInsp_CS.Lock();
	m_RsltRgn->push_back(hRslt_rgn);
	m_RsltFilterImg->push_back(hRsltImage2);
	m_RsltImgRect->push_back(rect);
	m_PmCntRslt.push_back(PmCntRslt);
	
	
	Hlong x0, y0, x1, y1;

	if (bInspImageDisp)
	{
		set_check("~give_error");
		Herror herr;
		herr = set_part(disp, rect.bottom - 20 - m_PartSize.Height(), m_PartSize.left, rect.bottom + 20, m_PartSize.right);
		set_check("give_error");

		if (H_MSG_TRUE == herr)
		{
		}
		else
		{

		}

		disp_obj(m_HalDispImage, disp);
		disp_obj(hRslt_rgn, disp);
		DisplayRslt();

		get_part(disp, &y0, &x0, &y1, &x1);
		set_tposition(disp, y0 + 50, x0 + 50);

		set_font(disp, "-Arial-20-*-*-*-*-*-1-");
	}
	else
	{
		set_tposition(disp, m_PartSize.Height() / 2 - 100, m_PartSize.Width() / 8);
		set_font(disp, "-Arial-40-*-*-*-*-*-1-");
	}

	m_nInspCnt++;
	
	
	set_draw(disp, HTuple("margin"));
	set_color(disp, HTuple("green"));

	strText.Format(_T("Inspecting"));
	for (int i = 0; i < m_nInspCnt; i++)
	{
		strText += _T(".");
	}

	sprintf_s(cText, strText.GetLength() + 1, "%S", strText);

	write_string(disp, HTuple(cText));
	m_VisionInsp_CS.Unlock();
	
	return rslt;
}

BOOL CVisionModule::GetFilterImageView()
{
	m_bRsltView = m_bFilterImageView;
	return m_bFilterImageView; 
}

BOOL CVisionModule::PMCntInspection(long disp, Hobject *image, BOOL bFilterDisp, BOOL bSave, BOOL bInspImageDisp)
{
	BOOL rslt = TRUE;
	BOOL bInspEnd = FALSE;
	m_bUnderInspEnd = bInspEnd;
	int nInspIdx = m_nInsp_Idx;
	CRect rect;
	Hlong width, height;
	char typ[128];
	long *Histo;
	Histo = new long[MAX_VAL];
	CString strText;
	char cText[512], result0[20];
	Hobject hRsltImage, ins_rgn, hRslt_rgn;
	double dRatio = 0.;
	long lRatioHeight = 0;
	m_InspRslt[m_nInsp_Idx].lRslt_num = 0;
	
	copy_image(*image, &hRsltImage);

	if (bInspImageDisp)
	{
		dRatio = m_PartSize.Width() / m_PartSize.Height();
		lRatioHeight = ((m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[0].lRectRight + 20) - (m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[0].lRectLeft - 20)) * dRatio;
	}

	for (int i = 0; i < m_InspPara.InspRectRecipe[nInspIdx].nYRectCnt; i++)
	{
		bInspEnd = m_bUnderInspEnd;
		Hobject hFilterImage, hIlluminate, hIlluminate1, hFilterImage1, hFilterImage2;
		Hobject hMeanImage, hEmphaImage;
		BYTE *bTempFilter;
		int nMaskWidth, nMaskHeight, nMaxNoiseSize;

		rect.left = m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[i].lRectLeft + m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[i].lOffsetX;
		rect.right = m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[i].lRectRight + m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[i].lOffsetX;
		rect.top = m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[i].lRectTop + m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[i].lOffsetY;
		rect.bottom = m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[i].lRectBottom + m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[i].lOffsetY;

		if (bInspImageDisp)
		{
			set_check("~give_error");
			Herror herr;
			//herr = set_part(disp, (rect.bottom) + 100 - m_PartSize.Height(), 0, (rect.bottom) + 100, m_PartSize.Width());
			herr = set_part(disp, (rect.bottom) + 100 - lRatioHeight, rect.left - 20, (rect.bottom) + 100, rect.right + 20);
			set_check("give_error");

			if (H_MSG_TRUE == herr)
			{
			}
			else
			{

			}
		}

		crop_rectangle1(*image, &hFilterImage, (Hlong)rect.top, (Hlong)rect.left, (Hlong)rect.bottom, (Hlong)rect.right);

		m_nTestVal = 0;
		if (bSave)
		{
			strText.Format(_T("d:\\%d_original.bmp"), m_nTestVal++);
			sprintf_s(cText, strText.GetLength() + 1, "%S", strText);
			SaveImage(&hFilterImage, cText);
		}

		illuminate(hFilterImage, &hIlluminate, 20, 20, 1);
		if (bSave)
		{
			strText.Format(_T("d:\\%d_illuminate.bmp"), m_nTestVal++);
			sprintf_s(cText, strText.GetLength() + 1, "%S", strText);
			SaveImage(&hIlluminate, cText);
		}

		mean_image(hIlluminate, &hFilterImage1, 3, 3);
		if (bSave)
		{
			strText.Format(_T("d:\\%d_mean_image.bmp"), m_nTestVal++);
			sprintf_s(cText, strText.GetLength() + 1, "%S", strText);
			SaveImage(&hFilterImage1, cText);
		}

		nMaskWidth = rect.Width();
		nMaskHeight = rect.Width();
		if (rect.Width() > rect.Height())
		{
			nMaskWidth = rect.Height();
			nMaskHeight = rect.Height();
		}

		emphasize(hFilterImage1, &hEmphaImage, (nMaskWidth / 10), (nMaskHeight / 10), 1);
		if (bSave)
		{
			strText.Format(_T("d:\\%d_empha_image.bmp"), m_nTestVal++);
			sprintf_s(cText, strText.GetLength() + 1, "%S", strText);
			SaveImage(&hEmphaImage, cText);
		}

		laplace_of_gauss(hEmphaImage, &hIlluminate1, 2);
		if (bSave)
		{
			strText.Format(_T("d:\\%d_laplace_ofgauss_image.bmp"), m_nTestVal++);
			sprintf_s(cText, strText.GetLength() + 1, "%S", strText);
			SaveImage(&hIlluminate1, cText);
		}

		scale_image_max(hIlluminate1, &hFilterImage2);

		get_image_pointer1(hFilterImage2, (Hlong*)&bTempFilter, typ, &width, &height);

		memset(Histo, NULL, sizeof(long) * MAX_VAL);
		GetHistgram(bTempFilter, width, height, Histo);

		int nVal = GetHistoMax(Histo);

		ClipImage(&hFilterImage2, &hRsltImage, rect);

		if (bFilterDisp)
		{
			disp_obj(hRsltImage, disp);
		}
		else
		{
			disp_obj(*image, disp);
		}

		Hobject hRectangle, hRectangle1, hFilterRslt, ImageReduced, hCropImage, Region, filter_rslt_rgn_black, filter_rslt_rgn_white;
		HTuple Num;

		gen_rectangle1(&hRectangle, (Hlong)rect.top, (Hlong)rect.left, (Hlong)rect.bottom, (Hlong)rect.right);
		copy_obj(hRectangle, &ins_rgn, 1, -1);

		nMaskWidth = rect.Width() / 10;
		nMaskHeight = rect.Width() / 10;
		nMaxNoiseSize = rect.Width();
		if (rect.Width() > rect.Height())
		{
			nMaskWidth = rect.Height() / 10;
			nMaskHeight = rect.Height() / 10;
			nMaxNoiseSize = rect.Height();
		}

		gen_rectangle1(&hRectangle1, (Hlong)(rect.top + (nMaskHeight - 1)), (Hlong)(rect.left + (nMaskWidth - 1)), (Hlong)(rect.bottom - nMaskHeight), (Hlong)(rect.right - nMaskWidth));
		reduce_domain(hRsltImage, hRectangle1, &ImageReduced);

		threshold(ImageReduced, &Region, 0., nVal - m_InspPara.PMSearchPara[m_nInsp_Idx].nThreshHold[0]);
		connection(Region, &filter_rslt_rgn_black);

		threshold(ImageReduced, &Region, nVal + m_InspPara.PMSearchPara[m_nInsp_Idx].nThreshHold[1], 255.);
		connection(Region, &filter_rslt_rgn_white);

		long black_cnt = 0, white_cnt = 0;
		count_obj(filter_rslt_rgn_black, &Num);
		black_cnt = (long)(Hlong)Num[0];
		count_obj(filter_rslt_rgn_white, &Num);
		white_cnt = (long)(Hlong)Num[0];

		erosion_circle(filter_rslt_rgn_black, &filter_rslt_rgn_black, HTuple(1));
		erosion_circle(filter_rslt_rgn_white, &filter_rslt_rgn_white, HTuple(1));

		if (m_InspPara.PMSearchPara[m_nInsp_Idx].nMargeDistance > 0)
		{
			dilation_circle(filter_rslt_rgn_black, &filter_rslt_rgn_black, HTuple(m_InspPara.PMSearchPara[m_nInsp_Idx].nMargeDistance));

			dilation_circle(filter_rslt_rgn_white, &filter_rslt_rgn_white, HTuple(m_InspPara.PMSearchPara[m_nInsp_Idx].nMargeDistance));
		}

		union1(filter_rslt_rgn_black, &filter_rslt_rgn_black);
		union1(filter_rslt_rgn_white, &filter_rslt_rgn_white);

		connection(filter_rslt_rgn_black, &filter_rslt_rgn_black);
		connection(filter_rslt_rgn_white, &filter_rslt_rgn_white);

		if (m_InspPara.PMSearchPara[m_nInsp_Idx].nMargeDistance > 0)
		{
			closing_circle(filter_rslt_rgn_black, &filter_rslt_rgn_black, HTuple(m_InspPara.PMSearchPara[m_nInsp_Idx].nMargeDistance));

			closing_circle(filter_rslt_rgn_white, &filter_rslt_rgn_white, HTuple(m_InspPara.PMSearchPara[m_nInsp_Idx].nMargeDistance));
		}

		select_shape(filter_rslt_rgn_black, &filter_rslt_rgn_black, HTuple("area"), HTuple("and"), HTuple(m_InspPara.PMSearchPara[m_nInsp_Idx].nNoiseSize[0] / 2), HTuple('max'));
		select_shape(filter_rslt_rgn_black, &filter_rslt_rgn_black, HTuple("width"), HTuple("and"), HTuple(m_InspPara.PMSearchPara[m_nInsp_Idx].nNoiseSize[1] / 2), HTuple('max'));
		select_shape(filter_rslt_rgn_black, &filter_rslt_rgn_black, HTuple("height"), HTuple("and"), HTuple(m_InspPara.PMSearchPara[m_nInsp_Idx].nNoiseSize[2] / 2), HTuple('max'));

		select_shape(filter_rslt_rgn_white, &filter_rslt_rgn_white, HTuple("area"), HTuple("and"), HTuple(m_InspPara.PMSearchPara[m_nInsp_Idx].nNoiseSize[0] / 2), HTuple('max'));
		select_shape(filter_rslt_rgn_white, &filter_rslt_rgn_white, HTuple("width"), HTuple("and"), HTuple(m_InspPara.PMSearchPara[m_nInsp_Idx].nNoiseSize[1] / 2), HTuple('max'));
		select_shape(filter_rslt_rgn_white, &filter_rslt_rgn_white, HTuple("height"), HTuple("and"), HTuple(m_InspPara.PMSearchPara[m_nInsp_Idx].nNoiseSize[2] / 2), HTuple('max'));

		black_cnt = 0, white_cnt = 0;
		count_obj(filter_rslt_rgn_black, &Num);
		black_cnt = (long)(Hlong)Num[0];
		count_obj(filter_rslt_rgn_white, &Num);
		white_cnt = (long)(Hlong)Num[0];

		/*set_color(disp, HTuple("yellow"));
		disp_obj(filter_rslt_rgn_black, disp);
		set_color(disp, HTuple("blue"));
		disp_obj(filter_rslt_rgn_white, disp);*/

		Hobject temp_rgn, sel_black_rgn, sel_white_rgn, inter_rgn, sub_tot_rgn, temp_rgn1;
		HTuple Area, row, col, row0, col0, row1, col1;
		CRect temp_rect, rslt_rect;
		BOOL bSum = FALSE;
		long rslt_cnt;
		long nRgn_Cnt = i;

		//black & white 붙어있는 것만 고르기
		////////////////////////////////////////////////////////////////////
		gen_empty_obj(&temp_rgn);
		rslt_cnt = 0;
		m_nRsltCnt[m_nInsp_Idx][nRgn_Cnt] = 0;
		if (black_cnt > 0 && white_cnt > 0)
		{
			for (int i = 0; i < black_cnt - 1; i++)
			{
				int nBlackX, nWhiteX;
				gen_empty_obj(&sel_black_rgn);
				select_obj(filter_rslt_rgn_black, &sel_black_rgn, i + 1);
				dilation_circle(sel_black_rgn, &sel_black_rgn, 3);
				area_center(sel_black_rgn, &Area, &row, &col);
				smallest_rectangle1(sel_black_rgn, &row0, &col0, &row1, &col1);
				temp_rect = CRect((Hlong)col0[0], (Hlong)row0[0], (Hlong)col1[0], (Hlong)row1[0]);
				nBlackX = temp_rect.CenterPoint().x;
				bSum = FALSE;

				if (rslt_rect.IntersectRect(rect, temp_rect) == 0)
				{
					continue;
				}

				for (int j = 0; j < white_cnt; j++)
				{
					gen_empty_obj(&sel_white_rgn);
					select_obj(filter_rslt_rgn_white, &sel_white_rgn, j + 1);
					dilation_circle(sel_white_rgn, &sel_white_rgn, 3);
					intersection(sel_black_rgn, sel_white_rgn, &inter_rgn);
					area_center(inter_rgn, &Area, &row, &col);

					smallest_rectangle1(sel_white_rgn, &row0, &col0, &row1, &col1);
					nWhiteX = ((Hlong)col0[0] + (Hlong)col1[0]) / 2;

					if (1 < (Hlong)Area[0] && nBlackX > nWhiteX)
					{
						bSum = TRUE;
						gen_empty_obj(&sub_tot_rgn);

						obj_diff(filter_rslt_rgn_black, sel_black_rgn, &filter_rslt_rgn_black);
						obj_diff(filter_rslt_rgn_white, sel_white_rgn, &filter_rslt_rgn_white);

						union2(sel_black_rgn, sel_white_rgn, &sub_tot_rgn);

						erosion_circle(sub_tot_rgn, &sub_tot_rgn, 3);

						union2(temp_rgn, sub_tot_rgn, &sub_tot_rgn);
						union1(sub_tot_rgn, &temp_rgn);
					}
				}

				if (bSum)
				{
					union1(temp_rgn, &temp_rgn);
				}
			}
			////////////////////////////////////////////////////////////////////
			connection(temp_rgn, &temp_rgn);

			//작은 노이즈 없에기
			//select_shape(temp_rgn, &temp_rgn, HTuple("area"), HTuple("and"), HTuple(m_InspPara.PMSearchPara[m_nInsp_Idx].nNoiseSize[0]), HTuple(nMaxNoiseSize * 2));
			select_shape(temp_rgn, &temp_rgn, HTuple("width"), HTuple("and"), HTuple(m_InspPara.PMSearchPara[m_nInsp_Idx].nNoiseSize[1]), HTuple(nMaxNoiseSize));
			select_shape(temp_rgn, &temp_rgn, HTuple("height"), HTuple("and"), HTuple(m_InspPara.PMSearchPara[m_nInsp_Idx].nNoiseSize[2]), HTuple(nMaxNoiseSize));

			count_obj(temp_rgn, &Num);
			rslt_cnt = 0;
			rslt_cnt = (long)(Hlong)Num[0];

			Hobject sel_rslt_rgn, Ellipses;
			HTuple Ra, Rb, Phi;

			gen_empty_obj(&temp_rgn1);
			for (int j = 0; j < rslt_cnt; j++)
			{
				gen_empty_obj(&sel_rslt_rgn);
				select_obj(temp_rgn, &sel_rslt_rgn, j + 1);
				area_center(sel_rslt_rgn, &Area, &row, &col);
				elliptic_axis(sel_rslt_rgn, &Ra, &Rb, &Phi);
				gen_ellipse(&Ellipses, row, col, Phi, Ra, Rb);
				union2(temp_rgn1, Ellipses, &temp_rgn1);
			}

			connection(temp_rgn1, &temp_rgn1);
			count_obj(temp_rgn1, &Num);
			rslt_cnt = 0;
			rslt_cnt = (long)(Hlong)Num[0];

			m_nRsltCnt[m_nInsp_Idx][nRgn_Cnt] = rslt_cnt;
			m_InspRslt[m_nInsp_Idx].lRslt_num += rslt_cnt;
			union1(temp_rgn1, &temp_rgn1);
			union2(hRslt_rgn, temp_rgn1, &hRslt_rgn);
			m_nInspEnd[m_nInsp_Idx][nRgn_Cnt] = TRUE;
			copy_obj(hRslt_rgn, &m_InspRslt[m_nInsp_Idx].hRslt_rgn, 1, -1);
			DisplayRslt();
		}

		if (disp)
		{
			set_font(disp, "-Arial-12-*-*-*-*-*-1-");
			set_draw(disp, HTuple("margin"));
			memset(result0, NULL, 20);
			sprintf(result0, "PM Cnt : %d", (int)rslt_cnt);

			set_color(disp, HTuple("yellow"));
			disp_obj(ins_rgn, disp);

			if (rslt_cnt >= 10)
			{
				set_color(disp, HTuple("green"));
			}
			else
			{
				set_color(disp, HTuple("red"));
			}

			if (rslt_cnt > 0)
				disp_obj(temp_rgn1, disp);

			set_tposition(disp, rect.bottom + 5, rect.left + 5);
			write_string(disp, HTuple(result0));

			sprintf_s(m_InspRslt[m_nInsp_Idx].cRsltText[i], strlen(result0) + 1, "%s", result0);
		}

		if (bInspEnd)
		{
			break;
		}
	}
	copy_obj(hRslt_rgn, &m_InspRslt[m_nInsp_Idx].hRslt_rgn, 1, -1);
	delete Histo;
	copy_image(hRsltImage, &m_HalDispFilterImage);
	return rslt;
}

BOOL CVisionModule::PMCntInspection(long disp, Hobject *image, long *rslt_num, Hobject *rslt_rgn, BOOL bSave)
{
	BOOL rslt = TRUE;
	BOOL bSum = FALSE;
	CString strLog;
	Hobject Save_Image;
	Hobject hInspImage, hFilterRslt, hRslt_rgn;

	HTuple Num, Area, row, col, row0, col0, row1, col1, Ra, Rb, Phi;

	CRect rslt_rect, temp_rect, rect;
	
	Hlong width = 0, height = 0;
	
	char typ[128];
	char result0[20];

	int nRgn_Cnt = 0;

	long rslt_cnt = 0, black_cnt = 0, white_cnt = 0;
	long lwidth = 0, lheight = 0, lcrop_top = 0, lcrop_left = 0;
	long *Histo;

	Histo = new long[sizeof(long) * MAX_VAL];
	*rslt_num = 0;

	set_check("~give_error");

	gen_empty_obj(rslt_rgn);
	*rslt_num = 0;
	//ins_rgn = GetRslt().hInsp_rgn;

	copy_image(*image, &hInspImage);
	copy_image(*image, &Save_Image);

	SetImageFilter(disp, &hInspImage, m_nInsp_Idx, GetFilterView(), bSave);

	gen_empty_obj(&hFilterRslt);
	//gen_image1(&hFilterRslt, "byte", lwidth, lheight, (Hlong)bFilterRslt);
	copy_image(hInspImage, &hFilterRslt);
	copy_image(hFilterRslt, &m_HalDispFilterImage);

	if (GetFilterView())
	{
		copy_image(*GetFilterImage(),GetDispImage());
		disp_obj(*GetDispImage(), disp);
	}
	else
	{
		copy_image(*GetGrabImage(), GetDispImage());
	}

	for (int i = 0; i < m_InspPara.InspRectRecipe[m_nInsp_Idx].nYRectCnt; i++)
	{
		//Sleep(1000);
		//for (int j = 0; j < m_InspPara.InspRectRecipe.nXRectCnt; j++)
		{
			Hobject hRectangle1, ImageReduced1, hRectangle, ImageReduced, Region, temp_rgn, temp_rgn1, filter_rslt_rgn_black, filter_rslt_rgn_white, filter_rslt_rgn, ins_rgn;
			Hobject sel_white_rgn, sel_black_rgn, inter_rgn, sub_tot_rgn, hCropImage;
			Hobject sel_rslt_rgn, Ellipses;
			int nMaskWidth, nMaskHeight, nMaxNoiseSize;
			double dAvgThreshHold;
			BYTE *by_crop;

			//검사 영역 추출
			/////////////////////////////////////////////////////
			rect.left = m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[i].lRectLeft + m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[i].lOffsetX;
			rect.right = m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[i].lRectRight + m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[i].lOffsetX;
			rect.top = m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[i].lRectTop + m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[i].lOffsetY;
			rect.bottom = m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[i].lRectBottom + m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[i].lOffsetY;

			gen_rectangle1(&hRectangle, (Hlong)rect.top, (Hlong)rect.left, (Hlong)rect.bottom, (Hlong)rect.right);
			copy_obj(hRectangle, &ins_rgn, 1, -1);
			
			nMaskWidth = rect.Width() / 10;
			nMaskHeight = rect.Width() / 10;
			nMaxNoiseSize = rect.Width();
			if (rect.Width() > rect.Height())
			{
				nMaskWidth = rect.Height() / 10;
				nMaskHeight = rect.Height() / 10;
				nMaxNoiseSize = rect.Height();
			}

			gen_rectangle1(&hRectangle1, (Hlong)(rect.top + (nMaskHeight - 1)), (Hlong)(rect.left + (nMaskWidth - 1)), (Hlong)(rect.bottom - nMaskHeight), (Hlong)(rect.right - nMaskWidth));
			reduce_domain(hFilterRslt, hRectangle1, &ImageReduced);

			crop_rectangle1(hFilterRslt, &hCropImage, (Hlong)(rect.top + (nMaskHeight - 1)), (Hlong)(rect.left + (nMaskWidth - 1)), (Hlong)(rect.bottom - nMaskHeight), (Hlong)(rect.right - nMaskWidth));
			get_image_pointer1(hCropImage, (Hlong*)&by_crop, typ, &width, &height);
			dAvgThreshHold = (double)GetImageAvgVal(by_crop, width, height);

			threshold(ImageReduced, &Region, 0., m_InspPara.PMSearchPara[m_nInsp_Idx].nThreshHold[0]);
			connection(Region, &filter_rslt_rgn_black);

			threshold(ImageReduced, &Region, m_InspPara.PMSearchPara[m_nInsp_Idx].nThreshHold[1], 255.);
			connection(Region, &filter_rslt_rgn_white);

			black_cnt = 0, white_cnt = 0;
			count_obj(filter_rslt_rgn_black, &Num);
			black_cnt = (long)(Hlong)Num[0];
			count_obj(filter_rslt_rgn_white, &Num);
			white_cnt = (long)(Hlong)Num[0];
			
			erosion_circle(filter_rslt_rgn_black, &filter_rslt_rgn_black, HTuple(1));
			erosion_circle(filter_rslt_rgn_white, &filter_rslt_rgn_white, HTuple(1));
			
			if (m_InspPara.PMSearchPara[m_nInsp_Idx].nMargeDistance > 0)
			{
				dilation_circle(filter_rslt_rgn_black, &filter_rslt_rgn_black, HTuple(m_InspPara.PMSearchPara[m_nInsp_Idx].nMargeDistance));

				dilation_circle(filter_rslt_rgn_white, &filter_rslt_rgn_white, HTuple(m_InspPara.PMSearchPara[m_nInsp_Idx].nMargeDistance));
			}

			union1(filter_rslt_rgn_black, &filter_rslt_rgn_black);
			union1(filter_rslt_rgn_white, &filter_rslt_rgn_white);

			connection(filter_rslt_rgn_black, &filter_rslt_rgn_black);
			connection(filter_rslt_rgn_white, &filter_rslt_rgn_white);

			if (m_InspPara.PMSearchPara[m_nInsp_Idx].nMargeDistance > 0)
			{
				closing_circle(filter_rslt_rgn_black, &filter_rslt_rgn_black, HTuple(m_InspPara.PMSearchPara[m_nInsp_Idx].nMargeDistance));

				closing_circle(filter_rslt_rgn_white, &filter_rslt_rgn_white, HTuple(m_InspPara.PMSearchPara[m_nInsp_Idx].nMargeDistance));
			}

			select_shape(filter_rslt_rgn_black, &filter_rslt_rgn_black, HTuple("area"), HTuple("and"), HTuple(m_InspPara.PMSearchPara[m_nInsp_Idx].nNoiseSize[0] / 2), HTuple('max'));
			select_shape(filter_rslt_rgn_black, &filter_rslt_rgn_black, HTuple("width"), HTuple("and"), HTuple(m_InspPara.PMSearchPara[m_nInsp_Idx].nNoiseSize[1] / 2), HTuple('max'));
			select_shape(filter_rslt_rgn_black, &filter_rslt_rgn_black, HTuple("height"), HTuple("and"), HTuple(m_InspPara.PMSearchPara[m_nInsp_Idx].nNoiseSize[2] / 2), HTuple('max'));

			select_shape(filter_rslt_rgn_white, &filter_rslt_rgn_white, HTuple("area"), HTuple("and"), HTuple(m_InspPara.PMSearchPara[m_nInsp_Idx].nNoiseSize[0] / 2), HTuple('max'));
			select_shape(filter_rslt_rgn_white, &filter_rslt_rgn_white, HTuple("width"), HTuple("and"), HTuple(m_InspPara.PMSearchPara[m_nInsp_Idx].nNoiseSize[1] / 2), HTuple('max'));
			select_shape(filter_rslt_rgn_white, &filter_rslt_rgn_white, HTuple("height"), HTuple("and"), HTuple(m_InspPara.PMSearchPara[m_nInsp_Idx].nNoiseSize[2] / 2), HTuple('max'));

			

			black_cnt = 0, white_cnt = 0;
			count_obj(filter_rslt_rgn_black, &Num);
			black_cnt = (long)(Hlong)Num[0];
			count_obj(filter_rslt_rgn_white, &Num);
			white_cnt = (long)(Hlong)Num[0];
			
			/*set_color(disp, HTuple("yellow"));
			disp_obj(filter_rslt_rgn_black, disp);
			set_color(disp, HTuple("blue"));
			disp_obj(filter_rslt_rgn_white, disp);*/

			//black & white 붙어있는 것만 고르기
			////////////////////////////////////////////////////////////////////
			gen_empty_obj(&temp_rgn);
			rslt_cnt = 0;
			m_nRsltCnt[m_nInsp_Idx][nRgn_Cnt] = 0;
			if (black_cnt > 0 && white_cnt > 0)
			{
				for (int i = 0; i < black_cnt - 1; i++)
				{
					int nBlackX, nWhiteX;
					gen_empty_obj(&sel_black_rgn);
					select_obj(filter_rslt_rgn_black, &sel_black_rgn, i + 1);
					dilation_circle(sel_black_rgn, &sel_black_rgn, 3);
					area_center(sel_black_rgn, &Area, &row, &col);
					smallest_rectangle1(sel_black_rgn, &row0, &col0, &row1, &col1);
					temp_rect = CRect((Hlong)col0[0], (Hlong)row0[0], (Hlong)col1[0], (Hlong)row1[0]);
					nBlackX = temp_rect.CenterPoint().x;
					bSum = FALSE;

					if (rslt_rect.IntersectRect(rect, temp_rect) == 0)
					{
						continue;
					}

					for (int j = 0; j < white_cnt; j++)
					{
						gen_empty_obj(&sel_white_rgn);
						select_obj(filter_rslt_rgn_white, &sel_white_rgn, j + 1);
						dilation_circle(sel_white_rgn, &sel_white_rgn, 3);
						intersection(sel_black_rgn, sel_white_rgn, &inter_rgn);
						area_center(inter_rgn, &Area, &row, &col);

						smallest_rectangle1(sel_white_rgn, &row0, &col0, &row1, &col1);
						nWhiteX = ((Hlong)col0[0] + (Hlong)col1[0]) / 2;

						if (1 < (Hlong)Area[0] && nBlackX > nWhiteX)
						{
							bSum = TRUE;
							gen_empty_obj(&sub_tot_rgn);

							obj_diff(filter_rslt_rgn_black, sel_black_rgn, &filter_rslt_rgn_black);
							obj_diff(filter_rslt_rgn_white, sel_white_rgn, &filter_rslt_rgn_white);
							
							union2(sel_black_rgn, sel_white_rgn, &sub_tot_rgn);

							erosion_circle(sub_tot_rgn, &sub_tot_rgn, 3);

							union2(temp_rgn, sub_tot_rgn, &sub_tot_rgn);
							union1(sub_tot_rgn, &temp_rgn);
						}
					}

					if (bSum)
					{
						union1(temp_rgn, &temp_rgn);
					}
				}
				////////////////////////////////////////////////////////////////////
				connection(temp_rgn, &temp_rgn);

				//작은 노이즈 없에기
				//select_shape(temp_rgn, &temp_rgn, HTuple("area"), HTuple("and"), HTuple(m_InspPara.PMSearchPara[m_nInsp_Idx].nNoiseSize[0]), HTuple(nMaxNoiseSize * 2));
				select_shape(temp_rgn, &temp_rgn, HTuple("width"), HTuple("and"), HTuple(m_InspPara.PMSearchPara[m_nInsp_Idx].nNoiseSize[1]), HTuple(nMaxNoiseSize));
				select_shape(temp_rgn, &temp_rgn, HTuple("height"), HTuple("and"), HTuple(m_InspPara.PMSearchPara[m_nInsp_Idx].nNoiseSize[2]), HTuple(nMaxNoiseSize));

				count_obj(temp_rgn, &Num);
				rslt_cnt = 0;
				rslt_cnt = (long)(Hlong)Num[0];

				gen_empty_obj(&temp_rgn1);
				for (int j = 0; j < rslt_cnt; j++)
				{
					gen_empty_obj(&sel_rslt_rgn);
					select_obj(temp_rgn, &sel_rslt_rgn, j + 1);
					area_center(sel_rslt_rgn, &Area, &row, &col);
					elliptic_axis(sel_rslt_rgn, &Ra, &Rb, &Phi);
					gen_ellipse(&Ellipses, row, col, Phi, Ra, Rb);
					union2(temp_rgn1, Ellipses, &temp_rgn1);
				}

				connection(temp_rgn1, &temp_rgn1);
				count_obj(temp_rgn1, &Num);
				rslt_cnt = 0;
				rslt_cnt = (long)(Hlong)Num[0];
				*rslt_num += rslt_cnt;

				m_nRsltCnt[m_nInsp_Idx][nRgn_Cnt++] = rslt_cnt;

				union1(temp_rgn1, &temp_rgn1);
				union2(hRslt_rgn, temp_rgn1, &hRslt_rgn);
			
				if (FALSE)
				{
					overpaint_region(Save_Image, temp_rgn1, 255, "margin");
					overpaint_region(Save_Image, ins_rgn, 180, "margin");
				}
			}

			if (FALSE)
			{
				overpaint_region(Save_Image, filter_rslt_rgn_black, 255, "margin");
				overpaint_region(Save_Image, filter_rslt_rgn_white, 0, "margin");
			}

			if (disp)
			{
				set_font(disp, "-Arial-12-*-*-*-*-*-1-");
				set_draw(disp, HTuple("margin"));
				memset(result0, NULL, 20);
				sprintf(result0, "PM Cnt : %d", (int)rslt_cnt);

				set_color(disp, HTuple("yellow"));
				disp_obj(ins_rgn, disp);

				if (rslt_cnt >= 10)
				{
					set_color(disp, HTuple("green"));
				}
				else
				{
					set_color(disp, HTuple("red"));
				}

				if (rslt_cnt > 0)
					disp_obj(temp_rgn1, disp);

				set_tposition(disp, rect.top - 40, rect.left);
				write_string(disp, HTuple(result0));

				sprintf_s(m_InspRslt[m_nInsp_Idx].cRsltText[i], strlen(result0) + 1, "%s", result0);
				
				copy_obj(hRslt_rgn, &m_InspRslt[m_nInsp_Idx].hRslt_rgn, 1, -1);
			}
		}
	}
	copy_obj(hRslt_rgn, rslt_rgn, 1, -1);

	delete Histo;

	if (bSave)
	{
		char cPath[512];
		size_t len1 = strlen("RsltImage.bmp");
		size_t len2 = strlen(m_cSavePath);
		sprintf_s(cPath, len1 + len2 + 1, "%sRsltImage.bmp", m_cSavePath);

		SaveImage(&Save_Image, cPath);
	}
	
	set_check("give_error");

	return rslt;
}

BOOL CVisionModule::SaveImage(BYTE *by_image, int width, int height, char *strPath)
{
	BOOL rslt = TRUE;
	Hobject hSaveImage;

	gen_image1(&hSaveImage, "byte", width, height, (Hlong)by_image);

	write_image(hSaveImage, "bmp", 0, strPath);

	return rslt;
}

BOOL CVisionModule::SaveImage(Hobject *hSaveImage, char* strPath)
{
	BOOL rslt = TRUE;
	
	write_image(*hSaveImage, "bmp", 0, strPath);

	return rslt;
}

void CVisionModule::LaplacianFilter(BYTE* by_source, long width, long height, double alpha)
{
	int temp = 0;
	BYTE* by_temp;
	by_temp = new BYTE[width * height];

	memset(by_temp, 0, sizeof(BYTE) * width * height);
	memcpy(by_temp, by_source, sizeof(BYTE)*width*height);

	for (int h = 1; h < height - 1; h++)
	{
		for (int w = 1; w < width - 1; w++)
		{
			temp = limite((4 + alpha) * by_temp[(h * width) + w] - by_temp[((h - 1) * width) + w] - by_temp[(h * width) + (w - 1)] - by_temp[((h + 1) * width) + w] - by_temp[(h * width) + (w + 1)]);

			by_source[(h * width) + w] = (BYTE)limite(temp);
		}
	}

	delete by_temp;
}

void CVisionModule::DiffusionFilter(BYTE* by_source, long width, long height, float lambda, float k, int iter)
{
	float *f_temp1, *f_temp2;
	float k2 = k * k;
	float gradn, grads, grade, gradw;
	float gcn, gcs, gce, gcw;
	float tmp;

	f_temp1 = new float[width * height];
	f_temp2 = new float[width * height];

	memset(f_temp1, 0., sizeof(float) * width * height);
	memset(f_temp2, 0., sizeof(float) * width * height);
	
	for (int h = 0; h < height; h++)
	{
		for (int w = 0; w < width; w++)
		{
			f_temp1[(h * width) + w] = (float)by_source[(h * width) + w];
			f_temp2[(h * width) + w] = (float)by_source[(h * width) + w];
		}
	}

	for (int i = 0; i < iter; i++)
	{
		for (int h = 1; h < height - 1; h++)
		{
			for (int w = 1; w < width - 1; w++)
			{
				tmp = f_temp1[(h * width) + w];

				gradn = f_temp1[((h - 1) * width) + w] - tmp;
				grads = f_temp1[((h + 1) * width) + w] - tmp;
				grade = f_temp1[(h * width) + (w - 1)] - tmp;
				gradw = f_temp1[(h * width) + (w + 1)] - tmp;

				gcn = gradn / (1.0f + gradn * gradn / k2);
				gcs = grads / (1.0f + grads * grads / k2);
				gce = grade / (1.0f + grade * grade / k2);
				gcw = gradw / (1.0f + gradw * gradw / k2);

				f_temp2[(h * width) + w] = f_temp1[(h * width) + w] + lambda * (gcn + gcs + gce + gcw);
			}
		}
	}

	for (int h = 0; h < height; h++)
	{
		for (int w = 0; w < width; w++)
		{
			f_temp1[(h * width) + w] = (float)f_temp2[(h * width) + w];
		}
	}

	for (int h = 0; h < height; h++)
	{
		for (int w = 0; w < width; w++)
		{
			by_source[(h * width) + w] = (BYTE)limite(f_temp1[(h * width) + w] + 0.5f);
		}
	}

	delete f_temp1;
	delete f_temp2;
}

void CVisionModule::TestFilter(BYTE *by_image, Hlong width, Hlong height, BOOL bSave)
{
	Hobject hFilterImage;
	int nThresh[5];
	double dThresh[5];
	float invgamma;
	long *Histo;

	Histo = new long[sizeof(long) * MAX_VAL];
	memset(Histo, NULL, sizeof(long) * MAX_VAL);

	for (int i = 0; i < 5; i++)
	{
		nThresh[i] = 0;
		dThresh[i] = 0.;
	}

	nThresh[1] = m_InspPara.ImageFilterPara[m_nInsp_Idx].nContrastVal;
	nThresh[2] = m_InspPara.ImageFilterPara[m_nInsp_Idx].nRangeVal;
	nThresh[3] = m_InspPara.ImageFilterPara[m_nInsp_Idx].nFillVal;
	dThresh[0] = m_InspPara.ImageFilterPara[m_nInsp_Idx].dGamma;
	dThresh[1] = 2;
	//	nThresh[1] = 65;
	//	nThresh[2] = 4;
	//	nThresh[3] = 15;
	//	dThresh[0] = 0.6;
	invgamma = 1.f / (float)dThresh[0];

	gen_image1(&hFilterImage, "byte", width, height, (Hlong)by_image);

	//BlurImage(by_image, width, height);

	LaplacianFilter(by_image, width, height, dThresh[1]);

	DiffusionFilter(by_image, width, height, 0.25, 4.0, 10);

	//감마 보정 GammaCorrection
	/////////////////////////////////////////////////////
	GammaCorrection(by_image, width, height, invgamma);
	/////////////////////////////////////////////////////

	nThresh[0] = GetImageAvgVal(by_image, width, height);
	//명암비 보정 ContrastCorrection
	////////////////////////////////////////////////////////////
	ContrastCorrection(by_image, width, height, nThresh[0], nThresh[1]);
	////////////////////////////////////////////////////////////

	DiffusionFilter(by_image, width, height, 0.25, 4.0, 10);

	delete Histo;
}


void CVisionModule::ImageFilter(BYTE *by_image, Hlong width, Hlong height, BOOL bSave)
{
	Hobject hFilterImage;
	int nThresh[5];
	double dThresh[5];
	float invgamma;
	long *Histo;

	Histo = new long[sizeof(long) * MAX_VAL];

	for (int i = 0; i < 5; i++)
	{
		nThresh[i] = 0;
		dThresh[i] = 0.;
	}

	nThresh[1] = m_InspPara.ImageFilterPara[m_nInsp_Idx].nContrastVal;
	nThresh[2] = m_InspPara.ImageFilterPara[m_nInsp_Idx].nRangeVal;
	nThresh[3] = m_InspPara.ImageFilterPara[m_nInsp_Idx].nFillVal;
	dThresh[0] = m_InspPara.ImageFilterPara[m_nInsp_Idx].dGamma;
	//	nThresh[1] = 65;
	//	nThresh[2] = 4;
	//	nThresh[3] = 15;
	//	dThresh[0] = 0.6;
	invgamma = 1.f / (float)dThresh[0];
	
	gen_image1(&hFilterImage, "byte", width, height, (Hlong)by_image);

	BlurImage(by_image, width, height);

	//감마 보정 GammaCorrection
	/////////////////////////////////////////////////////
	GammaCorrection(by_image, width, height, invgamma);
	/////////////////////////////////////////////////////

	//도전볼 강조 및 평균 값으로 채우기 AverageFill
	////////////////////////////////////////////////////////////
	AverageFill(by_image, width, height, nThresh[2], nThresh[3]);
	////////////////////////////////////////////////////////////

	nThresh[0] = GetImageAvgVal(by_image, width, height);
	//명암비 보정 ContrastCorrection
	////////////////////////////////////////////////////////////
	ContrastCorrection(by_image, width, height, nThresh[0], nThresh[1]);
	////////////////////////////////////////////////////////////

	BlurImage(by_image, width, height);

	nThresh[0] = GetImageAvgVal(by_image, width, height);
	//명암비 보정 ContrastCorrection
	////////////////////////////////////////////////////////////
	ContrastCorrection(by_image, width, height, nThresh[0], nThresh[1]);
	////////////////////////////////////////////////////////////

	delete Histo;
}

void CVisionModule::ImageFilter(Hobject *image, BYTE *rslt_image, BOOL bSave)
{
	Hobject hImage, hFilterImage, hSaveImage, hRectangle;
	BYTE *image_ptr, *bFilter_ptr;
	Hlong width, height;
	char typ[128];
	CRect rect;
	long lwidth, lheight, lcrop_left, lcrop_top;
	int nThresh[5];
	double dThresh[5];
	float invgamma;
	//BOOL bSave = FALSE;
	long *Histo;
	CString strTemp;
	char cSavePath[512];

	Histo = new long[sizeof(long) * MAX_VAL];                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       

	copy_image(*image, &hFilterImage);
	get_image_pointer1(hFilterImage, (Hlong*)&bFilter_ptr, typ, &width, &height);
	lwidth = (long)width;
	lheight = (long)height;


	for (int i = 0; i < 5; i++)
	{
		nThresh[i] = 0;
		dThresh[i] = 0.;
	}

//#ifdef NGH
	nThresh[1] = m_InspPara.ImageFilterPara[m_nInsp_Idx].nContrastVal;
	nThresh[2] = m_InspPara.ImageFilterPara[m_nInsp_Idx].nRangeVal;
	nThresh[3] = m_InspPara.ImageFilterPara[m_nInsp_Idx].nFillVal;
	dThresh[0] = m_InspPara.ImageFilterPara[m_nInsp_Idx].dGamma;
//#else
//	nThresh[1] = 65;
//	nThresh[2] = 4;
//	nThresh[3] = 15;
//	dThresh[0] = 0.6;
//#endif
	invgamma = 1.f / (float)dThresh[0];

	for (int i = 0; i < m_InspPara.InspRectRecipe[m_nInsp_Idx].nYRectCnt; i++)
	{
		Sleep(10);
		//검사 영역 추출
		/////////////////////////////////////////////////////
		rect.left = m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[i].lRectLeft + m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[i].lOffsetX;
		rect.right = m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[i].lRectRight + m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[i].lOffsetX;
		rect.top = m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[i].lRectTop + m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[i].lOffsetY;
		rect.bottom = m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[i].lRectBottom + m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[i].lOffsetY;

		crop_rectangle1(hFilterImage, &hImage, (Hlong)rect.top, (Hlong)rect.left, (Hlong)rect.bottom, (Hlong)rect.right);
		//gen_rectangle1(&hRectangle, (Hlong)rect.top, (Hlong)rect.left, (Hlong)rect.bottom, (Hlong)rect.right);
		width = 0;
		height = 0;
		get_image_pointer1(hImage, (Hlong*)&image_ptr, typ, &width, &height);
		/////////////////////////////////////////////////////
		if (bSave)
		{
			strTemp.Format(_T("%sstep1_%d_%d.bmp"), m_strModelPath, m_nInsp_Idx, i);
			sprintf_s(cSavePath, strTemp.GetLength() + 1, "%S", strTemp);
			//gen_image1(&hSaveImage, "byte", width, height, (Hlong)image_ptr);
			//copy_image(hImage, &hSaveImage);
			//write_image(hSaveImage, "bmp", 0, cSavePath);
			SaveImage(&hImage, cSavePath);
		}

		memset(Histo, NULL, sizeof(long) * MAX_VAL);
		GetHistgram(image_ptr, width, height, Histo);
		illuminate(hImage, &hImage, 10, 10, GetilluminateFactorCal(Histo));
			
		get_image_pointer1(hImage, (Hlong*)&image_ptr, typ, &width, &height);
		/////////////////////////////////////////////////////
		if (bSave)
		{
			gen_empty_obj(&hSaveImage);
			strTemp.Format(_T("%sstep2_%d_%d.bmp"), m_strModelPath, m_nInsp_Idx, i);
			sprintf_s(cSavePath, strTemp.GetLength() + 1, "%S", strTemp);
			gen_image1(&hSaveImage, "byte", width, height, (Hlong)image_ptr);
			//copy_image(hImage, &hSaveImage);
			//write_image(hSaveImage, "bmp", 0, cSavePath);
			SaveImage(&hSaveImage, cSavePath);
		}
		lcrop_left = rect.left;
		lcrop_top = rect.top;

		BlurImage(image_ptr, width, height);
		if (bSave)
		{
			gen_empty_obj(&hSaveImage);
			strTemp.Format(_T("%sstep3_%d_%d.bmp"), m_strModelPath, m_nInsp_Idx, i);
			sprintf_s(cSavePath, strTemp.GetLength() + 1, "%S", strTemp);
			//gen_image1(&hSaveImage, "byte", width, height, (Hlong)image_ptr);
			//write_image(hSaveImage, "bmp", 0, cSavePath);
			SaveImage(&hImage, cSavePath);
		}

		//감마 보정 GammaCorrection
		/////////////////////////////////////////////////////
		GammaCorrection(image_ptr, width, height, invgamma);
		/////////////////////////////////////////////////////
		if (bSave)
		{
			gen_empty_obj(&hSaveImage);
			strTemp.Format(_T("%sstep4_%d_%d.bmp"), m_strModelPath, m_nInsp_Idx, i);
			sprintf_s(cSavePath, strTemp.GetLength() + 1, "%S", strTemp);
			//gen_image1(&hSaveImage, "byte", width, height, (Hlong)image_ptr);
			//write_image(hSaveImage, "bmp", 0, cSavePath);
			SaveImage(&hImage, cSavePath);
		}

		//도전볼 강조 및 평균 값으로 채우기 AverageFill
		////////////////////////////////////////////////////////////
		AverageFill(image_ptr, width, height, nThresh[2], nThresh[3]);
		////////////////////////////////////////////////////////////
		if (bSave)
		{
			gen_empty_obj(&hSaveImage);
			strTemp.Format(_T("%sstep5_%d_%d.bmp"), m_strModelPath, m_nInsp_Idx, i);
			sprintf_s(cSavePath, strTemp.GetLength() + 1, "%S", strTemp);
			//gen_image1(&hSaveImage, "byte", width, height, (Hlong)image_ptr);
			//write_image(hSaveImage, "bmp", 0, cSavePath);
			SaveImage(&hImage, cSavePath);
		}

		nThresh[0] = GetImageAvgVal(image_ptr, width, height);
		//명암비 보정 ContrastCorrection
		////////////////////////////////////////////////////////////
		ContrastCorrection(image_ptr, width, height, nThresh[0], nThresh[1]);
		////////////////////////////////////////////////////////////
		if (bSave)
		{
			gen_empty_obj(&hSaveImage);
			strTemp.Format(_T("%sstep6_%d_%d.bmp"), m_strModelPath, m_nInsp_Idx, i);
			sprintf_s(cSavePath, strTemp.GetLength() + 1, "%S", strTemp);
			//gen_image1(&hSaveImage, "byte", width, height, (Hlong)image_ptr);
			//write_image(hSaveImage, "bmp", 0, cSavePath);
			SaveImage(&hImage, cSavePath);
		}

		BlurImage(image_ptr, width, height);
		if (bSave)
		{
			gen_empty_obj(&hSaveImage);
			strTemp.Format(_T("%sstep7_%d_%d.bmp"), m_strModelPath, m_nInsp_Idx, i);
			sprintf_s(cSavePath, strTemp.GetLength() + 1, "%S", strTemp);
			//gen_image1(&hSaveImage, "byte", width, height, (Hlong)image_ptr);
			//write_image(hSaveImage, "bmp", 0, cSavePath);
			SaveImage(&hImage, cSavePath);
		}

		nThresh[0] = GetImageAvgVal(image_ptr, width, height);
		//명암비 보정 ContrastCorrection
		////////////////////////////////////////////////////////////
		ContrastCorrection(image_ptr, width, height, nThresh[0], nThresh[1]);
		////////////////////////////////////////////////////////////
		if (bSave)
		{
			gen_empty_obj(&hSaveImage);
			strTemp.Format(_T("%sstep8_%d_%d.bmp"), m_strModelPath, m_nInsp_Idx, i);
			sprintf_s(cSavePath, strTemp.GetLength() + 1, "%S", strTemp);
			//gen_image1(&hSaveImage, "byte", width, height, (Hlong)image_ptr);
			//write_image(hSaveImage, "bmp", 0, cSavePath);
			SaveImage(&hImage, cSavePath);
		}

		lcrop_top = rect.top;
		for (int h = 0; h < height; h++)
		{
			lcrop_left = rect.left;
			for (int w = 0; w < rect.Width(); w++)
			{
				rslt_image[(lcrop_top * lwidth) + lcrop_left] = image_ptr[(h * width) + w];
				lcrop_left++;
			}
			lcrop_top++;
		}
		gen_empty_obj(&hImage);
		gen_empty_obj(&hRectangle);
	}
	delete Histo;
	gen_empty_obj(&hFilterImage);
	gen_empty_obj(&hSaveImage);
}

void CVisionModule::GammaCorrection(BYTE* by_source, long width, long height, float fGamma)
{
	for (int h = 0; h < height; h++)
	{
		for (int w = 0; w < width; w++)
		{
			by_source[(h * width) + w] = (BYTE)limite(pow((by_source[(h * width) + w] / 255.f), fGamma) * 255 + 0.5f);
		}
	}
}

void CVisionModule::AverageFill(BYTE* by_source, long width, long height, int nRangeVal, int nFillVal)
{
	int val;
	BYTE nAvg = GetImageAvgVal(by_source, width, height);
	BYTE *by_BlackSource, *by_WhiteSource;

	by_BlackSource = new BYTE[width * height];
	by_WhiteSource = new BYTE[width * height];

	memset(by_BlackSource, nAvg, sizeof(BYTE) * width * height);
	memset(by_WhiteSource, nAvg, sizeof(BYTE) * width * height);
	
	for (int h = 0; h < height; h++)
	{
		for (int w = 1; w < width - 1; w++)
		{
			val = by_source[(h * width) + w - 1];
			if (val < (nAvg - nRangeVal))
			{
				by_BlackSource[(h * width) + w - 1] = (BYTE)limite((by_source[(h * width) + w] + by_source[(h * width) + w + 1] + by_source[(h * width) + w - 1]) / 3 - nFillVal);
			}
			else
			{
				by_BlackSource[(h * width) + w - 1] = nAvg;
			}
		}
	}

	for (int h = 0; h < height; h++)
	{
		for (int w = 1; w < width - 1; w++)
		{
			val = by_source[(h * width) + w - 1];
			if (val > (nAvg + nRangeVal))
			{
				by_WhiteSource[(h * width) + w - 1] = (BYTE)limite((by_source[(h * width) + w] + by_source[(h * width) + w + 1] + by_source[(h * width) + w - 1]) / 3 + nFillVal);
			}
			else
			{
				by_WhiteSource[(h * width) + w - 1] = nAvg;
			}
		}
	}

	for (int h = 0; h < height; h++)
	{
		for (int w = 0; w < width; w++)
		{
			if (by_WhiteSource[(h * width) + w] > nAvg)
			{
				by_source[(h * width) + w] = by_WhiteSource[(h * width) + w];
			}
			else if(by_BlackSource[(h * width) + w] < nAvg)
			{
				by_source[(h * width) + w] = by_BlackSource[(h * width) + w];
			}
			else
			{
				by_source[(h * width) + w] = nAvg;
			}
		}
	}

	delete by_BlackSource;
	delete by_WhiteSource;
}

void CVisionModule::ContrastCorrection(BYTE* by_source, long width, long height, int nStanderdVal, int nContrastVal)
{
	int val;
	for (int h = 0; h < height; h++)
	{
		for (int w = 0; w < width; w++)
		{
			val = by_source[(h * width) + w];

			by_source[(h * width) + w] = (BYTE)limite(val + ((val - nStanderdVal) * nContrastVal / 100));
		}
	}
}

BYTE CVisionModule::GetImageAvgVal(BYTE* by_source, long width, long height)
{
	BYTE rslt = 0;
	long long sum = 0;
	int nCnt = 0;

	for (int h = 0; h < height; h++)
	{
		for (int w = 0; w < width; w++)
		{
			sum += (long long)by_source[(h * width) + w];
		}
	}

	return BYTE(sum / (width * height));
}

void CVisionModule::BlurImage(BYTE* by_source, long width, long height, int nRepeat)
{
	for (int i = 0; i < nRepeat; i++)
	{
		for (int h = 0; h < height; h++)
		{
			for (int w = 0; w < width; w++)
			{
				if (w != (width - 1))
				{
					by_source[(h * width) + w] = (by_source[(h * width) + w] + by_source[(h * width) + (w + 1)]) / 2;
				}
			}
		}

		for (int h = 0; h < height; h++)
		{
			for (int w = width; w >= 0; w--)
			{
				if (w != 0)
				{
					by_source[(h * width) + w] = (by_source[(h * width) + w] + by_source[(h * width) + (w - 1)]) / 2;
				}
			}
		}
	}

	for (int i = 0; i < nRepeat; i++)
	{
		for (int w = 0; w < width; w++)
		{
			for (int h = 0; h < height; h++)
			{
				if (h != (height - 1))
				{
					by_source[(h * width) + w] = (by_source[(h * width) + w] + by_source[((h + 1) * width) + w]) / 2;
				}
			}
		}

		for (int w = width; w > 0; w--)
		{
			for (int h = height; h >= 0; h--)
			{
				if (h != 0)
				{
					by_source[(h * width) + w] = (by_source[(h * width) + w] + by_source[((h - 1) * width) + w]) / 2;
				}
			}
		}
	}
}

void CVisionModule::GetHistgram(BYTE* by_source, long width, long height, long* lHisto)
{
	for (int h = 0; h < height; h++)
	{
		for (int w = 0; w < width; w++)
		{
			lHisto[by_source[(h * width) + w]]++;
		}
	}
}

int CVisionModule::GetHistoMax(long* lHisto)
{
	int nRslt = 0;
	int nMax = 0, nTemp = 0;
	nMax = lHisto[1];
	for (int i = 1; i < MAX_VAL - 1; i++)
	{
		nTemp = max(nMax, lHisto[i + 1]);

		if (nMax != nTemp)
		{
			nRslt = i;
		}

		nMax = nTemp;

	}


	return nRslt;
}

double CVisionModule::GetilluminateFactorCal(long* lHisto, int nHistoCnt)
{
	double rslt = 0.;
	int nMax = 0, nMin = MAX_VAL, nTemp = 0;

	for (int i = 0; i < MAX_VAL; i++)
	{
		if (lHisto[i] > nHistoCnt)
		{
			nMin = i;
			break;
		}
	}

	for (int i = (MAX_VAL - 1); i >= 0 ; i--)
	{
		if (lHisto[i] > nHistoCnt)
		{
			nMax = i;
			break;
		}
	}

	rslt = log10(log10((double)(nMax - nMin)));

	return rslt;
}

void CVisionModule::SumInspRslt()
{
	Hobject hDispImage, hInspRslt, hShiftRslt;
	gen_empty_obj(&m_InspRslt[m_nInsp_Idx].hRslt_rgn);
	copy_image(m_HalGrabImage, &hDispImage);
#ifdef THREAD_TEST
	int nIdx;
	m_InspPara.PMDistributionInspPara.nDistributionYCnt = 2;
	m_InspPara.PMDistributionInspPara.nDistributionXCnt = 4;

	//omp_set_num_threads(m_nInspThreadIdx / 4);
//#pragma omp parallel for private(m_Inspection) reduction(hShiftRslt, hInspRslt, hDispImage)
	for (int i = 0; i < m_nInspThreadIdx; i++)
	{
		ClipImage(m_Inspection[i]->GetRsltFilterImage(), &hDispImage, m_Inspection[i]->GetRsltImagePos());
		union2(hInspRslt, *m_Inspection[i]->GetRsltRgn(), &hInspRslt);
		for (int j = 0; j < m_Inspection[i]->GetInspRectCnt();j++)
		{
			nIdx = m_Inspection[i]->GetInspRectIdx(j);
			m_nRsltCnt[m_nInsp_Idx][nIdx] = m_Inspection[i]->GetRsltCnt(j);
			if (m_Inspection[i]->GetRsltText(j).GetLength() == 0)
			{
				continue;
			}
			sprintf_s(m_InspRslt[m_nInsp_Idx].cRsltText[nIdx], m_Inspection[i]->GetRsltText(j).GetLength() + 1, "%S", m_Inspection[i]->GetRsltText(j));
		}

		for (int j = 0; j < m_Inspection[i]->GetInspRectCnt(); j++)
		{
			nIdx = m_Inspection[i]->GetInspRectIdx(j);

			for (int k = 0; k < 4; k++)
			{
				if (m_InspPara.ShiftInspRect[k][nIdx].isInsp)
				{
					m_InspRslt[m_nInsp_Idx].bShiftInspRslt[k][nIdx] = m_Inspection[i]->GetShiftRslt(k, j);
				}
			}

			for (int y = 0; y < m_InspPara.PMDistributionInspPara.nDistributionYCnt; y++)
			{
				for (int x = 0; x < m_InspPara.PMDistributionInspPara.nDistributionXCnt; x++)
				{
					m_InspRslt[m_nInsp_Idx].nDistribution[nIdx][x][y] = m_Inspection[i]->GetDistributionRsltCnt(j, x, y);
				}
			}
		}
		union2(hShiftRslt, *m_Inspection[i]->GetShiftRsltRgn(), &hShiftRslt);
	}
	copy_image(hDispImage, &m_HalDispFilterImage);
	copy_obj(hInspRslt, &m_InspRslt[m_nInsp_Idx].hRslt_rgn, 1, -1);
	copy_obj(hShiftRslt, &m_InspRslt[m_nInsp_Idx].hShiftRslt_rgn, 1, -1);
#else
	deque<Hobject>::iterator iter;

	for (iter = m_RsltRgn->begin(); iter != m_RsltRgn->end(); iter++)
	{
		union2(m_InspRslt[m_nInsp_Idx].hRslt_rgn, *iter, &m_InspRslt[m_nInsp_Idx].hRslt_rgn);
	}

	deque<Hobject>::iterator iter1;
	deque<CRect>::iterator iter2;
	int i = 0;
	
	//omp_set_num_threads((int)m_RsltFilterImg->size() / 10);
//#pragma omp parallel for private(m_RsltImgRect, m_RsltFilterImg) reduction(|:m_HalDispFilterImage)
	for (iter1 = m_RsltFilterImg->begin(); iter1 != m_RsltFilterImg->end(); iter1++)
	{
		iter2 = m_RsltImgRect->begin()++;
		Hobject temp;
		CRect temp_rect;
		//copy_obj(*iter1, &temp, 1, -1);
		copy_image(*iter1, &temp);

		temp_rect = *iter2;

		ClipImage(&temp, &m_HalDispFilterImage, temp_rect);
		m_RsltImgRect->pop_front();
	}
#endif
	int j = 0, l = 0;
	int nRsltCnt = 0;
	BOOL bSearch = FALSE;
	BYTE byRslt = 0;
	typePMCntRslt PmCntRslt;
	m_PmEachRslt;
	bSearch = FALSE;

	for (l = 0; l < m_nInspThreadIdx; l++)
	{
		PmCntRslt = m_Inspection[l]->GetPMCntRslt();
		m_PmCntRslt.push_back(PmCntRslt);
		for (int i = m_Inspection[l]->GetInspRectIdx(0); i < (m_Inspection[l]->GetInspRectIdx(0) + m_Inspection[l]->GetInspRectCnt()); i++)
		{
			m_PmEachRslt[i].nCnt = PmCntRslt.lRsltCnt[i - m_Inspection[l]->GetInspRectIdx(0)];
			m_PmEachRslt[i].byRslt = 0;
			if (!PmCntRslt.bRslt[i - m_Inspection[l]->GetInspRectIdx(0)])
			{
				m_PmEachRslt[i].byRslt = 1;
			}
		}
	}

	if (m_InspPara.bFilterImgView)
	{
		m_bFilterImageView = TRUE;
		disp_obj(m_HalDispFilterImage, m_Disp);
	}
	else
	{
		m_bFilterImageView = FALSE;
		disp_obj(m_HalGrabImage, m_Disp);
	}
}

void CVisionModule::UpdateInspRgn()
{
	BOOL isRgn = FALSE;
	CRect rect;
	Hobject hRectangle, hInspRgn, hShiftRectangle, hShiftInspRgn;

	for (int i = 0; i < m_InspPara.InspRectRecipe[m_nInsp_Idx].nYRectCnt; i++)
	{
		rect.left = m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[i].lRectLeft + m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[i].lOffsetX;
		rect.right = m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[i].lRectRight + m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[i].lOffsetX;
		rect.top = m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[i].lRectTop + m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[i].lOffsetY;
		rect.bottom = m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[i].lRectBottom + m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[i].lOffsetY;
		gen_rectangle1(&hRectangle, (Hlong)rect.top, (Hlong)rect.left, (Hlong)rect.bottom, (Hlong)rect.right);

		union2(hInspRgn, hRectangle, &hInspRgn);

		for (int j = 0; j < 4; j++)
		{
			if (m_InspPara.ShiftInspRect[j][i].isInsp)
			{
				rect.left = m_InspPara.ShiftInspRect[j][i].lRectLeft;
				rect.top = m_InspPara.ShiftInspRect[j][i].lRectTop;
				rect.right = m_InspPara.ShiftInspRect[j][i].lRectRight;
				rect.bottom = m_InspPara.ShiftInspRect[j][i].lRectBottom;

				gen_rectangle1(&hShiftRectangle, (Hlong)rect.top, (Hlong)rect.left, (Hlong)rect.bottom, (Hlong)rect.right);
				union2(hShiftInspRgn, hShiftRectangle, &hShiftInspRgn);
			}
		}
		
		isRgn = TRUE;
	}

	if (isRgn)
	{
		//disp_obj(hInspRgn, m_Disp);
		copy_obj(hInspRgn, &m_InspRslt[m_nInsp_Idx].hInsp_rgn, 1, -1);
		copy_obj(hShiftInspRgn, &m_InspRslt[m_nInsp_Idx].hShiftInsp_rgn, 1, -1);
	}
}

void CVisionModule::DisplayRslt()
{
	CRect rect, shift_rect, temp_rect;
	char result0[512];
	int nRgnCnt = 0;
	int rslt_cnt = 0;
	//Hobject hRectangle;
	long lRsltCnt = 0;
	BOOL bRslt = FALSE;
	BOOL isSearch = FALSE;
	CString strTemp, strTemp1;
	Hobject hShiftRslt;
	BOOL bSearch = FALSE;

	for (int i = 0; i < m_InspPara.InspRectRecipe[m_nInsp_Idx].nYRectCnt; i++)
	{
		lRsltCnt += m_nRsltCnt[m_nInsp_Idx][i];
	}

	m_InspRslt[m_nInsp_Idx].lRslt_num = lRsltCnt;

	set_color(m_Disp, HTuple("yellow"));
	disp_obj(m_InspRslt[m_nInsp_Idx].hRslt_rgn, m_Disp);
	disp_obj(m_InspRslt[m_nInsp_Idx].hInsp_rgn, m_Disp);
	disp_obj(m_InspRslt[m_nInsp_Idx].hShiftInsp_rgn, m_Disp);

	if(m_InspRslt[m_nInsp_Idx].lRslt_num < 1)
	{
		return;
	}

	m_nLastInspRectIdx = m_InspPara.InspRectRecipe[m_nInsp_Idx].nYRectCnt;

	m_bPMCntRslt[m_nInsp_Idx] = TRUE;

	for (int i = 0; i < m_nLastInspRectIdx/*m_InspPara.InspRectRecipe[m_nInsp_Idx].nYRectCnt*/; i++)
	{
#ifndef THREAD_TEST
		/*if (!m_nInspEnd[m_nInsp_Idx][i])
		{
			break;
		}*/
#endif
		rect.left = m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[i].lRectLeft + m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[i].lOffsetX;
		rect.right = m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[i].lRectRight + m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[i].lOffsetX;
		rect.top = m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[i].lRectTop + m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[i].lOffsetY;
		rect.bottom = m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[i].lRectBottom + m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[i].lOffsetY;
		//gen_rectangle1(&hRectangle, (Hlong)rect.top, (Hlong)rect.left, (Hlong)rect.bottom, (Hlong)rect.right);

		rslt_cnt = m_nRsltCnt[m_nInsp_Idx][i];

		set_font(m_Disp, "-Arial-12-*-*-*-*-*-1-");
		set_draw(m_Disp, HTuple("margin"));

		sprintf(result0, "%d", (int)rslt_cnt);

		if (rslt_cnt >= 10)
		{
			set_color(m_Disp, HTuple("green"));
			bRslt = TRUE;
		}
		else
		{
			set_color(m_Disp, HTuple("red"));
			bRslt = FALSE;
		}

		BOOL isShiftR[4] = { TRUE, };
		bSearch = FALSE;
		strTemp.Empty();
		//typeShiftInspRect ShiftInspRect[4];
		for (int k = 0; k < 4; k++)
		{
			if (m_InspPara.ShiftInspRect[k][i].isInsp)
			{
				if (!m_InspRslt[m_nInsp_Idx].bShiftInspRslt[k][i])
				{
					//Hobject hShiftRsltTemp;
					//ShiftInspRect[k] = m_InspPara.ShiftInspRect[k][i];
					//gen_rectangle1(&hShiftRsltTemp, ShiftInspRect[k].lRectTop, ShiftInspRect[k].lRectLeft, ShiftInspRect[k].lRectBottom, ShiftInspRect[k].lRectRight);
					//union2(hShiftRsltTemp, hShiftRslt, &hShiftRslt);
					isShiftR[k] = FALSE;
					isSearch = TRUE;
					bSearch = TRUE;
					switch (k)
					{
					case 0:
						strTemp1.Format(_T(" L"));
						break;
					case 1:
						strTemp1.Format(_T(" R"));
						break;
					case 2:
						strTemp1.Format(_T(" T"));
						break;
					case 3:
						strTemp1.Format(_T(" B"));
						break;
					}

					if (k < 3)
					{
						strTemp += strTemp1;
						strTemp += _T(",");
					}
				}
			}
		}

		int nLen = strlen(m_InspRslt[m_nInsp_Idx].cRsltText[i]);
		int nLen2 = 0;
		if (strTemp.Left(1) == _T("0"))
		{
			sprintf_s(result0, nLen + 1, "%s", m_InspRslt[m_nInsp_Idx].cRsltText[i]);
			set_tposition(m_Disp, rect.bottom + 5, rect.left + 5);
			write_string(m_Disp, HTuple(result0));

			if (!bRslt)
			{
				m_bPMCntRslt[m_nInsp_Idx] = FALSE;
			}
			continue;
		}

		for (int k = 0; k < 4; k++)
		{
			if (m_InspPara.ShiftInspRect[k][i].isInsp)
			{
				if (!isShiftR[k])
				{
					strTemp1 = strTemp;
					nLen2 = strTemp.GetLength();
					if (strTemp.Mid(strTemp.GetLength() - 1, 1) == _T(","))
					{
						strTemp1.Format(_T("%s"), strTemp.Mid(0, strTemp.GetLength() - 1));
					}
					strTemp.Format(_T("%s Shift"), strTemp1);
					break;

				}
			}
		}

		
		if (bSearch)
		{
			isSearch = TRUE;
			set_color(m_Disp, HTuple("red"));
			//disp_obj(hShiftRslt, m_Disp);
			sprintf_s(result0, strTemp.GetLength() + nLen + 3, "%s, %S", m_InspRslt[m_nInsp_Idx].cRsltText[i], strTemp);
		}
		else
		{
			sprintf_s(result0, nLen + 1, "%s", m_InspRslt[m_nInsp_Idx].cRsltText[i]);
		}

		set_tposition(m_Disp, rect.bottom + 5, rect.left + 5);
		write_string(m_Disp, HTuple(result0));

		if (!bRslt)
		{
			m_bPMCntRslt[m_nInsp_Idx] = FALSE;
		}
	}
	if (isSearch)
	{
		set_color(m_Disp, HTuple("red"));
		disp_obj(m_InspRslt[m_nInsp_Idx].hShiftRslt_rgn, m_Disp);
	}

	Herror herr;
	HTuple Num;
	
	if (m_InspRslt[m_nInsp_Idx].hDistributionInspRgn.Id() != NULL)
	{
		set_check("~give_error");
		herr = count_obj(m_InspRslt[m_nInsp_Idx].hDistributionInspRgn, &Num);
		set_check("give_error");
		if (H_MSG_TRUE == herr)
		{
			if ((Hlong)Num[0] > 0)
			{
				set_color(m_Disp, HTuple("white"));
				disp_obj(m_InspRslt[m_nInsp_Idx].hDistributionInspRgn, m_Disp);
			}
		}
		else
		{

		}
	}
}

void CVisionModule::SetSelectRect(int insp_idx, int y_idx, BOOL isSelect)
{
	CRect rect;
	Hobject hRectangle;
	/*typeInsRect rect;

	if (y_idx == 0)
	{
		rect.left = m_InspPara.InspRectRecipe[insp_idx].StartRect.lRectLeft + m_InspPara.InspRectRecipe[insp_idx].StartRect.lOffsetX;
		rect.right = m_InspPara.InspRectRecipe[insp_idx].StartRect.lRectRight + m_InspPara.InspRectRecipe[insp_idx].StartRect.lOffsetX;
		rect.top = m_InspPara.InspRectRecipe[insp_idx].StartRect.lRectTop + m_InspPara.InspRectRecipe[insp_idx].StartRect.lOffsetY;
		rect.bottom = m_InspPara.InspRectRecipe[insp_idx].StartRect.lRectBottom + m_InspPara.InspRectRecipe[insp_idx].StartRect.lOffsetY;
	}
	else
	{
		rect.left = m_InspPara.InspRectRecipe[insp_idx].EndRect.lRectLeft + m_InspPara.InspRectRecipe[insp_idx].EndRect.lOffsetX;
		rect.right = m_InspPara.InspRectRecipe[insp_idx].EndRect.lRectRight + m_InspPara.InspRectRecipe[insp_idx].EndRect.lOffsetX;
		rect.top = m_InspPara.InspRectRecipe[insp_idx].EndRect.lRectTop + m_InspPara.InspRectRecipe[insp_idx].EndRect.lOffsetY;
		rect.bottom = m_InspPara.InspRectRecipe[insp_idx].EndRect.lRectBottom + m_InspPara.InspRectRecipe[insp_idx].EndRect.lOffsetY;
	}*/

	rect.left = m_InspPara.InspRectRecipe[insp_idx].InspRect[y_idx].lRectLeft + m_InspPara.InspRectRecipe[insp_idx].InspRect[y_idx].lOffsetX;
	rect.right = m_InspPara.InspRectRecipe[insp_idx].InspRect[y_idx].lRectRight + m_InspPara.InspRectRecipe[insp_idx].InspRect[y_idx].lOffsetX;
	rect.top = m_InspPara.InspRectRecipe[insp_idx].InspRect[y_idx].lRectTop + m_InspPara.InspRectRecipe[insp_idx].InspRect[y_idx].lOffsetY;
	rect.bottom = m_InspPara.InspRectRecipe[insp_idx].InspRect[y_idx].lRectBottom + m_InspPara.InspRectRecipe[insp_idx].InspRect[y_idx].lOffsetY;
	gen_rectangle1(&hRectangle, (Hlong)rect.top, (Hlong)rect.left, (Hlong)rect.bottom, (Hlong)rect.right);
	set_draw(m_Disp, HTuple("margin"));
	set_color(m_Disp, HTuple("blue"));

	if (isSelect)
	{
		set_check("~give_error");
		Herror herr;
		Hlong x1, x2, y1, y2, part_height;
		get_part(m_Disp, &y1, &x1, &y2, &x2);
		part_height = y2 - y1;
		herr = set_part(m_Disp, rect.CenterPoint().y - (part_height / 2), x1, rect.CenterPoint().y + (part_height / 2), x2);
		set_check("give_error");

		if (H_MSG_TRUE == herr)
		{
		}
		else
		{

		}
	}
	disp_obj(hRectangle, m_Disp);
}

BOOL CVisionModule::SaveInspPara(char* strPath)
{
	BOOL rslt = TRUE;
	CString key, dat, app, temp_key;
	CString file_path, temp_path, strLog;

	m_strModelPath.Format(_T("%S\\"), strPath);
	MakeDirectories(m_strModelPath);
	file_path.Format(_T("%s%s"), m_strModelPath, INSP_STR_PARAFILENAME);


	//file 존재 확인 
	HANDLE fd = CreateFile(file_path,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);
	if (((LONG)fd)<0)
	{

		return FALSE;
	}
	else
	{
		CloseHandle(fd);
		

		//Insp Para
		app.Format(_T("INSP_PARA"));
		dat.Empty();
		dat.Format(_T("%d"), m_InspPara.bInspImgSave);
		WritePrivateProfileString(app, _T("INSP_IMAGE_SAVE"), dat, file_path);

		dat.Empty();
		dat.Format(_T("%d"), m_InspPara.bFilterImgView);
		WritePrivateProfileString(app, _T("INSP_FILTER_IMAGE_VIEW"), dat, file_path);

		dat.Empty();
		dat.Format(_T("%d"), m_InspPara.bInspImgView);
		WritePrivateProfileString(app, _T("INSP_IMAGE_VIEW"), dat, file_path);

		dat.Empty();
		dat.Format(_T("%d"), m_InspPara.bAutoInspRectTeaching);
		WritePrivateProfileString(app, _T("AUTO_INSP_RECT_TEACHING"), dat, file_path);

		dat.Empty();
		dat.Format(_T("%f"), m_InspPara.dPixelSize);
		WritePrivateProfileString(app, _T("PIXEL_SIZE"), dat, file_path);

		dat.Empty();
		dat.Format(_T("%d"), m_InspPara.nInspTypeCnt);
		WritePrivateProfileString(app, _T("INSP_TYPE_CNT"), dat, file_path);

		dat.Empty();
		dat.Format(_T("%d"), m_InspPara.bInspection);
		WritePrivateProfileString(app, _T("INSP_TYPE_INPECTION"), dat, file_path);

		for (int i = 0; i < m_InspPara.nInspTypeCnt; i++)
		{
			app.Empty();
			app.Format(_T("INSP_TYPE_%d_PM_SEARCH_PARA"), i + 1);
			dat.Empty();
			dat.Format(_T("%d"), m_InspPara.nLightVal[i]);
			WritePrivateProfileString(app, _T("LIGHT_VALUE"), dat, file_path);

			dat.Empty();
			dat.Format(_T("%d"), m_InspPara.PMSearchPara[i].nThreshHold[0]);
			WritePrivateProfileString(app, _T("THRESH_HOLD_BLACK"), dat, file_path);

			dat.Empty();
			dat.Format(_T("%d"), m_InspPara.PMSearchPara[i].nThreshHold[1]);
			WritePrivateProfileString(app, _T("THRESH_HOLD_WHITE"), dat, file_path);

			dat.Empty();
			dat.Format(_T("%d"), m_InspPara.PMSearchPara[i].nNoiseSize[0]);
			WritePrivateProfileString(app, _T("NOISE_AREA"), dat, file_path);

			dat.Empty();
			dat.Format(_T("%d"), m_InspPara.PMSearchPara[i].nNoiseSize[1]);
			WritePrivateProfileString(app, _T("NOISE_WIDTH"), dat, file_path);

			dat.Empty();
			dat.Format(_T("%d"), m_InspPara.PMSearchPara[i].nNoiseSize[2]);
			WritePrivateProfileString(app, _T("NOISE_HEIGHT"), dat, file_path);

			dat.Empty();
			dat.Format(_T("%d"), m_InspPara.PMSearchPara[i].nMargeDistance);
			WritePrivateProfileString(app, _T("MARGE_DISTANCE"), dat, file_path);

			dat.Empty();
			dat.Format(_T("%d"), m_InspPara.PMSearchPara[i].nDefectCnt);
			WritePrivateProfileString(app, _T("DEFECT_CNT"), dat, file_path);

			dat.Empty();
			dat.Format(_T("%d"), m_InspPara.PMSearchPara[i].nPmType);
			WritePrivateProfileString(app, _T("PM_TYPE"), dat, file_path);

			//ImageFilter Para
			app.Empty();
			app.Format(_T("INSP_TYPE_%d_FILLTER_PARA"), i + 1);
			dat.Empty();
#ifdef HAL_FILTER
			dat.Format(_T("%d"), m_InspPara.ImageFilterPara[i].nMeanSize);
			WritePrivateProfileString(app, _T("HAL_MEAN_SIZE"), dat, file_path);

			dat.Empty();
			dat.Format(_T("%d"), m_InspPara.ImageFilterPara[i].nIllumiSize);
			WritePrivateProfileString(app, _T("HAL_ILLUMINATE_SIZE"), dat, file_path);

			dat.Empty();
			dat.Format(_T("%f"), m_InspPara.ImageFilterPara[i].dLaplaceSigma);
			WritePrivateProfileString(app, _T("HAL_LAPLACE_SIGMA"), dat, file_path);
#else
			dat.Format(_T("%d"), m_InspPara.ImageFilterPara[i].nRangeVal);
			WritePrivateProfileString(app, _T("RANGE_VALUE"), dat, file_path);

			dat.Empty();
			dat.Format(_T("%d"), m_InspPara.ImageFilterPara[i].nFillVal);
			WritePrivateProfileString(app, _T("FILL_VALUE"), dat, file_path);

			dat.Empty();
			dat.Format(_T("%d"), m_InspPara.ImageFilterPara[i].nContrastVal);
			WritePrivateProfileString(app, _T("CONTRAST_VALUE"), dat, file_path);

			dat.Empty();
			dat.Format(_T("%f"), m_InspPara.ImageFilterPara[i].dGamma);
			WritePrivateProfileString(app, _T("GAMMA_VALUE"), dat, file_path);
#endif
			//Insp Rect
			app.Empty();
			app.Format(_T("INSP_TYPE_%d_RECT_PARA"), i + 1);
			dat.Empty();
			dat.Format(_T("%.4f"), m_InspPara.InspRectRecipe[i].dLaplaceSigma);
			WritePrivateProfileString(app, _T("LAPLACE_SIGMA"), dat, file_path);

			dat.Empty();
			dat.Format(_T("%d"), m_InspPara.InspRectRecipe[i].StartRect.lRectLeft);
			WritePrivateProfileString(app, _T("START_RECT_LEFT"), dat, file_path);
			dat.Empty();
			dat.Format(_T("%d"), m_InspPara.InspRectRecipe[i].StartRect.lRectTop);
			WritePrivateProfileString(app, _T("START_RECT_TOP"), dat, file_path);
			dat.Empty();
			dat.Format(_T("%d"), m_InspPara.InspRectRecipe[i].StartRect.lRectRight);
			WritePrivateProfileString(app, _T("START_RECT_RIGHT"), dat, file_path);
			dat.Empty();
			dat.Format(_T("%d"), m_InspPara.InspRectRecipe[i].StartRect.lRectBottom);
			WritePrivateProfileString(app, _T("START_RECT_BOTTOM"), dat, file_path);

			dat.Empty();
			dat.Format(_T("%d"), m_InspPara.InspRectRecipe[i].EndRect.lRectLeft);
			WritePrivateProfileString(app, _T("END_RECT_LEFT"), dat, file_path);
			dat.Empty();
			dat.Format(_T("%d"), m_InspPara.InspRectRecipe[i].EndRect.lRectTop);
			WritePrivateProfileString(app, _T("END_RECT_TOP"), dat, file_path);
			dat.Empty();
			dat.Format(_T("%d"), m_InspPara.InspRectRecipe[i].EndRect.lRectRight);
			WritePrivateProfileString(app, _T("END_RECT_RIGHT"), dat, file_path);
			dat.Empty();
			dat.Format(_T("%d"), m_InspPara.InspRectRecipe[i].EndRect.lRectBottom);
			WritePrivateProfileString(app, _T("END_RECT_BOTTOM"), dat, file_path);

			dat.Empty();
			dat.Format(_T("%d"), m_InspPara.InspRectRecipe[i].nYRectCnt);
			WritePrivateProfileString(app, _T("RECT_Y_CNT"), dat, file_path);

			dat.Empty();
			dat.Format(_T("%d"), m_InspPara.InspRectRecipe[i].nInspYSpace);
			WritePrivateProfileString(app, _T("RECT_Y_SPACE"), dat, file_path);

			for (int j = 0; j < m_InspPara.InspRectRecipe[i].nYRectCnt; j++)
			{
				dat.Empty();
				key.Empty();
				key.Format(_T("RECT_LEFT[%d]"), j);
				dat.Format(_T("%d"), m_InspPara.InspRectRecipe[i].InspRect[j].lRectLeft);
				WritePrivateProfileString(app, key, dat, file_path);

				dat.Empty();
				key.Empty();
				key.Format(_T("RECT_TOP[%d]"), j);
				dat.Format(_T("%d"), m_InspPara.InspRectRecipe[i].InspRect[j].lRectTop);
				WritePrivateProfileString(app, key, dat, file_path);

				dat.Empty();
				key.Empty();
				key.Format(_T("RECT_RIGHT[%d]"), j);
				dat.Format(_T("%d"), m_InspPara.InspRectRecipe[i].InspRect[j].lRectRight);
				WritePrivateProfileString(app, key, dat, file_path);

				dat.Empty();
				key.Empty();
				key.Format(_T("RECT_BOTTOM[%d]"), j);
				dat.Format(_T("%d"), m_InspPara.InspRectRecipe[i].InspRect[j].lRectBottom);
				WritePrivateProfileString(app, key, dat, file_path);

				dat.Empty();
				key.Empty();
				key.Format(_T("RECT_OFFSET_X[%d]"), j);
				dat.Format(_T("%d"), m_InspPara.InspRectRecipe[i].InspRect[j].lOffsetX);
				WritePrivateProfileString(app, key, dat, file_path);

				dat.Empty();
				key.Empty();
				key.Format(_T("RECT_OFFSET_Y[%d]"), j);
				dat.Format(_T("%d"), m_InspPara.InspRectRecipe[i].InspRect[j].lOffsetY);
				WritePrivateProfileString(app, key, dat, file_path);
			}

			for (int j = 0; j < m_InspPara.InspRectRecipe[i].nYRectCnt; j++)
			{
				for (int k = 0; k < 4; k++)
				{
					if (m_InspPara.ShiftInspRect[k][j].isInsp)
					{
						switch (k)
						{
						case 0:
							temp_key.Format(_T("LEFT"));
							break;
						case 1:
							temp_key.Format(_T("RIGHT"));
							break;
						case 2:
							temp_key.Format(_T("TOP"));
							break;
						case 3:
							temp_key.Format(_T("BOTTOM"));
							break;
						}
						key.Format(_T("SHIFT_%s_RECT_INSPECTION[%d]"), temp_key, j);
						dat.Format(_T("%d"), m_InspPara.ShiftInspRect[k][j].isInsp);
						WritePrivateProfileString(app, key, dat, file_path);
						key.Format(_T("SHIFT_%s_RECT_LEFT[%d]"), temp_key, j);
						dat.Format(_T("%d"), m_InspPara.ShiftInspRect[k][j].lRectLeft);
						WritePrivateProfileString(app, key, dat, file_path);
						key.Format(_T("SHIFT_%s_RECT_TOP[%d]"), temp_key, j);
						dat.Format(_T("%d"), m_InspPara.ShiftInspRect[k][j].lRectTop);
						WritePrivateProfileString(app, key, dat, file_path);
						key.Format(_T("SHIFT_%s_RECT_RIGHT[%d]"), temp_key, j);
						dat.Format(_T("%d"), m_InspPara.ShiftInspRect[k][j].lRectRight);
						WritePrivateProfileString(app, key, dat, file_path);
						key.Format(_T("SHIFT_%s_RECT_BOTTOM[%d]"), temp_key, j);
						dat.Format(_T("%d"), m_InspPara.ShiftInspRect[k][j].lRectBottom);
						WritePrivateProfileString(app, key, dat, file_path);
					}
				}
			}
		}
	}

	return rslt;
}

BOOL CVisionModule::LoadInspPara(char* strPath)
{
	BOOL rslt = TRUE;
	CString key, dat, app, temp_key;
	CString file_path, temp_path;
	//char temp[256];

	//m_strModelPath.Format(_T("d:\\Database\\MODEL\\Test"));
	m_strModelPath.Format(_T("%S\\"), strPath);
	MakeDirectories(m_strModelPath);
	file_path.Format(_T("%s%s"), m_strModelPath, INSP_STR_PARAFILENAME);

	//file 존재 확인 
	HANDLE fd = CreateFile(file_path,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);
	if (((LONG)fd)<0)
	{
		return FALSE;
	}
	else
	{
		CloseHandle(fd);

		app.Empty();
		app.Format(_T("INSP_PARA"));
		dat.Empty();
		GetPrivateProfileString(app, _T("INSP_IMAGE_SAVE"), _T("0"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
		m_InspPara.bInspImgSave = _wtoi(dat);

		dat.Empty();
		GetPrivateProfileString(app, _T("INSP_FILTER_IMAGE_VIEW"), _T("0"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
		m_InspPara.bFilterImgView = _wtoi(dat);

		dat.Empty();
		GetPrivateProfileString(app, _T("INSP_IMAGE_VIEW"), _T("0"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
		m_InspPara.bInspImgView = _wtoi(dat);

		dat.Empty();
		GetPrivateProfileString(app, _T("AUTO_INSP_RECT_TEACHING"), _T("1"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
		m_InspPara.bAutoInspRectTeaching = _wtoi(dat);

		dat.Empty();
		GetPrivateProfileString(app, _T("PIXEL_SIZE"), _T("0.700"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
		m_InspPara.dPixelSize = _wtof(dat);

		dat.Empty();
		GetPrivateProfileString(app, _T("INSP_TYPE_CNT"), _T("1"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
		m_InspPara.nInspTypeCnt = _wtoi(dat);

		dat.Empty();
		GetPrivateProfileString(app, _T("INSP_TYPE_INPECTION"), _T("1"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
		m_InspPara.bInspection = _wtoi(dat);

		for (int i = 0; i < m_InspPara.nInspTypeCnt; i++)
		{
			app.Empty();
			app.Format(_T("INSP_TYPE_%d_PM_SEARCH_PARA"), i + 1);
			dat.Empty();
			GetPrivateProfileString(app, _T("LIGHT_VALUE"), _T("100"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
			m_InspPara.nLightVal[i] = _wtoi(dat);

			dat.Empty();
			GetPrivateProfileString(app, _T("THRESH_HOLD_BLACK"), _T("70"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
			m_InspPara.PMSearchPara[i].nThreshHold[0] = _wtoi(dat);

			dat.Empty();
			GetPrivateProfileString(app, _T("THRESH_HOLD_WHITE"), _T("100"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
			m_InspPara.PMSearchPara[i].nThreshHold[1] = _wtoi(dat);

			dat.Empty();
			GetPrivateProfileString(app, _T("NOISE_AREA"), _T("15"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
			m_InspPara.PMSearchPara[i].nNoiseSize[0] = _wtoi(dat);

			dat.Empty();
			GetPrivateProfileString(app, _T("NOISE_WIDTH"), _T("8"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
			m_InspPara.PMSearchPara[i].nNoiseSize[1] = _wtoi(dat);

			dat.Empty();
			GetPrivateProfileString(app, _T("NOISE_HEIGHT"), _T("8"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
			m_InspPara.PMSearchPara[i].nNoiseSize[2] = _wtoi(dat);

			dat.Empty();
			GetPrivateProfileString(app, _T("MARGE_DISTANCE"), _T("30"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
			m_InspPara.PMSearchPara[i].nMargeDistance = _wtoi(dat);

			dat.Empty();
			GetPrivateProfileString(app, _T("DEFECT_CNT"), _T("10"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
			m_InspPara.PMSearchPara[i].nDefectCnt = _wtoi(dat);

			dat.Empty();
			GetPrivateProfileString(app, _T("PM_TYPE"), _T("1"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
			m_InspPara.PMSearchPara[i].nPmType = _wtoi(dat);

			//ImageFillter Para
			app.Empty();
			app.Format(_T("INSP_TYPE_%d_FILLTER_PARA"), i + 1);
			dat.Empty();
#ifdef HAL_FILTER
			GetPrivateProfileString(app, _T("HAL_MEAN_SIZE"), _T("10"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
			m_InspPara.ImageFilterPara[i].nMeanSize = _wtoi(dat);

			dat.Empty();
			GetPrivateProfileString(app, _T("HAL_ILLUMINATE_SIZE"), _T("20"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
			m_InspPara.ImageFilterPara[i].nIllumiSize = _wtoi(dat);

			dat.Empty();
			GetPrivateProfileString(app, _T("HAL_LAPLACE_SIGMA"), _T("2.5"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
			m_InspPara.ImageFilterPara[i].dLaplaceSigma = _wtof(dat);

#else
			GetPrivateProfileString(app, _T("RANGE_VALUE"), _T("4"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
			m_InspPara.ImageFilterPara[i].nRangeVal = _wtoi(dat);

			dat.Empty();
			GetPrivateProfileString(app, _T("FILL_VALUE"), _T("15"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
			m_InspPara.ImageFilterPara[i].nFillVal = _wtoi(dat);

			dat.Empty();
			GetPrivateProfileString(app, _T("CONTRAST_VALUE"), _T("65"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
			m_InspPara.ImageFilterPara[i].nContrastVal = _wtoi(dat);

			dat.Empty();
			GetPrivateProfileString(app, _T("GAMMA_VALUE"), _T("0.600"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
			m_InspPara.ImageFilterPara[i].dGamma = _wtof(dat);
#endif
			//Insp Rect
			app.Empty();
			app.Format(_T("INSP_TYPE_%d_RECT_PARA"), i + 1);
			dat.Empty();
			GetPrivateProfileString(app, _T("LAPLACE_SIGMA"), _T("3"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
			m_InspPara.InspRectRecipe[i].dLaplaceSigma = _wtof(dat);

			dat.Empty();
			GetPrivateProfileString(app, _T("START_RECT_LEFT"), _T("620"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
			m_InspPara.InspRectRecipe[i].StartRect.lRectLeft = _wtoi(dat);
			GetPrivateProfileString(app, _T("START_RECT_TOP"), _T("12525"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
			m_InspPara.InspRectRecipe[i].StartRect.lRectTop = _wtoi(dat);
			GetPrivateProfileString(app, _T("START_RECT_RIGHT"), _T("1440"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
			m_InspPara.InspRectRecipe[i].StartRect.lRectRight = _wtoi(dat);
			GetPrivateProfileString(app, _T("START_RECT_BOTTOM"), _T("12605"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
			m_InspPara.InspRectRecipe[i].StartRect.lRectBottom = _wtoi(dat);

			GetPrivateProfileString(app, _T("END_RECT_LEFT"), _T("620"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
			m_InspPara.InspRectRecipe[i].EndRect.lRectLeft = _wtoi(dat);
			GetPrivateProfileString(app, _T("END_RECT_TOP"), _T("60620"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
			m_InspPara.InspRectRecipe[i].EndRect.lRectTop = _wtoi(dat);
			GetPrivateProfileString(app, _T("END_RECT_RIGHT"), _T("1440"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
			m_InspPara.InspRectRecipe[i].EndRect.lRectRight = _wtoi(dat);
			GetPrivateProfileString(app, _T("END_RECT_BOTTOM"), _T("60700"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
			m_InspPara.InspRectRecipe[i].EndRect.lRectBottom = _wtoi(dat);

			dat.Empty();
			GetPrivateProfileString(app, _T("RECT_Y_CNT"), _T("10"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
			m_InspPara.InspRectRecipe[i].nYRectCnt = _wtoi(dat);

			dat.Empty();
			GetPrivateProfileString(app, _T("RECT_Y_SPACE"), _T("115"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
			m_InspPara.InspRectRecipe[i].nInspYSpace = _wtoi(dat);

			
			for (int j = 0; j < m_InspPara.InspRectRecipe[i].nYRectCnt; j++)
			{
				dat.Empty();
				key.Empty();
				key.Format(_T("RECT_LEFT[%d]"), j);
				GetPrivateProfileString(app, key, _T("100"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
				m_InspPara.InspRectRecipe[i].InspRect[j].lRectLeft = _wtoi(dat);

				dat.Empty();
				key.Empty();
				key.Format(_T("RECT_TOP[%d]"), j);
				GetPrivateProfileString(app, key, _T("100"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
				m_InspPara.InspRectRecipe[i].InspRect[j].lRectTop = _wtoi(dat);

				dat.Empty();
				key.Empty();
				key.Format(_T("RECT_RIGHT[%d]"), j);
				GetPrivateProfileString(app, key, _T("200"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
				m_InspPara.InspRectRecipe[i].InspRect[j].lRectRight = _wtoi(dat);

				dat.Empty();
				key.Empty();
				key.Format(_T("RECT_BOTTOM[%d]"), j);
				GetPrivateProfileString(app, key, _T("200"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
				m_InspPara.InspRectRecipe[i].InspRect[j].lRectBottom = _wtoi(dat);

				dat.Empty();
				key.Empty();
				key.Format(_T("RECT_OFFSET_X[%d]"), j);
				GetPrivateProfileString(app, key, _T("0"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
				m_InspPara.InspRectRecipe[i].InspRect[j].lOffsetX = _wtoi(dat);

				dat.Empty();
				key.Empty();
				key.Format(_T("RECT_OFFSET_Y[%d]"), j);
				GetPrivateProfileString(app, key, _T("0"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
				m_InspPara.InspRectRecipe[i].InspRect[j].lOffsetY = _wtoi(dat);
			}

			for (int j = 0; j < m_InspPara.InspRectRecipe[i].nYRectCnt; j++)
			{
				for (int k = 0; k < 4; k++)
				{
					switch (k)
					{
					case 0:
						temp_key.Format(_T("LEFT"));
						break;
					case 1:
						temp_key.Format(_T("RIGHT"));
						break;
					case 2:
						temp_key.Format(_T("TOP"));
						break;
					case 3:
						temp_key.Format(_T("BOTTOM"));
						break;
					}
					key.Format(_T("SHIFT_%s_RECT_INSPECTION[%d]"), temp_key, j);

					GetPrivateProfileString(app, key, _T("0"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
					int nTemp = _wtoi(dat);
					if (nTemp == 1)
					{
						m_InspPara.ShiftInspRect[k][j].isInsp = _wtoi(dat);

						key.Format(_T("SHIFT_%s_RECT_LEFT[%d]"), temp_key, j);
						GetPrivateProfileString(app, key, _T("100"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
						m_InspPara.ShiftInspRect[k][j].lRectLeft = _wtoi(dat);

						key.Format(_T("SHIFT_%s_RECT_TOP[%d]"), temp_key, j);
						GetPrivateProfileString(app, key, _T("100"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
						m_InspPara.ShiftInspRect[k][j].lRectTop = _wtoi(dat);

						key.Format(_T("SHIFT_%s_RECT_RIGHT[%d]"), temp_key, j);
						GetPrivateProfileString(app, key, _T("200"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
						m_InspPara.ShiftInspRect[k][j].lRectRight = _wtoi(dat);

						key.Format(_T("SHIFT_%s_RECT_BOTTOM[%d]"), temp_key, j);
						GetPrivateProfileString(app, key, _T("200"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
						m_InspPara.ShiftInspRect[k][j].lRectBottom = _wtoi(dat);
					}
				}
			}
		}
	}

	return rslt;
}

void CVisionModule::SetInspPara(typeInspPara *inspPara)
{
	m_InspPara = *inspPara; 
	//SelectInspectionSpace(INSPECTION_SPACE_PM_COUNT);
}

//20180919 ngh
void CVisionModule::PMShiftInspection(long disp, Hobject *image, BOOL bSave)
{
	Hobject hInspImage, Save_Image, hInspRgn, hRslt_rgn;
	BYTE *bFilterRslt;
	char typ[128], result0[20];;
	Hlong width, height;
	long lwidth, lheight;
	CRect rect;
	BOOL bNGRslt = TRUE;
	WORD wInspRslt = 0;

	//copy_image(*image, &hInspImage);
	//copy_image(*image, &Save_Image);

	m_InspPara.InspRectRecipe[m_nInsp_Idx].nShiftInspSpaceX = 10;
	m_InspPara.InspRectRecipe[m_nInsp_Idx].nShiftInspSpaceY = 10;

	for (int i = 0; i < m_InspPara.InspRectRecipe[m_nInsp_Idx].nYRectCnt; i++)
	{
		copy_image(*image, &hInspImage);
		get_image_pointer1(hInspImage, (Hlong*)&bFilterRslt, typ, &width, &height);
		lwidth = width;
		lheight = height;

		for (int j = 0; j < MAX_SHIFT_INSP_CNT; j++)
		{
			if (m_InspPara.ShiftInspRect[j][i].isInsp)
			{
				Hobject hRectangle, hCropImage, ImageReduced, Region, temp_rgn, filter_rslt_rgn_black, filter_rslt_rgn_white, filter_rslt_rgn, ins_rgn;
				Hobject sel_white_rgn, sel_black_rgn, inter_rgn, sub_tot_rgn;

				rect.SetRect(m_InspPara.ShiftInspRect[j][i].lRectLeft, m_InspPara.ShiftInspRect[j][i].lRectTop, m_InspPara.ShiftInspRect[j][i].lRectRight, m_InspPara.ShiftInspRect[j][i].lRectBottom);

				gen_rectangle1(&hRectangle, (Hlong)rect.top, (Hlong)rect.left, (Hlong)rect.bottom, (Hlong)rect.right);
				crop_rectangle1(hInspImage, &hCropImage, (Hlong)rect.top, (Hlong)rect.left, (Hlong)rect.bottom, (Hlong)rect.right);

				SetImageFilter(disp, &hCropImage, m_nInsp_Idx, m_InspPara.bFilterImgView, bSave);
				ClipImage(&hCropImage, &hInspImage, rect);

				copy_obj(hRectangle, &ins_rgn, 1, -1);
				union2(hInspRgn, ins_rgn, &hInspRgn);
				reduce_domain(hInspImage, hRectangle, &ImageReduced);

				threshold(ImageReduced, &Region, 0., (double)m_InspPara.PMSearchPara[m_nInsp_Idx].nThreshHold[0]);
				connection(Region, &filter_rslt_rgn_black);

				threshold(ImageReduced, &Region, (double)m_InspPara.PMSearchPara[m_nInsp_Idx].nThreshHold[1], 255.);
				connection(Region, &filter_rslt_rgn_white);

				if (m_InspPara.PMSearchPara[m_nInsp_Idx].nMargeDistance > 0)
				{
					dilation_circle(filter_rslt_rgn_black, &filter_rslt_rgn_black, HTuple(m_InspPara.PMSearchPara[m_nInsp_Idx].nMargeDistance));

					dilation_circle(filter_rslt_rgn_white, &filter_rslt_rgn_white, HTuple(m_InspPara.PMSearchPara[m_nInsp_Idx].nMargeDistance));
				}

				union1(filter_rslt_rgn_black, &filter_rslt_rgn_black);
				union1(filter_rslt_rgn_white, &filter_rslt_rgn_white);

				connection(filter_rslt_rgn_black, &filter_rslt_rgn_black);
				connection(filter_rslt_rgn_white, &filter_rslt_rgn_white);

				if (m_InspPara.PMSearchPara[m_nInsp_Idx].nMargeDistance > 0)
				{
					closing_circle(filter_rslt_rgn_black, &filter_rslt_rgn_black, HTuple(m_InspPara.PMSearchPara[m_nInsp_Idx].nMargeDistance));

					closing_circle(filter_rslt_rgn_white, &filter_rslt_rgn_white, HTuple(m_InspPara.PMSearchPara[m_nInsp_Idx].nMargeDistance));
				}

				HTuple Num, Area, row, col, row0, col0, row1, col1;
				CRect temp_rect, rslt_rect;
				BOOL bSum;
				long black_cnt = 0, white_cnt = 0, rslt_cnt;
				count_obj(filter_rslt_rgn_black, &Num);
				black_cnt = (long)(Hlong)Num[0];
				count_obj(filter_rslt_rgn_white, &Num);

				white_cnt = (long)(Hlong)Num[0];

				//black & white 붙어있는 것만 고르기
				////////////////////////////////////////////////////////////////////
				gen_empty_obj(&temp_rgn);
				rslt_cnt = 0;
				if (black_cnt > 0 && white_cnt > 0)
				{
					for (int i = 0; i < black_cnt - 1; i++)
					{
						gen_empty_obj(&sel_black_rgn);
						select_obj(filter_rslt_rgn_black, &sel_black_rgn, i + 1);
						area_center(sel_black_rgn, &Area, &row, &col);
						smallest_rectangle1(sel_black_rgn, &row0, &col0, &row1, &col1);
						temp_rect = CRect((Hlong)col0[0], (Hlong)row0[0], (Hlong)col1[0], (Hlong)row1[0]);
						bSum = FALSE;

						if (rslt_rect.IntersectRect(rect, temp_rect) == 0)
						{
							continue;
						}

						for (int j = 0; j < white_cnt; j++)
						{
							gen_empty_obj(&sel_white_rgn);
							select_obj(filter_rslt_rgn_white, &sel_white_rgn, j + 1);
							intersection(sel_black_rgn, sel_white_rgn, &inter_rgn);
							area_center(inter_rgn, &Area, &row, &col);

							if (1 < (Hlong)Area[0])
							{
								bSum = TRUE;
								union2(sel_black_rgn, sel_white_rgn, &sub_tot_rgn);
								union2(temp_rgn, sub_tot_rgn, &sub_tot_rgn);
								union1(sub_tot_rgn, &temp_rgn);
							}
						}

						if (bSum)
						{
							union1(temp_rgn, &temp_rgn);
						}
					}
					////////////////////////////////////////////////////////////////////
					connection(temp_rgn, &temp_rgn);

					count_obj(temp_rgn, &Num);

					rslt_cnt = 0;
					rslt_cnt = (long)(Hlong)Num[0];

					//작은 노이즈 없에기
					select_shape(temp_rgn, &temp_rgn, HTuple("area"), HTuple("and"), HTuple(m_InspPara.PMSearchPara[m_nInsp_Idx].nNoiseSize[0]), HTuple("max"));
					select_shape(temp_rgn, &temp_rgn, HTuple("width"), HTuple("and"), HTuple(m_InspPara.PMSearchPara[m_nInsp_Idx].nNoiseSize[1]), HTuple(rect.Height()));
					select_shape(temp_rgn, &temp_rgn, HTuple("height"), HTuple("and"), HTuple(m_InspPara.PMSearchPara[m_nInsp_Idx].nNoiseSize[2]), HTuple(rect.Height()));

					count_obj(temp_rgn, &Num);

					rslt_cnt = 0;
					rslt_cnt = (long)(Hlong)Num[0];

					union1(temp_rgn, &temp_rgn);
					union2(hRslt_rgn, temp_rgn, &hRslt_rgn);
				}

				if (disp)
				{
					set_font(disp, "-Arial-12-*-*-*-*-*-1-");
					set_draw(disp, HTuple("margin"));
					memset(result0, NULL, 20);
					sprintf(result0, "%d", (int)rslt_cnt);

					set_color(disp, HTuple("yellow"));
					disp_obj(ins_rgn, disp);
					bNGRslt = FALSE;
					if (rslt_cnt > 0)
					{
						set_color(disp, HTuple("red"));

						if (rslt_cnt > 0)
							disp_obj(temp_rgn, disp);

						bNGRslt = TRUE;
						set_tposition(disp, rect.top - 20, rect.left);
						switch (j)
						{
						case 0:
							write_string(disp, HTuple("Left Shift Cnt : ") + HTuple(result0));
							break;
						case 1:
							write_string(disp, HTuple("Right Shift Cnt : ") + HTuple(result0));
							break;
						case 2:
							write_string(disp, HTuple("Top Shift Cnt : ") + HTuple(result0));
							break;
						case 3:
							write_string(disp, HTuple("Bottom Shift Cnt : ") + HTuple(result0));
							break;
						}
					}
					for (int k = 3; k > -1; k--)
					{
						m_InspRslt[m_nInsp_Idx].wShiftInspRslt[m_nInsp_Idx][i] = m_InspRslt[m_nInsp_Idx].wShiftInspRslt[m_nInsp_Idx][i] | (bNGRslt << k);
					}
				}
			}
		}
	}

	//m_InspRslt[m_nInsp_Idx].bShiftInspRslt = bInspRslt;
	HTuple aaaa;
	long aaa;
	count_obj(hInspRgn, &aaaa);
	aaa = (Hlong)aaaa[0];
	copy_obj(hInspRgn, &m_InspRslt[m_nInsp_Idx].hShiftInsp_rgn, 1, -1);

	//if (bInspRslt)
	{
		copy_obj(hRslt_rgn, &m_InspRslt[m_nInsp_Idx].hShiftRslt_rgn, 1, -1);
	}
}

Hobject CVisionModule::GetInspRgn(int nInspIdx, int nInspType)
{
	HTuple Num;
	long lNum;
	if (nInspType == 0)
	{
		if (m_InspRslt[nInspIdx].hInsp_rgn.Id() == NULL)
		{
			return NULL;
		}
		count_obj(m_InspRslt[nInspIdx].hInsp_rgn, &Num);
		lNum = (long)(Hlong)Num[0];
		if(lNum > 0)
			return m_InspRslt[nInspIdx].hInsp_rgn;
	}
	else
	{
		if (m_InspRslt[nInspIdx].hShiftInsp_rgn.Id() == NULL)
		{
			return NULL;
		}
		count_obj(m_InspRslt[nInspIdx].hShiftInsp_rgn, &Num);
		lNum = (long)(Hlong)Num[0];
		if (lNum > 0)
			return m_InspRslt[nInspIdx].hShiftInsp_rgn;
	}
	return NULL;
}

Hobject CVisionModule::GetInspRsltRgn(int nInspIdx, int nInspType)
{ 
	if (nInspType == 0)
		return m_InspRslt[nInspIdx].hRslt_rgn; 
	else
		return m_InspRslt[nInspIdx].hShiftRslt_rgn;
}

BOOL CVisionModule::SetInspImageSize(int nEndGrabHeight, int nStartYIdx, int nEndYIdx, int nParallelInspIdx)
{
	BOOL rslt = FALSE;
	int nEndY = 0;
	if (nEndYIdx > m_InspPara.InspRectRecipe[m_nInsp_Idx].nYRectCnt)
	{
		nEndYIdx = m_InspPara.InspRectRecipe[m_nInsp_Idx].nYRectCnt - 1;
	}

	nEndY = m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[nEndYIdx].lRectBottom + m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[nEndYIdx].lOffsetY + (m_InspPara.InspRectRecipe[m_nInsp_Idx].nInspYSpace / 2);

	if (nEndY <= nEndGrabHeight)
	{
		m_dStartTime = GetCurrentTime();
		SetInspImageSize(nStartYIdx, nEndYIdx, nParallelInspIdx);
		rslt = TRUE;
	}

	return rslt;
}

void CVisionModule::SetInspImageSize(int nStartYIdx, int nEndYIdx, int nParallelInspIdx)
{
	int nStartY = 0, nEndY = 0;
	if (nEndYIdx > m_InspPara.InspRectRecipe[m_nInsp_Idx].nYRectCnt)
	{
		nEndYIdx = m_InspPara.InspRectRecipe[m_nInsp_Idx].nYRectCnt - 1;
	}
	nStartY = m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[nStartYIdx].lRectTop + m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[nStartYIdx].lOffsetY - (m_InspPara.InspRectRecipe[m_nInsp_Idx].nInspYSpace / 2);
	nEndY = m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[nEndYIdx].lRectBottom + m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[nEndYIdx].lOffsetY + (m_InspPara.InspRectRecipe[m_nInsp_Idx].nInspYSpace / 2);
	m_nStartInspRectIdx[nParallelInspIdx] = nStartYIdx;
	m_nEndInspRectIdx[nParallelInspIdx] = nEndYIdx;

	m_InspImageSize[nParallelInspIdx].left = 0;
	m_InspImageSize[nParallelInspIdx].top = nStartY;
	m_InspImageSize[nParallelInspIdx].right = m_lWidth;
	m_InspImageSize[nParallelInspIdx].bottom = nEndY;
}
/*
BOOL CVisionModule::SetInspImageSize(int nStartY, int nEndY, int nParallelInspIdx)
{
	m_InspImageSize[nParallelInspIdx].left = 0;
	m_InspImageSize[nParallelInspIdx].top = nStartY;
	m_InspImageSize[nParallelInspIdx].right = m_lWidth;
	m_InspImageSize[nParallelInspIdx].bottom = nEndY;
	CRect rect, temp_rect;
	CPoint pTopLeft, pTopRight, pBottomLeft, pBottomRight;
	int nCnt = 0;
	temp_rect = m_InspImageSize[nParallelInspIdx];
	BOOL bSearch = FALSE;

	for (int i = 0; i < m_InspPara.InspRectRecipe[m_nInsp_Idx].nYRectCnt; i++)
	{
		rect.left = m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[i].lRectLeft + m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[i].lOffsetX;
		rect.right = m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[i].lRectRight + m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[i].lOffsetX;
		rect.top = m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[i].lRectTop + m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[i].lOffsetY;
		rect.bottom = m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[i].lRectBottom + m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[i].lOffsetY;
		
		//if (rect.top < m_InspImageSize[nParallelInspIdx].top)
		//	continue;

		if (nCnt == 0)
		{
			m_nStartInspRectIdx[nParallelInspIdx] = i;
		}

		if (rect.bottom > m_InspImageSize[nParallelInspIdx].bottom)
			break;

		pTopLeft = rect.TopLeft();
		pTopRight.SetPoint(rect.right, rect.top);
		pBottomLeft.SetPoint(rect.left, rect.bottom);
		pBottomRight = rect.BottomRight();

		if (nCnt == 0)
		{
			if ((!temp_rect.PtInRect(pTopLeft) && !temp_rect.PtInRect(pTopRight)) &&
				(temp_rect.PtInRect(pBottomLeft) && temp_rect.PtInRect(pBottomRight)) && !bSearch)
			{
				temp_rect.top -= m_InspImageSize[nParallelInspIdx].Height();
				temp_rect.bottom -= m_InspImageSize[nParallelInspIdx].Height();
				i = -1;
				bSearch = TRUE;
				continue;
			}
			else if ((temp_rect.PtInRect(pTopLeft) && temp_rect.PtInRect(pTopRight)) &&
				(temp_rect.PtInRect(pBottomLeft) && temp_rect.PtInRect(pBottomRight)))
			{
				if(temp_rect.top != m_InspImageSize[nParallelInspIdx].top)
					continue;
				else
				{
					nCnt++;
				}
			}
			else if ((temp_rect.PtInRect(pTopLeft) && temp_rect.PtInRect(pTopRight)) &&
				(!temp_rect.PtInRect(pBottomLeft) && !temp_rect.PtInRect(pBottomRight)))
			{

				nCnt++;
			}
		}
		else
		{
			if ((m_InspImageSize[nParallelInspIdx].PtInRect(pTopLeft) && m_InspImageSize[nParallelInspIdx].PtInRect(pTopRight)) &&
				(m_InspImageSize[nParallelInspIdx].PtInRect(pBottomLeft) && m_InspImageSize[nParallelInspIdx].PtInRect(pBottomRight)))
			{
				nCnt++;
			}
			else if ((m_InspImageSize[nParallelInspIdx].PtInRect(pTopLeft) && m_InspImageSize[nParallelInspIdx].PtInRect(pTopRight)) &&
				(!m_InspImageSize[nParallelInspIdx].PtInRect(pBottomLeft) && !m_InspImageSize[nParallelInspIdx].PtInRect(pBottomRight)))
			{
				break;
			}
		}
	}

	if (nCnt == 0)
	{
		return FALSE;
	}
	m_nEndInspRectIdx[nParallelInspIdx] = m_nStartInspRectIdx[nParallelInspIdx] + nCnt;
	m_nMaxInspRectCnt += m_nEndInspRectIdx[nParallelInspIdx];

	if (temp_rect.bottom < m_InspImageSize[nParallelInspIdx].bottom)
	{
		m_InspImageSize[nParallelInspIdx].SetRect(temp_rect.TopLeft(), m_InspImageSize[nParallelInspIdx].BottomRight());
	}

	return TRUE;
}
*/

void CVisionModule::InspInit(int nThreadCnt)
{
	for (int nInsThreadIdx = 0; nInsThreadIdx < nThreadCnt; nInsThreadIdx++)
	{
		m_Inspection[nInsThreadIdx]->InitRslt();
		if (!m_Inspection[nInsThreadIdx]->GetUnderInspection())
		{
			delete m_Inspection[nInsThreadIdx];
			m_Inspection[nInsThreadIdx] = NULL;
		}
		else
		{
			if (m_Inspection[nInsThreadIdx]->WaitEndThread())
			{
				delete m_Inspection[nInsThreadIdx];
				m_Inspection[nInsThreadIdx] = NULL;
			}
		}
	}
}

void CVisionModule::ParallelMainInsp()
{
	BOOL bInspEnd = FALSE, bInspInit = FALSE;
	int i = 0, j = 0, k = 0, o = 0;
	CRect rect[1000];
	double s_time, e_time;
	CString strText, strText1, strText2;
	CTime NowTime = CTime::GetCurrentTime();
	int nLineCnt = 0;

	while (TRUE)
	{
		if (m_bParallelMainInspThreadStart)
		{
			if (g_MilCtrl.GetGrabLineCnt() == 0)
			{
				
			}

			if (m_bInspectionEnd)
			{
				m_bInspectionStart = FALSE;
				m_bInspectionEnd = TRUE;
				bInspInit = FALSE;
			}
			nLineCnt = g_MilCtrl.GetGrabLineCnt();
			if (g_MilCtrl.GetGrabLineCnt() > 0 && m_bInspectionStart)
			{
				if (g_MilCtrl.GetGrabLineCnt() > 0 && !bInspInit)
				{
					bInspInit = TRUE;
					s_time = GetCurrentTime();
					NowTime = CTime::GetCurrentTime();
					m_nInspThreadIdx = 0;

					if (m_InspPara.bAutoInspRectTeaching)
					{
						SelectInspectionSpace(-1);
						SelectInspectionSpace(INSPECTION_SPACE_PM_SHIFT);
					}
					SelectInspectionSpace(3);
					
					m_bUnderInspEnd = FALSE;
					bInspEnd = FALSE;
					i = 0;
					o = 0;// m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[0].lRectTop / m_lGrabHeight;
					m_nInspCnt = 0;
					m_bFilterImageView = FALSE;
					m_InspRslt[m_nInsp_Idx].lRslt_num = 0;
					m_strTack.Empty();
					m_nParallelInspIdx = 0;
					g_MilCtrl.SetGrabLineCnt(o + 1);
					//g_MilCtrl.SetGrabLineCnt(0);
					if (m_VisionInsp_CS.m_sect.LockCount > -1)
					{
						for (int z = 0; z < (m_VisionInsp_CS.m_sect.LockCount + 1); z++)
						{
							m_VisionInsp_CS.Unlock();
						}
					}
					if (m_VisionInsp_CS2.m_sect.LockCount > -1)
					{
						for (int z = 0; z < (m_VisionInsp_CS2.m_sect.LockCount + 1); z++)
						{
							m_VisionInsp_CS2.Unlock();
						}
					}
					if (m_VisionInsp_CS3.m_sect.LockCount > -1)
					{
						for (int z = 0; z < (m_VisionInsp_CS3.m_sect.LockCount + 1); z++)
						{
							m_VisionInsp_CS3.Unlock();
						}
					}

					m_strRsltFileName.Format(_T("%02d%02d%02d"), NowTime.GetHour(), NowTime.GetMinute(), NowTime.GetSecond());
					k = 0;
					g_MilCtrl.SetGrabCopyIdx(k);
				}

				if (o > MAX_GRAB_HEIGHT / m_lGrabHeight)
				{
					g_MilCtrl.SetGrabLineCnt(0);
					bInspInit = FALSE;
				}

				j = (o) * m_lGrabHeight;// *4;
				m_nStartInspRectIdx[k] = 0;
				m_nEndInspRectIdx[k] = 0;

				//if (nLineCnt >= o)
				{
					if (SetInspImageSize(j + m_lGrabHeight, k * INSP_LEAD_CNT, k * INSP_LEAD_CNT + INSP_LEAD_CNT - 1, k))
					{
						if (!GetTestInsp())
						{
							if (!g_MilCtrl.GetGrabCopy(k))
							{
								g_MilCtrl.SetGrabCopy(k);
								int nSleepCnt = 0;
								while (TRUE)
								{
									if (!g_MilCtrl.GetGrabCopy(k))
									{
										break;
									}
									if (nSleepCnt > 5)
									{
										break;
									}
									Sleep(1);
									nSleepCnt++;
								}

								g_MilCtrl.GetImage(&m_ImageInfo);
								if (!ConvImgToHalBuf(&m_HalGrabImage, &m_ImageInfo))
								{
									g_MilCtrl.m_VisionGrab_CS.Unlock();
									continue;
								}
								g_MilCtrl.m_VisionGrab_CS.Unlock();
							}
						}
						SetParallelInsp(&m_HalGrabImage, k);
						m_dEndTime = GetCurrentTime();
						m_strTest[k].Format(_T("%.3f sec, EndGrabHeight %d"), (m_dEndTime - m_dStartTime) / 1000., j + m_lGrabHeight);
						m_nInspThreadIdx++;
						k++;
					}

					/*
					SetInspImageSize(k * INSP_LEAD_CNT, k * INSP_LEAD_CNT + INSP_LEAD_CNT - 1, k);
					SetParallelInsp(&m_HalGrabImage, k);
					m_nInspThreadIdx++;
					k++;
					*/
				}
					o++;

				bInspEnd = m_bUnderInspEnd;

				//if ((g_MilCtrl.GetGrabLineCnt() * m_lGrabHeight > m_nLastInspRectY) || bInspEnd)
				if((k - 1) * INSP_LEAD_CNT + INSP_LEAD_CNT - 1 > m_InspPara.InspRectRecipe[m_nInsp_Idx].nYRectCnt)
				{
					g_MilCtrl.SetGrabLineCnt(0);
					g_MilCtrl.SetGrabCopyStart(FALSE);
					o = -1;
					g_MilCtrl.GetImage(&m_ImageInfo);
					if (!ConvImgToHalBuf(&m_HalGrabImage, &m_ImageInfo))
					{
						g_MilCtrl.m_VisionGrab_CS.Unlock();
						continue;
					}
					gen_image_proto(m_HalDispFilterImage, &m_HalDispFilterImage, 0.0);
					copy_image(m_HalGrabImage, &m_HalDispFilterImage);
					copy_image(m_HalGrabImage, &m_HalDispImage);
					
					BOOL *bThreadEnd;
					BOOL bAllThreadEnd = FALSE;
					bThreadEnd = new BOOL[k];
					m_nInspThreadIdx = k;

					while (TRUE)
					{
						for (int nInsThreadIdx = 0; nInsThreadIdx < k; nInsThreadIdx++)
						{
							bThreadEnd[nInsThreadIdx] = FALSE;
							if (m_Inspection[nInsThreadIdx] != NULL)
							{
								if (m_Inspection[nInsThreadIdx]->WaitEndThread())
								{
									bThreadEnd[nInsThreadIdx] = TRUE;
								}
							}
						}

						for (int nInsThreadIdx = 0; nInsThreadIdx < k; nInsThreadIdx++)
						{
							if (bThreadEnd[nInsThreadIdx])
							{
								bAllThreadEnd = TRUE;
							}
							else
							{
								break;
							}
						}

						if (bAllThreadEnd)
						{
							break;
						}
						Sleep(10);
					}

					delete bThreadEnd;

					SumInspRslt();
					e_time = GetCurrentTime();
					strText.Format(_T("%.3f sec"), (e_time - s_time) / 1000.0);
					DisplayRslt();
					if (!bInspEnd)
					{
						WritePMInfoRslt();
						NowTime = CTime::GetCurrentTime();
						strText1.Format(_T("%02d:%02d:%02d,%s"), NowTime.GetHour(), NowTime.GetMinute(), NowTime.GetSecond(), strText);
						for (int z = 0; z < m_InspPara.InspRectRecipe[m_nInsp_Idx].nYRectCnt; z++)
						{
							strText2.Format(_T("%S"), m_InspRslt[m_nInsp_Idx].cRsltText[z]);
							strText.Format(_T("%s,%s"), strText1, strText2.Mid(9, strText2.GetLength() - 9));
							strText1.Format(_T("%s"), strText);
						}
						WriteRslt(strText1);
					}

					InspInit(k);

					m_bInspectionEnd = TRUE;
					m_bInspectionStart = FALSE;
					m_nParallelInspCnt = 0;
					bInspInit = FALSE;
					i = 0;
					o = -1;
					sprintf_s(m_cSavePath, m_strModelPath.GetLength() + 1, "%S", m_strModelPath);
					SetTestInsp(FALSE);
					//SaveImage(&m_HalDispFilterImage, ("d:\\abcdefg.bmp"));
				}
				//g_MilCtrl.SetGrabLineCnt(o + 1);				
			}
		}

		if (m_bParalleMainlInspThreadEnd)
		{
			break;
		}
		Sleep(1);
	}
}

BOOL CVisionModule::GetParallelThreadEnd(int nThreadIdx)
{
	BOOL rslt = FALSE;
	if (m_pThreadParallelInsp[nThreadIdx] != NULL)
	{
		DWORD dwRet = WaitForSingleObject(m_pThreadParallelInsp[nThreadIdx]->m_hThread, 10000);
		//_ASSERTE( dwRet != WAIT_TIMEOUT );		//150702 khyoo message로 변경
		if (m_pThreadParallelInsp[nThreadIdx])
		{
			delete m_pThreadParallelInsp[nThreadIdx];
			m_pThreadParallelInsp[nThreadIdx] = NULL;
		}

		rslt = TRUE;
	}

	return rslt;
}

UINT CVisionModule::ParallelMainInspThread(LPVOID pParam)
{
	CVisionModule *pdlg = (CVisionModule *)pParam;

	pdlg->ParallelMainInsp();

	return 0;
}

void CVisionModule::SetParallelInsp(Hobject *image, int nIdx)
{
	Hobject hInspImage, hTempImage;
	copy_image(*image, &hTempImage);
	if (m_Inspection[nIdx] == NULL)
	{
		m_Inspection[nIdx] = new CInspection();
	}

	if (m_Inspection[nIdx]->GetUnderInspection())
	{
		m_Inspection[nIdx]->WaitEndThread();
	}

	m_ThreadInspPara[nIdx].nYRectCnt = m_nEndInspRectIdx[nIdx] - m_nStartInspRectIdx[nIdx] + 1;

	int n = 0;
	memset(m_ThreadInspPara[nIdx].ShiftInspRect[0], NULL, sizeof(CRect) * 200);
	memset(m_ThreadInspPara[nIdx].ShiftInspRect[1], NULL, sizeof(CRect) * 200);
	memset(m_ThreadInspPara[nIdx].ShiftInspRect[2], NULL, sizeof(CRect) * 200);
	memset(m_ThreadInspPara[nIdx].ShiftInspRect[3], NULL, sizeof(CRect) * 200);
	for (int m = m_nStartInspRectIdx[nIdx]; m < m_nEndInspRectIdx[nIdx] + 1; m++)
	{
		m_ThreadInspPara[nIdx].InspRect[n].left = m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[m].lRectLeft + m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[m].lOffsetX;
		m_ThreadInspPara[nIdx].InspRect[n].right = m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[m].lRectRight + m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[m].lOffsetX;
		m_ThreadInspPara[nIdx].InspRect[n].top = m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[m].lRectTop + m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[m].lOffsetY - m_InspImageSize[nIdx].top;
		m_ThreadInspPara[nIdx].InspRect[n].bottom = m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[m].lRectBottom + m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[m].lOffsetY - m_InspImageSize[nIdx].top;
		m_ThreadInspPara[nIdx].nInspRectIdx[n] = m;

		for (int i = 0; i < 4; i++)
		{
			if (m_InspPara.ShiftInspRect[i][m].isInsp)
			{
				m_ThreadInspPara[nIdx].ShiftInspRect[i][n] = m_InspPara.ShiftInspRect[i][m];
			}
		}

		n++;
	}
	m_nLastInspRectIdx = m_nEndInspRectIdx[nIdx];
	m_ThreadInspPara[nIdx].nStartLeadIdx = m_nStartInspRectIdx[nIdx];
	m_ThreadInspPara[nIdx].nEndLeadIdx = m_nEndInspRectIdx[nIdx];
	m_ThreadInspPara[nIdx].InspImageSize = m_InspImageSize[nIdx];
	m_ThreadInspPara[nIdx].nInspYSpace = m_InspPara.InspRectRecipe[m_nInsp_Idx].nInspYSpace;
	m_ThreadInspPara[nIdx].bFilterImgView = m_InspPara.bFilterImgView;
	m_ThreadInspPara[nIdx].bInspImgSave = m_InspPara.bInspImgSave;
	m_ThreadInspPara[nIdx].bInspImgView = m_InspPara.bInspImgView;
	m_ThreadInspPara[nIdx].ImageFilterPara = m_InspPara.ImageFilterPara[m_nInsp_Idx];
	m_ThreadInspPara[nIdx].PMSearchPara = m_InspPara.PMSearchPara[m_nInsp_Idx];
	m_ThreadInspPara[nIdx].dPixelSize = m_InspPara.dPixelSize;
	m_ThreadInspPara[nIdx].PMDistributionInspPara = m_InspPara.PMDistributionInspPara;
	m_ThreadInspPara[nIdx].PMDistributionInspPara.nDistributionMinCnt = 1;
	m_ThreadInspPara[nIdx].PMDistributionInspPara.nDistributionXCnt = 4;
	m_ThreadInspPara[nIdx].PMDistributionInspPara.nDistributionYCnt = 2;

	crop_rectangle1(hTempImage, &hInspImage, m_InspImageSize[nIdx].top, m_InspImageSize[nIdx].left, m_InspImageSize[nIdx].bottom, m_InspImageSize[nIdx].right - 1);

	m_Inspection[nIdx]->SetParallelInsp(m_Disp, m_PartSize, nIdx, m_InspImageSize[nIdx], &hInspImage, &m_ThreadInspPara[nIdx]);
}

void CVisionModule::SetParallelInsp(int nIdx)
{
	if (m_pThreadParallelInsp[nIdx] != NULL || m_bParallelUnderInspection[nIdx])
	{
		DWORD dwRet = WaitForSingleObject(m_pThreadParallelInsp[nIdx]->m_hThread, 60000);

		if (m_pThreadParallelInsp[nIdx])
		{
			delete m_pThreadParallelInsp[nIdx];
			m_pThreadParallelInsp[nIdx] = NULL;
			m_bParallelUnderInspection[nIdx] = FALSE;
		}
	}

	m_bParallelUnderInspection[nIdx] = TRUE;

	m_bParallelInspThreadStart[nIdx] = TRUE;
	m_bParallelInspThreadEnd[nIdx] = FALSE;
	m_bParallelInspStart[nIdx] = FALSE;
	m_bParallelInspEnd[nIdx] = TRUE;
	m_bParallelUnderInspection[nIdx] = TRUE;
	m_nParallelInspIdx = nIdx;

	m_ThreadInspPara[nIdx].nYRectCnt = m_nEndInspRectIdx[nIdx] - m_nStartInspRectIdx[nIdx] + 1;

	int n = 0;
	for (int m = m_nStartInspRectIdx[nIdx]; m < m_nEndInspRectIdx[nIdx] + 1; m++)
	{
		m_ThreadInspPara[nIdx].InspRect[n].left = m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[m].lRectLeft + m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[m].lOffsetX;
		m_ThreadInspPara[nIdx].InspRect[n].right = m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[m].lRectRight + m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[m].lOffsetX;
		m_ThreadInspPara[nIdx].InspRect[n].top = m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[m].lRectTop + m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[m].lOffsetY - m_InspImageSize[nIdx].top;
		m_ThreadInspPara[nIdx].InspRect[n].bottom = m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[m].lRectBottom + m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[m].lOffsetY - m_InspImageSize[nIdx].top;
		m_ThreadInspPara[nIdx].nInspRectIdx[n] = m;
		n++;
	}
	m_nLastInspRectIdx = m_nEndInspRectIdx[nIdx];
	m_ThreadInspPara[nIdx].InspImageSize = m_InspImageSize[nIdx];
	m_ThreadInspPara[nIdx].nInspYSpace = m_InspPara.InspRectRecipe[m_nInsp_Idx].nInspYSpace;
	m_ThreadInspPara[nIdx].bFilterImgView = m_InspPara.bFilterImgView;
	m_ThreadInspPara[nIdx].bInspImgSave = m_InspPara.bInspImgSave;
	m_ThreadInspPara[nIdx].bInspImgView = m_InspPara.bInspImgView;
	m_ThreadInspPara[nIdx].ImageFilterPara = m_InspPara.ImageFilterPara[m_nInsp_Idx];
	m_ThreadInspPara[nIdx].PMSearchPara = m_InspPara.PMSearchPara[m_nInsp_Idx];
	m_ThreadInspPara[nIdx].dPixelSize = m_InspPara.dPixelSize;

#ifndef THREAD_TEST
	m_VisionInsp_CS.Lock();
	m_ThreadInspPara[nIdx].hInspImage.Reset();
	m_ThreadInspPara[nIdx].hFilterImage.Reset();
	crop_rectangle1(m_HalGrabImage, &m_ThreadInspPara[nIdx].hInspImage, m_InspImageSize[nIdx].top, m_InspImageSize[nIdx].left, m_InspImageSize[nIdx].bottom, m_InspImageSize[nIdx].right - 1);
	m_VisionInsp_CS.Unlock();

	m_pThreadParallelInsp[nIdx] = AfxBeginThread(ParallelInspThread, this, THREAD_PRIORITY_HIGHEST, 0, CREATE_SUSPENDED);
	m_pThreadParallelInsp[nIdx]->m_bAutoDelete = FALSE;
	m_pThreadParallelInsp[nIdx]->ResumeThread();
#endif
}

void CVisionModule::ParallelInsp()
{
	double start_timer, end_timer;
	CString strText;
	typeThreadInsp ThreadInspPara;
	//Hobject hInspImg;
	int nInspIdx = m_nParallelInspIdx;
	long disp = m_Disp;
#ifndef THREAD_TEST
	copy_image(m_ThreadInspPara[nInspIdx].hInspImage, &ThreadInspPara.hInspImage);
#endif
	strText.Format(_T("%d Inspection Thread Start"), nInspIdx);

	start_timer = GetCurrentTime();

	for (int i = 0; i < m_ThreadInspPara[nInspIdx].nYRectCnt; i++)
	{
		ThreadInspPara.InspRect[i] = m_ThreadInspPara[nInspIdx].InspRect[i];
		ThreadInspPara.nInspRectIdx[i] = m_ThreadInspPara[nInspIdx].nInspRectIdx[i];
	}
	ThreadInspPara.nYRectCnt = m_ThreadInspPara[nInspIdx].nYRectCnt;
	ThreadInspPara.nInspYSpace = m_ThreadInspPara[nInspIdx].nInspYSpace;
	ThreadInspPara.nShiftInspSpaceX = m_ThreadInspPara[nInspIdx].nShiftInspSpaceX;
	ThreadInspPara.nShiftInspSpaceY = m_ThreadInspPara[nInspIdx].nShiftInspSpaceY;
	ThreadInspPara.InspImageSize = m_ThreadInspPara[nInspIdx].InspImageSize;
	ThreadInspPara.ImageFilterPara = m_ThreadInspPara[nInspIdx].ImageFilterPara;
	ThreadInspPara.PMSearchPara = m_ThreadInspPara[nInspIdx].PMSearchPara;
	ThreadInspPara.bFilterImgView = m_ThreadInspPara[nInspIdx].bFilterImgView;
	ThreadInspPara.bInspImgSave = m_ThreadInspPara[nInspIdx].bInspImgSave;
	ThreadInspPara.bInspImgView = m_ThreadInspPara[nInspIdx].bInspImgView;
	ThreadInspPara.dPixelSize = m_ThreadInspPara[nInspIdx].dPixelSize;

	m_bParallelUnderInspection[nInspIdx] = TRUE;
	
	//PMCntInspection(disp, &ThreadInspPara.hInspImage, ThreadInspPara, ThreadInspPara.bFilterImgView, ThreadInspPara.bInspImgSave, ThreadInspPara.bInspImgView);
#ifndef THREAD_TEST
	PMCntInspection(disp, &ThreadInspPara.hInspImage, ThreadInspPara, ThreadInspPara.bFilterImgView, ThreadInspPara.bInspImgSave, ThreadInspPara.bInspImgView);
#endif
	m_bParallelUnderInspection[nInspIdx] = FALSE;

	end_timer = GetCurrentTime();
	strText.Format(_T("%d InspectionThread Tack Time = %.4f sec"), nInspIdx, (end_timer - start_timer) / 1000.0);
	strText.Format(_T("%.4f sec,"), (end_timer - start_timer) / 1000.0);
	m_strTack += strText;
	strText.Format(_T("%d Inspection Thread End"), nInspIdx);
	//ThreadInspPara.hInspImage.Reset();
}

BOOL CVisionModule::PMCntInspection(long disp, Hobject *image, typeThreadInsp ThreadInspPara, BOOL bFilterDisp, BOOL bSave)
{
	Hobject hInspImage, Save_Image, hFilterRslt, hRsltImage, hCropImage;
	CRect rect;
	int nRgnOffsetY = ThreadInspPara.InspImageSize.top;
	char result0[20];
	Hobject hRslt_rgn;
	int nMaskWidth, nMaskHeight, nMaxNoiseSize;
	long black_cnt, white_cnt;
	HTuple Num, Area, row, col, row0, col0, row1, col1, Ra, Rb, Phi;
	BOOL bSum = FALSE;
	CRect temp_rect, rslt_rect;
	int rslt_cnt;
	Hlong temp_row;
	HTuple row_temp, row_temp1;

	copy_obj(m_InspRslt[m_nInsp_Idx].hRslt_rgn, &hRslt_rgn, 1, -1);

	copy_image(m_HalGrabImage, &hRsltImage);
	copy_image(*image, &hInspImage);
	copy_image(*image, &hFilterRslt);
	copy_image(*image, &Save_Image);

	SetImageFilter(disp, &hInspImage, &hFilterRslt, ThreadInspPara, bSave);

	//gen_empty_obj(&hFilterRslt);
	//gen_image1(&hFilterRslt, "byte", lwidth, lheight, (Hlong)bFilterRslt);
	//copy_image(hInspImage, &hFilterRslt);

	rect = ThreadInspPara.InspImageSize;

	rect.top = (ThreadInspPara.InspImageSize.top + ThreadInspPara.InspRect[0].top) - (ThreadInspPara.nInspYSpace / 2);

	if (rect.top >= ThreadInspPara.InspImageSize.top)
	{
		crop_rectangle1(hFilterRslt, &hCropImage, rect.top - ThreadInspPara.InspImageSize.top, rect.left, rect.bottom - ThreadInspPara.InspImageSize.top, rect.right - 1);
	}
	else
	{
		copy_image(hFilterRslt, &hCropImage);
		rect = ThreadInspPara.InspImageSize;
	}

	ClipImage(&hCropImage, &m_HalDispFilterImage, rect);

	if (bFilterDisp)
	{
		copy_image(*GetFilterImage(), GetDispImage());
		disp_obj(*GetDispImage(), disp);
	}
	else
	{
		copy_image(*GetGrabImage(), GetDispImage());
	}

	for (int i = 0; i < ThreadInspPara.nYRectCnt; i++)
	{
		Hobject hRectangle1, ImageReduced1, hRectangle, ImageReduced, temp_rgn, temp_rgn1, filter_rslt_rgn_black, filter_rslt_rgn_white, filter_rslt_rgn, ins_rgn;
		Hobject sel_white_rgn, sel_black_rgn, inter_rgn, sub_tot_rgn;
		
		

		//검사 영역 추출
		/////////////////////////////////////////////////////
		//rect.left = m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[i].lRectLeft + m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[i].lOffsetX;
		//rect.right = m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[i].lRectRight + m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[i].lOffsetX;
		//rect.top = m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[i].lRectTop + m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[i].lOffsetY;
		//rect.bottom = m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[i].lRectBottom + m_InspPara.InspRectRecipe[m_nInsp_Idx].InspRect[i].lOffsetY;
		rect.left = ThreadInspPara.InspRect[i].left;
		rect.right = ThreadInspPara.InspRect[i].right;
		rect.top = ThreadInspPara.InspRect[i].top;
		rect.bottom = ThreadInspPara.InspRect[i].bottom;

		gen_rectangle1(&hRectangle, (Hlong)rect.top, (Hlong)rect.left, (Hlong)rect.bottom, (Hlong)rect.right);
		gen_rectangle1(&ins_rgn, (Hlong)rect.top + nRgnOffsetY, (Hlong)rect.left, (Hlong)rect.bottom + nRgnOffsetY, (Hlong)rect.right);

		nMaskWidth = rect.Width() / 10;
		nMaskHeight = rect.Width() / 10;
		nMaxNoiseSize = rect.Width();
		if (rect.Width() > rect.Height())
		{
			nMaskWidth = rect.Height() / 10;
			nMaskHeight = rect.Height() / 10;
			nMaxNoiseSize = rect.Height();
		}

		gen_rectangle1(&hRectangle1, (Hlong)(rect.top + (nMaskHeight - 1)), (Hlong)(rect.left + (nMaskWidth - 1)), (Hlong)(rect.bottom - nMaskHeight), (Hlong)(rect.right - nMaskWidth));
		reduce_domain(hFilterRslt, hRectangle1, &ImageReduced);

		threshold(ImageReduced, &filter_rslt_rgn_black, 0., m_InspPara.PMSearchPara[m_nInsp_Idx].nThreshHold[0]);
		connection(filter_rslt_rgn_black, &filter_rslt_rgn_black);

		threshold(ImageReduced, &filter_rslt_rgn_white, m_InspPara.PMSearchPara[m_nInsp_Idx].nThreshHold[1], 255.);
		connection(filter_rslt_rgn_white, &filter_rslt_rgn_white);
		black_cnt = 0, white_cnt = 0;
		count_obj(filter_rslt_rgn_black, &Num);
		black_cnt = (long)(Hlong)Num[0];
		count_obj(filter_rslt_rgn_white, &Num);
		white_cnt = (long)(Hlong)Num[0];
		
		erosion_circle(filter_rslt_rgn_black, &filter_rslt_rgn_black, HTuple(1));
		erosion_circle(filter_rslt_rgn_white, &filter_rslt_rgn_white, HTuple(1));

		if (m_InspPara.PMSearchPara[m_nInsp_Idx].nMargeDistance > 0)
		{
			dilation_circle(filter_rslt_rgn_black, &filter_rslt_rgn_black, HTuple(m_InspPara.PMSearchPara[m_nInsp_Idx].nMargeDistance));

			dilation_circle(filter_rslt_rgn_white, &filter_rslt_rgn_white, HTuple(m_InspPara.PMSearchPara[m_nInsp_Idx].nMargeDistance));
		}

		union1(filter_rslt_rgn_black, &filter_rslt_rgn_black);
		union1(filter_rslt_rgn_white, &filter_rslt_rgn_white);

		connection(filter_rslt_rgn_black, &filter_rslt_rgn_black);
		connection(filter_rslt_rgn_white, &filter_rslt_rgn_white);

		if (m_InspPara.PMSearchPara[m_nInsp_Idx].nMargeDistance > 0)
		{
			closing_circle(filter_rslt_rgn_black, &filter_rslt_rgn_black, HTuple(m_InspPara.PMSearchPara[m_nInsp_Idx].nMargeDistance));

			closing_circle(filter_rslt_rgn_white, &filter_rslt_rgn_white, HTuple(m_InspPara.PMSearchPara[m_nInsp_Idx].nMargeDistance));
		}

		select_shape(filter_rslt_rgn_black, &filter_rslt_rgn_black, HTuple("area"), HTuple("and"), HTuple(m_InspPara.PMSearchPara[m_nInsp_Idx].nNoiseSize[0] / 2), HTuple('max'));
		select_shape(filter_rslt_rgn_black, &filter_rslt_rgn_black, HTuple("width"), HTuple("and"), HTuple(m_InspPara.PMSearchPara[m_nInsp_Idx].nNoiseSize[1] / 2), HTuple('max'));
		select_shape(filter_rslt_rgn_black, &filter_rslt_rgn_black, HTuple("height"), HTuple("and"), HTuple(m_InspPara.PMSearchPara[m_nInsp_Idx].nNoiseSize[2] / 2), HTuple('max'));

		select_shape(filter_rslt_rgn_white, &filter_rslt_rgn_white, HTuple("area"), HTuple("and"), HTuple(m_InspPara.PMSearchPara[m_nInsp_Idx].nNoiseSize[0] / 2), HTuple('max'));
		select_shape(filter_rslt_rgn_white, &filter_rslt_rgn_white, HTuple("width"), HTuple("and"), HTuple(m_InspPara.PMSearchPara[m_nInsp_Idx].nNoiseSize[1] / 2), HTuple('max'));
		select_shape(filter_rslt_rgn_white, &filter_rslt_rgn_white, HTuple("height"), HTuple("and"), HTuple(m_InspPara.PMSearchPara[m_nInsp_Idx].nNoiseSize[2] / 2), HTuple('max'));



		black_cnt = 0, white_cnt = 0;
		count_obj(filter_rslt_rgn_black, &Num);
		black_cnt = (long)(Hlong)Num[0];
		count_obj(filter_rslt_rgn_white, &Num);
		white_cnt = (long)(Hlong)Num[0];
		
		//black & white 붙어있는 것만 고르기
		////////////////////////////////////////////////////////////////////
		gen_empty_obj(&temp_rgn);
		rslt_cnt = 0;
		m_nRsltCnt[m_nInsp_Idx][ThreadInspPara.nInspRectIdx[i]] = 0;
		if (black_cnt > 0 && white_cnt > 0)
		{
			for (int i = 0; i < black_cnt - 1; i++)
			{
				int nBlackX, nWhiteX;
				gen_empty_obj(&sel_black_rgn);
				select_obj(filter_rslt_rgn_black, &sel_black_rgn, i + 1);
				dilation_circle(sel_black_rgn, &sel_black_rgn, 3);
				area_center(sel_black_rgn, &Area, &row, &col);
				smallest_rectangle1(sel_black_rgn, &row0, &col0, &row1, &col1);
				temp_rect = CRect((Hlong)col0[0], (Hlong)row0[0], (Hlong)col1[0], (Hlong)row1[0]);
				nBlackX = temp_rect.CenterPoint().x;
				bSum = FALSE;

				if (rslt_rect.IntersectRect(rect, temp_rect) == 0)
				{
					continue;
				}

				for (int j = 0; j < white_cnt; j++)
				{
					gen_empty_obj(&sel_white_rgn);
					select_obj(filter_rslt_rgn_white, &sel_white_rgn, j + 1);
					dilation_circle(sel_white_rgn, &sel_white_rgn, 3);
					intersection(sel_black_rgn, sel_white_rgn, &inter_rgn);
					area_center(inter_rgn, &Area, &row, &col);

					smallest_rectangle1(sel_white_rgn, &row0, &col0, &row1, &col1);
					nWhiteX = ((Hlong)col0[0] + (Hlong)col1[0]) / 2;

					if (1 < (Hlong)Area[0] && nBlackX > nWhiteX)
					{
						bSum = TRUE;
						gen_empty_obj(&sub_tot_rgn);

						obj_diff(filter_rslt_rgn_black, sel_black_rgn, &filter_rslt_rgn_black);
						obj_diff(filter_rslt_rgn_white, sel_white_rgn, &filter_rslt_rgn_white);

						union2(sel_black_rgn, sel_white_rgn, &sub_tot_rgn);

						erosion_circle(sub_tot_rgn, &sub_tot_rgn, 3);

						union2(temp_rgn, sub_tot_rgn, &sub_tot_rgn);
						union1(sub_tot_rgn, &temp_rgn);
					}
				}

				if (bSum)
				{
					union1(temp_rgn, &temp_rgn);
				}
			}
			////////////////////////////////////////////////////////////////////
			connection(temp_rgn, &temp_rgn);

			//작은 노이즈 없에기
			//select_shape(temp_rgn, &temp_rgn, HTuple("area"), HTuple("and"), HTuple(m_InspPara.PMSearchPara[m_nInsp_Idx].nNoiseSize[0]), HTuple(nMaxNoiseSize * 2));
			select_shape(temp_rgn, &temp_rgn, HTuple("width"), HTuple("and"), HTuple(m_InspPara.PMSearchPara[m_nInsp_Idx].nNoiseSize[1]), HTuple(nMaxNoiseSize));
			select_shape(temp_rgn, &temp_rgn, HTuple("height"), HTuple("and"), HTuple(m_InspPara.PMSearchPara[m_nInsp_Idx].nNoiseSize[2]), HTuple(nMaxNoiseSize));

			count_obj(temp_rgn, &Num);
			rslt_cnt = 0;
			rslt_cnt = (long)(Hlong)Num[0];
			
			gen_empty_obj(&temp_rgn1);
			for (int j = 0; j < rslt_cnt; j++)
			{
				Hobject sel_rslt_rgn, Ellipses;
				gen_empty_obj(&Ellipses);
				gen_empty_obj(&sel_rslt_rgn);
				select_obj(temp_rgn, &sel_rslt_rgn, j + 1);
				area_center(sel_rslt_rgn, &Area, &row, &col);
				elliptic_axis(sel_rslt_rgn, &Ra, &Rb, &Phi);
				temp_row = (Hlong)(row[0]) + nRgnOffsetY;
				tuple_gen_const(1, temp_row, &row_temp1);
				gen_ellipse(&Ellipses, row_temp1, col, Phi, Ra, Rb);
				concat_obj(temp_rgn1, Ellipses, &temp_rgn1);
				//union2(temp_rgn1, Ellipses, &temp_rgn1);
			}
			
			connection(temp_rgn1, &temp_rgn1);
			count_obj(temp_rgn1, &Num);
			rslt_cnt = 0;
			rslt_cnt = (long)(Hlong)Num[0];

			m_nRsltCnt[m_nInsp_Idx][ThreadInspPara.nInspRectIdx[i]] = rslt_cnt;
			m_InspRslt[m_nInsp_Idx].lRslt_num += rslt_cnt;

			union1(temp_rgn1, &temp_rgn1);
			union2(hRslt_rgn, temp_rgn1, &hRslt_rgn);
		}

		if (disp)
		{
			set_font(disp, "-Arial-12-*-*-*-*-*-1-");
			set_draw(disp, HTuple("margin"));
			memset(result0, NULL, 20);
			sprintf(result0, "%d", (int)rslt_cnt);

			set_color(disp, HTuple("yellow"));
			disp_obj(ins_rgn, disp);

			if (rslt_cnt > 10)
			{
				set_color(disp, HTuple("green"));
			}
			else
			{
				set_color(disp, HTuple("red"));
			}

			set_tposition(disp, rect.bottom + 5 + nRgnOffsetY, rect.left + 5);
			write_string(disp, HTuple("PM Cnt : ") + HTuple(result0));
			 
			if (rslt_cnt > 0)
			{
				disp_obj(temp_rgn1, disp);
			}
		}
	}

	if (bSave)
	{
		char cPath[512];
		size_t len1 = strlen("RsltImage.bmp");
		size_t len2 = strlen(m_cSavePath);
		sprintf_s(cPath, len1 + len2 + 1, "%sRsltImage.bmp", m_cSavePath);

		SaveImage(&Save_Image, cPath);
	}

	//m_VisionInsp_CS2.Lock();
	//copy_obj(hRslt_rgn, &m_InspRslt[m_nInsp_Idx].hRslt_rgn, 1, -1);
	union2(m_InspRslt[m_nInsp_Idx].hRslt_rgn, hRslt_rgn, &m_InspRslt[m_nInsp_Idx].hRslt_rgn);
	//m_VisionInsp_CS2.Unlock();
	set_check("give_error");

	return TRUE;
}

UINT CVisionModule::ImageSaveThread(LPVOID pParam)
{
	CVisionModule *pdlg = (CVisionModule *)pParam;

	pdlg->ThreadImageSave();

	return 0;
}

void CVisionModule::ThreadImageSave()
{
	while (TRUE)
	{
		if (m_bImageSaveThreadStart)
		{

		}

		if (m_bImageSaveThreadEnd)
		{
			break;
		}
		Sleep(1);
	}

}

UINT CVisionModule::ParallelInspThread(LPVOID pParam)
{
	CVisionModule *pdlg = (CVisionModule *)pParam;

	pdlg->ParallelInsp();

	return 0;
}

void CVisionModule::SetImageFilter(long disp, Hobject *image, Hobject *rslt_image, typeThreadInsp ThreadInspPara, BOOL isSave)
{
	long *Histo;
	long lcrop_top = 0, lcrop_left = 0;
	Hlong width, height, lwidth, lheight;
	Hobject hRsltImage;
	CRect rect;
	char typ[128];
	BYTE *bFilterRslt, *bRsltImage;
	Histo = new long[sizeof(long) * MAX_VAL];
	copy_image(*image, &hRsltImage);
	get_image_pointer1(hRsltImage, (Hlong*)&bFilterRslt, typ, &width, &height);
	get_image_pointer1(*rslt_image, (Hlong*)&bRsltImage, typ, &width, &height);

	lwidth = width;
	lheight = height;

	for (int i = 0; i < ThreadInspPara.nYRectCnt; i++)
	{
		Hobject hFilterImage, hIlluminate, hIlluminate1, hFilterImage1, hFilterImage2;
		Hobject hMeanImage, hEmphaImage;
		BYTE *bFilter_ptr, *bTempFilter_ptr;
		int nMaskWidth, nMaskHeight;

		rect.left = ThreadInspPara.InspRect[i].left;
		rect.right = ThreadInspPara.InspRect[i].right;
		rect.top = ThreadInspPara.InspRect[i].top;
		rect.bottom = ThreadInspPara.InspRect[i].bottom;

		crop_rectangle1(*image, &hFilterImage, (Hlong)rect.top, (Hlong)rect.left, (Hlong)rect.bottom, (Hlong)rect.right);

		width = 0;
		height = 0;
		get_image_pointer1(hFilterImage, (Hlong*)&bFilter_ptr, typ, &width, &height);

		memset(Histo, NULL, sizeof(long) * MAX_VAL);
		GetHistgram(bFilter_ptr, width, height, Histo);

		illuminate(hFilterImage, &hIlluminate, 20, 20, GetilluminateFactorCal(Histo));
		mean_image(hIlluminate, &hFilterImage1, 3, 3);

		ImageFilter(disp, &hFilterImage1, FILTER_TYPE_LAPLACIAN);
		ImageFilter(disp, &hFilterImage1, FILTER_TYPE_DIFFUSION);
		ImageFilter(disp, &hFilterImage1, FILTER_TYPE_GAMMA_CORRECTION);
		ImageFilter(disp, &hFilterImage1, FILTER_TYPE_CONTRAST_CORRECTION);
		ImageFilter(disp, &hFilterImage1, FILTER_TYPE_DIFFUSION);

		nMaskWidth = rect.Width();
		nMaskHeight = rect.Width();
		if (rect.Width() > rect.Height())
		{
			nMaskWidth = rect.Height();
			nMaskHeight = rect.Height();
		}

		get_image_pointer1(hFilterImage1, (Hlong*)&bTempFilter_ptr, typ, &width, &height);

		memset(Histo, NULL, sizeof(long) * MAX_VAL);
		GetHistgram(bTempFilter_ptr, width, height, Histo);

		illuminate(hFilterImage1, &hIlluminate1, nMaskWidth, nMaskHeight, 1);// GetilluminateFactorCal(Histo));
		mean_image(hIlluminate1, &hMeanImage, (nMaskWidth / 10), (nMaskHeight / 10));
		emphasize(hMeanImage, &hEmphaImage, (nMaskWidth / 10), (nMaskHeight / 10), 1);

		ClipImage(&hEmphaImage, &hRsltImage, rect);
		ClipImage(&hEmphaImage, rslt_image, rect);
	}

	if (TRUE)
	{
		char cPath[512];
		CString strTemp;
		strTemp.Format(_T("ImageFilter_%d.bmp"), m_nParallelInspCnt++);
		size_t len1 = strlen(m_cSavePath);
		sprintf_s(cPath, len1 + strTemp.GetLength() + 1, "%s%S", m_cSavePath, strTemp);

		SaveImage(&hRsltImage, cPath);
	}
	delete Histo;
	copy_image(hRsltImage, image);
}

void CVisionModule::WritePMInfoRslt()
{
	CStdioFile pFile;
	CTime NowTime = CTime::GetCurrentTime();
	CString strCurtime, strCurtimeday;
	strCurtime = NowTime.Format(_T("%Y")) + "\\" + NowTime.Format(_T("%m")) + "\\" + NowTime.Format(_T("%d"));// + "\\" + NowTime.Format("%d");// 일까지,
	strCurtimeday = NowTime.Format(_T("%d"));// 일까지,,
	CString strPath, tStr, tempStr, tempStr1, strRslt;
	//strPath.Format("D:\\Insp_Result\\%s\\%s",strCurtime,g_ImsData.GetLotNo());
	strPath.Format(_T("D:\\PMInsLog\\InspReport\\%s\\"), strCurtime);
	tStr.Format(_T("%s%s_InspReport.csv"), strPath, m_strRsltFileName);
	MakeDirectories(strPath);

	if (GetFileAttributes(strPath) != FILE_ATTRIBUTE_DIRECTORY)		// 폴더가 이미 존재하는 경우.
	{
		if (CreateDirectory(strPath, NULL) == 0) return;
	}

	CFileFind finder;
	BOOL bWorking = finder.FindFile(tStr);//파일을 찾는다..

	BOOL bOpen;
	if (!bWorking) //파일을 못 찾았으면 파일을 새로 생성
	{
		if (bOpen = pFile.Open(tStr, CFile::modeCreate | CFile::modeWrite))
		{
			tempStr.Format(_T("Lead Idx,PM Idx,CenterX,CenterY,LongLenght(um),ShortLenght(um),Phi\n"));
			pFile.WriteString(tempStr);
		}
	}
	else
		bOpen = pFile.Open(tStr, CFile::modeNoInherit | CFile::modeWrite);

	if (bOpen)
	{

		strRslt;
		tempStr.Empty();
		tempStr1.Empty();
		strRslt.Empty();
		for (int i = 0; i < m_PmCntRslt.size(); i++)
		{
			for (int j = 0; j < m_PmCntRslt[i].PMRslt.size(); j++)
			{
				tempStr.Format(_T("%d,%d,%d,%d,%d,%d,%.4f\n"), m_PmCntRslt[i].PMRslt[j].lLeadNum, m_PmCntRslt[i].PMRslt[j].lRsltIdx, m_PmCntRslt[i].PMRslt[j].pCenter.x, m_PmCntRslt[i].PMRslt[j].pCenter.y, m_PmCntRslt[i].PMRslt[j].lLongLenght, m_PmCntRslt[i].PMRslt[j].lShortLenght, m_PmCntRslt[i].PMRslt[j].dPhi);
				pFile.Seek(0L, CFile::end);
				pFile.WriteString(tempStr);
			}
		}

		pFile.Close();
	}
}

void CVisionModule::WriteRslt(CString strRslt)
{
	CStdioFile pFile;
	CString strResultData, tempStr, tempStr1;

	CTime NowTime = CTime::GetCurrentTime();
	CString strCurtime, strCurtimeday;
	strCurtime = NowTime.Format(_T("%Y")) + "\\" + NowTime.Format(_T("%m"));// + "\\" + NowTime.Format("%d");// 일까지,
	strCurtimeday = NowTime.Format(_T("%d"));// 일까지,,
	CString strPath, tStr;
	//strPath.Format("D:\\Insp_Result\\%s\\%s",strCurtime,g_ImsData.GetLotNo());
	strPath.Format(_T("D:\\PMInsLog\\InspReport\\%s\\"), strCurtime);
	tStr.Format(_T("%sInspReport_%s_day.csv"), strPath, strCurtimeday);

	MakeDirectories(strPath);//경로에따라 찾고 없으면 만든다...


	if (GetFileAttributes(strPath) != FILE_ATTRIBUTE_DIRECTORY)		// 폴더가 이미 존재하는 경우.
	{
		if (CreateDirectory(strPath, NULL) == 0) return;
	}

	CFileFind finder;
	BOOL bWorking = finder.FindFile(tStr);//파일을 찾는다..

	BOOL bOpen;
	if (!bWorking) //파일을 못 찾았으면 파일을 새로 생성
	{
		if (bOpen = pFile.Open(tStr, CFile::modeCreate | CFile::modeWrite))
		{
			tempStr.Format(_T("CurTime,Tack Time,S/L_Result,Distribution_Result,Shift_Result,Cnt_Result"));
			for (int i = 0; i < m_InspPara.InspRectRecipe[m_nInsp_Idx].nYRectCnt; i++)
			{
				tempStr1.Format(_T("%s,%d"), tempStr, i + 1);
				tempStr.Format(_T("%s"), tempStr1);
			}

			if (tempStr.Right(tempStr.GetLength()) == _T(","))
			{
				tempStr1.Format(_T("%s\n"), tempStr.Mid(0, tempStr.GetLength() - 1));
			}
			else
			{
				tempStr1.Format(_T("%s\n"), tempStr);
			}

			pFile.WriteString(tempStr1);
		}
	}
	else
		bOpen = pFile.Open(tStr, CFile::modeNoInherit | CFile::modeWrite);

	if (bOpen)
	{
		tempStr.Format(_T("%s\n"),/*GetDateString2(),GetTimeString(),*/strRslt);
		pFile.Seek(0L, CFile::end);
		pFile.WriteString(tempStr);
		pFile.Close();
	}

	if (FALSE)
	{
		char cSaveFile[512];
		strPath.Format(_T("D:\\PMInsLog\\InspImage\\%s\\"), strCurtime);
		MakeDirectories(strPath);//경로에따라 찾고 없으면 만든다...

		tStr.Format(_T("%sInspGrabImage_%s.bmp"), strPath, m_strRsltFileName);
		sprintf_s(cSaveFile, tStr.GetLength() + 1, "%S", tStr);
		SaveImage(&m_HalGrabImage, cSaveFile);

		tStr.Format(_T("%sInspFilterImage_%s.bmp"), strPath, m_strRsltFileName);
		sprintf_s(cSaveFile, tStr.GetLength() + 1, "%S", tStr);
		SaveImage(&m_HalDispFilterImage, cSaveFile);
	}
}
void CVisionModule::SetStartEndInspRectView(int nInspIdx)
{
	Hobject hRectangle1, hRectangle2;
	CRect rect;
	set_draw(m_Disp, HTuple("margin"));
	set_color(m_Disp, HTuple("blue"));

	rect.left = m_InspPara.InspRectRecipe[nInspIdx].StartRect.lRectLeft;
	rect.right = m_InspPara.InspRectRecipe[nInspIdx].StartRect.lRectRight;
	rect.top = m_InspPara.InspRectRecipe[nInspIdx].StartRect.lRectTop;
	rect.bottom = m_InspPara.InspRectRecipe[nInspIdx].StartRect.lRectBottom;
	gen_rectangle1(&hRectangle1, (Hlong)rect.top, (Hlong)rect.left, (Hlong)rect.bottom, (Hlong)rect.right);
	disp_obj(hRectangle1, m_Disp);

	rect.left = m_InspPara.InspRectRecipe[nInspIdx].EndRect.lRectLeft;
	rect.right = m_InspPara.InspRectRecipe[nInspIdx].EndRect.lRectRight;
	rect.top = m_InspPara.InspRectRecipe[nInspIdx].EndRect.lRectTop;
	rect.bottom = m_InspPara.InspRectRecipe[nInspIdx].EndRect.lRectBottom;
	gen_rectangle1(&hRectangle2, (Hlong)rect.top, (Hlong)rect.left, (Hlong)rect.bottom, (Hlong)rect.right);
	disp_obj(hRectangle2, m_Disp);
}


#ifdef THREAD_TEST
UINT CVisionModule::ParallelInspRsltSumThread(LPVOID pParam)
{
	CVisionModule *pdlg = (CVisionModule *)pParam;

	pdlg->ParallelInspRsltSum();

	return 0;
}

void CVisionModule::ParallelInspRsltSum()
{
	while (TRUE)
	{
		if (m_bParallelInspRsltSumThreadStart &&!m_bParallelInspRsltSumThreadEnd)
		{
			
		}

		if (!m_bParallelInspRsltSumThreadStart && m_bParallelInspRsltSumThreadEnd)
		{
			break;
		}
		Sleep(1);
	}
}

CInspection::CInspection()
{
	m_RsltRgn.Reset();
	m_RsltFilterImage.Reset();
	m_RsltImagePos.SetRect(0,0,100,100);
	m_bRsltCopy = FALSE;

	for (int i = 0; i < MAX_PARALLEL_INSP_CNT; i++)
	{
		m_bInspEnd[i] = FALSE;
		m_nRsltCnt[i] = 0;
	}

	memset(&m_ThreadInspPara, NULL, sizeof(ThreadInspPara));

	m_pThreadParallelInsp = NULL;
	m_bParallelUnderInspection;
	m_nParallelInspIdx;
}

CInspection::~CInspection(void)
{
	if(m_pThreadParallelInsp != NULL)
		DWORD dwRet = WaitForSingleObject(m_pThreadParallelInsp->m_hThread, 60000);

	if (m_pThreadParallelInsp)
	{
		delete m_pThreadParallelInsp;
		m_pThreadParallelInsp = NULL;
	}

	InitRslt();
}

void CInspection::SetParallelInsp(long disp, CRect PartSize, int nIdx, CRect rInspRect, Hobject *image, typeThreadInsp *ThreadInspPara)
{
	if (m_pThreadParallelInsp != NULL)
	{
		DWORD dwRet = WaitForSingleObject(m_pThreadParallelInsp->m_hThread, 60000);
		if (m_pThreadParallelInsp)
		{
			delete m_pThreadParallelInsp;
			m_pThreadParallelInsp = NULL;
			m_bParallelUnderInspection = FALSE;
		}
	}
	m_RsltRgn.Reset();
	m_RsltFilterImage.Reset();
	m_RsltImagePos.SetRect(0, 0, 100, 100);

	m_PmCntRslt.hInspRgn.Reset();
	m_PmCntRslt.hRsltRgn.Reset();
	memset(&m_ThreadInspPara, NULL, sizeof(ThreadInspPara));

	m_Disp = disp;
	m_PartSize = PartSize;

	m_bParallelUnderInspection = TRUE;
	m_nParallelInspIdx = nIdx;

	m_ThreadInspPara.bFilterImgView = ThreadInspPara->bFilterImgView;
	m_ThreadInspPara.bInspImgSave = ThreadInspPara->bInspImgSave;
	m_ThreadInspPara.ImageFilterPara = ThreadInspPara->ImageFilterPara;
	m_ThreadInspPara.InspImageSize = ThreadInspPara->InspImageSize;
	m_ThreadInspPara.nYRectCnt = ThreadInspPara->nYRectCnt;
	m_ThreadInspPara.dPixelSize = ThreadInspPara->dPixelSize;

	for (int i = 0; i < m_ThreadInspPara.nYRectCnt; i++)
	{
		m_ThreadInspPara.InspRect[i] = ThreadInspPara->InspRect[i];
		m_ThreadInspPara.nInspRectIdx[i] = ThreadInspPara->nInspRectIdx[i];
		for (int j = 0; j < 4; j++)
		{
			m_ThreadInspPara.ShiftInspRect[j][i] = ThreadInspPara->ShiftInspRect[j][i];
		}
	}
	
	m_ThreadInspPara.nInspYSpace = ThreadInspPara->nInspYSpace;
	m_ThreadInspPara.PMSearchPara = ThreadInspPara->PMSearchPara;
	m_ThreadInspPara.PMDistributionInspPara = ThreadInspPara->PMDistributionInspPara;
	m_ThreadInspPara.nStartLeadIdx = ThreadInspPara->nStartLeadIdx;
	m_ThreadInspPara.nEndLeadIdx = ThreadInspPara->nEndLeadIdx;

	InitRslt();

	if (copy_image(*image, &m_InspImage) != H_MSG_TRUE)
	{
		return;
	}
	if (copy_image(*image, &m_RsltFilterImage) != H_MSG_TRUE)
	{
		return;
	}

	m_RsltImagePos = m_ThreadInspPara.InspImageSize;

	m_pThreadParallelInsp = AfxBeginThread(ParallelInspThread, this, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
	m_pThreadParallelInsp->m_bAutoDelete = FALSE;
	m_pThreadParallelInsp->ResumeThread();
}

BOOL CInspection::WaitEndThread()
{
	BOOL rslt = FALSE;
	if (m_pThreadParallelInsp != NULL)
	{
		if (m_bParallelUnderInspection)
		{
			DWORD dwRet = WaitForSingleObject(m_pThreadParallelInsp->m_hThread, 60000);
			if (m_pThreadParallelInsp)
			{
				delete m_pThreadParallelInsp;
				m_pThreadParallelInsp = NULL;
				rslt = TRUE;
			}
		}
		else
		{
			delete m_pThreadParallelInsp;
			m_pThreadParallelInsp = NULL;
			rslt = TRUE;
		}
	}
	else
	{
		rslt = TRUE;
	}

	return rslt;
}

void CInspection::ParallelInsp()
{
	PMCntInspection(m_Disp, &m_InspImage, m_ThreadInspPara, m_ThreadInspPara.bFilterImgView, m_ThreadInspPara.bInspImgSave, FALSE);
	PMShiftInspection(m_Disp, &m_InspImage, m_ThreadInspPara);
	PMDistributionInspection(m_Disp);
	m_bParallelUnderInspection = FALSE;
}

UINT CInspection::ParallelInspThread(LPVOID pParam)
{
	CInspection *pdlg = (CInspection *)pParam;

	pdlg->ParallelInsp();

	return 0;
}

void CInspection::InitRslt()
{
	for (int i = 0; i < MAX_PARALLEL_INSP_CNT; i++)
	{
		m_bInspEnd[i] = FALSE;
		m_nRsltCnt[i] = 0;
		m_strRsltText[i].Empty();

		for(int j = 0; j < 4; j++)
			m_bShiftRslt[j][i] = TRUE;
	}

	if (m_PmCntRslt.PMRslt.size() > 0)
	{
		m_PmCntRslt.PMRslt.clear();
	}

	for (int i = 0; i < m_ThreadInspPara.nYRectCnt; i++)
	{
		for (int y = 0; y < m_ThreadInspPara.PMDistributionInspPara.nDistributionYCnt; y++)
		{
			for (int x = 0; x < m_ThreadInspPara.PMDistributionInspPara.nDistributionXCnt; x++)
			{
				m_PMDistributionRslt.nDistribution[i][x][y] = 0;
			}
		}
	}
}

BOOL CInspection::PMShiftInspection(long disp, Hobject*image, typeThreadInsp ThreadInspPara)
{
	BOOL rslt = TRUE;
	int nRgnOffsetY = ThreadInspPara.InspImageSize.top;
	int nInspIdx = m_nParallelInspIdx;
	CRect rect, image_rect;
	Hlong width, height;
	char typ[128];
	long *Histo;
	Histo = new long[MAX_VAL];
	CString strText;
	Hobject hRsltImage, ins_rgn, hRslt_rgn, hInspImage, hReduce_rect, rslt_rgn;
	typePMRslt PmRslt;
	int nRsltIdx = 0;
	image_rect = ThreadInspPara.InspImageSize;
	copy_image(*image, &hInspImage);
	copy_image(*image, &hRsltImage);

	set_check("~give_error");
	for (int i = 0; i < ThreadInspPara.nYRectCnt; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (ThreadInspPara.ShiftInspRect[j][i].isInsp)
			{
				Hobject hFilterImage, hIlluminate, hIlluminate1, hFilterImage1, hFilterImage2;
				Hobject hMeanImage, hEmphaImage;
				BYTE *bTempFilter;
				int nMaskWidth, nMaskHeight, nMaxNoiseSize;

				rect.left = ThreadInspPara.ShiftInspRect[j][i].lRectLeft;
				rect.right = ThreadInspPara.ShiftInspRect[j][i].lRectRight;
				rect.top = ThreadInspPara.ShiftInspRect[j][i].lRectTop - ThreadInspPara.InspImageSize.top;
				rect.bottom = ThreadInspPara.ShiftInspRect[j][i].lRectBottom - ThreadInspPara.InspImageSize.top;

				crop_rectangle1(hInspImage, &hFilterImage, (Hlong)rect.top, (Hlong)rect.left, (Hlong)rect.bottom, (Hlong)rect.right);

				if (illuminate(hFilterImage, &hIlluminate, 20, 20, 1) != H_MSG_TRUE)
				{
					continue;
				}

				if (mean_image(hIlluminate, &hFilterImage1, 2, 2) != H_MSG_TRUE)
				{
					continue;
				}

				nMaskWidth = rect.Width();
				nMaskHeight = rect.Width();
				if (rect.Width() > rect.Height())
				{
					nMaskWidth = rect.Height();
					nMaskHeight = rect.Height();
				}

				if (emphasize(hFilterImage1, &hEmphaImage, 3, 3, 1) != H_MSG_TRUE)
				{
					continue;
				}

				if (laplace_of_gauss(hEmphaImage, &hIlluminate1, 3) != H_MSG_TRUE)
				{
					continue;
				}
				if (scale_image_max(hIlluminate1, &hFilterImage2) != H_MSG_TRUE)
				{
					continue;
				}


				get_image_pointer1(hFilterImage2, (Hlong*)&bTempFilter, typ, &width, &height);

				memset(Histo, NULL, sizeof(long) * MAX_VAL);
				GetHistgram(bTempFilter, width, height, Histo);

				if(!ClipImage(&hFilterImage2, &hRsltImage, rect))
				{
					continue;
				}

				int nVal = GetHistoMax(Histo);

				Hobject hRectangle, hRectangle1, hRectangle2, hFilterRslt, ImageReduced, hCropImage, Region, filter_rslt_rgn_black, filter_rslt_rgn_white;
				HTuple Num;
				int nThresh = 0;

				gen_rectangle1(&hRectangle, (Hlong)rect.top, (Hlong)rect.left, (Hlong)rect.bottom, (Hlong)rect.right);
				copy_obj(hRectangle, &ins_rgn, 1, -1);

				nMaskWidth = rect.Width() / 10;
				nMaskHeight = rect.Width() / 10;
				nMaxNoiseSize = rect.Width();
				if (rect.Width() > rect.Height())
				{
					nMaskWidth = rect.Height() / 10;
					nMaskHeight = rect.Height() / 10;
					nMaxNoiseSize = rect.Height();
				}

				gen_rectangle1(&hRectangle1, (Hlong)(rect.top + 1), (Hlong)(rect.left + 1), (Hlong)(rect.bottom - 1), (Hlong)(rect.right - 1));
				reduce_domain(hRsltImage, hRectangle1, &ImageReduced);

				nThresh = ((long)nVal - ThreadInspPara.PMSearchPara.nThreshHold[0]);
				if (nThresh <= 0)
				{
					nThresh = 1;
				}
				threshold(ImageReduced, &Region, 0, nThresh);
				connection(Region, &filter_rslt_rgn_black);

				nThresh = ((long)nVal + ThreadInspPara.PMSearchPara.nThreshHold[1]);
				if (nThresh >= 255)
				{
					nThresh = 254;
				}
				threshold(ImageReduced, &Region, nThresh, 255);
				connection(Region, &filter_rslt_rgn_white);

				long black_cnt = 0, white_cnt = 0;
				count_obj(filter_rslt_rgn_black, &Num);
				black_cnt = (long)(Hlong)Num[0];
				count_obj(filter_rslt_rgn_white, &Num);
				white_cnt = (long)(Hlong)Num[0];

				/*erosion_circle(filter_rslt_rgn_black, &filter_rslt_rgn_black, HTuple(1));
				erosion_circle(filter_rslt_rgn_white, &filter_rslt_rgn_white, HTuple(1));*/

				if (ThreadInspPara.PMSearchPara.nMargeDistance > 0)
				{
					dilation_circle(filter_rslt_rgn_black, &filter_rslt_rgn_black, HTuple(ThreadInspPara.PMSearchPara.nMargeDistance));

					dilation_circle(filter_rslt_rgn_white, &filter_rslt_rgn_white, HTuple(ThreadInspPara.PMSearchPara.nMargeDistance));
				}

				union1(filter_rslt_rgn_black, &filter_rslt_rgn_black);
				union1(filter_rslt_rgn_white, &filter_rslt_rgn_white);

				connection(filter_rslt_rgn_black, &filter_rslt_rgn_black);
				connection(filter_rslt_rgn_white, &filter_rslt_rgn_white);

				if (ThreadInspPara.PMSearchPara.nMargeDistance > 0)
				{
					closing_circle(filter_rslt_rgn_black, &filter_rslt_rgn_black, HTuple(ThreadInspPara.PMSearchPara.nMargeDistance));

					closing_circle(filter_rslt_rgn_white, &filter_rslt_rgn_white, HTuple(ThreadInspPara.PMSearchPara.nMargeDistance));
				}

				select_shape(filter_rslt_rgn_black, &filter_rslt_rgn_black, HTuple("area"), HTuple("and"), HTuple(2), HTuple('max'));
				select_shape(filter_rslt_rgn_black, &filter_rslt_rgn_black, HTuple("width"), HTuple("and"), HTuple(2), HTuple('max'));
				select_shape(filter_rslt_rgn_black, &filter_rslt_rgn_black, HTuple("height"), HTuple("and"), HTuple(2), HTuple('max'));

				select_shape(filter_rslt_rgn_white, &filter_rslt_rgn_white, HTuple("area"), HTuple("and"), HTuple(2), HTuple('max'));
				select_shape(filter_rslt_rgn_white, &filter_rslt_rgn_white, HTuple("width"), HTuple("and"), HTuple(2), HTuple('max'));
				select_shape(filter_rslt_rgn_white, &filter_rslt_rgn_white, HTuple("height"), HTuple("and"), HTuple(2), HTuple('max'));

				black_cnt = 0, white_cnt = 0;
				count_obj(filter_rslt_rgn_black, &Num);
				black_cnt = (long)(Hlong)Num[0];
				count_obj(filter_rslt_rgn_white, &Num);
				white_cnt = (long)(Hlong)Num[0];

				/*union1(filter_rslt_rgn_black, &filter_rslt_rgn_black);
				union1(filter_rslt_rgn_white, &filter_rslt_rgn_white);
				set_color(disp, HTuple("yellow"));
				disp_obj(filter_rslt_rgn_black, disp);
				set_color(disp, HTuple("blue"));
				disp_obj(filter_rslt_rgn_white, disp);
				connection(filter_rslt_rgn_black, &filter_rslt_rgn_black);
				connection(filter_rslt_rgn_white, &filter_rslt_rgn_white);*/

				Hobject temp_rgn, sel_black_rgn, sel_white_rgn, inter_rgn, sub_tot_rgn, temp_rgn1;
				HTuple Area, row, col, row0, col0, row1, col1;
				CRect temp_rect, rslt_rect;
				BOOL bSum = FALSE;
				long rslt_cnt;
				long nRgn_Cnt = i;

				//black & white 붙어있는 것만 고르기
				////////////////////////////////////////////////////////////////////
				gen_empty_obj(&temp_rgn);
				rslt_cnt = 0;
				if (black_cnt > 0 && white_cnt > 0)
				{
					for (int i = 0; i < white_cnt; i++)
					{
						int nBlackY, nWhiteY1, nWhiteY2, nBlackX, nWhiteX1, nWhiteX2;
						gen_empty_obj(&sel_white_rgn);
						select_obj(filter_rslt_rgn_white, &sel_white_rgn, i + 1);
						dilation_circle(sel_white_rgn, &sel_white_rgn, 10);
						area_center(sel_white_rgn, &Area, &row, &col);
						smallest_rectangle1(sel_white_rgn, &row0, &col0, &row1, &col1);
						temp_rect = CRect((Hlong)col0[0], (Hlong)row0[0], (Hlong)col1[0], (Hlong)row1[0]);
						nWhiteY1 = temp_rect.top;
						nWhiteY2 = temp_rect.bottom;
						nWhiteX1 = temp_rect.left;
						nWhiteX2 = temp_rect.right;
						bSum = FALSE;

						if (rslt_rect.IntersectRect(rect, temp_rect) == 0)
						{
							continue;
						}

						for (int j = 0; j < black_cnt; j++)
						{
							gen_empty_obj(&sel_black_rgn);
							select_obj(filter_rslt_rgn_black, &sel_black_rgn, j + 1);
							dilation_circle(sel_black_rgn, &sel_black_rgn, 10);
							intersection(sel_white_rgn, sel_black_rgn, &inter_rgn);
							area_center(inter_rgn, &Area, &row, &col);

							smallest_rectangle1(sel_black_rgn, &row0, &col0, &row1, &col1);

							//if(ThreadInspPara.PMSearchPara.nPmType == INSPECTION_PM_TYPE_W_DOWN_B_UP)

							switch (ThreadInspPara.PMSearchPara.nPmType)
							{
							case INSPECTION_PM_TYPE_W_UP_B_DOWN:

								break;
							case INSPECTION_PM_TYPE_W_DOWN_B_UP:
								nBlackY = ((Hlong)row0[0] + (Hlong)row1[0]) / 2;
								nBlackX = ((Hlong)col0[0] + (Hlong)col1[0]) / 2;
								nWhiteY1 = temp_rect.CenterPoint().y;
								nWhiteY2 = temp_rect.bottom + 10;
								break;
							case INSPECTION_PM_TYPE_W_LEFT_B_RIGHT:

								break;
							case INSPECTION_PM_TYPE_W_RIGHT_B_LEFT:

								break;
							}


							if (1 < (Hlong)Area[0]
								&& (nWhiteY1 < nBlackY && nBlackY < nWhiteY2)
								&& (nWhiteX1 + 5 <= nBlackX && nBlackX <= nWhiteX2 - 5))
							{
								bSum = TRUE;
								gen_empty_obj(&sub_tot_rgn);
								union1(sel_black_rgn, &sel_black_rgn);
								union1(sel_white_rgn, &sel_white_rgn);
								union2(sel_black_rgn, sel_white_rgn, &sub_tot_rgn);

								erosion_circle(sub_tot_rgn, &sub_tot_rgn, 10);

								union1(temp_rgn, &temp_rgn);
								union2(temp_rgn, sub_tot_rgn, &sub_tot_rgn);
								union1(sub_tot_rgn, &temp_rgn);

								obj_diff(filter_rslt_rgn_black, sel_black_rgn, &filter_rslt_rgn_black);
								connection(filter_rslt_rgn_black, &filter_rslt_rgn_black);
								obj_diff(filter_rslt_rgn_white, sel_white_rgn, &filter_rslt_rgn_white);
								count_obj(filter_rslt_rgn_black, &Num);
								black_cnt = (long)(Hlong)Num[0];
								break;
							}
						}

						if (bSum)
						{
							union1(temp_rgn, &temp_rgn);
						}
					}
					////////////////////////////////////////////////////////////////////
					connection(temp_rgn, &temp_rgn);

					count_obj(temp_rgn, &Num);
					rslt_cnt = 0;
					rslt_cnt = (long)(Hlong)Num[0];

					Hobject sel_rslt_rgn, Ellipses;
					HTuple Ra, Rb, Phi, row_temp1;
					Hlong temp_row;
					long temp_Ra, temp_Rb;

					double dNosis = (double)rect.Height() / 1.5;

					if (rect.Height() > rect.Width())
					{
						dNosis = (double)rect.Width() / 1.5;
					}


					gen_empty_obj(&temp_rgn1);
					for (int j = 0; j < rslt_cnt; j++)
					{
						Hobject sel_rslt_rgn, Ellipses;
						gen_empty_obj(&Ellipses);
						gen_empty_obj(&sel_rslt_rgn);
						select_obj(temp_rgn, &sel_rslt_rgn, j + 1);
						area_center(sel_rslt_rgn, &Area, &row, &col);
						elliptic_axis(sel_rslt_rgn, &Ra, &Rb, &Phi);

						temp_Ra = (Hlong)Ra[0];
						temp_Rb = (Hlong)Rb[0];

						if ((temp_Ra < ThreadInspPara.PMSearchPara.nNoiseSize[1]) || (temp_Rb < ThreadInspPara.PMSearchPara.nNoiseSize[2]) ||
							temp_Ra > dNosis || temp_Rb > dNosis)
						{
							continue;
						}

						temp_row = (Hlong)(row[0]) + nRgnOffsetY;
						tuple_gen_const(1, temp_row, &row_temp1);
						gen_ellipse(&Ellipses, row_temp1, col, Phi, Ra, Rb);
						concat_obj(temp_rgn1, Ellipses, &temp_rgn1);
					}

					connection(temp_rgn1, &temp_rgn1);

					//작은 노이즈 없에기
					select_shape(temp_rgn1, &temp_rgn, HTuple("area"), HTuple("and"), HTuple(ThreadInspPara.PMSearchPara.nNoiseSize[0]), HTuple("max"));// HTuple((double)rect.Height() / 1.5));
					select_shape(temp_rgn, &temp_rgn, HTuple("width"), HTuple("and"), HTuple(ThreadInspPara.PMSearchPara.nNoiseSize[1]), HTuple("max"));// HTuple((double)rect.Height() / 1.5));
					select_shape(temp_rgn, &temp_rgn1, HTuple("height"), HTuple("and"), HTuple(ThreadInspPara.PMSearchPara.nNoiseSize[2]), HTuple("max"));// HTuple((double)rect.Height() / 1.5));

					count_obj(temp_rgn1, &Num);
					rslt_cnt = 0;
					rslt_cnt = (long)(Hlong)Num[0];

					nRsltIdx = 0;
					typePMCntRslt PmCntRslt;
					for (int j = 0; j < rslt_cnt; j++)
					{
						Hobject sel_rslt_rgn, Ellipses;
						gen_empty_obj(&Ellipses);
						gen_empty_obj(&sel_rslt_rgn);
						select_obj(temp_rgn1, &sel_rslt_rgn, j + 1);
						area_center(sel_rslt_rgn, &Area, &row, &col);
						elliptic_axis(sel_rslt_rgn, &Ra, &Rb, &Phi);

						PmCntRslt.lRsltAllCnt++;
						PmCntRslt.lRsltCnt[i]++;
						PmRslt.dPhi = (double)Phi[0];
						PmRslt.lLongLenght = (double)Ra[0] * ThreadInspPara.dPixelSize;
						PmRslt.lShortLenght = (double)Rb[0] * ThreadInspPara.dPixelSize;
						PmRslt.pCenter.SetPoint((Hlong)col[0], (Hlong)row[0]);
						PmRslt.lLeadNum = ThreadInspPara.nInspRectIdx[i];
						PmRslt.lRsltIdx = nRsltIdx++;
						PmCntRslt.PMRslt.push_back(PmRslt);

					}

					count_obj(temp_rgn1, &Num);
					rslt_cnt = 0;
					rslt_cnt = (long)(Hlong)Num[0];
					m_bShiftRslt[j][i] = TRUE;
					if (rslt_cnt > 0)
					{
						m_bShiftRslt[j][i] = FALSE;
						//gen_rectangle1(&hRectangle2, (Hlong)rect.top, (Hlong)rect.left, (Hlong)rect.bottom, (Hlong)rect.right);
						//union2(hRectangle2, rslt_rgn, &rslt_rgn);
						union2(temp_rgn1, rslt_rgn, &rslt_rgn);
					}
				}
			}
		}
	}

	union1(rslt_rgn, &rslt_rgn);
	copy_obj(rslt_rgn, &m_ShiftRsltRgn, 1, -1);
	set_check("give_error");
	return rslt;
}

BOOL CInspection::PMCntInspection(long disp, Hobject *image, typeThreadInsp ThreadInspPara, BOOL bFilterDisp, BOOL bSave, BOOL bInspImageDisp)
{
	BOOL rslt = TRUE;
	int nRgnOffsetY = ThreadInspPara.InspImageSize.top;
	int nInspIdx = m_nParallelInspIdx;
	CRect rect, image_rect;
	Hlong width, height;
	char typ[128];
	long *Histo;
	CString strText;
	char cText[512];
	Hobject hRsltImage, ins_rgn, hRslt_rgn, hInspImage, hReduce_rect;
	typePMRslt PmRslt;
	int nRsltIdx = 0;

	image_rect = ThreadInspPara.InspImageSize;

	Hlong x0, y0, x1, y1;
	set_check("~give_error");
	if (get_part(disp, &y0, &x0, &y1, &x1) != H_MSG_TRUE)
	{
		set_check("give_error");
		return FALSE;
	}
	Histo = new long[MAX_VAL];

	set_tposition(disp, y0 + (m_PartSize.Height() / 2) - 10, x0 + (m_PartSize.Width() / 8));
	set_font(disp, "-Arial-40-*-*-*-*-*-1-");


	set_draw(disp, HTuple("margin"));
	set_color(disp, HTuple("green"));

	strText.Format(_T("Inspecting"));
	for (int i = 0; i < m_nParallelInspIdx; i++)
	{
		strText += _T(".");
	}

	sprintf_s(cText, strText.GetLength() + 1, "%S", strText);

	write_string(disp, HTuple(cText));

	copy_image(*image, &hInspImage);
	copy_image(hInspImage, &hRsltImage);

	for (int i = 0; i < ThreadInspPara.nYRectCnt; i++)
	{
		m_PmCntRslt.lRsltCnt[i] = 0;
		Hobject hFilterImage, hIlluminate, hIlluminate1, hFilterImage1, hFilterImage2;
		Hobject hMeanImage, hEmphaImage;
		BYTE *bTempFilter;
		int nMaskWidth, nMaskHeight, nMaxNoiseSize;

		rect.left = ThreadInspPara.InspRect[i].left;
		rect.right = ThreadInspPara.InspRect[i].right;
		rect.top = ThreadInspPara.InspRect[i].top;
		rect.bottom = ThreadInspPara.InspRect[i].bottom;

		if (rect.Width() < 10 || rect.Height() < 10)
		{
			continue;
		}

		if (crop_rectangle1(hInspImage, &hFilterImage, (Hlong)rect.top, (Hlong)rect.left, (Hlong)rect.bottom, (Hlong)rect.right) != H_MSG_TRUE)
		{
			continue;
		}

		if (illuminate(hFilterImage, &hIlluminate, ThreadInspPara.ImageFilterPara.nIllumiSize, ThreadInspPara.ImageFilterPara.nIllumiSize, 1) != H_MSG_TRUE)
		{
			continue;
		}

		if (mean_image(hIlluminate, &hFilterImage1, ThreadInspPara.ImageFilterPara.nMeanSize, ThreadInspPara.ImageFilterPara.nMeanSize) != H_MSG_TRUE)
		{
			continue;
		}

		nMaskWidth = rect.Width();
		nMaskHeight = rect.Width();
		if (rect.Width() > rect.Height())
		{
			nMaskWidth = rect.Height();
			nMaskHeight = rect.Height();
		}

		if (emphasize(hFilterImage1, &hEmphaImage, (nMaskWidth / 10), (nMaskHeight / 10), 1) != H_MSG_TRUE)
		{
			continue;
		}

		if (laplace_of_gauss(hEmphaImage, &hIlluminate1, 2.5) != H_MSG_TRUE)
		{
			continue;
		}
		if (scale_image_max(hIlluminate1, &hFilterImage2) != H_MSG_TRUE)
		{
			continue;
		}


		get_image_pointer1(hFilterImage2, (Hlong*)&bTempFilter, typ, &width, &height);

		memset(Histo, NULL, sizeof(long) * MAX_VAL);
		GetHistgram(bTempFilter, width, height, Histo);

		int nVal = GetHistoMax(Histo);

		if (!ClipImage(&hFilterImage2, &hRsltImage, rect))
		{
			continue;
		}

		Hobject hRectangle, hRectangle1, hFilterRslt, ImageReduced, hCropImage, Region, filter_rslt_rgn_black, filter_rslt_rgn_white;
		HTuple Num;
		int nThresh = 0;

		gen_rectangle1(&hRectangle, (Hlong)rect.top, (Hlong)rect.left, (Hlong)rect.bottom, (Hlong)rect.right);
		copy_obj(hRectangle, &ins_rgn, 1, -1);

		nMaskWidth = rect.Width() / 10;
		nMaskHeight = rect.Width() / 10;
		nMaxNoiseSize = rect.Width();
		if (rect.Width() > rect.Height())
		{
			nMaskWidth = rect.Height() / 10;
			nMaskHeight = rect.Height() / 10;
			nMaxNoiseSize = rect.Height();
		}

		gen_rectangle1(&hRectangle1, (Hlong)(rect.top + 1), (Hlong)(rect.left + 1), (Hlong)(rect.bottom - 1), (Hlong)(rect.right - 1));
		reduce_domain(hRsltImage, hRectangle1, &ImageReduced);

		nThresh = ((long)nVal - ThreadInspPara.PMSearchPara.nThreshHold[0]);
		threshold(ImageReduced, &Region, 0, nThresh);
		connection(Region, &filter_rslt_rgn_black);

		nThresh = ((long)nVal + ThreadInspPara.PMSearchPara.nThreshHold[1]);
		if (nThresh >= 255)
		{
			nThresh = 254;
		}
		threshold(ImageReduced, &Region, nThresh, 255);
		connection(Region, &filter_rslt_rgn_white);

		long black_cnt = 0, white_cnt = 0;
		count_obj(filter_rslt_rgn_black, &Num);
		black_cnt = (long)(Hlong)Num[0];
		count_obj(filter_rslt_rgn_white, &Num);
		white_cnt = (long)(Hlong)Num[0];

		/*erosion_circle(filter_rslt_rgn_black, &filter_rslt_rgn_black, HTuple(1));
		erosion_circle(filter_rslt_rgn_white, &filter_rslt_rgn_white, HTuple(1));*/

		if (ThreadInspPara.PMSearchPara.nMargeDistance > 0)
		{
			dilation_circle(filter_rslt_rgn_black, &filter_rslt_rgn_black, HTuple(ThreadInspPara.PMSearchPara.nMargeDistance));

			dilation_circle(filter_rslt_rgn_white, &filter_rslt_rgn_white, HTuple(ThreadInspPara.PMSearchPara.nMargeDistance));
		}

		union1(filter_rslt_rgn_black, &filter_rslt_rgn_black);
		union1(filter_rslt_rgn_white, &filter_rslt_rgn_white);

		connection(filter_rslt_rgn_black, &filter_rslt_rgn_black);
		connection(filter_rslt_rgn_white, &filter_rslt_rgn_white);

		if (ThreadInspPara.PMSearchPara.nMargeDistance > 0)
		{
			closing_circle(filter_rslt_rgn_black, &filter_rslt_rgn_black, HTuple(ThreadInspPara.PMSearchPara.nMargeDistance));

			closing_circle(filter_rslt_rgn_white, &filter_rslt_rgn_white, HTuple(ThreadInspPara.PMSearchPara.nMargeDistance));
		}

		select_shape(filter_rslt_rgn_black, &filter_rslt_rgn_black, HTuple("area"), HTuple("and"), HTuple(2), HTuple('max'));
		select_shape(filter_rslt_rgn_black, &filter_rslt_rgn_black, HTuple("width"), HTuple("and"), HTuple(2), HTuple('max'));
		select_shape(filter_rslt_rgn_black, &filter_rslt_rgn_black, HTuple("height"), HTuple("and"), HTuple(2), HTuple('max'));

		select_shape(filter_rslt_rgn_white, &filter_rslt_rgn_white, HTuple("area"), HTuple("and"), HTuple(2), HTuple('max'));
		select_shape(filter_rslt_rgn_white, &filter_rslt_rgn_white, HTuple("width"), HTuple("and"), HTuple(2), HTuple('max'));
		select_shape(filter_rslt_rgn_white, &filter_rslt_rgn_white, HTuple("height"), HTuple("and"), HTuple(2), HTuple('max'));

		black_cnt = 0, white_cnt = 0;
		count_obj(filter_rslt_rgn_black, &Num);
		black_cnt = (long)(Hlong)Num[0];
		count_obj(filter_rslt_rgn_white, &Num);
		white_cnt = (long)(Hlong)Num[0];

		/*union1(filter_rslt_rgn_black, &filter_rslt_rgn_black);
		union1(filter_rslt_rgn_white, &filter_rslt_rgn_white);
		set_color(disp, HTuple("yellow"));
		disp_obj(filter_rslt_rgn_black, disp);
		set_color(disp, HTuple("blue"));
		disp_obj(filter_rslt_rgn_white, disp);
		connection(filter_rslt_rgn_black, &filter_rslt_rgn_black);
		connection(filter_rslt_rgn_white, &filter_rslt_rgn_white);*/

		Hobject temp_rgn, sel_black_rgn, sel_white_rgn, inter_rgn, sub_tot_rgn, temp_rgn1;
		HTuple Area, row, col, row0, col0, row1, col1;
		CRect temp_rect, rslt_rect;
		BOOL bSum = FALSE;
		long rslt_cnt;
		long nRgn_Cnt = i;

		//black & white 붙어있는 것만 고르기
		////////////////////////////////////////////////////////////////////
		gen_empty_obj(&temp_rgn);
		rslt_cnt = 0;
		if (black_cnt > 0 && white_cnt > 0)
		{
			for (int i = 0; i < white_cnt; i++)
			{
				int nBlackY, nWhiteY1, nWhiteY2, nBlackX, nWhiteX1, nWhiteX2;
				gen_empty_obj(&sel_white_rgn);
				select_obj(filter_rslt_rgn_white, &sel_white_rgn, i + 1);
				dilation_circle(sel_white_rgn, &sel_white_rgn, 10);
				area_center(sel_white_rgn, &Area, &row, &col);
				smallest_rectangle1(sel_white_rgn, &row0, &col0, &row1, &col1);
				temp_rect = CRect((Hlong)col0[0], (Hlong)row0[0], (Hlong)col1[0], (Hlong)row1[0]);
				nWhiteY1 = temp_rect.top;
				nWhiteY2 = temp_rect.bottom;
				nWhiteX1 = temp_rect.left;
				nWhiteX2 = temp_rect.right;
				bSum = FALSE;

				if (rslt_rect.IntersectRect(rect, temp_rect) == 0)
				{
					continue;
				}

				for (int j = 0; j < black_cnt; j++)
				{
					gen_empty_obj(&sel_black_rgn);
					select_obj(filter_rslt_rgn_black, &sel_black_rgn, j + 1);
					dilation_circle(sel_black_rgn, &sel_black_rgn, 10);
					intersection(sel_white_rgn, sel_black_rgn, &inter_rgn);
					area_center(inter_rgn, &Area, &row, &col);

					smallest_rectangle1(sel_black_rgn, &row0, &col0, &row1, &col1);

					//if(ThreadInspPara.PMSearchPara.nPmType == INSPECTION_PM_TYPE_W_DOWN_B_UP)

					switch (ThreadInspPara.PMSearchPara.nPmType)
					{
					case INSPECTION_PM_TYPE_W_UP_B_DOWN:

						break;
					case INSPECTION_PM_TYPE_W_DOWN_B_UP:
						nBlackY = ((Hlong)row0[0] + (Hlong)row1[0]) / 2;
						nBlackX = ((Hlong)col0[0] + (Hlong)col1[0]) / 2;
						nWhiteY1 = temp_rect.CenterPoint().y;
						nWhiteY2 = temp_rect.bottom + 10;
						break;
					case INSPECTION_PM_TYPE_W_LEFT_B_RIGHT:

						break;
					case INSPECTION_PM_TYPE_W_RIGHT_B_LEFT:

						break;
					}


					if (1 < (Hlong)Area[0]
						&& (nWhiteY1 < nBlackY && nBlackY < nWhiteY2)
						&& (nWhiteX1 + 5 <= nBlackX && nBlackX <= nWhiteX2 - 5))
					{
						bSum = TRUE;
						gen_empty_obj(&sub_tot_rgn);
						union1(sel_black_rgn, &sel_black_rgn);
						union1(sel_white_rgn, &sel_white_rgn);
						union2(sel_black_rgn, sel_white_rgn, &sub_tot_rgn);

						erosion_circle(sub_tot_rgn, &sub_tot_rgn, 10);

						union1(temp_rgn, &temp_rgn);
						union2(temp_rgn, sub_tot_rgn, &sub_tot_rgn);
						union1(sub_tot_rgn, &temp_rgn);

						obj_diff(filter_rslt_rgn_black, sel_black_rgn, &filter_rslt_rgn_black);
						connection(filter_rslt_rgn_black, &filter_rslt_rgn_black);
						obj_diff(filter_rslt_rgn_white, sel_white_rgn, &filter_rslt_rgn_white);
						count_obj(filter_rslt_rgn_black, &Num);
						black_cnt = (long)(Hlong)Num[0];
						break;
					}
				}

				if (bSum)
				{
					union1(temp_rgn, &temp_rgn);
				}
			}
			////////////////////////////////////////////////////////////////////
			connection(temp_rgn, &temp_rgn);

			count_obj(temp_rgn, &Num);
			rslt_cnt = 0;
			rslt_cnt = (long)(Hlong)Num[0];

			Hobject sel_rslt_rgn, Ellipses;
			HTuple Ra, Rb, Phi, row_temp1;
			Hlong temp_row;
			long temp_Ra, temp_Rb;

			double dNosis = (double)rect.Height() / 2;

			if (rect.Height() > rect.Width())
			{
				dNosis = (double)rect.Width() / 2;
			}

			gen_empty_obj(&temp_rgn1);
			for (int j = 0; j < rslt_cnt; j++)
			{
				Hobject sel_rslt_rgn, Ellipses;
				gen_empty_obj(&Ellipses);
				gen_empty_obj(&sel_rslt_rgn);
				select_obj(temp_rgn, &sel_rslt_rgn, j + 1);
				area_center(sel_rslt_rgn, &Area, &row, &col);
				elliptic_axis(sel_rslt_rgn, &Ra, &Rb, &Phi);

				temp_Ra = (Hlong)Ra[0];
				temp_Rb = (Hlong)Rb[0];

				if ((temp_Ra < ThreadInspPara.PMSearchPara.nNoiseSize[1]) || (temp_Rb < ThreadInspPara.PMSearchPara.nNoiseSize[2]) ||
					temp_Ra > dNosis || temp_Rb > dNosis)
				{
					continue;
				}

				temp_row = (Hlong)(row[0]) + nRgnOffsetY;
				tuple_gen_const(1, temp_row, &row_temp1);
				gen_ellipse(&Ellipses, row_temp1, col, Phi, Ra, Rb);
				concat_obj(temp_rgn1, Ellipses, &temp_rgn1);
			}

			connection(temp_rgn1, &temp_rgn1);

			//작은 노이즈 없에기
			select_shape(temp_rgn, &temp_rgn, HTuple("area"), HTuple("and"), HTuple(ThreadInspPara.PMSearchPara.nNoiseSize[0]), HTuple("max"));
			select_shape(temp_rgn, &temp_rgn, HTuple("width"), HTuple("and"), HTuple(ThreadInspPara.PMSearchPara.nNoiseSize[1]), HTuple("max"));
			select_shape(temp_rgn, &temp_rgn, HTuple("height"), HTuple("and"), HTuple(ThreadInspPara.PMSearchPara.nNoiseSize[2]), HTuple("max"));

			count_obj(temp_rgn1, &Num);
			rslt_cnt = 0;
			rslt_cnt = (long)(Hlong)Num[0];

			nRsltIdx = 0;
			for (int j = 0; j < rslt_cnt; j++)
			{
				Hobject sel_rslt_rgn, Ellipses;
				gen_empty_obj(&Ellipses);
				gen_empty_obj(&sel_rslt_rgn);
				select_obj(temp_rgn1, &sel_rslt_rgn, j + 1);
				area_center(sel_rslt_rgn, &Area, &row, &col);
				elliptic_axis(sel_rslt_rgn, &Ra, &Rb, &Phi);

				m_PmCntRslt.lRsltAllCnt++;
				m_PmCntRslt.lRsltCnt[i]++;
				PmRslt.dPhi = (double)Phi[0];
				PmRslt.lLongLenght = (double)Ra[0] * ThreadInspPara.dPixelSize;
				PmRslt.lShortLenght = (double)Rb[0] * ThreadInspPara.dPixelSize;
				PmRslt.pCenter.SetPoint((Hlong)col[0], (Hlong)row[0]);
				PmRslt.lLeadNum = ThreadInspPara.nInspRectIdx[i];
				PmRslt.lRsltIdx = nRsltIdx++;
				m_PmCntRslt.PMRslt.push_back(PmRslt);

			}

			count_obj(temp_rgn1, &Num);
			rslt_cnt = 0;
			rslt_cnt = (long)(Hlong)Num[0];

			union1(temp_rgn1, &temp_rgn1);
			union2(hRslt_rgn, temp_rgn1, &hRslt_rgn);
		}

		if (rslt_cnt >= 10)
		{
			m_PmCntRslt.bRslt[i] = TRUE;
		}
		else
		{
			m_PmCntRslt.bRslt[i] = FALSE;
		}

		m_strRsltText[i].Format(_T("PM Cnt : %d"), rslt_cnt);
		m_nInspEnd[i] = TRUE;
	}

	delete Histo;

	copy_image(hRsltImage, &m_RsltFilterImage);
	copy_obj(hRslt_rgn, &m_RsltRgn, 1, -1);
	copy_obj(hRslt_rgn, &m_PmCntRslt.hRsltRgn, 1, -1);
	set_check("give_error");
	return rslt;
}

void CInspection::GetHistgram(BYTE* by_source, long width, long height, long* lHisto)
{
	for (int h = 0; h < height; h++)
	{
		for (int w = 0; w < width; w++)
		{
			lHisto[by_source[(h * width) + w]]++;
		}
	}
}

int CInspection::GetHistoMax(long* lHisto)
{
	int nRslt = 0;
	int nMax = 0, nTemp = 0;
	nMax = lHisto[0];
	for (int i = 0; i < MAX_VAL - 1; i++)
	{
		nTemp = max(nMax, lHisto[i + 1]);

		if (nMax != nTemp)
		{
			nRslt = i;
		}

		nMax = nTemp;
	}

	return nRslt;
}

BOOL CInspection::ClipImage(Hobject *src_image, Hobject *rslt_image, CRect rect)
{
	BYTE *by_rslt;

	long lcrop_top = 0, lcrop_left = 0;
	Hobject temp_image, hRectangle, hReduceImage;
	Hlong width, height, lwidth, lheight;
	char typ[128];
	width = rect.Width();
	height = rect.Height();
	
	if (get_image_pointer1(*rslt_image, (Hlong*)&by_rslt, typ, &lwidth, &lheight) != H_MSG_TRUE)
	{
		return FALSE;
	}

	if (tile_images_offset(*src_image, &temp_image, rect.top, rect.left, 0, 0, rect.Height(), rect.Width(), lwidth, lheight) != H_MSG_TRUE)
	{
		return FALSE;
	}

	if (gen_rectangle1(&hRectangle, rect.top, rect.left, rect.bottom, rect.right) != H_MSG_TRUE)
	{
		return FALSE;
	}

	if (reduce_domain(temp_image, hRectangle, &hReduceImage) != H_MSG_TRUE)
	{
		return FALSE;
	}

	if (paint_gray(hReduceImage, *rslt_image, rslt_image) != H_MSG_TRUE)
	{
		return FALSE;
	}

	/*
	BYTE *by_src;
	set_check("~give_error");
	if (get_image_pointer1(*src_image, (Hlong*)&by_src, typ, &width, &height) != H_MSG_TRUE)
	{
		set_check("give_error");
		return FALSE;
	}
	if (get_image_pointer1(*rslt_image, (Hlong*)&by_rslt, typ, &lwidth, &lheight) != H_MSG_TRUE)
	{
		set_check("give_error");
		return FALSE;
	}
	
	lcrop_top = rect.top;
	for (int h = 0; h < height; h++)
	{
		lcrop_top = rect.top + h;

		lcrop_left = rect.left;
		for (int w = 0; w < width; w++)
		{
			by_rslt[(lcrop_top * lwidth) + lcrop_left] = by_src[(h * width) + w];
			lcrop_left++;
		}
	}

	if (gen_image1(rslt_image, "byte", lwidth, lheight, (Hlong)by_rslt) != H_MSG_TRUE)
	{
		set_check("give_error");
		return FALSE;
	}
	set_check("give_error");*/
	
	return TRUE;
}

BOOL CInspection::PMDistributionInspection(long disp)
{
	BOOL rslt = TRUE;
	int nXcnt = m_ThreadInspPara.PMDistributionInspPara.nDistributionXCnt, nYcnt = m_ThreadInspPara.PMDistributionInspPara.nDistributionYCnt;
	int nInspYcnt = m_ThreadInspPara.nYRectCnt;
	int nInspWidth, nInspHeight;
	typePMCntRslt PmRslt = m_PmCntRslt;
	Hobject Ellipses, hRectangle, inter_rgn, hSelRect, hInspRgn, hRectangle1;
	HTuple Ra, Rb, Phi, col, row, Area, row0, col0, row1, col1;
	long ltemp;
	double dtemp;
	long nRgnOffsetY = m_ThreadInspPara.InspImageSize.top;

	for (int i = 0; i < nInspYcnt; i++)
	{
		CRect rect;
		rect = m_ThreadInspPara.InspRect[i];
		nInspWidth = rect.Width() / nXcnt;
		nInspHeight = rect.Height() / nYcnt;

		for (int y = 0; y < nYcnt; y++)
		{
			for (int x = 0; x < nXcnt; x++)
			{
				CRect inspRect, selRsltRect;
				int nStartX, nStartY;
				nStartX = x * nInspWidth + rect.left;
				nStartY = y * nInspHeight + rect.top;
				inspRect.SetRect(nStartX, nStartY, nStartX + nInspWidth, nStartY + nInspHeight);
				gen_rectangle1(&hRectangle, inspRect.top, inspRect.left, inspRect.bottom, inspRect.right);
				for (int n = 0; n < PmRslt.lRsltAllCnt; n++)
				{
					ltemp = PmRslt.PMRslt[n].pCenter.y - nRgnOffsetY;
					tuple_gen_const(1, ltemp, &row);
					ltemp = PmRslt.PMRslt[n].pCenter.x;
					tuple_gen_const(1, ltemp, &col);
					dtemp = PmRslt.PMRslt[n].dPhi;
					tuple_gen_const(1, dtemp, &Phi);
					ltemp = PmRslt.PMRslt[n].lShortLenght;
					tuple_gen_const(1, ltemp, &Ra);
					ltemp = PmRslt.PMRslt[n].lLongLenght;
					tuple_gen_const(1, ltemp, &Rb);
					gen_ellipse(&Ellipses, row, col, Phi, Ra, Rb);
					smallest_rectangle1(Ellipses, &row0, &col0, &row1, &col1);
					gen_rectangle1(&hSelRect, row0, col0, row1, col1);
					
					intersection(hRectangle, hSelRect, &inter_rgn);
					area_center(inter_rgn, &Area, &row, &col);

					if (1 < (Hlong)Area[0])
					{
						m_PMDistributionRslt.nDistribution[i][x][y]++;
					}
				}
			}
		}
	}

	union1(hInspRgn, &hInspRgn);
	return rslt;
}
#endif

BOOL CVisionModule::ConvImgToHalBuf(Hobject *pHalBuf, P_MILCTRL_IMGINFO pImg)
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