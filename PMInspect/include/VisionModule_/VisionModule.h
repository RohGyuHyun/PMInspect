#pragma once
#include "MilCtrl\MILCTRL.h"

#define THREAD_TEST
#define HAL_FILTER

#define FILTER_TYPE_AVERAGE_FILL			0
#define FILTER_TYPE_GAMMA_CORRECTION		1
#define FILTER_TYPE_CONTRAST_CORRECTION		2
#define FILTER_TYPE_LAPLACIAN				3
#define FILTER_TYPE_DIFFUSION				4
#define FILTER_TYPE_BLUR					5

#define MAX_INS_CNT						3
#define MAX_INS_Y_RECT_CNT				1000

#define MAX_INS_PARA_CNT				5
#define MAX_INS_FILTER_CNT				1

#define INSPECTION_SPACE_PM_COUNT_TEMP	-1
#define INSPECTION_SPACE_PM_COUNT		0
#define INSPECTION_SPACE_PM_SHIFT		1

#define MAX_NOISE_SIZE_PARA_CNT			3
#define MAX_MARGE_DISTANCE_PARA_CNT		1
#define MAX_THRESH_PARA_CNT				5
#define MAX_DEFECT_CNT					5


#define MAX_HALCON_IMAGE_HEIGHT			MAX_GRAB_HEIGHT
#define MAX_HALCON_IMAGE_WIDTH			2448

#define MAX_VAL							256

#define TYPE_PRESS_MARK_INSP			0
#define TYPE_SHIFT_INSP					1

#define SHIFT_INSP_WIDTH_SIZE			50
#define SHIFT_INSP_HEIGHT_SIZE			50

#define MAX_SHIFT_INSP_CNT				4
#define SHIFT_IMAGE_AVG_VAL_RANGE		20


#define MAX_PARALLEL_THREAD		40


#define INSPECTION_TYPE_NORMAL			0
#define INSPECTION_TYPE_PARALLEL		1


#define INSPECTION_PM_TYPE_W_UP_B_DOWN			0
#define INSPECTION_PM_TYPE_W_DOWN_B_UP			1
#define INSPECTION_PM_TYPE_W_LEFT_B_RIGHT		2
#define INSPECTION_PM_TYPE_W_RIGHT_B_LEFT		3

#define FULL_WIDTH_DIVISION_MAX_CNT				10
#define FULL_HEIGHT_DIVISION_MAX_CNT			4

typedef struct InspectionRect
{
	LONG lRectLeft;
	LONG lRectTop;
	LONG lRectRight;
	LONG lRectBottom;
	LONG lOffsetX;
	LONG lOffsetY;
}typeInsRect;

typedef struct ShiftInspectionRect
{
	LONG lRectLeft;
	LONG lRectTop;
	LONG lRectRight;
	LONG lRectBottom;
	BOOL isInsp;
}typeShiftInspRect;

typedef struct InspectionRectRecipe
{
	typeInsRect InspRect[MAX_INS_Y_RECT_CNT];
	typeInsRect StartRect;
	typeInsRect EndRect;
	LONG nInspYSpace;
	LONG nYRectCnt;
	LONG nShiftInspSpaceX;
	LONG nShiftInspSpaceY;
	double dLaplaceSigma;
}typeInspRectRecipe;

typedef struct ImageFilterPara
{
	DOUBLE dGamma;
	LONG nRangeVal;
	LONG nFillVal;
	LONG nContrastVal;
	DOUBLE dLaplAlpha;
	DOUBLE dDiffLambda;
	DOUBLE dDiffK;
	LONG nDiffIter;

	int nMeanSize;
	int nIllumiSize;
	double dLaplaceSigma;
}typeImageFilter;

typedef struct PMSearchPara
{
	LONG nThreshHold[MAX_INS_PARA_CNT];
	LONG nMargeDistance;
	LONG nNoiseSize[MAX_NOISE_SIZE_PARA_CNT];
	LONG nDefectCnt;
	int nPmType;
	double dLaplaceSigma;
}typePMSearch;

typedef struct PMDistributionInspPara
{
	int nDistributionXCnt;
	int nDistributionYCnt;
	int nDistributionMinCnt;
}typePMDistributionInspPara;

