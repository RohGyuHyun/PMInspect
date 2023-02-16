#pragma once
#ifndef	__MILCTRL_H__
#define	__MILCTRL_H__


#include "mil.h"
#include "AtfCtrl.h"

#define NGH
#define DOUBLE_BUF
#define LINE_SCAN
#define NOTE_BOOK

#define MAX_GRAB_WIDTH			2448
#define MAX_GRAB_HEIGHT			65000
#define GRAB_TIME_OUT_MS		1000000

#define	g_MilCtrl	MILCTRL::GetInstance()


const	enum	MILCTRL_RESULT
{
	MILCTRL_RESULT_GRAB_SUCCESS,		//	Grab 성공
	MILCTRL_RESULT_GRAB_FAIL,			//	Grab 실패
	MILCTRL_RESULT_IMAGE_SAVE_SUCCESS,	//	Grab 후 이미지 저장 성공
	MILCTRL_RESULT_IMAGE_SAVE_FAIL,		//	Grab 후 이미지 저장 실패
	MILCTRL_RESULT_AUTO_EXP_SUCCESS,	//	Auto Exposure Time 계산 성공
	MILCTRL_RESULT_AUTO_EXP_FAIL		//	Auto Exposure Time 계산 실패(Grab 실패일 경우만 Fail)
}	;

const	enum	MILCTRL_COLOR_PTN
{
	MILCTRL_COLOR_PTN_GB	=	0,
	MILCTRL_COLOR_PTN_BG	=	1,
	MILCTRL_COLOR_PTN_GR	=	2,
	MILCTRL_COLOR_PTN_RG	=	3,
	MILCTRL_COLOR_PTN_NONE	=	4
}	;

typedef	struct	MILCTRL_WB
{
	MIL_FLOAT	m_WhiteBalance[3]	;

}MILCTRL_WB, *P_MILCTRL_WB	;

typedef	struct	MILCTRL_MASKING_AREA
{
	POINT	m_LTPos	;
	POINT	m_LTLen	;
	POINT	m_RBPos	;
	POINT	m_RBLen	;
}MILCTRL_MASKING_AREA, *P_MILCTRL_MASKING_AREA	;

typedef	struct MILCTRL_IMG
{
	int					m_Width		;	//	가로 사이즈
	int					m_Height	;	//	세로 사이즈
	int					m_Bpp		;	//	Bit per Pixel
	int					m_Channel	;	//	채널(MONO, BAYER -> 1, RGB, BGR -> 3)
	MILCTRL_COLOR_PTN	m_ColorType	;	//	컬러 타입
	int					m_DataSize	;	//	전체 데이터 사이즈(바이트 수)
	BYTE*				m_pData		;	//	이미지 데이터 시작 포인터
}MILCTRL_IMGINFO, *P_MILCTRL_IMGINFO;

