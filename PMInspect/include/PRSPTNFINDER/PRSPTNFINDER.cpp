#include "stdafx.h"
#include "PRSPTNFINDER.h"

#define	MAX_VAL	255	//	or 256
#define	BYTELIMITE(x)	( x < 0 ) ? (BYTE)0 : ((x > 255) ? (BYTE)255 : (BYTE)x)

PRSPTNFINDER::PRSPTNFINDER()
{
	Release();
}


PRSPTNFINDER::~PRSPTNFINDER()
{
	Release();
}

BOOL	PRSPTNFINDER::Create()
{
	return	TRUE;
}

VOID	PRSPTNFINDER::Release()
{
	m_qSearchArea.clear();
}

BOOL	PRSPTNFINDER::PushSearchArea(RECT *pRect)
{
	if (pRect == NULL)
		return	FALSE;

	m_qSearchArea.push_back(*pRect);

	return	TRUE;
}

BOOL	PRSPTNFINDER::SetSearchArea(int Idx, RECT *pRect)
{
	if (pRect == NULL)
		return	FALSE;

	if (Idx >= m_qSearchArea.size())
		return	FALSE;

	memcpy(&m_qSearchArea[Idx], pRect, sizeof(RECT));

	return	TRUE;
}

LONG	PRSPTNFINDER::GetAreaCount()
{
	return	(LONG)m_qSearchArea.size();
}

BOOL	PRSPTNFINDER::GetSearchArea(int Idx, RECT *pRect)
{
	if (pRect == NULL)
		return	FALSE;

	if (Idx >= m_qSearchArea.size())
		return	FALSE;

	memcpy(pRect, &m_qSearchArea[Idx], sizeof(RECT));

	return	TRUE;
}

BOOL	PRSPTNFINDER::DeleteSearchArea(int Idx)
{
	if (Idx >= m_qSearchArea.size())
		return	FALSE;

	m_qSearchArea.erase(m_qSearchArea.begin() + Idx);

	return	TRUE;
}

VOID	PRSPTNFINDER::DeleteAllSearchArea()
{
	m_qSearchArea.clear();
}

BOOL	PRSPTNFINDER::FindPressPattern(VOID* pData, LONG Width, LONG Height, LONG Bpp, P_PRSPTNFINDER_RESULT pResult)
{
	if (pData == NULL)
		return	FALSE;

	Hobject	hImage;
	if (Bpp <= 8)
	{
		gen_image_const(&hImage, "byte", Width, Height);
		gen_image1_extern(&hImage, "byte", Width, Height, (Hlong)pData, (Hlong)0);
	}
	else if (Bpp <= 16)
	{
		gen_image_const(&hImage, "uint2", Width, Height);
		gen_image1_extern(&hImage, "uint2", Width, Height, (Hlong)pData, (Hlong)0);
	}
	else
	{
		return	FALSE;
	}

	return	FindPressPattern(&hImage, Width, Height, Bpp, pResult);
}

BOOL	PRSPTNFINDER::FindPressPattern(Hobject* pData, LONG Width, LONG Height, LONG Bpp, P_PRSPTNFINDER_RESULT pResult)
{
	if (pData == NULL || pResult == NULL)
		return	FALSE;

	deque<DOUBLE> qPattern;
	pResult->m_qPatternSize.push_back(qPattern);

	if (!ProcessFindPattern(pData, pResult->m_qPatternSize[0]))
	{
		pResult->m_qPatternSize[0].clear();
		return	FALSE;
	}

	return	TRUE;
}

BOOL	PRSPTNFINDER::ProcessFindPattern(Hobject* pImage, deque<DOUBLE> &qPattern)
{
	//	자체 필터링
	Hobject hFilterImage;
	BYTE *bFilter_ptr;
	Hlong width, height;
	char typ[128];

	get_image_pointer1(*pImage, (Hlong*)&bFilter_ptr, typ, &width, &height);
	ImageFilter(pImage, bFilter_ptr);
/*
	//	임계값으로 분리
	threshold(ImageReduced, &Region, 0, 80);
	connection(Region, &filter_rslt_rgn_black);

	threshold(ImageReduced, &Region, 140, 255);
	connection(Region, &filter_rslt_rgn_white);


	if (insPara.nMargeDistance[0] > 0)
	{
		dilation_circle(filter_rslt_rgn_black, &filter_rslt_rgn_black, HTuple(insPara.nMargeDistance[0]));
		closing_circle(filter_rslt_rgn_black, &filter_rslt_rgn_black, HTuple(insPara.nMargeDistance[0]));

		dilation_circle(filter_rslt_rgn_white, &filter_rslt_rgn_white, HTuple(insPara.nMargeDistance[0]));
		closing_circle(filter_rslt_rgn_white, &filter_rslt_rgn_white, HTuple(insPara.nMargeDistance[0]));
	}

	//	이어 붙이기
	union2(filter_rslt_rgn, filter_rslt_rgn_black, &filter_rslt_rgn);
	union2(filter_rslt_rgn, filter_rslt_rgn_white, rslt_rgn);

	connection(*rslt_rgn, rslt_rgn);

	//	오브젝트 선택
	select_shape(*rslt_rgn, rslt_rgn, HTuple("area"), HTuple("and"), HTuple(insPara.nNoiseSize[0]), HTuple("max"));
	select_shape(*rslt_rgn, rslt_rgn, HTuple("width"), HTuple("and"), HTuple(insPara.nNoiseSize[1]), HTuple("max"));
	select_shape(*rslt_rgn, rslt_rgn, HTuple("height"), HTuple("and"), HTuple(insPara.nNoiseSize[2]), HTuple("max"));

	//	선택된 오브젝트 수량 가져오기
	count_obj(*rslt_rgn, &Num);
	*rslt_num = (long)(Hlong)Num[0];
*/
	return	TRUE;
}