typedef struct InspectionPara
{
	DOUBLE dPixelSize;
	int nInspTypeCnt;
	int nLightVal[MAX_INS_CNT];
	typeInspRectRecipe InspRectRecipe[MAX_INS_CNT];
	typeShiftInspRect ShiftInspRect[MAX_SHIFT_INSP_CNT][MAX_INS_Y_RECT_CNT];
	typeImageFilter ImageFilterPara[MAX_INS_CNT];
	typePMSearch PMSearchPara[MAX_INS_CNT];
	BOOL bInspImgSave;
	BOOL bFilterImgView;
	BOOL bInspImgView;
	BOOL bAutoInspRectTeaching;
	BOOL bInspection;
	typePMDistributionInspPara PMDistributionInspPara;
}typeInspPara;

typedef struct InspRslt
{
	LONG lRslt_num;
	Hobject hRslt_rgn;
	Hobject hInsp_rgn;
	char cRsltText[MAX_INS_Y_RECT_CNT][20];
	BOOL bShiftInspRslt[4][MAX_INS_Y_RECT_CNT];
	WORD wShiftInspRslt[MAX_INS_CNT][MAX_INS_Y_RECT_CNT];
	Hobject hShiftInsp_rgn;
	Hobject hShiftRslt_rgn;
	BOOL bInspRslt;
	Hobject hDistributionInspRgn;
	int nDistribution[MAX_INS_Y_RECT_CNT][FULL_WIDTH_DIVISION_MAX_CNT][FULL_HEIGHT_DIVISION_MAX_CNT];
	BYTE byDistributionRsltType[MAX_INS_Y_RECT_CNT];
}typeInspRslt;

typedef struct ThreadInspPara
{
	CRect InspRect[200];
	typeShiftInspRect ShiftInspRect[4][200];
	int nInspRectIdx[200];
	LONG nInspYSpace;
	LONG nYRectCnt;
	LONG nShiftInspSpaceX;
	LONG nShiftInspSpaceY;
	CRect InspImageSize;
	int nStartLeadIdx;
	int nEndLeadIdx;
#ifndef THREAD_TEST
	Hobject hInspImage;
#endif
	typeImageFilter ImageFilterPara;
	typePMSearch PMSearchPara;
	BOOL bInspImgSave;
	BOOL bFilterImgView;
	BOOL bInspImgView;
	double dPixelSize;
	typePMDistributionInspPara PMDistributionInspPara;
#ifdef THREAD_TEST
	//Hobject hFilterImage;
	char cRsltText[20][20];
#endif
}typeThreadInsp;

typedef struct PMRslt
{
	long lLeadNum;
	long lRsltIdx;
	CPoint pCenter;
	LONG lLongLenght;
	LONG lShortLenght;
	double dPhi;
}typePMRslt;

typedef struct PMCntRslt
{
	Hobject hInspRgn;
	Hobject hRsltRgn;
	deque<typePMRslt> PMRslt;
	BOOL bInspRslt;
	LONG lRsltAllCnt;
	LONG lRsltCnt[MAX_INS_Y_RECT_CNT];
	BOOL bRslt[MAX_INS_Y_RECT_CNT];
	int nStartLeadIdx;
	int nEndLeadIdx;
}typePMCntRslt;

typedef struct PMShiftRslt
{
	Hobject hInspRgn;
	Hobject hRsltRgn;
	int nRsltType[MAX_INS_Y_RECT_CNT];
	BOOL bInspRslt;
}typePMShiftRslt;

typedef struct PMDistributionRslt
{
	Hobject hInspRgn;
	int nDistribution[MAX_INS_Y_RECT_CNT][FULL_WIDTH_DIVISION_MAX_CNT][FULL_HEIGHT_DIVISION_MAX_CNT];
	BOOL bInspRslt;
	BYTE byDistributionRsltType[MAX_INS_Y_RECT_CNT];

}typePMDistributionRslt;

typedef struct PMSLRslt
{
	deque<typePMRslt> PMSRslt;
	deque<typePMRslt> PMLRslt;
	Hobject hSRsltRgn;
	Hobject hLRsltRgn;
	BOOL bInspRslt;
	long lRsltSCnt[MAX_INS_Y_RECT_CNT];
	long lRsltLCnt[MAX_INS_Y_RECT_CNT];
	BOOL bSLRslt[MAX_INS_Y_RECT_CNT];
}typePMSLRslt;


