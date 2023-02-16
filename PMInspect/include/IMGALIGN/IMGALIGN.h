#ifndef	__IMGALIGN_H__
#define	__IMGALIGN_H__

#include "..\_FuncModuleHal.h"
#include "..\PARAMANAGER\PARAMANAGER.h"
#include "..\DialogDispAlign.h"

const	enum IMGALIGN_STATUS
{
	IMGALIGN_STATUS_NOT_CREATED,
	IMGALIGN_STATUS_IDLE,
	IMGALIGN_STATUS_GRAB,
	IMGALIGN_STATUS_GRAB_FAILED,
	IMGALIGN_STATUS_GRAB_COMPLETE,
	IMGALIGN_STATUS_PATTERN_BUILD,
	IMGALIGN_STATUS_PATTERN_BUILD_FAILED,
	IMGALIGN_STATUS_PATTERN_BUILD_COMPLETE,
	IMGALIGN_STATUS_PATTERN_MATCH,
	IMGALIGN_STATUS_PATTERN_MATCH_FAILED,
	IMGALIGN_STATUS_PATTERN_MATCH_COMPLETE,
	IMGALIGN_STATUS_CALC_ALIGN,
	IMGALIGN_STATUS_CALC_ALIGN_FAILED,
	IMGALIGN_STATUS_CALC_ALIGN_COMPLETE
};

#define	IMGALIGN_NUM_MAX_PTN	2

static	LPCTSTR	IMGALIGN_STR_SECTIONNAME = _T("ALIGN_PARAMETER");
static	LPCTSTR	IMGALIGN_STR_PARAFILENAME = _T("Align.par");
static	LPCTSTR	IMGALIGN_STR_PTNFILENAME[IMGALIGN_NUM_MAX_PTN] =
{
	_T("Align_00.ptn"),
	_T("Align_01.ptn")
};

typedef	struct IMGALIGN_PTNDIFF
{
	DOUBLE	m_XCenter;
	DOUBLE	m_YCenter;
	DOUBLE	m_Angle;
	DOUBLE	m_Scale;
	DOUBLE	m_Score;
}IMGALIGN_PTNDIFF, *P_IMGALIGN_PTNDIFF;

typedef	struct IMGALIGN_PARAM
{
	//	공용
	DOUBLE		m_umPerPixelX;								//	픽셀당 미크론
	DOUBLE		m_umPerPixelY;								//	픽셀당 미크론
	DOUBLE		m_umPerPulseX;								//	펄스당 미크론
	DOUBLE		m_umPerPulseY;								//	펄스당 미크론
	DOUBLE		m_AnglePerPulse;							//	펄스당 미크론
	DOUBLE		m_DistRotX;									//	모션장비가 원점에 위치했을때 카메라 중심과 회전중심축 간의 X축 거리(미크론 단위)
	DOUBLE		m_DistRotY;									//	모션장비가 원점에 위치했을때 카메라 중심과 회전중심축 간의 Y축 거리(미크론 단위)
	BOOL		m_bAlignCheckAllCam;						//	얼라인 시퀀스 진행 중 최종 얼라인 위치를 카메라 1번부터 재확인

	//	패턴별 설정값
	DOUBLE		m_MotionXPos[IMGALIGN_NUM_MAX_PTN];			//	조명 값
	DOUBLE		m_MotionYPos[IMGALIGN_NUM_MAX_PTN];			//	조명 값
	DOUBLE		m_MotionTPos[IMGALIGN_NUM_MAX_PTN];			//	조명 값
	DOUBLE		m_LightLevel[IMGALIGN_NUM_MAX_PTN];			//	조명 값
	DOUBLE		m_Threshold[IMGALIGN_NUM_MAX_PTN];			//	패턴의 쓰레숄드
	typePatPara	m_PatPara[IMGALIGN_NUM_MAX_PTN];

	//	내부 생성 데이터
	RECT		m_PatternBuildArea[IMGALIGN_NUM_MAX_PTN];	//	패턴 생성 시 사용할 영역(모듈 외부에서 수정 불가)
	RECT		m_PatternSearchArea[IMGALIGN_NUM_MAX_PTN];	//	패턴 검색 시 사용할 영역(모듈 외부에서 수정 불가)
	DOUBLE		m_PtnCenterX[IMGALIGN_NUM_MAX_PTN];			//	패턴의 중심 X좌표(모듈 외부에서 수정 불가)
	DOUBLE		m_PtnCenterY[IMGALIGN_NUM_MAX_PTN];			//	패턴의 중심 Y좌표(모듈 외부에서 수정 불가)

}IMGALIGN_PARAM, *P_IMGALIGN_PARAM;

