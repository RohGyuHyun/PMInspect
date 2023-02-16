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
	MILCTRL_RESULT_GRAB_SUCCESS,		//	Grab ����
	MILCTRL_RESULT_GRAB_FAIL,			//	Grab ����
	MILCTRL_RESULT_IMAGE_SAVE_SUCCESS,	//	Grab �� �̹��� ���� ����
	MILCTRL_RESULT_IMAGE_SAVE_FAIL,		//	Grab �� �̹��� ���� ����
	MILCTRL_RESULT_AUTO_EXP_SUCCESS,	//	Auto Exposure Time ��� ����
	MILCTRL_RESULT_AUTO_EXP_FAIL		//	Auto Exposure Time ��� ����(Grab ������ ��츸 Fail)
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
	int					m_Width		;	//	���� ������
	int					m_Height	;	//	���� ������
	int					m_Bpp		;	//	Bit per Pixel
	int					m_Channel	;	//	ä��(MONO, BAYER -> 1, RGB, BGR -> 3)
	MILCTRL_COLOR_PTN	m_ColorType	;	//	�÷� Ÿ��
	int					m_DataSize	;	//	��ü ������ ������(����Ʈ ��)
	BYTE*				m_pData		;	//	�̹��� ������ ���� ������
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
		//	System �� ��Ʈ��
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

		BOOL				m_bIsFreePathMIL	;	//	MIL ���� ���� ������ ó�� �Ϸ� ����
		BOOL				m_bIsCreated		;
		BOOL				m_bIsColor			;
		MILCTRL_COLOR_PTN	m_ColorPtn			;
		int					m_CamWidth			;
		int					m_CamHeight			;
		int					m_CamBpp			;
		int					m_nArrGrabBufSize	;
		int					m_ExposureTime		;	//	���� �ð�

		//ngh
		MIL_ID	m_MilGrabBuf[2];
		long m_lGrabHeight;
		int m_nCamErrorType;
	public	:
		//	hWnd : �ֿ� ó�� ������ ������ ���� ���� ������ �ڵ�
		//	UserMessage : ������ �޽���
		//	strDCFName : �ε��� DCF ���ϸ�
		//	GrabbedBufSize : Grab �� �ش� �̹��� �����͸� ������ m_ArrGrabBuf ������
		//	IsColor : TRUE = �÷����, FALSE = MONO ���
		//	WB_R, WB_G, WB_B : ȭ��Ʈ�뷱�� RGB ��
		BOOL	Create( HWND MsgRecvhWnd, UINT UserMessage, HWND MainDisplayHwnd, LPCTSTR strDCFName, int GrabbedBufSize, BOOL IsColor, MIL_FLOAT WB_R, MIL_FLOAT WB_G, MIL_FLOAT WB_B, BOOL IsFreePath, int nCamType = 0, BOOL isDoubleBuf = FALSE )	;
		VOID	Release()	;

		//	���� ���÷���(m_ArrGrabBuf) ������ �ڵ� ����
		BOOL	SetSubDisplayWindowHandle( HWND hWnd, int SubIdx )	;

		BOOL	SetExposureTime( int ExposureTime )										;	//	���� �ð� ����
		BOOL	GrabSync()																;	//	Grab ���� ����
		BOOL	GrabASync()																;	//	Grab �񵿱� ���� ����� Create�� �Ѱ��� �ڵ�� ������ �޽����� MILCTRL_RESULT �� �ΰ� ������ ����
		BOOL	IsGrabbing()															;	//	���� Grab ���� ������ üũ
		BOOL	SaveImageSync( int SubIdx, LPCTSTR SaveFileName, int Format = M_BMP, P_MILCTRL_MASKING_AREA pMaskingArea = NULL )	;	//	SubIdx ���� -1�� ��� ���� ȭ��, 0���� ũ�ų� ���� ��� �迭�� �����ص� �̹��� ����
		BOOL	SaveImageASync( int SubIdx, LPCTSTR SaveFileName, int Format = M_BMP, P_MILCTRL_MASKING_AREA pMaskingArea = NULL )	;	//	SubIdx ���� -1�� ��� ���� ȭ��, 0���� ũ�ų� ���� ��� �迭�� �����ص� �̹��� ����
		BOOL	LoadImage( int SubIdx, LPCTSTR SaveFileName, int Format = M_BMP )		;	//	SubIdx ���� -1�� ��� ���� ȭ��, 0���� ũ�ų� ���� ��� ���� �̹��� �迭�� �ε�
		BOOL	GetGrabBuffer( void** ppBuf )	;
		BOOL	SetGrabToDisplay()	;
		LONG	GetGrabBufPitch()	;
		BOOL	SetGrabToProc( DOUBLE Gain )	;
		BOOL	GetProcBuffer( void** ppBuf )	;
		BOOL	SetProcToDisplay()	;
		LONG	GetProcBufPitch()	;

		WORD	GetDisplayPixelValue( WORD X, WORD Y, BYTE Color )	;	//	0 : Mono, 1 : R, 2 : G, 3 : B

		BOOL	Zoom( MIL_DOUBLE ZoomFactorX, MIL_DOUBLE ZoomFactorY )	;	//	�� ���÷����� �� ����
		BOOL	Pan( MIL_DOUBLE PanX, MIL_DOUBLE PanY )					;	//	�� ���÷����� �̵� ����

		//	�������� Draw�� �Լ�
		BOOL	ClearOverlay()																				;
		BOOL	DrawOverlayRect( LPCRECT lpcRect, LPCTSTR strText, MIL_DOUBLE Color, BOOL bFillColor, int DotSize = 5 )		;	//	������ �簢�� �������� �簢���� �ؽ�Ʈ �׸���
		BOOL	DrawOverlayCircle( LPCRECT lpcRect, LPCTSTR strText, MIL_DOUBLE Color, BOOL bFillColor, int DotSize = 5 )	;	//	������ �簢�� �������� ���� �ؽ�Ʈ �׸���
		BOOL	DrawOverlayCross( LPCRECT lpcRect, LPCTSTR strText, MIL_DOUBLE Color, int DotSize = 5 )						;	//	������ �簢�� �������� ũ�ν��� �ؽ�Ʈ �׸���

		//	SubIdx : ���� �̹��� ���� �ε���, ColorBuffer : 0 = ��ֹ���, 1 = R����, 2 = G����, 3 = B����
		BOOL	SetMainToSub( int SubIdx, int ColorBuffer = 0 )	;	//	�� ȭ���� �̹����� ���� �̹��� ���ۿ� ����
		BOOL	SetSubToMain( int SubIdx, int ColorBuffer = 0 )	;	//	���� �̹��� ������ �̹����� �� ȭ�� �̹��� ���ۿ� ����
		BOOL	ClearSubBuffer( int SubIdx = -1 )				;	//	-1 : ��ü Ŭ����, 0~ : �ش� �ε��� Ŭ����

		BOOL	SetWhiteBalance( P_MILCTRL_WB pWhiteBalance )	;
		BOOL	GetWhiteBalance( P_MILCTRL_WB pWhiteBalance )	;
		BOOL	GetCalcAutoWB( P_MILCTRL_WB pWhiteBalance )		;

		VOID	SetColorPattern( MILCTRL_COLOR_PTN ColorPattern )	;

		BOOL	GetFocusValue( double *pFocusVal, int *pPixelVal, RECT *pArea = NULL )	;
		BOOL	GetColorPixelInfo( int *pMinData, int *pMaxData, double *pAvrgData, MILCTRL_COLOR_PTN Bayer )	;	//	R, G, B ��

		MIL_ID	AllocMaskingImage( MIL_ID SrcID, P_MILCTRL_MASKING_AREA pMaskingArea )	;

		BOOL	CalcExposureTime( HWND hWndRecvResult, UINT UserMessage, LONG DesAvrgtLumi, LONG LumiMargin, RECT *pROI )	;

		BOOL	IsCreated()			{	return	m_bIsCreated	;	}
		BOOL	IsColor()			{	return	m_bIsColor		;	}
		int		GetCameraWidth()	{	return	m_CamWidth		;	}
		int		GetCameraHeight()	{	return	m_CamHeight		;	}
		int		GetCameraBpp()		{	return	m_CamBpp		;	}
		BOOL	CreateNewOverlay()	;

		/*
		���� ����Ǿ��ִ� Grab �̹����� ����
		AllocNewBuffer�� TRUE�� ��� pImage->m_pData�� ���� ���۸� �Ҵ��Ͽ� ������ ����
		�̶� pImage->m_pData�� NULL�� �ƴϸ� FALSE ��ȯ
		AllocNewBuffer�� FALSE�� ��� pImage->m_pData�� �ٷ� ������ ����
		�̶� pImage->m_pData�� NULL�̸� FALSE ��ȯ
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