typedef struct PMEachRslt
{
	BYTE byRslt;
	int nCnt;
}typePMEachRslt;


static	LPCTSTR	INSP_STR_PARAFILENAME = _T("Inspection.par");

#ifdef THREAD_TEST
#define MAX_PARALLEL_INSP_THREAD_CNT	60
#define MAX_PARALLEL_INSP_CNT			MAX_PARALLEL_INSP_THREAD_CNT
#define INSP_LEAD_CNT					10
//#define	g_Inspection	CInspection::GetInstance()
class CInspection
{
private:
	static	CInspection	m_Instance;
public:
	CInspection();
	virtual ~CInspection(void);

public:
	static	CInspection&	GetInstance() { return	m_Instance; }

	void SetParallelInsp(long disp, CRect PartSize, int nIdx, CRect rInspRect, Hobject *image, typeThreadInsp *ThreadInspPara);
	BOOL GetUnderInspection() { return m_bParallelUnderInspection; };
	BOOL WaitEndThread();

	CString GetRsltText(int nIdx) { return m_strRsltText[nIdx]; };
	int GetRsltCnt(int nIdx) { return m_PmCntRslt.lRsltCnt[nIdx]; };
	BOOL GetInspEnd(int nIdx) { return m_bInspEnd[nIdx]; };
	Hobject *GetRsltRgn() { return &m_RsltRgn; };
	Hobject *GetShiftRsltRgn() { return &m_ShiftRsltRgn; };
	Hobject *GetRsltFilterImage() { return &m_RsltFilterImage; };
	CRect *GetRsltImagePos() { return &m_RsltImagePos; };
	BOOL GetRsltCopy() { return m_bRsltCopy; };
	void SetRsltCopy(BOOL isCopy) { m_bRsltCopy = isCopy; };

	int GetStartRectIdx() { return m_ThreadInspPara.nInspRectIdx[0]; };
	int GetInspRectIdx(int nIdx){return m_ThreadInspPara.nInspRectIdx[nIdx];};
	int GetInspRectCnt() { return m_ThreadInspPara.nYRectCnt; };
	void InitRslt();

	BOOL GetShiftRslt(int nDirection, int nIdx) { return m_bShiftRslt[nDirection][nIdx]; };
	Hobject *GetDistributionInspRgn() { return &m_PMDistributionRslt.hInspRgn; };
	int GetDistributionRsltCnt(int nIdx, int nX, int nY) { return m_PMDistributionRslt.nDistribution[nIdx][nX][nY]; };

	typePMCntRslt GetPMCntRslt() { return m_PmCntRslt; };
private:
	void ParallelInsp();
	BOOL PMCntInspection(long disp, Hobject *image, typeThreadInsp ThreadInspPara, BOOL bFilterDisp, BOOL bSave, BOOL bInspImageDisp = FALSE);
	void GetHistgram(BYTE* by_source, long width, long height, long* lHisto);
	int GetHistoMax(long* lHisto);
	BOOL ClipImage(Hobject *src_image, Hobject *rslt_image, CRect rect);

	BOOL PMShiftInspection(long disp, Hobject*image, typeThreadInsp ThreadInspPara);
	BOOL PMDistributionInspection(long disp);

protected:
	long m_Disp;
	Hobject m_RsltRgn;
	Hobject m_RsltFilterImage;
	Hobject m_InspImage;
	Hobject m_ShiftRsltRgn;
	CRect m_RsltImagePos;
	BOOL m_bInspEnd[MAX_PARALLEL_INSP_CNT];
	int m_nRsltCnt[MAX_PARALLEL_INSP_CNT];
	CString m_strRsltText[MAX_PARALLEL_INSP_CNT];
	BOOL m_nInspEnd[MAX_PARALLEL_INSP_CNT];
	BOOL m_bShiftRslt[4][MAX_PARALLEL_INSP_CNT];
	CRect m_PartSize;

	BOOL m_bRsltCopy;

	ThreadInspPara m_ThreadInspPara;
	typePMCntRslt m_PmCntRslt;
	typePMDistributionRslt m_PMDistributionRslt;
	typePMSLRslt m_PMSLRslt;

