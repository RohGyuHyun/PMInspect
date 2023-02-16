#ifndef __PRSPTNFINDER_H__
#define	__PRSPTNFINDER_H__

#include	<deque>

using namespace	std;

typedef	struct PRSPTNFINDER_RESULT
{
	deque<deque<DOUBLE>>	m_qPatternSize;	//	SearchArea별 패턴 사이즈

	VOID	Clear()
	{
		for (int i = 0; i < m_qPatternSize.size(); i++)
		{
			if (m_qPatternSize[i].size() > 0)
				m_qPatternSize[i].clear();
		}

		m_qPatternSize.clear();
	}
}PRSPTNFINDER_RESULT, *P_PRSPTNFINDER_RESULT;

typedef	class PRSPTNFINDER
{
private:
	deque<RECT>	m_qSearchArea;

	BOOL	ProcessFindPattern(Hobject* pImage, deque<DOUBLE> &qPattern);

	void ImageFilter(Hobject *image, BYTE *rslt_image);
	void GammaCorrection(BYTE* by_source, long width, long height, float fGamma);
	void AverageFill(BYTE* by_source, long width, long height, int nRangeVal, int nFillVal);
	void ContrastCorrection(BYTE* by_source, long width, long height, int nStanderdVal, int nContrastVal);
	BYTE GetImageAvgVal(BYTE* by_source, long width, long height);
	void BlurImage(BYTE* by_source, long width, long height, int nRepeat = 1);
	void GetHistgram(BYTE* by_source, long width, long height, long* lHisto);
	double GetilluminateFactorCal(long* lHisto, int nHistoCnt = 5);

public:
	PRSPTNFINDER();
	virtual ~PRSPTNFINDER();

	BOOL	Create();
	VOID	Release();

	BOOL	PushSearchArea(RECT *pRect);
	BOOL	SetSearchArea(int Idx, RECT *pRect);
	LONG	GetAreaCount();
	BOOL	GetSearchArea(int Idx, RECT *pRect);
	BOOL	DeleteSearchArea(int Idx);
	VOID	DeleteAllSearchArea();

	BOOL	FindPressPattern(VOID* pData, LONG Width, LONG Height, LONG Bpp, P_PRSPTNFINDER_RESULT pResult);
	BOOL	FindPressPattern(Hobject* pData, LONG Width, LONG Height, LONG Bpp, P_PRSPTNFINDER_RESULT pResult);

}PRSPTNFINDER, *P_PRSPTNFINDER;

#endif