void PRSPTNFINDER::ImageFilter(Hobject *image, BYTE *rslt_image)
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
	BOOL bSave = FALSE;
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
/*
	nThresh[1] = m_InspPara.ImageFilterPara[0].nContrastVal;
	nThresh[2] = m_InspPara.ImageFilterPara[0].nRangeVal;
	nThresh[3] = m_InspPara.ImageFilterPara[0].nFillVal;

	dThresh[0] = m_InspPara.ImageFilterPara[0].dGamma;
*/
	nThresh[1] = 2;
	nThresh[2] = 0;
	nThresh[3] = 0;

	dThresh[0] = 0.5;

	invgamma = 1.f / (float)dThresh[0];

	bSave = TRUE;
	for (int i = 0; i < m_qSearchArea.size(); i++)
	{
		//검사 영역 추출
		/////////////////////////////////////////////////////
		rect.left = m_qSearchArea[i].left;
		rect.right = m_qSearchArea[i].right;
		rect.top = m_qSearchArea[i].top;
		rect.bottom = m_qSearchArea[i].bottom;

		crop_rectangle1(*image, &hImage, (Hlong)rect.top, (Hlong)rect.left, (Hlong)rect.bottom, (Hlong)rect.right);
		gen_rectangle1(&hRectangle, (Hlong)rect.top, (Hlong)rect.left, (Hlong)rect.bottom, (Hlong)rect.right);
		get_image_pointer1(hImage, (Hlong*)&image_ptr, typ, &width, &height);
		/////////////////////////////////////////////////////
		if (bSave)
		{
			strTemp.Format(_T("D:\\DATABASE\\MODEL\\step1_%d.bmp"), i);
			sprintf_s(cSavePath, strTemp.GetLength() + 1, "%S", strTemp);
			gen_image1(&hSaveImage, "byte", width, height, (Hlong)image_ptr);
			write_image(hSaveImage, "bmp", 0, cSavePath);
		}

		GetHistgram(image_ptr, width, height, Histo);
		illuminate(hImage, &hImage, 10, 10, GetilluminateFactorCal(Histo));

		get_image_pointer1(hImage, (Hlong*)&image_ptr, typ, &width, &height);
		/////////////////////////////////////////////////////
		if (bSave)
		{
			strTemp.Format(_T("D:\\DATABASE\\MODEL\\step2_%d.bmp"), i);
			sprintf_s(cSavePath, strTemp.GetLength() + 1, "%S", strTemp);
			gen_image1(&hSaveImage, "byte", width, height, (Hlong)image_ptr);
			write_image(hSaveImage, "bmp", 0, cSavePath);
		}
		lcrop_left = rect.left;
		lcrop_top = rect.top;

		BlurImage(image_ptr, width, height);
		if (bSave)
		{
			strTemp.Format(_T("D:\\DATABASE\\MODEL\\step3_%d.bmp"), i);
			sprintf_s(cSavePath, strTemp.GetLength() + 1, "%S", strTemp);
			gen_image1(&hSaveImage, "byte", width, height, (Hlong)image_ptr);
			write_image(hSaveImage, "bmp", 0, cSavePath);
		}

		//감마 보정 GammaCorrection
		/////////////////////////////////////////////////////
		GammaCorrection(image_ptr, width, height, invgamma);
		/////////////////////////////////////////////////////
		if (bSave)
		{
			strTemp.Format(_T("D:\\DATABASE\\MODEL\\step4_%d.bmp"), i);
			sprintf_s(cSavePath, strTemp.GetLength() + 1, "%S", strTemp);
			gen_image1(&hSaveImage, "byte", width, height, (Hlong)image_ptr);
			write_image(hSaveImage, "bmp", 0, cSavePath);
		}

		//도전볼 강조 및 평균 값으로 채우기 AverageFill
		////////////////////////////////////////////////////////////
		AverageFill(image_ptr, width, height, nThresh[2], nThresh[3]);
		////////////////////////////////////////////////////////////
		if (bSave)
		{
			strTemp.Format(_T("D:\\DATABASE\\MODEL\\step5_%d.bmp"), i);
			sprintf_s(cSavePath, strTemp.GetLength() + 1, "%S", strTemp);
			gen_image1(&hSaveImage, "byte", width, height, (Hlong)image_ptr);
			write_image(hSaveImage, "bmp", 0, cSavePath);
		}

		nThresh[0] = GetImageAvgVal(image_ptr, width, height);
		//명암비 보정 ContrastCorrection
		////////////////////////////////////////////////////////////
		ContrastCorrection(image_ptr, width, height, nThresh[0], nThresh[1]);
		////////////////////////////////////////////////////////////
		if (bSave)
		{
			strTemp.Format(_T("D:\\DATABASE\\MODEL\\step6_%d.bmp"), i);
			sprintf_s(cSavePath, strTemp.GetLength() + 1, "%S", strTemp);
			gen_image1(&hSaveImage, "byte", width, height, (Hlong)image_ptr);
			write_image(hSaveImage, "bmp", 0, cSavePath);
		}

		BlurImage(image_ptr, width, height);
		if (bSave)
		{
			strTemp.Format(_T("D:\\DATABASE\\MODEL\\step7_%d.bmp"), i);
			sprintf_s(cSavePath, strTemp.GetLength() + 1, "%S", strTemp);
			gen_image1(&hSaveImage, "byte", width, height, (Hlong)image_ptr);
			write_image(hSaveImage, "bmp", 0, cSavePath);
		}

		nThresh[0] = GetImageAvgVal(image_ptr, width, height);
		//명암비 보정 ContrastCorrection
		////////////////////////////////////////////////////////////
		ContrastCorrection(image_ptr, width, height, nThresh[0], nThresh[1]);
		////////////////////////////////////////////////////////////
		if (bSave)
		{
			strTemp.Format(_T("D:\\DATABASE\\MODEL\\step8_%d.bmp"), i);
			sprintf_s(cSavePath, strTemp.GetLength() + 1, "%S", strTemp);
			gen_image1(&hSaveImage, "byte", width, height, (Hlong)image_ptr);
			write_image(hSaveImage, "bmp", 0, cSavePath);
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
	}
	delete Histo;

}