	CWinThread *m_pThreadParallelInsp;
	UINT static ParallelInspThread(LPVOID pParam);
	BOOL m_bParallelUnderInspection;
	int m_nParallelInspIdx;
};
#endif
class CVisionModule
{
public:
	CVisionModule(long lWidth, long lHeight, int nInspType = INSPECTION_TYPE_NORMAL);
	virtual ~CVisionModule(void);

public:
	typeInspPara GetInspPara() { return m_InspPara; };
	void SetInspPara(typeInspPara *inspPara);
	
	BOOL GetGrabEnd(){return m_bGrabEnd;};
	void SetGrabEnd(BOOL bGrabEnd){m_bGrabEnd = bGrabEnd;};

	BOOL GetInspectionStart() { return m_bInspectionStart; };
	BOOL GetInspectionEnd() { return m_bInspectionEnd; };
	void SetInspectionStart(BOOL isStart) { m_bInspectionStart = isStart; };
	void SetInspectionEnd(BOOL isEnd) { m_bInspectionEnd = isEnd; };
	void SetInspection();
	
	Hobject *GetGrabImage(){return &m_HalGrabImage;};
	void SetGrabImage(Hobject *HalImage){m_HalGrabImage = *HalImage;};
	Hobject *GetDispImage() { return &m_HalDispImage; };
	void SetDispImage(Hobject *HalImage) { m_HalDispImage = *HalImage; };
	void SetDisp(long disp){m_Disp = disp;};

	typeInspRslt GetRslt(int nInspIdx = 0) { return m_InspRslt[nInspIdx]; };
	BOOL SetInspRgn(int nSpaceType){return SelectInspectionSpace(nSpaceType);};
	Hobject GetInspRgn(int nInspIdx = 0, int nInspType = 0);
	Hobject GetInspRsltRgn(int nInspIdx = 0, int nInspType = 0);
	long GetInspRsltNum(int nInspIdx = 0) { return m_InspRslt[nInspIdx].lRslt_num; };
	void SetSelectRect(int x_idx, int y_idx, BOOL isSelect = FALSE);
	void SetStartEndInspRectView(int nInspIdx);

	int GetLightVal(int nInspIdx) {return m_InspPara.nLightVal[nInspIdx];};

	int limite(int nVal);

	void DisplayRslt();

	BOOL SaveInspPara(char* strPath);
	BOOL LoadInspPara(char* strPath);

	void InitRslt();

	BOOL SaveImage(Hobject *hSaveImage, char* strPath);
	BOOL SaveImage(BYTE *by_image, int width, int height, char *strPath);
	BOOL GetFilterImageView();
	void SetFilterImageVIew(BOOL isView) { m_bFilterImageView = isView; };
	BOOL GetRsltView() { return m_bRsltView; };
	Hobject *GetFilterImage() { return &m_HalDispFilterImage; };

	void SetInspIdx(int nIdx) { m_nInsp_Idx = nIdx; };
	int GetInspIdx() { return m_nInsp_Idx; };
	void SetSaveImagePath(char *cPath);

	BOOL GetFilterView() { m_bFilterImageView = m_InspPara.bFilterImgView; return m_InspPara.bFilterImgView; };
	void SetInspectionStart();

	BOOL SetInspImageSize(int nEndGrabHeight, int nStartYIdx, int nEndYIdx, int nParallelInspIdx);
	void SetInspImageSize(int nStartYIdx, int nEndYIdx, int nParallelInspIdx);
	//BOOL SetInspImageSize(int nStartY, int nEndY, int nParallelInspIdx);
	void SetGrabHeight(long lHeight) { m_lGrabHeight = lHeight; };

	void ParallelMainInsp();
	void ParallelInsp();
	void SetParallelInsp(Hobject *image, int nIdx);
	void SetParallelInsp(int nIdx);
	void SetParallelInspThreadStart(BOOL isStart) { m_bParallelMainInspThreadStart = isStart; };
	void SetSetPartHeight(int nHeight) { m_nSetPartHeight = nHeight; };
	void SetSetPartSize(long x1, long y1, long x2, long y2);

	BOOL GetParallelThreadEnd(int nThreadIdx);