typedef	class	MILCTRL
{
	////////////////////////////////////////////////////////////////////////
	//	Singlton
	private	:
		static	MILCTRL	m_Instance		;

		MILCTRL()	;
		~MILCTRL()	;
		MILCTRL(const MILCTRL&)		{}
		void operator=(const MILCTRL&)	{}

	public:
		static	MILCTRL&	GetInstance()	{	return	m_Instance	;	}
	/////////////////////////////////////////////////////////////////////////


	private	:
		//	System 및 컨트롤
		MIL_ID	m_App			;
		MIL_ID	m_Sys			;
		MIL_ID	m_Dig			;

		//	Display
		MIL_ID	m_MainDisp		;
		MIL_ID	*m_ArrSubDisp	;

		//	Grab Buffer
		MIL_ID	m_GrabBuf		;

		//	Processing Buffer
		MIL_ID	m_ProcBuf		;

		//	Main Display Buffer
		MIL_ID	m_DisplayBuf	;
		MIL_ID	m_DispBufR		;
		MIL_ID	m_DispBufG		;
		MIL_ID	m_DispBufB		;
		MIL_ID	m_OverlayBuf	;

		//	Grabbed Image buffer Array
		MIL_ID	*m_ArrSubGrabBuf	;
		MIL_ID	*m_ArrSubGrabBufR	;
		MIL_ID	*m_ArrSubGrabBufG	;
		MIL_ID	*m_ArrSubGrabBufB	;

		MIL_ID m_TempBuf;

		//	Color White Balance
		MIL_ID		m_IdWBCoefficients		;
		MIL_FLOAT	m_ArrWBCoefficients[3]	;

		BOOL				m_bIsFreePathMIL	;	//	MIL 생성 없이 무조건 처리 완료 리턴
		BOOL				m_bIsCreated		;
		BOOL				m_bIsColor			;
		MILCTRL_COLOR_PTN	m_ColorPtn			;
		int					m_CamWidth			;
		int					m_CamHeight			;
		int					m_CamBpp			;
		int					m_nArrGrabBufSize	;
		int					m_ExposureTime		;	//	노출 시간

		//ngh
		MIL_ID	m_MilGrabBuf[2];
		long m_lGrabHeight;
		int m_nCamErrorType;
	public	:
		//	hWnd : 주요 처리 끝나고 내용을 전달 받을 윈도우 핸들
		//	UserMessage : 전달할 메시지
		//	strDCFName : 로드할 DCF 파일명
		//	GrabbedBufSize : Grab 후 해당 이미지 데이터를 저장할 m_ArrGrabBuf 사이즈
		//	IsColor : TRUE = 컬러모드, FALSE = MONO 모드
		//	WB_R, WB_G, WB_B : 화이트밸런스 RGB 값
		BOOL	Create( HWND MsgRecvhWnd, UINT UserMessage, HWND MainDisplayHwnd, LPCTSTR strDCFName, int GrabbedBufSize, BOOL IsColor, MIL_FLOAT WB_R, MIL_FLOAT WB_G, MIL_FLOAT WB_B, BOOL IsFreePath, int nCamType = 0, BOOL isDoubleBuf = FALSE )	;
		VOID	Release()	;

		//	서브 디스플레이(m_ArrGrabBuf) 윈도우 핸들 설정
		BOOL	SetSubDisplayWindowHandle( HWND hWnd, int SubIdx )	;

		BOOL	SetExposureTime( int ExposureTime )										;	//	노출 시간 설정
		BOOL	GrabSync()																;	//	Grab 동기 실행
		BOOL	GrabASync()																;	//	Grab 비동기 실행 결과는 Create시 넘겨준 핸들로 지정한 메시지와 MILCTRL_RESULT 및 부가 데이터 전달
		BOOL	IsGrabbing()															;	//	현재 Grab 진행 중인지 체크
		BOOL	SaveImageSync( int SubIdx, LPCTSTR SaveFileName, int Format = M_BMP, P_MILCTRL_MASKING_AREA pMaskingArea = NULL )	;	//	SubIdx 값이 -1일 경우 현재 화면, 0보다 크거나 같은 경우 배열에 저장해둔 이미지 저장
		BOOL	SaveImageASync( int SubIdx, LPCTSTR SaveFileName, int Format = M_BMP, P_MILCTRL_MASKING_AREA pMaskingArea = NULL )	;	//	SubIdx 값이 -1일 경우 현재 화면, 0보다 크거나 같은 경우 배열에 저장해둔 이미지 저장
		BOOL	LoadImage( int SubIdx, LPCTSTR SaveFileName, int Format = M_BMP )		;	//	SubIdx 값이 -1일 경우 현재 화면, 0보다 크거나 같은 경우 서브 이미지 배열에 로드
		BOOL	GetGrabBuffer( void** ppBuf )	;
		BOOL	SetGrabToDisplay()	;
		LONG	GetGrabBufPitch()	;
		BOOL	SetGrabToProc( DOUBLE Gain )	;
		BOOL	GetProcBuffer( void** ppBuf )	;
		BOOL	SetProcToDisplay()	;
		LONG	GetProcBufPitch()	;

		WORD	GetDisplayPixelValue( WORD X, WORD Y, BYTE Color )	;	//	0 : Mono, 1 : R, 2 : G, 3 : B

		BOOL	Zoom( MIL_DOUBLE ZoomFactorX, MIL_DOUBLE ZoomFactorY )	;	//	주 디스플레이의 줌 설정
		BOOL	Pan( MIL_DOUBLE PanX, MIL_DOUBLE PanY )					;	//	주 디스플레이의 이동 설정

		//	오버레이 Draw용 함수
		BOOL	ClearOverlay()																				;
		BOOL	DrawOverlayRect( LPCRECT lpcRect, LPCTSTR strText, MIL_DOUBLE Color, BOOL bFillColor, int DotSize = 5 )		;	//	지정된 사각형 사이즈의 사각형과 텍스트 그리기
		BOOL	DrawOverlayCircle( LPCRECT lpcRect, LPCTSTR strText, MIL_DOUBLE Color, BOOL bFillColor, int DotSize = 5 )	;	//	지정된 사각형 사이즈의 원과 텍스트 그리기
		BOOL	DrawOverlayCross( LPCRECT lpcRect, LPCTSTR strText, MIL_DOUBLE Color, int DotSize = 5 )						;	//	지정된 사각형 사이즈의 크로스와 텍스트 그리기

		//	SubIdx : 서브 이미지 버퍼 인덱스, ColorBuffer : 0 = 노멀버퍼, 1 = R버퍼, 2 = G버퍼, 3 = B버퍼
		BOOL	SetMainToSub( int SubIdx, int ColorBuffer = 0 )	;	//	주 화면의 이미지를 서브 이미지 버퍼에 복사
		BOOL	SetSubToMain( int SubIdx, int ColorBuffer = 0 )	;	//	서브 이미지 버퍼의 이미지를 주 화면 이미지 버퍼에 복사
		BOOL	ClearSubBuffer( int SubIdx = -1 )				;	//	-1 : 전체 클리어, 0~ : 해당 인덱스 클리어

		BOOL	SetWhiteBalance( P_MILCTRL_WB pWhiteBalance )	;
		BOOL	GetWhiteBalance( P_MILCTRL_WB pWhiteBalance )	;
		BOOL	GetCalcAutoWB( P_MILCTRL_WB pWhiteBalance )		;

		VOID	SetColorPattern( MILCTRL_COLOR_PTN ColorPattern )	;

		BOOL	GetFocusValue( double *pFocusVal, int *pPixelVal, RECT *pArea = NULL )	;
		BOOL	GetColorPixelInfo( int *pMinData, int *pMaxData, double *pAvrgData, MILCTRL_COLOR_PTN Bayer )	;	//	R, G, B 순

		MIL_ID	AllocMaskingImage( MIL_ID SrcID, P_MILCTRL_MASKING_AREA pMaskingArea )	;

		BOOL	CalcExposureTime( HWND hWndRecvResult, UINT UserMessage, LONG DesAvrgtLumi, LONG LumiMargin, RECT *pROI )	;

		BOOL	IsCreated()			{	return	m_bIsCreated	;	}
		BOOL	IsColor()			{	return	m_bIsColor		;	}
		int		GetCameraWidth()	{	return	m_CamWidth		;	}
		int		GetCameraHeight()	{	return	m_CamHeight		;	}
		int		GetCameraBpp()		{	return	m_CamBpp		;	}
		BOOL	CreateNewOverlay()	;

		/*
		현재 저장되어있는 Grab 이미지를 복사
		AllocNewBuffer가 TRUE일 경우 pImage->m_pData에 새로 버퍼를 할당하여 데이터 복사
		이때 pImage->m_pData가 NULL이 아니면 FALSE 반환
		AllocNewBuffer가 FALSE일 경우 pImage->m_pData에 바로 데이터 복사
		이때 pImage->m_pData가 NULL이면 FALSE 반환
		*/
		BOOL	GetImage(P_MILCTRL_IMGINFO pImage, BOOL AllocNewBuffer = FALSE);

		//20181002 ngh
		BOOL GetGrabThreadStart() { return m_bGrabThreadStart; };
		BOOL GetGrabThreadEnd() { return m_bGrabThreadEnd; };
		BOOL GetGrabStart() { return m_bGrabStart; };
		BOOL GetGrabEnd() { return m_bGrabEnd; };

		void SetGrabThreadStart(BOOL isStart) {	m_bGrabThreadStart = isStart; };
		void SetGrabThreadEnd(BOOL isEnd) { m_bGrabThreadEnd = isEnd; };
		void SetGrabStart(BOOL isStart) { m_bGrabStart = isStart; };
		void SetGrabEnd(BOOL isEnd) { m_bGrabEnd = isEnd; };
		void SetGrabThreadExit(BOOL isExit) { m_bGrabThreExit = isExit; };

		

		BOOL SetLineScanGrab();
		void SetHalDisp(long disp) { m_Disp = disp; };
		BOOL LineScanGrab(long disp);
		void SetGrab();
		void ErrorTypeView(int nErrorType);
		long GetGrabHeight() { return m_lGrabHeight; };
		int GetGrabLineCnt() { return m_nGrabLineCnt; };
		void SetGrabLineCnt(int nCnt) { m_nGrabLineCnt = nCnt; };

		BOOL GetATFStatus();
		BOOL SetATFLaserOn();
		BOOL SetATFLaserOff();
		BOOL SetATFAFOn();
		BOOL SetATFAFOff();
		BOOL GetATFLaserStatus() { return m_cAtcCtrl->GetLaser(); };

		BOOL GetGrabWait() { return m_bGrabWait; };

		CAtfCtrl *m_cAtcCtrl;

		void LoadImage(CString strPath);

		void GrabCopyThread();
		int GetGrabCopy(int idx) { return m_bGrabCopy[idx]; };
		void SetGrabCopy(int idx) { m_bGrabCopy[idx] = FALSE; };
		int GetGrabCopyIdx() { return m_nGrabCopyIdx; };
		void SetGrabCopyIdx(int idx) { m_nGrabCopyIdx = idx; };
		void SetGrabCopyStart(BOOL isStart) { bGrabCopyStart = isStart; };

		void GrabBufCopy() {MbufCopy(m_TempBuf, m_GrabBuf); };
		void GrabBufClear() { MbufClear(m_TempBuf, 0x00); MbufClear(m_GrabBuf, 0x00); };

		CCriticalSection	m_VisionGrab_CS;
protected:
		//20181002 ngh
		BOOL m_bLineScan;
		BOOL m_bDoubleBuf;
		long m_Disp;
		CWinThread *m_pThreadLineScanGrab;
		UINT static LIneScanGrabThread(LPVOID pParam);
		BOOL m_bGrabThreadStart;
		BOOL m_bGrabThreadEnd;
		BOOL m_bGrabThreExit;
		BOOL m_bGrabStart;
		BOOL m_bGrabEnd;
		BOOL m_bGrabWait;
		BOOL InitLineScanGrabThread();
		int m_nGrabLineCnt;
		int m_nGrabCopyIdx;


		BOOL bGrabCopyStart;
		BOOL bGrabCopyEnd;
		CWinThread *m_pThreadGrabCopy;
		UINT static GrabCopyThread(LPVOID pParam);
		BOOL m_bGrabCopy[100];
		

}MILCTRL, *P_MILCTRL	;


#endif