void PRSPTNFINDER::GammaCorrection(BYTE* by_source, long width, long height, float fGamma)
{
	float	Tmp;
	for (int h = 0; h < height; h++)
	{
		for (int w = 0; w < width; w++)
		{
			Tmp = pow((by_source[(h * width) + w] / 255.f), fGamma) * 255 + 0.5f;
			by_source[(h * width) + w] = BYTELIMITE(Tmp);
//			by_source[(h * width) + w] = BYTELIMITE(pow((by_source[(h * width) + w] / 255.f), fGamma) * 255 + 0.5f);
		}
	}
}

void PRSPTNFINDER::AverageFill(BYTE* by_source, long width, long height, int nRangeVal, int nFillVal)
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
				by_BlackSource[(h * width) + w - 1] = BYTELIMITE((by_source[(h * width) + w] + by_source[(h * width) + w + 1] + by_source[(h * width) + w - 1]) / 3 - nFillVal);
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
			if (val >(nAvg + nRangeVal))
			{
				by_WhiteSource[(h * width) + w - 1] = BYTELIMITE((by_source[(h * width) + w] + by_source[(h * width) + w + 1] + by_source[(h * width) + w - 1]) / 3 + nFillVal);
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
			else if (by_BlackSource[(h * width) + w] < nAvg)
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

void PRSPTNFINDER::ContrastCorrection(BYTE* by_source, long width, long height, int nStanderdVal, int nContrastVal)
{
	int val;
	for (int h = 0; h < height; h++)
	{
		for (int w = 0; w < width; w++)
		{
			val = by_source[(h * width) + w];

			by_source[(h * width) + w] = BYTELIMITE(val + ((val - nStanderdVal) * nContrastVal / 100));
		}
	}
}

BYTE PRSPTNFINDER::GetImageAvgVal(BYTE* by_source, long width, long height)
{
	BYTE rslt = 0;
	long long sum = 0;
	int nCnt = 0;

	for (int h = 0; h < height; h++)
	{
		for (int w = 0; w < width; w++)
		{
			sum += by_source[(h * width) + w];
		}
	}

	return BYTE(sum / (width * height));
}

void PRSPTNFINDER::BlurImage(BYTE* by_source, long width, long height, int nRepeat)
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

void PRSPTNFINDER::GetHistgram(BYTE* by_source, long width, long height, long* lHisto)
{
	memset(lHisto, NULL, sizeof(long) * MAX_VAL);

	for (int h = 0; h < height; h++)
	{
		for (int w = 0; w < width; w++)
		{
			lHisto[by_source[(h * width) + w]]++;
		}
	}
}

double PRSPTNFINDER::GetilluminateFactorCal(long* lHisto, int nHistoCnt)
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

	for (int i = (MAX_VAL - 1); i >= 0; i--)
	{
		if (lHisto[i] > nHistoCnt)
		{
			nMax = i;
			break;
		}
	}

	rslt = log10(sqrt((double)(nMax - nMin)));

	return rslt;
}