	BOOL GetInspRslt(int nInspType) { return m_bPMCntRslt[nInspType]; };
	void SetUnderInspectionEnd(BOOL isEnd) { m_bUnderInspEnd = isEnd; };
	void UpdateInspRgn();

	//20181106 ngh
	BYTE GetPMEachRslt(int nLeadIdx) { return m_PmEachRslt[nLeadIdx].byRslt; };
	int GetPmEachRsltCnt(int nLeadIdx) { return m_PmEachRslt[nLeadIdx].nCnt; };
#ifdef THREAD_TEST
	void ParallelInspRsltSum();
	void InspInit(int nThreadCnt);;
#endif
	BOOL ConvImgToHalBuf(Hobject *pHalBuf, P_MILCTRL_IMGINFO pImg);
	void SetTestInsp(BOOL isTest) { m_bTestInsp = isTest; };
	BOOL GetTestInsp() { return m_bTestInsp; };
protected:
	long m_Disp;
	BOOL m_bInit;
	BOOL m_bGrabEnd;

	long m_lWidth;
	long m_lHeight;

	BOOL InitThread();
	CCriticalSection	m_VisionInsp_CS;
	CCriticalSection	m_VisionInsp_CS2;
	CCriticalSection	m_VisionInsp_CS3;

	CWinThread *m_pThreadParallelMainInsp;
	UINT static ParallelMainInspThread(LPVOID pParam);
	BOOL m_bParallelMainInspThreadStart;
	BOOL m_bParalleMainlInspThreadEnd;

	CWinThread *m_pThreadParallelInsp[MAX_PARALLEL_THREAD];
	UINT static ParallelInspThread(LPVOID pParam);
	BOOL m_bParallelInspThreadStart[MAX_PARALLEL_THREAD];
	BOOL m_bParallelInspThreadEnd[MAX_PARALLEL_THREAD];
	BOOL m_bParallelInspStart[MAX_PARALLEL_THREAD];
	BOOL m_bParallelInspEnd[MAX_PARALLEL_THREAD];
	BOOL m_bParallelUnderInspection[MAX_PARALLEL_THREAD];
	int m_nParallelInspIdx;

	CWinThread *m_pThreadInspection;
	UINT static InspectionThread(LPVOID pParam);
	BOOL m_bInspThreadStart;
	BOOL m_bInspThreadEnd;
	BOOL m_bInspectionStart;
	BOOL m_bInspectionEnd;
	BOOL SelectInspectionSpace(int nSpaceType);// 0 : PM Count, 1 : PM Shift
	BOOL PMCntInspection(long disp, Hobject *image, long *rslt_num, Hobject *rslt_rgn, BOOL bSave);
	void PMShiftInspection(long disp, Hobject *image, BOOL bSave);

	BOOL PMCntInspection(long disp, Hobject *image, typeThreadInsp ThreadInspPara, BOOL bFilterDisp, BOOL bSave);
	void SetImageFilter(long disp, Hobject *image, Hobject *rslt_image, typeThreadInsp ThreadInspPara, BOOL isSave);

	BOOL PMCntInspection(long disp, Hobject *image, BOOL bFilterDisp, BOOL bSave, BOOL bInspImageDisp = FALSE);

	BOOL PMCntInspection(long disp, Hobject *image, typeThreadInsp ThreadInspPara, BOOL bFilterDisp, BOOL bSave, BOOL bInspImageDisp = FALSE);


	void SetImageFilter(long disp, Hobject *image, int nInspIdx, BOOL isDisp, BOOL isSave);
	void ImageFilter(long disp, Hobject *image, int nFilterType);

	void ImageFilter(Hobject *image, BYTE *rslt_image, BOOL bSave);
	void ImageFilter(BYTE *by_image, Hlong width, Hlong height, BOOL bSave);
	void TestFilter(BYTE *by_image, Hlong width, Hlong height, BOOL bSave);

	void AverageFill(BYTE* by_source, long width, long height, int nRangeVal, int nFillVal);					//FilterType = 0
	void GammaCorrection(BYTE* by_source, long width, long height, float fGamma);								//FilterType = 1
	void ContrastCorrection(BYTE* by_source, long width, long height, int nStanderdVal, int nContrastVal);		//FilterType = 2
	void LaplacianFilter(BYTE* by_source, long width, long height, double alpha);								//FilterType = 3
	void DiffusionFilter(BYTE* by_source, long width, long height, float lambda, float k, int iter);			//FilterType = 4
	void BlurImage(BYTE* by_source, long width, long height, int nRepeat = 1);									//FilterType = 5

