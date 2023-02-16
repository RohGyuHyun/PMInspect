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
	//	����
	DOUBLE		m_umPerPixelX;								//	�ȼ��� ��ũ��
	DOUBLE		m_umPerPixelY;								//	�ȼ��� ��ũ��
	DOUBLE		m_umPerPulseX;								//	�޽��� ��ũ��
	DOUBLE		m_umPerPulseY;								//	�޽��� ��ũ��
	DOUBLE		m_AnglePerPulse;							//	�޽��� ��ũ��
	DOUBLE		m_DistRotX;									//	������ ������ ��ġ������ ī�޶� �߽ɰ� ȸ���߽��� ���� X�� �Ÿ�(��ũ�� ����)
	DOUBLE		m_DistRotY;									//	������ ������ ��ġ������ ī�޶� �߽ɰ� ȸ���߽��� ���� Y�� �Ÿ�(��ũ�� ����)
	BOOL		m_bAlignCheckAllCam;						//	����� ������ ���� �� ���� ����� ��ġ�� ī�޶� 1������ ��Ȯ��

	//	���Ϻ� ������
	DOUBLE		m_MotionXPos[IMGALIGN_NUM_MAX_PTN];			//	���� ��
	DOUBLE		m_MotionYPos[IMGALIGN_NUM_MAX_PTN];			//	���� ��
	DOUBLE		m_MotionTPos[IMGALIGN_NUM_MAX_PTN];			//	���� ��
	DOUBLE		m_LightLevel[IMGALIGN_NUM_MAX_PTN];			//	���� ��
	DOUBLE		m_Threshold[IMGALIGN_NUM_MAX_PTN];			//	������ �������
	typePatPara	m_PatPara[IMGALIGN_NUM_MAX_PTN];

	//	���� ���� ������
	RECT		m_PatternBuildArea[IMGALIGN_NUM_MAX_PTN];	//	���� ���� �� ����� ����(��� �ܺο��� ���� �Ұ�)
	RECT		m_PatternSearchArea[IMGALIGN_NUM_MAX_PTN];	//	���� �˻� �� ����� ����(��� �ܺο��� ���� �Ұ�)
	DOUBLE		m_PtnCenterX[IMGALIGN_NUM_MAX_PTN];			//	������ �߽� X��ǥ(��� �ܺο��� ���� �Ұ�)
	DOUBLE		m_PtnCenterY[IMGALIGN_NUM_MAX_PTN];			//	������ �߽� Y��ǥ(��� �ܺο��� ���� �Ұ�)

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

	//	ī�޶� �� 2���� ���濡 ���� ���� ����
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