typedef	class IMGALIGN
{
private:
	LONGLONG			m_PatternID[IMGALIGN_NUM_MAX_PTN];
	IMGALIGN_PTNDIFF	m_PatternDiff[IMGALIGN_NUM_MAX_PTN];
	PYLONCTRL_IMG		m_CamImage[IMGALIGN_NUM_MAX_PTN];
	CDialogDispAlign*	m_pWndDisplay[IMGALIGN_NUM_MAX_PTN];
	BOOL				m_bIsPtnLoaded[IMGALIGN_NUM_MAX_PTN];

	CString				m_strModelPath;
	PARAMANAGER			m_ParaManager;
	IMGALIGN_PARAM		m_Parameter;
	IMGALIGN_STATUS		m_Status;

	VOID	SetLinkConfigData();

	BOOL	SavePattern(int Idx, LPCTSTR FileName);
	BOOL	LoadPattern(int Idx, LPCTSTR FileName);

public:
	IMGALIGN();
	virtual ~IMGALIGN();

	//	카메라 뷰 2개로 변경에 따른 구조 수정
	BOOL	Create(CWnd* pWndParent, LPRECT pRectAlign1, LPRECT pRectAlign2, LPCTSTR strParaPath);
	VOID	Release();

	BOOL	BuildPattern(int Idx, CDialogDispAlign* pDlgDispAlign = NULL);
	BOOL	PatternMatch(int Idx, CDialogDispAlign* pDlgDispAlign = NULL);
	BOOL	DeletePattern(int Idx);
	BOOL	GetAlignCompPos(DOUBLE LinearPosX0, DOUBLE LinearPosX1, DOUBLE LinearPosY0, DOUBLE LinearPosY1, DOUBLE *pAngle, DOUBLE *pX, DOUBLE *pY, DOUBLE *pDistance = NULL, BOOL IsIgnoreAngle = FALSE, BOOL IsTeaching = FALSE);
	BOOL	Grab(int Idx);
	BOOL	IsGrabbing(int Idx) { return	m_pWndDisplay[Idx]->IsGrabbing(); }
	BOOL	Live(int Idx, BOOL bOn);
	LONG	GetCamWidth();
	LONG	GetCamHeight();

	BOOL	GetParameter(P_IMGALIGN_PARAM pParam);
	BOOL	SetParameter(P_IMGALIGN_PARAM pParam);
	VOID	SetActiveTracker(BOOL bActivate);
	VOID	SetTrackerPos(int Idx, LPRECT pRect);
	BOOL	SaveAlignData();
	BOOL	LoadAlignData(LPCTSTR strParaPath = NULL);

	BOOL	GetPtnDiff(int Idx, P_IMGALIGN_PTNDIFF pPtnDiff);
	IMGALIGN_STATUS	GetStatus();

	BOOL	GetPatternID(int Idx, LONGLONG* pPatternID);
	BOOL	IsPatternLoaded(int Idx) { return	m_bIsPtnLoaded[Idx]; }
	BOOL	IsCheckAllCam() { return	m_Parameter.m_bAlignCheckAllCam; }
	BOOL	ForceDisplay(int Idx) {	return	m_pWndDisplay[Idx]->ForceDisplay();	}


}IMGALIGN, *P_IMGALIGN;

#endif