	BOOL ClipImage(Hobject *src_image, Hobject *rslt_image, CRect rect);
	BOOL ClipImage(BYTE *src_image, Hobject *rslt_image, CRect rect);

	BYTE GetImageAvgVal(BYTE* by_source, long width, long height);
	void GetHistgram(BYTE* by_source, long width, long height, long* lHisto);
	double GetilluminateFactorCal(long* lHisto, int nHistoCnt = 5);
	int GetHistoMax(long* lHisto);

	void SumInspRslt();

	//20180919 ngh
	BOOL InitVision(long lWidth, long lHeight);
	BOOL CloseVision();
	BOOL GetInit() { return m_bInit; };
	BOOL InitImage();

	typeInspPara m_InspPara;
	typeInspRslt m_InspRslt[MAX_INS_CNT];
	CString m_strModelPath;

	Hobject m_HalGrabImage;
	Hobject m_HalDispImage;
	Hobject m_HalDispFilterImage;

	//ngh
	int *m_nRsltCnt[MAX_INS_CNT];
	BOOL *m_nInspEnd[MAX_INS_CNT];
	BOOL m_bFilterImageView;
	BOOL m_bRsltView;

	int m_nInsp_Idx;
	char m_cSavePath[512];

#ifdef THREAD_TEST
	CRect m_InspImageSize[MAX_PARALLEL_INSP_THREAD_CNT];
	int m_nStartInspRectIdx[MAX_PARALLEL_INSP_THREAD_CNT];
	int m_nEndInspRectIdx[MAX_PARALLEL_INSP_THREAD_CNT];
	long m_lGrabHeight;

	int m_nMaxInspRectCnt;

	int m_nParallelInspCnt;
	typeThreadInsp m_ThreadInspPara[MAX_PARALLEL_INSP_THREAD_CNT];
#else
	CRect m_InspImageSize[MAX_PARALLEL_THREAD];
	int m_nStartInspRectIdx[MAX_PARALLEL_THREAD];
	int m_nEndInspRectIdx[MAX_PARALLEL_THREAD];
	long m_lGrabHeight;

	int m_nMaxInspRectCnt;

	int m_nParallelInspCnt;
	typeThreadInsp m_ThreadInspPara[MAX_PARALLEL_THREAD];
#endif
	MILCTRL_IMGINFO	m_ImageInfo;

	int m_nLastInspRectIdx;
	int m_nSetPartHeight;
	int m_nLastInspRectY;
	CString m_strTack;

	int m_nMainInspType;

	int m_nTestVal;

	CRect m_PartSize;

	deque<Hobject> *m_RsltRgn;
	deque<Hobject> *m_RsltFilterImg;
	deque<CRect> *m_RsltImgRect;
	deque<typePMCntRslt> m_PmCntRslt;

	typePMEachRslt m_PmEachRslt[MAX_INS_Y_RECT_CNT];

	BOOL m_bPMCntRslt[MAX_INS_CNT];

	void WriteRslt(CString strRslt);
	void WritePMInfoRslt();
	CString m_strRsltFileName;

	int m_nInspCnt;

	BOOL m_bUnderInspEnd;

#ifdef THREAD_TEST
	CInspection *m_Inspection[MAX_PARALLEL_INSP_THREAD_CNT];

	int m_nInspThreadIdx;
	CWinThread *m_pThreadParallelInspRsltSum;
	UINT static ParallelInspRsltSumThread(LPVOID pParam);
	BOOL m_bParallelInspRsltSumThreadStart;
	BOOL m_bParallelInspRsltSumThreadEnd;
#endif

	CWinThread *m_pThreadImageSave;
	UINT static ImageSaveThread(LPVOID pParam);
	BOOL m_bImageSaveThreadStart;
	BOOL m_bImageSaveThreadEnd;
	void ThreadImageSave();

	CString m_strTest[MAX_PARALLEL_INSP_THREAD_CNT];
	double m_dStartTime;
	double m_dEndTime;
	BOOL m_bTestInsp;
};