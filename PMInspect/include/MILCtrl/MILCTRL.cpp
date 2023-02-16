#include "StdAfx.h"
#include "MILCTRL.h"
#include <deque>
#include <math.h>
#include <float.h>


using namespace std;

//#define RGB2GRAY(r, g, b) (0.299*(r) + 0.587*(g) + 0.114*(b))		//	PAL, NTSC - YPbPr	-> 원래 사용 공식
//#define RGB2GRAY(r, g, b) (0.2126*(r) + 0.7152*(g) + 0.0722*(b))	//	ATSC HDTC - YCbCr
#define RGB2GRAY(r, g, b) ((r + g + b)/3)							//	3 Color Average		-> 20171129 LGD 요청 밝기 값 계산 공식

template<typename T>
inline T limit(const T& value)
{
	return ( (value > 255) ? 255 : ((value < 0) ? 0 : value) );
}


void RGBtoGray(long width, long height, long pitch, BYTE* img_source_r, BYTE* img_source_g, BYTE* img_source_b, BYTE* img_dest)
{
	register long i, j;

	long w = width;
	long h = height;
	long idx;

	for(j = 0 ; j < h ; j++)
	{
		for(i = 0 ; i < w ; i++)
		{
			idx = j * pitch + i;

			img_dest[idx] = (BYTE)limit(RGB2GRAY(img_source_r[idx], img_source_g[idx], img_source_b[idx]));
		}
	}

}



int RGBtoGrayF(const long width, const long height, const long dest_pitch, const long src_pitch, const long src_left, const long src_top, 
			   BYTE* img_source_r, BYTE* img_source_g, BYTE* img_source_b, BYTE* img_dest)
{
	register long i, j;

	long idx, idx2;
	long start;

	long long val = 0;
	long long cnt = 0;

	//원본 이미지의 시작점. 
	//열 * 피치 + 행
	start = src_top * src_pitch + src_left;

	for(j = 0 ; j < height ; j++)
	{
		for(i = 0 ; i < width ; i++)
		{
			idx = j * dest_pitch + i;
			idx2 = start + (src_pitch * j + i);

			img_dest[idx] = (BYTE)limit(RGB2GRAY(img_source_r[idx2], img_source_g[idx2], img_source_b[idx2]));
			val = val + img_dest[idx];
			cnt++;
		}
	}

	val = val / cnt;

	return val;
}


double MonotoGrayF(const long width, const long height, const long dest_pitch, const long src_pitch, const long src_left, const long src_top, 
												 BYTE* img_source, BYTE* img_dest)
{
	register long i, j;

	long idx, idx2;
	long start;

	long long val = 0;
	long long cnt = 0;

	//원본 이미지의 시작점. 
	//열 * 피치 + 행
	start = src_top * src_pitch + src_left;

	for(j = 0 ; j < height ; j++)
	{
		for(i = 0 ; i < width ; i++)
		{
			idx = j * dest_pitch + i;
			idx2 = start + (src_pitch * j + i);

			img_dest[idx] = img_source[idx2];
			val = val + img_dest[idx];
			cnt++;
		}
	}

	val = val / cnt;

	return val;
}


//Filter : robert(8bit image only)
double EdgeRoberts(long width, long height, long pitch, BYTE* img_source, BYTE* img_dest, double multi)
{
	double dev = 0;
	register long i, j;

	long w = width;
	long h = height;

	long h1, h2;
	long idx, cnt;
	double max;
	double hval, diff;
	max = 0;
	if(1 > multi)
	{
		multi = 1;
	}

	for(j = 10 ; j < h - 10 ; j++)
	{
		for(i = 10 ; i < w - 10 ; i++)
		{
			idx = j * pitch + i;
			h1 = img_source[idx] - img_source[(j - 10) * pitch + (i - 10)];
			h2 = img_source[idx] - img_source[(j - 10) * pitch + (i + 10)];

			hval = sqrt( (double)h1 * h1 + h2 * h2 );

			//Multi
			hval = hval * multi;

			img_dest[idx] = (BYTE)limit(hval);

			if(max < hval)
			{
				max = hval;
			}
		}
	}

	max = max * 0.8;

	//Average difference
	diff = 0;
	cnt = 0;
	for(j = 1 ; j < h - 1 ; j++)
	{
		for(i = 1 ; i < w - 1 ; i++)
		{
			idx = j * pitch + i;

			if(max < img_dest[idx])
			{
				diff = diff + img_dest[idx];
				cnt++;
			}
		}
	}

	diff = diff / cnt;

	return diff;
}


double EdgeRoberts_D(long width, long height, long pitch, BYTE* img_source, double* img_dest, double multi)
{
	double dev = 0;
	register long i, j;

	long w = width;
	long h = height;

	long h1, h2;
	long idx, cnt;
	double max;
	double hval, diff;
	max = 0;
	if(1 > multi)
	{
		multi = 1;
	}

	for(j = 10 ; j < h - 10 ; j++)
	{
		for(i = 10 ; i < w - 10 ; i++)
		{
			idx = j * pitch + i;
			h1 = img_source[idx] - img_source[(j - 10) * pitch + (i - 10)];
			h2 = img_source[idx] - img_source[(j - 10) * pitch + (i + 10)];

			hval = sqrt( (double)h1 * h1 + h2 * h2 );

			//Multi
			hval = hval * multi;

			img_dest[idx] = hval;

			if(max < hval)
			{
				max = hval;
			}
		}
	}

	max = max * 0.8;

	

	//Average difference
	diff = 0;
	cnt = 0;
	for(j = 1 ; j < h - 1 ; j++)
	{
		for(i = 1 ; i < w - 1 ; i++)
		{
			idx = j * pitch + i;

			if(max < img_dest[idx])
			{
				diff = diff + img_dest[idx];
				cnt++;
			}
		}
	}

	diff = diff / cnt;

	return diff;
}



























#define	MILCTRL_IF_FAILED(x)					(x)	;	\
												if( MappGetError(M_CURRENT+M_THREAD_CURRENT, M_NULL) != M_NULL_ERROR )	\

#define	MILCTRL_BUFFER_FREE_IF_NOT_NULL(x)		if( x != M_NULL )	\
												{	\
													MbufFree(x)	;	\
													x = M_NULL	;	\
												}


//	ImageData 저장 정보
typedef	struct	MILCTRL_SAVEIMAGEDATA
{
	LPTSTR					m_pstrFileName	;
	int						m_SubIdx		;
	int						m_Format		;
	P_MILCTRL_MASKING_AREA	m_pMaskingArea	;
}MILCTRL_SAVEIMAGEDATA, *P_MILCTRL_SAVEIMAGEDATA	;

static	CCriticalSection	g_MILCTRL_CS							;	//	크리티컬 섹션
static	BOOL				g_MILCTRL_IsGrabThreadRunning			;	//	그랩 쓰레드 활성화 상태
static	BOOL				g_MILCTRL_IsSaveFileThreadRunning		;	//	파일저장 쓰레드 활성화 상태
static	BOOL				g_MILCTRL_SaveFileThreadForceTerminate	;	//	파일저장 쓰레드 강제 종료

static	deque<MILCTRL_SAVEIMAGEDATA>	g_MILCTRL_qSaveImage	;

//	Window Handle 및 User Define Message
static	HWND	g_MILCTRL_hWndRecvMsg		;
static	UINT	g_MILCTRL_UserMessage		;


//	자동 노출시간 계산용 데이터 정보
typedef	struct	MILCTRL_AUTOEXPDATA
{
	HWND	m_hWndRecvResult	;
	UINT	m_UserMessage		;
	LONG	m_DestLumi			;
	LONG	m_LumiMargin		;
	RECT	m_ROI				;
}MILCTRL_AUTOEXPDATA, *P_MILCTRL_AUTOEXPDATA	;

static	MILCTRL_AUTOEXPDATA	g_MILCTRL_AutoExpData			;
static	BOOL				g_MILCTRL_IsCalcAutoExpTimeNow	;

static	UINT	MILCTRL_GRAB_THREAD( LPVOID pObj )
{
	while( g_MILCTRL_IsCalcAutoExpTimeNow )
	{
		Sleep(1)	;
	}
	g_MILCTRL_CS.Lock()							;
	g_MILCTRL_IsGrabThreadRunning	=	TRUE	;
	g_MILCTRL_CS.Unlock()						;

	if( g_MilCtrl.GrabSync() )
	{
		PostMessage( g_MILCTRL_hWndRecvMsg, g_MILCTRL_UserMessage, MILCTRL_RESULT_GRAB_SUCCESS, 0 )	;
	}
	else
	{
		PostMessage( g_MILCTRL_hWndRecvMsg, g_MILCTRL_UserMessage, MILCTRL_RESULT_GRAB_FAIL, 0 )	;
	}

	g_MILCTRL_CS.Lock()							;
	g_MILCTRL_IsGrabThreadRunning	=	FALSE	;
	g_MILCTRL_CS.Unlock()						;

	return	0	;
}

static	UINT	MILCTRL_AUTOEXP_THREAD( LPVOID pObj )
{
	long	BeforExpTime, CurExpTime, BeforLumi, CurLumi	;
	double	FocusVal	;

	long	DiffExpTime, DiffLumi, BalanceTime, absBeforDiffLumi, absCurDiffLumi	;
	double	ExpTimePerLumi	;
	UINT	ResultMsg, ResultExpTime	;


	while( g_MILCTRL_IsGrabThreadRunning )
	{
		Sleep(1)	;
	}
	g_MILCTRL_CS.Lock()							;
	g_MILCTRL_IsCalcAutoExpTimeNow	=	TRUE	;


	BeforExpTime	=	BeforLumi	=	0	;
	CurExpTime		=	BalanceTime	=	10	;

	while( TRUE )
	{
		g_MilCtrl.SetExposureTime( CurExpTime )	;
		if( g_MilCtrl.GrabSync() )
		{
			if( !g_MilCtrl.GetFocusValue(&FocusVal, (int*)&CurLumi, &g_MILCTRL_AutoExpData.m_ROI) )
			{
				ResultMsg		=	MILCTRL_RESULT_AUTO_EXP_FAIL	;
				ResultExpTime	=	0								;
				break	;
			}

			if( CurLumi == g_MILCTRL_AutoExpData.m_DestLumi )	//	밝기값이 목표치와 같으면 성공 리턴
			{
				ResultMsg		=	MILCTRL_RESULT_AUTO_EXP_SUCCESS	;
				ResultExpTime	=	CurExpTime						;
				break	;
			}
			else
			{
				//	1ms 노출차이로 밝기값 맞추지 못할 경우
				if( g_MILCTRL_AutoExpData.m_DestLumi > BeforLumi && g_MILCTRL_AutoExpData.m_DestLumi < CurLumi && abs(CurExpTime - BeforExpTime) <= 1 )
				{
					//	두 밝기 값 중 목표치에 가장 근접한 밝기값에 대하여 마진 이내일 경우 성공, 마진 초과할 경우 실패
					//	두 밝기 값의 목표치에 대한 차가 같은 경우 노출시간 적은 쪽으로 선택
					absBeforDiffLumi	=	abs( g_MILCTRL_AutoExpData.m_DestLumi - BeforLumi )	;
					absCurDiffLumi		=	abs( g_MILCTRL_AutoExpData.m_DestLumi - CurLumi )	;
					if( absBeforDiffLumi > absCurDiffLumi )
					{
						if( absCurDiffLumi <= g_MILCTRL_AutoExpData.m_LumiMargin )
						{
							ResultMsg		=	MILCTRL_RESULT_AUTO_EXP_SUCCESS	;
							ResultExpTime	=	CurExpTime						;
						}
						else
						{
							ResultMsg		=	MILCTRL_RESULT_AUTO_EXP_FAIL	;
							ResultExpTime	=	0								;
						}
						break	;
					}
					else
					{
						if( absBeforDiffLumi <= g_MILCTRL_AutoExpData.m_LumiMargin )
						{
							ResultMsg		=	MILCTRL_RESULT_AUTO_EXP_SUCCESS	;
							ResultExpTime	=	BeforExpTime					;
						}
						else
						{
							ResultMsg		=	MILCTRL_RESULT_AUTO_EXP_FAIL	;
							ResultExpTime	=	0								;
						}
						break	;
					}
				}
				else
				{
					DiffExpTime		=	CurExpTime	-	BeforExpTime	;
					DiffLumi		=	CurLumi		-	BeforLumi		;

					if( DiffLumi == 0 )
					{
						BalanceTime	+=	BalanceTime	;	//	기존 조절 시간 * 2
						CurExpTime	+=	BalanceTime	;

						//	조절시간 적용 하여도 밝기값 변화가 없을 경우 2배로 늘린 조절시간을 적용 시 밝기값이 최소 1이 늘어날 것이라고 가정 하고
						//	현재 노출 시간 + (목표 밝기값까지 남은 밝기값 * 조절 시간) 의 값이 4999ms 이상이거나 0 이하일 경우 실패 처리
						if( CurExpTime + ((g_MILCTRL_AutoExpData.m_DestLumi - CurLumi) * BalanceTime) >= 4999 ||
							CurExpTime + ((g_MILCTRL_AutoExpData.m_DestLumi - CurLumi) * BalanceTime) <= 0)
						{
							ResultMsg		=	MILCTRL_RESULT_AUTO_EXP_FAIL	;
							ResultExpTime	=	0								;
							break	;
						}
					}
					else
					{
						ExpTimePerLumi	=	(double)((double)DiffExpTime / (double)DiffLumi)	;
						BalanceTime	=	(long)((double)(g_MILCTRL_AutoExpData.m_DestLumi - CurLumi) * (double)((double)DiffExpTime / (double)DiffLumi))	;
						if( BalanceTime == 0 )
						{
							if( DiffLumi < 0 )
								BalanceTime	=	-1	;
							else
								BalanceTime	=	1	;
						}

						BeforExpTime	=	CurExpTime	;
						BeforLumi		=	CurLumi		;
						CurExpTime		+=	BalanceTime	;
					}

					if( CurExpTime <= 0 || CurExpTime >= 4999 )	//	밸런스 타임 합친 값이 0보다 작거나 4999ms보다 크면 실패)
					{
						ResultMsg		=	MILCTRL_RESULT_AUTO_EXP_FAIL	;
						ResultExpTime	=	0								;
						break	;
					}
				}
			}
		}
		else
		{
			ResultMsg		=	MILCTRL_RESULT_AUTO_EXP_FAIL	;
			ResultExpTime	=	0								;
			break	;
		}
	}

	PostMessage( g_MILCTRL_AutoExpData.m_hWndRecvResult, g_MILCTRL_AutoExpData.m_UserMessage, ResultMsg, ResultExpTime )	;
	g_MILCTRL_IsCalcAutoExpTimeNow	=	FALSE	;
	g_MILCTRL_CS.Unlock()						;

	return	0	;
}

static	UINT	MILCTRL_SAVEIMAGE_THREAD( LPVOID pObj )
{
	MILCTRL_SAVEIMAGEDATA	SaveImageData	;

	g_MILCTRL_CS.Lock()								;
	g_MILCTRL_IsSaveFileThreadRunning	=	TRUE	;
	g_MILCTRL_CS.Unlock()							;


	while( !g_MILCTRL_SaveFileThreadForceTerminate )
	{
		g_MILCTRL_CS.Lock()	;
		if( g_MILCTRL_qSaveImage.size() > 0 )
		{
			SaveImageData	=	g_MILCTRL_qSaveImage[0]	;
			g_MILCTRL_qSaveImage.pop_front()			;
			g_MILCTRL_CS.Unlock()						;

			if( g_MilCtrl.SaveImageSync(SaveImageData.m_SubIdx, SaveImageData.m_pstrFileName, SaveImageData.m_Format, SaveImageData.m_pMaskingArea) )
			{
				PostMessage( g_MILCTRL_hWndRecvMsg, g_MILCTRL_UserMessage, MILCTRL_RESULT_IMAGE_SAVE_SUCCESS, SaveImageData.m_SubIdx )	;
			}
			else
			{
				PostMessage( g_MILCTRL_hWndRecvMsg, g_MILCTRL_UserMessage, MILCTRL_RESULT_IMAGE_SAVE_FAIL, SaveImageData.m_SubIdx )		;
			}

			delete[]	SaveImageData.m_pstrFileName	;
			if( SaveImageData.m_pMaskingArea != NULL )
				delete		SaveImageData.m_pMaskingArea	;
		}
		else
		{
			g_MILCTRL_CS.Unlock()	;
			break	;
		}
	}

	g_MILCTRL_CS.Lock()								;
	g_MILCTRL_IsSaveFileThreadRunning	=	FALSE	;
	g_MILCTRL_CS.Unlock()							;

	return	0	;
}

MILCTRL	MILCTRL::m_Instance	;

MILCTRL::MILCTRL(void)
{
	m_App					=	M_NULL	;
	m_Sys					=	M_NULL	;
	m_Dig					=	M_NULL	;
	m_MainDisp				=	M_NULL	;

	m_ArrSubDisp			=	NULL	;

	m_GrabBuf				=	M_NULL	;
	m_ProcBuf				=	M_NULL	;
	m_DisplayBuf			=	M_NULL	;
	m_DispBufR				=	M_NULL	;
	m_DispBufG				=	M_NULL	;
	m_DispBufB				=	M_NULL	;
	m_OverlayBuf			=	M_NULL	;

	m_ArrSubGrabBuf			=	NULL	;
	m_ArrSubGrabBufR		=	NULL	;
	m_ArrSubGrabBufG		=	NULL	;
	m_ArrSubGrabBufB		=	NULL	;

	m_IdWBCoefficients		=	M_NULL	;

	m_bIsFreePathMIL		=	FALSE					;
	m_bIsCreated			=	FALSE					;
	m_bIsColor				=	FALSE					;
	m_ColorPtn				=	MILCTRL_COLOR_PTN_GB	;
	m_CamWidth				=	0						;
	m_CamHeight				=	0						;
	m_nArrGrabBufSize		=	0						;
	m_ExposureTime			=	0						;

	g_MILCTRL_hWndRecvMsg					=	NULL	;
	g_MILCTRL_IsGrabThreadRunning			=	FALSE	;
	g_MILCTRL_IsSaveFileThreadRunning		=	FALSE	;
	g_MILCTRL_SaveFileThreadForceTerminate	=	TRUE	;
	g_MILCTRL_IsCalcAutoExpTimeNow			=	FALSE	;
	ZeroMemory( &g_MILCTRL_AutoExpData, sizeof(MILCTRL_AUTOEXPDATA) )	;

	m_bLineScan = FALSE;
	m_bDoubleBuf = FALSE;
	m_bGrabThreExit = FALSE;
	m_bGrabStart = FALSE;
	m_bGrabEnd = TRUE;
}

MILCTRL::~MILCTRL(void)
{
	Release()	;
}

BOOL	MILCTRL::Create( HWND MsgRecvhWnd, UINT UserMessage, HWND MainDisplayHwnd, LPCTSTR strDCFName, int GrabbedBufSize, BOOL IsColor, MIL_FLOAT WB_R, MIL_FLOAT WB_G, MIL_FLOAT WB_B, BOOL IsFreePath, int nCamType, BOOL isDoubleBuf )
{
	//	이미 생성되어있으면 FALSE 리턴
	if( m_bIsFreePathMIL || m_bIsCreated )
		return	FALSE	;

	//	윈도우 핸들 또는 DCF 파일명이 유효하지 않으면 FALSE 리턴
	/*if( MsgRecvhWnd == NULL || strDCFName == NULL )
		return	FALSE	;
*/
	//	SubBuffer Size가 1개 미만일 경우 FALSE 리턴
	if( GrabbedBufSize < 1 )
		return	FALSE	;

	//	IsFreePath로 생성할 경우 다른 모든 생성처리는 스킵
	if( IsFreePath )
	{
		m_bIsFreePathMIL	=	TRUE	;
		m_bIsCreated		=	TRUE	;
		return	TRUE	;
	}

	MIL_INT	Width, Height, Bpp, FullHeight	;
	MIL_INT	BitsPerPixel	;

	if( MappAlloc(M_DEFAULT, &m_App) == M_NULL )											//	Application 할당
		return	FALSE	;

	MILCTRL_IF_FAILED( MappControl(M_ERROR, M_PRINT_DISABLE) )								//	에러메시지 대화상자 출력 비활성화
	{
		Release()		;
		return	FALSE	;
	}

#ifndef NOTE_BOOK
	if( MsysAlloc(M_SYSTEM_RADIENTEVCL, 0, M_DEFAULT, &m_Sys) == M_NULL )						//	System 할당
	{
		Release()		;
		return	FALSE	;
	}
#else
	if (MsysAlloc(M_SYSTEM_HOST, 0, M_DEFAULT, &m_Sys) == M_NULL)						//	System 할당
	{
		Release();
		return	FALSE;
	}
#endif

#ifndef NOTE_BOOK
	if (MdigAlloc(m_Sys, 0, strDCFName, M_DEFAULT, &m_Dig) == M_NULL )						//	Digitizer 할당
	{
		Release()		;
		return	FALSE	;
	}

	MILCTRL_IF_FAILED( MdigControl(m_Dig, M_GRAB_MODE, M_ASYNCHRONOUS) )	//	Grab 모드 비동기 설정
	{
		Release()		;
		return	FALSE	;
	}
#endif
	m_nCamErrorType = 0;
	if (nCamType == 1)
	{
		m_bLineScan = TRUE;
#ifndef NOTE_BOOK
		MdigControl(m_Dig, M_GRAB_TIMEOUT, GRAB_TIME_OUT_MS);
		MdigControl(m_Dig, M_GRAB_TRIGGER, M_ACTIVATE);
		MdigControl(m_Dig, M_GRAB_TRIGGER_SOURCE, 37L);
#endif
		m_cAtcCtrl = NULL;
		m_cAtcCtrl = new CAtfCtrl();
	}

#ifndef NOTE_BOOK
	MdigInquire( m_Dig, M_SIZE_X, &Width )	;	//	Camera Resolution Width in DCF
	MdigInquire( m_Dig, M_SIZE_Y, &Height )	;	//	Camera Resolution Height in DCF
	MdigInquire( m_Dig, M_SIZE_BIT, &Bpp )	;	//	Camera Resolution Bpp in DCF
	m_lGrabHeight = Height;
#else
	Width = MAX_GRAB_WIDTH;
	Height = 1000;
	m_lGrabHeight = Height;
	Bpp = 8;
	FullHeight = MAX_GRAB_HEIGHT;
#endif
	if (nCamType == 1)
	{
		if (isDoubleBuf)
		{
			m_bDoubleBuf = TRUE;
			FullHeight = MAX_GRAB_HEIGHT;
		}
		else
		{
			FullHeight = Height;
		}
	}	
	else
	{
#ifndef NOTE_BOOK
		MILCTRL_IF_FAILED(MdigControl(m_Dig, M_TIMER_TRIGGER_SOURCE, M_SOFTWARE))	//	노출 시간 소프트웨어 방식 설정
		{
			Release();
			return	FALSE;
		}
#endif
	}

	if( Bpp <= 8 )
		BitsPerPixel	=	8L	;
	else if( Bpp <= 16 )
		BitsPerPixel	=	16L	;
	else
	{
		Release()		;
		return	FALSE	;
	}


	//	그랩 버퍼 할당
	if( MbufAllocColor(m_Sys, 1, Width, FullHeight, BitsPerPixel+M_UNSIGNED,M_IMAGE+M_GRAB+M_PLANAR, &m_GrabBuf) == M_NULL )
	{
		Release()		;
		return	FALSE	;
	}
	MbufClear(m_GrabBuf, 0x00);
	if (nCamType == 1)
	{
		for (int i = 0; i < 2; i++)
		{
			if (MbufAllocColor(m_Sys, 1, Width, Height, BitsPerPixel + M_UNSIGNED, M_IMAGE + M_GRAB, &m_MilGrabBuf[i]) == M_NULL)
			{
				Release();
				return	FALSE;
			}
		}

		if (MbufAllocColor(m_Sys, 1, Width, FullHeight, BitsPerPixel + M_UNSIGNED, M_IMAGE + M_GRAB + M_PLANAR, &m_TempBuf) == M_NULL)
		{
			Release();
			return	FALSE;
		}
	}

	//	프로세싱 버퍼 할당(8bit 고정)
	if( MbufAllocColor(m_Sys, 1, Width, Height, 8L+M_UNSIGNED,M_IMAGE+M_DISP+M_PLANAR, &m_ProcBuf) == M_NULL )
	{
		Release()		;
		return	FALSE	;
	}
	MbufClear( m_ProcBuf, 0x00 )						;


	//	메인 디스플레이 버퍼 할당
	m_bIsColor	=	IsColor	;
	if (MainDisplayHwnd != NULL)
	{
		if (m_bIsColor)
		{
			if (MbufAllocColor(m_Sys, 3, Width, Height, 8L + M_UNSIGNED, M_IMAGE + M_DISP + M_PLANAR, &m_DisplayBuf) == M_NULL)
			{
				Release();
				return	FALSE;
			}

			if (MbufChildColor(m_DisplayBuf, M_RED, &m_DispBufR) == M_NULL ||
				MbufChildColor(m_DisplayBuf, M_GREEN, &m_DispBufG) == M_NULL ||
				MbufChildColor(m_DisplayBuf, M_BLUE, &m_DispBufB) == M_NULL)
			{
				Release();
				return	FALSE;
			}
		}
		else
		{
			if (MbufAllocColor(m_Sys, 1, Width, Height, 8L + M_UNSIGNED, M_IMAGE + M_DISP + M_PLANAR, &m_DisplayBuf) == M_NULL)
			{
				Release();
				return	FALSE;
			}
		}

		MbufClear(m_DisplayBuf, 0x00);
	}

	//	서브 디스플레이와 서브 그랩 버퍼 할당
	m_nArrGrabBufSize	=	GrabbedBufSize	;
	if (MainDisplayHwnd != NULL)
	{
		if (m_nArrGrabBufSize > 0)
		{
			m_ArrSubDisp = new	MIL_ID[m_nArrGrabBufSize];
			m_ArrSubGrabBuf = new	MIL_ID[m_nArrGrabBufSize];

			if (m_bIsColor)
			{
				m_ArrSubGrabBufR = new	MIL_ID[m_nArrGrabBufSize];
				m_ArrSubGrabBufG = new	MIL_ID[m_nArrGrabBufSize];
				m_ArrSubGrabBufB = new	MIL_ID[m_nArrGrabBufSize];
			}

			for (int i = 0; i < GrabbedBufSize; i++)
			{
				if (m_bIsColor)
				{
					if (MbufAllocColor(m_Sys, 3, Width, Height, BitsPerPixel + M_UNSIGNED, M_IMAGE + M_DISP + M_PLANAR, &m_ArrSubGrabBuf[i]) == M_NULL)
					{
						Release();
						return	FALSE;
					}

					if (MbufChildColor(m_ArrSubGrabBuf[i], M_RED, &m_ArrSubGrabBufR[i]) == M_NULL ||
						MbufChildColor(m_ArrSubGrabBuf[i], M_GREEN, &m_ArrSubGrabBufG[i]) == M_NULL ||
						MbufChildColor(m_ArrSubGrabBuf[i], M_BLUE, &m_ArrSubGrabBufB[i]) == M_NULL)
					{
						Release();
						return	FALSE;
					}

					MbufClear(m_ArrSubGrabBuf[i], 0x00);
				}
				else
				{
					if (MbufAlloc2d(m_Sys, Width, Height, BitsPerPixel + M_UNSIGNED, M_IMAGE + M_DISP + M_PLANAR, &m_ArrSubGrabBuf[i]) == M_NULL)
					{
						Release();
						return	FALSE;
					}

					MbufClear(m_ArrSubGrabBuf[i], 0x00);
				}

				if (MdispAlloc(m_Sys, M_DEFAULT, _T("M_DEFAULT"), M_WINDOWED, &m_ArrSubDisp[i]) == M_NULL)
				{
					Release();
					return	FALSE;
				}
			}
		}
	}
	if (nCamType != 1)
	{
		if (MainDisplayHwnd != NULL)
		{
			MILCTRL_IF_FAILED(MdispControl(m_MainDisp, M_OVERLAY, M_ENABLE))	//	오버레이 활성화
			{
				Release();
				return	FALSE;
			}

			MILCTRL_IF_FAILED(MdispSelectWindow(m_MainDisp, m_DisplayBuf, MainDisplayHwnd))
			{
				Release();
				return	FALSE;
			}

			if (MdispInquire(m_MainDisp, M_OVERLAY_ID, &m_OverlayBuf) == M_NULL)
			{
				Release();
				return	FALSE;
			}

			MILCTRL_IF_FAILED(MdispControl(m_MainDisp, M_INTERPOLATION_MODE, M_FAST))
			{
				Release();
				return	FALSE;
			}
		}
#ifndef NOTE_BOOK
		MILCTRL_IF_FAILED(MdigControl(m_Dig, M_TIMER_OUTPUT_INVERTER, M_LEVEL_HIGH))
		{
			Release();
			return	FALSE;
		}
#endif
		MILCTRL_IF_FAILED(MdigControl(m_Dig, M_GRAB_TIMEOUT, 5000))
		{
			Release();
			return	FALSE;
		}
	}

	//	화이트 밸런스 설정
	if (m_bIsColor)
	{
		if (MbufAlloc1d(m_Sys, 3, 32 + M_FLOAT, M_ARRAY, &m_IdWBCoefficients) == M_NULL)
		{
			Release();
			return	FALSE;
		}

		m_ArrWBCoefficients[0] = WB_R;
		m_ArrWBCoefficients[1] = WB_G;
		m_ArrWBCoefficients[2] = WB_B;

		MILCTRL_IF_FAILED(MbufPut(m_IdWBCoefficients, (void *)m_ArrWBCoefficients))
		{
			Release();
			return	FALSE;
		}
	}

	if (nCamType == 1)
	{
		if (!InitLineScanGrabThread())
		{
			Release();
			return	FALSE;
		}
	}

	m_CamWidth				=	(int)Width		;
	m_CamHeight				=	(int)FullHeight;
	m_CamBpp				=	(int)Bpp		;

	g_MILCTRL_hWndRecvMsg	=	MsgRecvhWnd		;
	g_MILCTRL_UserMessage	=	UserMessage		;

	m_bIsCreated			=	TRUE			;

	return	TRUE	;
}

VOID	MILCTRL::Release()
{
	//20181002 ngh
	if (m_bLineScan)
	{
		m_bGrabThreadStart = FALSE;
		m_bGrabThreadEnd = TRUE;

		Sleep(100);
		if (m_bGrabThreExit)
		{
			m_pThreadLineScanGrab = NULL;
		}

		if (m_cAtcCtrl != NULL)
		{
			delete m_cAtcCtrl;
			m_cAtcCtrl = NULL;
		}
	}

	//	쓰레드 종료
	g_MILCTRL_CS.Lock()									;
	g_MILCTRL_SaveFileThreadForceTerminate	=	TRUE	;
	g_MILCTRL_CS.Unlock();
		
	bGrabCopyStart = FALSE;
	bGrabCopyEnd = TRUE;
	if (m_VisionGrab_CS.m_sect.LockCount > -1)
	{
		for (int z = 0; z < (m_VisionGrab_CS.m_sect.LockCount + 1); z++)
		{
			m_VisionGrab_CS.Unlock();
		}
	}

	while( g_MILCTRL_IsSaveFileThreadRunning )
	{
		Sleep(1)	;
	}

	//	쓰레드로 저장하려던 이미지 정보가 있으면 삭제
	for( int i = 0 ; i < g_MILCTRL_qSaveImage.size() ; i++ )
	{
		delete[]	g_MILCTRL_qSaveImage[i].m_pstrFileName	;
		if( g_MILCTRL_qSaveImage[i].m_pMaskingArea != NULL )
			delete		g_MILCTRL_qSaveImage[i].m_pMaskingArea	;
	}

	g_MILCTRL_qSaveImage.clear()	;


	//	MIL 자원 할당 해제
	MILCTRL_BUFFER_FREE_IF_NOT_NULL( m_IdWBCoefficients )	;

	if( m_ArrSubGrabBufR != NULL )
	{
		for( int i = 0 ; i < m_nArrGrabBufSize ; i++ )
		{
			MILCTRL_BUFFER_FREE_IF_NOT_NULL( m_ArrSubGrabBufR[i] )	;
			MILCTRL_BUFFER_FREE_IF_NOT_NULL( m_ArrSubGrabBufG[i] )	;
			MILCTRL_BUFFER_FREE_IF_NOT_NULL( m_ArrSubGrabBufB[i] )	;
		}

		delete[]	m_ArrSubGrabBufR	;
		delete[]	m_ArrSubGrabBufG	;
		delete[]	m_ArrSubGrabBufB	;

		m_ArrSubGrabBufR	=	NULL	;
		m_ArrSubGrabBufG	=	NULL	;
		m_ArrSubGrabBufB	=	NULL	;
	}

	MILCTRL_BUFFER_FREE_IF_NOT_NULL( m_DispBufR )	;
	MILCTRL_BUFFER_FREE_IF_NOT_NULL( m_DispBufG )	;
	MILCTRL_BUFFER_FREE_IF_NOT_NULL( m_DispBufB )	;

	if( m_ArrSubGrabBuf != NULL )
	{
		for( int i = 0 ; i < m_nArrGrabBufSize ; i++ )
		{
			MILCTRL_BUFFER_FREE_IF_NOT_NULL( m_ArrSubGrabBuf[i] )	;
			MdispFree( m_ArrSubDisp[i] )	; 
		}

		delete[]	m_ArrSubGrabBuf	;
		delete[]	m_ArrSubDisp	;

		m_ArrSubGrabBuf	=	NULL	;
		m_ArrSubDisp	=	NULL	;
	}

	MILCTRL_BUFFER_FREE_IF_NOT_NULL( m_DisplayBuf )	;
	MILCTRL_BUFFER_FREE_IF_NOT_NULL( m_ProcBuf )	;
	MILCTRL_BUFFER_FREE_IF_NOT_NULL( m_GrabBuf )	;
	if (m_bLineScan && m_bDoubleBuf)
	{
		MILCTRL_BUFFER_FREE_IF_NOT_NULL(m_MilGrabBuf[0]);
		MILCTRL_BUFFER_FREE_IF_NOT_NULL(m_MilGrabBuf[1]);
		MILCTRL_BUFFER_FREE_IF_NOT_NULL(m_TempBuf);
	}

	if( m_MainDisp != M_NULL )
	{
		MdispFree( m_MainDisp )	;
		m_MainDisp	=	NULL	;
	}

	if( m_Dig != M_NULL )
	{
		MdigFree( m_Dig )	;
		m_Dig	=	NULL	;
	}

	if( m_Sys != M_NULL )
	{
		MsysFree( m_Sys )	;
		m_Sys	=	NULL	;
	}

	if( m_App != M_NULL )
	{
		MappFree( m_App )	;
		m_App	=	NULL	;
	}

	m_bIsFreePathMIL		=	FALSE	;
	m_bIsCreated			=	FALSE	;
	m_bIsColor				=	FALSE	;
	m_CamWidth				=	0		;
	m_CamHeight				=	0		;
	m_nArrGrabBufSize		=	0		;
	m_ExposureTime			=	0		;
	g_MILCTRL_hWndRecvMsg	=	NULL	;
}

BOOL	MILCTRL::SetSubDisplayWindowHandle( HWND hWnd, int SubIdx )
{
	if( !m_bIsCreated || hWnd == NULL || SubIdx < 0 || SubIdx >= m_nArrGrabBufSize )
		return	FALSE	;

	if( m_bIsFreePathMIL )
		return	TRUE	;

	g_MILCTRL_CS.Lock()	;
	MILCTRL_IF_FAILED( MdispSelectWindow(m_ArrSubDisp[SubIdx], m_ArrSubGrabBuf[SubIdx], hWnd) )
	{
		g_MILCTRL_CS.Unlock()	;
		return	FALSE			;
	}

	MIL_DOUBLE	ZoomFactorX, ZoomFactorY	;
	CRect	rect	;

	if( !GetClientRect(hWnd, &rect) )
	{
		g_MILCTRL_CS.Unlock()	;
		return	FALSE			;
	}

	ZoomFactorX	=	ZoomFactorY	=	(MIL_DOUBLE)rect.Width() / (MIL_DOUBLE)m_CamWidth	;

	MILCTRL_IF_FAILED( MdispZoom(m_ArrSubDisp[SubIdx], ZoomFactorX, ZoomFactorY) )
	{
		g_MILCTRL_CS.Unlock()	;
		return	FALSE			;
	}

	g_MILCTRL_CS.Unlock()	;
	return	TRUE			;
}

BOOL	MILCTRL::SetExposureTime( int ExposureTime )
{
	if( !m_bIsCreated || ExposureTime < 0 )
		return	FALSE	;

	if( m_bIsFreePathMIL )
		return	TRUE	;

	MIL_DOUBLE	exp	;

	exp	=	(ExposureTime * 1000000.)	;	//	ExposureTime은 ms단위이므로 ns 단위로 변경

	g_MILCTRL_CS.Lock()	;
#ifndef LINE_SCAN
	MILCTRL_IF_FAILED( MdigControl(m_Dig, M_GRAB_EXPOSURE_TIME + M_TIMER1, exp) )
	{
		g_MILCTRL_CS.Unlock()	;
		return	FALSE			;
	}
#endif
	m_ExposureTime	=	ExposureTime	;
	g_MILCTRL_CS.Unlock()	;
	return	TRUE			;
}

BOOL	MILCTRL::GrabSync()
{
	if( !m_bIsCreated )
		return	FALSE	;

	if( m_bIsFreePathMIL )
		return	TRUE	;

	g_MILCTRL_CS.Lock()	;


	MILCTRL_IF_FAILED( MdigGrab(m_Dig, m_GrabBuf) )
	{
		g_MILCTRL_CS.Unlock()	;
		return	FALSE			;
	}
#ifndef NOTE_BOOK
	MILCTRL_IF_FAILED( MdigControl(m_Dig, M_TIMER_STATE + M_TIMER1, M_ACTIVATE) )
	{
		g_MILCTRL_CS.Unlock()	;
		return	FALSE			;
	}
#endif
	MILCTRL_IF_FAILED( MdigGrabWait(m_Dig, M_GRAB_END) )
	{
		g_MILCTRL_CS.Unlock()	;
		return	FALSE			;
	}

	MdigControl(m_Dig, M_GRAB_ABORT, M_DEFAULT);
	g_MILCTRL_CS.Unlock()	;
	m_bGrabEnd = TRUE;
	return	TRUE			;
}

BOOL	MILCTRL::GrabASync()
{
	if( !m_bIsCreated )
		return	FALSE	;

	if( m_bIsFreePathMIL )
		return	TRUE	;

	CWinThread	*pThread	=	AfxBeginThread(MILCTRL_GRAB_THREAD, NULL, THREAD_PRIORITY_NORMAL);
	if( pThread == NULL )
		return	FALSE	;

	pThread->m_bAutoDelete			=	TRUE	;

	return	TRUE	;
}

BOOL	MILCTRL::IsGrabbing()
{
	if( !m_bIsCreated )
		return	FALSE	;

	if( m_bIsFreePathMIL )
		return	TRUE	;

	return	g_MILCTRL_IsGrabThreadRunning	;
}

BOOL	MILCTRL::SaveImageSync( int SubIdx, LPCTSTR SaveFileName, int Format /*M_BMP*/, P_MILCTRL_MASKING_AREA pMaskingArea /*NULL*/ )
{
	if( !m_bIsCreated || SubIdx >= m_nArrGrabBufSize || SaveFileName == NULL)
		return	FALSE	;

	if( m_bIsFreePathMIL )
		return	TRUE	;

	g_MILCTRL_CS.Lock()	;
	if( SubIdx == -2 )
	{
		if( pMaskingArea == NULL )
		{
			MILCTRL_IF_FAILED( MbufExport(SaveFileName, Format, m_GrabBuf) )
			{
				g_MILCTRL_CS.Unlock()	;
				return	FALSE			;
			}
		}
		else
		{
			MIL_ID	MaskedImage	=	AllocMaskingImage( m_GrabBuf, pMaskingArea )	;
			if( MaskedImage == M_NULL )
			{
				g_MILCTRL_CS.Unlock()	;
				return	FALSE			;
			}
			else
			{
				MILCTRL_IF_FAILED( MbufExport(SaveFileName, Format, MaskedImage) )
				{
					g_MILCTRL_CS.Unlock()	;
					return	FALSE			;
				}
			}
			MbufFree( MaskedImage )	;
		}
	}
	else if( SubIdx == -1 )
	{
		if( pMaskingArea == NULL )
		{
			MILCTRL_IF_FAILED( MbufExport(SaveFileName, Format, m_DisplayBuf) )
			{
				g_MILCTRL_CS.Unlock()	;
				return	FALSE			;
			}
		}
		else
		{
			MIL_ID	MaskedImage	=	AllocMaskingImage( m_DisplayBuf, pMaskingArea )	;
			if( MaskedImage == M_NULL )
			{
				g_MILCTRL_CS.Unlock()	;
				return	FALSE			;
			}
			else
			{
				MILCTRL_IF_FAILED( MbufExport(SaveFileName, Format, MaskedImage) )
				{
					g_MILCTRL_CS.Unlock()	;
					return	FALSE			;
				}
			}
			MbufFree( MaskedImage )	;
		}
	}
	else if( SubIdx >= 0 )
	{
		if( pMaskingArea == NULL )
		{
			MILCTRL_IF_FAILED( MbufExport(SaveFileName, Format, m_ArrSubGrabBuf[SubIdx]) )
			{
				g_MILCTRL_CS.Unlock()	;
				return	FALSE			;
			}
		}
		else
		{
			MIL_ID	MaskedImage	=	AllocMaskingImage( m_ArrSubGrabBuf[SubIdx], pMaskingArea )	;
			if( MaskedImage == M_NULL )
			{
				g_MILCTRL_CS.Unlock()	;
				return	FALSE			;
			}
			else
			{
				MILCTRL_IF_FAILED( MbufExport(SaveFileName, Format, MaskedImage) )
				{
					g_MILCTRL_CS.Unlock()	;
					return	FALSE			;
				}
			}
			MbufFree( MaskedImage )	;
		}
	}
	else
	{
		g_MILCTRL_CS.Unlock()	;
		return	FALSE			;
	}

	g_MILCTRL_CS.Unlock()	;
	return	TRUE			;
}

BOOL	MILCTRL::SaveImageASync( int SubIdx, LPCTSTR SaveFileName, int Format /*M_BMP*/, P_MILCTRL_MASKING_AREA pMaskingArea /*NULL*/ )
{
	if( !m_bIsCreated || SubIdx >= m_nArrGrabBufSize || SaveFileName == NULL)
		return	FALSE	;

	if( m_bIsFreePathMIL )
		return	TRUE	;

	MILCTRL_SAVEIMAGEDATA	ImageData	;

	ZeroMemory( &ImageData, sizeof(MILCTRL_SAVEIMAGEDATA) )	;
	ImageData.m_SubIdx			=	SubIdx								;
	ImageData.m_Format			=	Format								;
	ImageData.m_pstrFileName	=	new	TCHAR[_tcslen(SaveFileName)+1]	;
	_stprintf( ImageData.m_pstrFileName, _T("%s"), SaveFileName )		;
	if( pMaskingArea != NULL )
	{
		ImageData.m_pMaskingArea	=	new	MILCTRL_MASKING_AREA	;
		memcpy( ImageData.m_pMaskingArea, pMaskingArea, sizeof(MILCTRL_MASKING_AREA) )	;
	}


	g_MILCTRL_CS.Lock()	;
	if( g_MILCTRL_qSaveImage.size() == 0 )
	{
		g_MILCTRL_CS.Unlock()	;
		while( g_MILCTRL_IsSaveFileThreadRunning )
		{
			Sleep(1)	;
		}

		CWinThread	*pThread	=	AfxBeginThread(MILCTRL_SAVEIMAGE_THREAD, NULL, THREAD_PRIORITY_NORMAL);
		if( pThread == NULL )
		{
			delete[]	ImageData.m_pstrFileName	;
			if( ImageData.m_pMaskingArea != NULL )
				delete		ImageData.m_pMaskingArea	;

			return	FALSE	;
		}

		pThread->m_bAutoDelete	=	TRUE	;
	}
	else
	{
		g_MILCTRL_CS.Unlock()	;
	}

	g_MILCTRL_CS.Lock()	;
	g_MILCTRL_qSaveImage.push_back( ImageData )			;
	g_MILCTRL_SaveFileThreadForceTerminate	=	FALSE	;
	g_MILCTRL_CS.Unlock()	;

	return	TRUE			;
}

BOOL	MILCTRL::LoadImage( int SubIdx, LPCTSTR SaveFileName, int Format /* M_BMP */ )
{
	if( !m_bIsCreated || SubIdx >= m_nArrGrabBufSize || SaveFileName == NULL)
		return	FALSE	;

	if( m_bIsFreePathMIL )
		return	TRUE	;

	g_MILCTRL_CS.Lock()	;


	if( SubIdx < 0 )
	{
		if( MbufImport(SaveFileName, Format, M_LOAD, m_Sys, &m_DisplayBuf) == M_NULL )
		{
			g_MILCTRL_CS.Unlock()	;
			return	FALSE			;
		}
	}
	else
	{
		if( MbufImport(SaveFileName, Format, M_LOAD, m_Sys, &m_ArrSubGrabBuf[SubIdx]) == M_NULL )
		{
			g_MILCTRL_CS.Unlock()	;
			return	FALSE			;
		}

		if( !SetSubToMain(SubIdx) )
		{
			g_MILCTRL_CS.Unlock()	;
			return	FALSE			;
		}
	}

	g_MILCTRL_CS.Unlock()	;
	return	TRUE			;
}

BOOL	MILCTRL::GetGrabBuffer( void** ppBuf )
{
	if( !m_bIsCreated )
		return	FALSE	;

	if( ppBuf == NULL )
		return	FALSE	;

	if( m_bIsFreePathMIL )
		return	FALSE	;

	g_MILCTRL_CS.Lock()	;

	if( MbufInquire(m_GrabBuf, M_HOST_ADDRESS, ppBuf) == M_NULL )
	{
		g_MILCTRL_CS.Unlock()	;
		return	FALSE			;
	}

	g_MILCTRL_CS.Unlock()	;
	return	TRUE			;
}

BOOL	MILCTRL::SetGrabToDisplay()
{
	if( !m_bIsCreated )
		return	FALSE	;

	if( m_bIsFreePathMIL )
		return	TRUE	;

	g_MILCTRL_CS.Lock()	;

	//	메인 디스플레이 버퍼에 복사
	BOOL	BayerResult	=	TRUE	;
	switch( m_ColorPtn )
	{
		case MILCTRL_COLOR_PTN_GB	:
			MILCTRL_IF_FAILED( MbufBayer(m_GrabBuf, m_DisplayBuf, m_IdWBCoefficients, M_BAYER_GB) )
			{
				BayerResult	=	FALSE	;
			}
			break;

		case MILCTRL_COLOR_PTN_BG	:
			MILCTRL_IF_FAILED( MbufBayer(m_GrabBuf, m_DisplayBuf, m_IdWBCoefficients, M_BAYER_BG) ) 
			{
				BayerResult	=	FALSE	;
			}
			break;

		case MILCTRL_COLOR_PTN_GR	:
			MILCTRL_IF_FAILED( MbufBayer(m_GrabBuf, m_DisplayBuf, m_IdWBCoefficients, M_BAYER_GR) )
			{
				BayerResult	=	FALSE	;
			}
			break;

		case MILCTRL_COLOR_PTN_RG	:
			MILCTRL_IF_FAILED( MbufBayer(m_GrabBuf, m_DisplayBuf, m_IdWBCoefficients, M_BAYER_RG) )
			{
				BayerResult	=	FALSE	;
			}
			break;

		case MILCTRL_COLOR_PTN_NONE	:
			MILCTRL_IF_FAILED( MbufCopy(m_GrabBuf, m_DisplayBuf) )
			{
				g_MILCTRL_CS.Unlock()	;
				return	FALSE			;
			}
			break	;

		default :
			BayerResult	=	FALSE	;
			break;
	}

	g_MILCTRL_CS.Unlock()	;

	if( !BayerResult )
	{
		return	FALSE	;
	}

	return	TRUE	;
}

LONG	MILCTRL::GetGrabBufPitch()
{
	if( !m_bIsCreated )
		return	-1	;

	if( m_bIsFreePathMIL )
		return	-1	;

	__int64	pitch	;

	MbufInquire( m_GrabBuf, M_PITCH, &pitch )	;

	return	(LONG)pitch	;
}

BOOL	MILCTRL::SetGrabToProc( DOUBLE Gain )
{
	if( !m_bIsCreated )
		return	FALSE	;

	if( m_bIsFreePathMIL )
		return	TRUE	;

	g_MILCTRL_CS.Lock()	;

	if( m_CamBpp <= 8 )
	{
		MILCTRL_IF_FAILED( MbufCopy(m_GrabBuf, m_ProcBuf) )
		{
			g_MILCTRL_CS.Unlock()	;
			return	FALSE			;
		}
	}
	else
	{
		WORD	*pGrabBuf	;
		BYTE	*pProcBuf	;
		LONG	GrabPitch, ProcPitch, Conv	;
		LONG	GrabIdx, ProcIdx	;
		WORD	MaxValue	;

		if( !GetGrabBuffer((VOID**)&pGrabBuf) || !GetProcBuffer((VOID**)&pProcBuf) )
		{
			g_MILCTRL_CS.Unlock()	;
			return	FALSE			;
		}

		GrabPitch	=	GetGrabBufPitch()	;
		ProcPitch	=	GetProcBufPitch()	;

		Conv		=	1 << (m_CamBpp - 8)	;
		MaxValue	=	(0xFFFF >> (16 - m_CamBpp)) - 1	;
		for( int i = 0 ; i < m_CamWidth ; i++ )
		{
			for( int j = 0 ; j < m_CamHeight ; j++ )
			{
				GrabIdx	=	i + (GrabPitch * j)	;
				ProcIdx	=	i + (ProcPitch * j)	;
				pProcBuf[ProcIdx]	=	(LONG)((((DOUBLE)pGrabBuf[GrabIdx] * Gain) / Conv) + 0.5)	;
				if( pProcBuf[ProcIdx] > MaxValue )
					pProcBuf[ProcIdx]	=	MaxValue	;
			}
		}
	}
	g_MILCTRL_CS.Unlock()	;

	return	TRUE	;
}

BOOL	MILCTRL::GetProcBuffer( void** ppBuf )
{
	if( !m_bIsCreated )
		return	FALSE	;

	if( ppBuf == NULL )
		return	FALSE	;

	if( m_bIsFreePathMIL )
		return	FALSE	;

	g_MILCTRL_CS.Lock()	;

	if( MbufInquire(m_ProcBuf, M_HOST_ADDRESS, ppBuf) == M_NULL )
	{
		g_MILCTRL_CS.Unlock()	;
		return	FALSE			;
	}

	g_MILCTRL_CS.Unlock()	;
	return	TRUE			;
}

BOOL	MILCTRL::SetProcToDisplay()
{
	if( !m_bIsCreated )
		return	FALSE	;

	if( m_bIsFreePathMIL )
		return	TRUE	;

	g_MILCTRL_CS.Lock()	;

	//	메인 디스플레이 버퍼에 복사

	BOOL	BayerResult	=	TRUE	;
	switch( m_ColorPtn )
	{
		case MILCTRL_COLOR_PTN_GB	:
			MILCTRL_IF_FAILED( MbufBayer(m_ProcBuf, m_DisplayBuf, m_IdWBCoefficients, M_BAYER_GB) )
			{
				BayerResult	=	FALSE	;
			}
			break;

		case MILCTRL_COLOR_PTN_BG	:
			MILCTRL_IF_FAILED( MbufBayer(m_ProcBuf, m_DisplayBuf, m_IdWBCoefficients, M_BAYER_BG) ) 
			{
				BayerResult	=	FALSE	;
			}
			break;

		case MILCTRL_COLOR_PTN_GR	:
			MILCTRL_IF_FAILED( MbufBayer(m_ProcBuf, m_DisplayBuf, m_IdWBCoefficients, M_BAYER_GR) )
			{
				BayerResult	=	FALSE	;
			}
			break;

		case MILCTRL_COLOR_PTN_RG	:
			MILCTRL_IF_FAILED( MbufBayer(m_ProcBuf, m_DisplayBuf, m_IdWBCoefficients, M_BAYER_RG) )
			{
				BayerResult	=	FALSE	;
			}
			break;

		case MILCTRL_COLOR_PTN_NONE	:
			MILCTRL_IF_FAILED( MbufCopy(m_ProcBuf, m_DisplayBuf) )
			{
				g_MILCTRL_CS.Unlock()	;
				return	FALSE			;
			}
			break	;

		default :
			BayerResult	=	FALSE	;
			break;
	}

	g_MILCTRL_CS.Unlock()	;

	if( !BayerResult )
	{
		return	FALSE	;
	}

	return	TRUE	;
}

LONG	MILCTRL::GetProcBufPitch()
{
	if( !m_bIsCreated )
		return	-1	;

	if( m_bIsFreePathMIL )
		return	-1	;

	__int64	pitch	;

	MbufInquire( m_ProcBuf, M_PITCH, &pitch )	;

	return	(LONG)pitch	;
}

WORD	MILCTRL::GetDisplayPixelValue( WORD X, WORD Y, BYTE Color )
{
	if( !m_bIsCreated )
		return	0	;

	if( m_bIsFreePathMIL )
		return	0	;

	g_MILCTRL_CS.Unlock()	;

	VOID*	pData	;

	if( MbufInquire(m_DisplayBuf, M_HOST_ADDRESS, &pData) == M_NULL )
	{
		g_MILCTRL_CS.Unlock()	;
		return	FALSE			;
	}

	__int64	pitch	;

	MbufInquire( m_ProcBuf, M_PITCH, &pitch )	;

	WORD	RetVal	;

	if( m_CamBpp == 8 )
	{
		BYTE	*pImg	=	(BYTE*)pData	;

		RetVal	=	(WORD)pImg[(Y*pitch)+X]	;
	}
	else if( m_CamBpp <= 16 )
	{
		WORD	*pImg	=	(WORD*)pData	;

		RetVal	=	pImg[(Y*pitch)+X]	;
	}
	else
	{
		RetVal	=	0	;
	}

	return	RetVal	;
}

BOOL	MILCTRL::Zoom( MIL_DOUBLE ZoomFactorX, MIL_DOUBLE ZoomFactorY )
{
	if( !m_bIsCreated )
		return	FALSE	;

	if( m_bIsFreePathMIL )
		return	TRUE	;

//	g_MILCTRL_CS.Lock()	;
	MILCTRL_IF_FAILED( MdispZoom(m_MainDisp, ZoomFactorX, ZoomFactorY) )
	{
//		g_MILCTRL_CS.Unlock()	;
		return	FALSE			;
	}

//	g_MILCTRL_CS.Unlock()	;
	return	TRUE			;
}

BOOL	MILCTRL::Pan( MIL_DOUBLE PanX, MIL_DOUBLE PanY )
{
	if( !m_bIsCreated )
		return	FALSE	;

	if( m_bIsFreePathMIL )
		return	TRUE	;

//	g_MILCTRL_CS.Lock()	;
	MILCTRL_IF_FAILED( MdispPan(m_MainDisp, PanX, PanY) )
	{
//		g_MILCTRL_CS.Unlock()	;
		return	FALSE			;
	}

//	g_MILCTRL_CS.Unlock()	;
	return	TRUE			;
}

BOOL	MILCTRL::ClearOverlay()
{
	if( !m_bIsCreated )
		return	FALSE	;

	if( m_bIsFreePathMIL )
		return	TRUE	;

	g_MILCTRL_CS.Lock()	;
	MILCTRL_IF_FAILED( MdispControl(m_MainDisp, M_OVERLAY_CLEAR, M_DEFAULT) )
	{
		g_MILCTRL_CS.Unlock()	;
		return	FALSE			;
	}

	g_MILCTRL_CS.Unlock()	;
	return	TRUE			;
}

BOOL	MILCTRL::DrawOverlayRect( LPCRECT lpcRect, LPCTSTR strText, MIL_DOUBLE Color, BOOL bFillColor, int DotSize )
{
	if( !m_bIsCreated || lpcRect == NULL )
		return	FALSE	;

	if( m_bIsFreePathMIL )
		return	TRUE	;

	g_MILCTRL_CS.Lock()	;
	MILCTRL_IF_FAILED( MgraControl(M_DEFAULT, M_BACKGROUND_MODE, M_TRANSPARENT) )
	{
		g_MILCTRL_CS.Unlock()	;
		return	FALSE			;
	}

	MILCTRL_IF_FAILED( MgraColor(M_DEFAULT, Color) )
	{
		g_MILCTRL_CS.Unlock()	;
		return	FALSE			;
	}

	int	Diff	;

	for( int i = 0 ; i < DotSize ; i++ )
	{
		Diff	=	(int)i / 2	;

		if( i % 2 == 1 )
		{
			Diff	=	-( Diff + 1 )	;
		}

		if( bFillColor )
		{
			MILCTRL_IF_FAILED( MgraRectFill(M_DEFAULT, m_OverlayBuf, lpcRect->left + Diff , lpcRect->top + Diff, lpcRect->right - Diff, lpcRect->bottom - Diff) )
			{
				g_MILCTRL_CS.Unlock()	;
				return	FALSE			;
			}
		}
		else
		{
			MILCTRL_IF_FAILED( MgraRect(M_DEFAULT, m_OverlayBuf, lpcRect->left + Diff , lpcRect->top + Diff, lpcRect->right - Diff, lpcRect->bottom - Diff) )
			{
				g_MILCTRL_CS.Unlock()	;
				return	FALSE			;
			}
		}
	}

	if( strText != NULL )
	{
		MILCTRL_IF_FAILED( MgraFont(M_DEFAULT, M_FONT_DEFAULT_LARGE) )
		{
			g_MILCTRL_CS.Unlock()	;
			return	FALSE			;
		}

		MILCTRL_IF_FAILED( MgraText(M_DEFAULT, m_OverlayBuf, lpcRect->left, lpcRect->top - 25, strText) )
		{
			g_MILCTRL_CS.Unlock()	;
			return	FALSE			;
		}
	}

	g_MILCTRL_CS.Unlock()	;
	return	TRUE			;
}

BOOL	MILCTRL::DrawOverlayCircle( LPCRECT lpcRect, LPCTSTR strText, MIL_DOUBLE Color, BOOL bFillColor, int DotSize )
{
	if( !m_bIsCreated || lpcRect == NULL )
		return	FALSE	;

	if( m_bIsFreePathMIL )
		return	TRUE	;

	g_MILCTRL_CS.Lock()	;

	MIL_INT32	CenterX, CenterY, RadX, RadY	;

	RadX	=	(MIL_INT32)((lpcRect->right - lpcRect->left)/2)	;
	RadY	=	(MIL_INT32)((lpcRect->bottom - lpcRect->top)/2)	;
	CenterX	=	lpcRect->left + RadX	;
	CenterY	=	lpcRect->top + RadY		;

	MILCTRL_IF_FAILED( MgraControl(M_DEFAULT, M_BACKGROUND_MODE, M_TRANSPARENT) )
	{
		g_MILCTRL_CS.Unlock()	;
		return	FALSE			;
	}

	MILCTRL_IF_FAILED( MgraColor(M_DEFAULT, Color) )
	{
		g_MILCTRL_CS.Unlock()	;
		return	FALSE			;
	}

	int	Diff	;

	for( int i = 0 ; i < DotSize ; i++ )
	{
		Diff	=	(int)i / 2	;

		if( i % 2 == 1 )
		{
			Diff	=	-( Diff + 1 )	;
		}

		if( bFillColor )
		{
			MILCTRL_IF_FAILED( MgraArcFill(M_DEFAULT, m_OverlayBuf, CenterX, CenterY, RadX + Diff, RadY + Diff, 0, 360) )
			{
				g_MILCTRL_CS.Unlock()	;
				return	FALSE			;
			}
		}
		else
		{
			MILCTRL_IF_FAILED( MgraArc(M_DEFAULT, m_OverlayBuf, CenterX, CenterY, RadX + Diff, RadY + Diff, 0, 360) )
			{
				g_MILCTRL_CS.Unlock()	;
				return	FALSE			;
			}
		}
	}

	if( strText != NULL )
	{
		MILCTRL_IF_FAILED( MgraFont(M_DEFAULT, M_FONT_DEFAULT_LARGE) )
		{
			g_MILCTRL_CS.Unlock()	;
			return	FALSE			;
		}

		MILCTRL_IF_FAILED( MgraText(M_DEFAULT, m_OverlayBuf, lpcRect->left, lpcRect->top - 25, strText) )
		{
			g_MILCTRL_CS.Unlock()	;
			return	FALSE			;
		}
	}

	g_MILCTRL_CS.Unlock()	;
	return	TRUE			;
}

BOOL	MILCTRL::DrawOverlayCross( LPCRECT lpcRect, LPCTSTR strText, MIL_DOUBLE Color, int DotSize )
{
	if( !m_bIsCreated || lpcRect == NULL )
		return	FALSE	;

	if( m_bIsFreePathMIL )
		return	TRUE	;

	g_MILCTRL_CS.Lock()	;

	MIL_INT32	*BeginX, *BeginY, *EndX, *EndY	;
	BeginX	=	new	MIL_INT32[DotSize*2]	;
	BeginY	=	new	MIL_INT32[DotSize*2]	;
	EndX	=	new	MIL_INT32[DotSize*2]	;
	EndY	=	new	MIL_INT32[DotSize*2]	;

	//	가로 라인
	int	Diff	;
	for( int i = 0 ; i < DotSize ; i++ )
	{
		Diff	=	(int)i / 2	;

		if( i % 2 == 1 )
		{
			Diff	=	-( Diff + 1 )	;
		}

		BeginX[i]	=	lpcRect->left	;
		EndX[i]		=	lpcRect->right	;
		BeginY[i]	=	EndY[i]	=	lpcRect->top + (MIL_INT32)((lpcRect->bottom - lpcRect->top)/2) + Diff	;

		BeginY[DotSize+i]	=	lpcRect->top	;
		EndY[DotSize+i]		=	lpcRect->bottom	;
		BeginX[DotSize+i]	=	EndX[DotSize+i]	=	lpcRect->left + (MIL_INT32)((lpcRect->right - lpcRect->left)/2) + Diff	;
	}

	MILCTRL_IF_FAILED( MgraControl(M_DEFAULT, M_BACKGROUND_MODE, M_TRANSPARENT) )
	{
		delete[]	BeginX	;
		delete[]	BeginY	;
		delete[]	EndX	;
		delete[]	EndY	;
		g_MILCTRL_CS.Unlock()	;
		return	FALSE			;
	}

	MILCTRL_IF_FAILED( MgraColor(M_DEFAULT, Color) )
	{
		delete[]	BeginX	;
		delete[]	BeginY	;
		delete[]	EndX	;
		delete[]	EndY	;
		g_MILCTRL_CS.Unlock()	;
		return	FALSE			;
	}

	MILCTRL_IF_FAILED( MgraLines(M_DEFAULT, m_OverlayBuf, DotSize*2, BeginX, BeginY, EndX, EndY, M_DEFAULT) )
	{
		delete[]	BeginX	;
		delete[]	BeginY	;
		delete[]	EndX	;
		delete[]	EndY	;
		g_MILCTRL_CS.Unlock()	;
		return	FALSE			;
	}

	delete[]	BeginX	;
	delete[]	BeginY	;
	delete[]	EndX	;
	delete[]	EndY	;

	if( strText != NULL )
	{
		MILCTRL_IF_FAILED( MgraFont(M_DEFAULT, M_FONT_DEFAULT_LARGE) )
		{
			g_MILCTRL_CS.Unlock()	;
			return	FALSE			;
		}

		MILCTRL_IF_FAILED( MgraText(M_DEFAULT, m_OverlayBuf, lpcRect->left, lpcRect->top - 25, strText) )
		{
			g_MILCTRL_CS.Unlock()	;
			return	FALSE			;
		}
	}

	g_MILCTRL_CS.Unlock()	;
	return	TRUE			;
}

BOOL	MILCTRL::SetMainToSub( int SubIdx, int ColorBuffer /* 0 */ )
{
	if( !m_bIsCreated || SubIdx < 0 || SubIdx >= m_nArrGrabBufSize || ColorBuffer < 0 || ColorBuffer > 3 )
		return	FALSE	;

	if( m_bIsFreePathMIL )
		return	TRUE	;

	g_MILCTRL_CS.Lock()	;
	if( ColorBuffer == 0 )
	{
		MILCTRL_IF_FAILED( MbufCopy(m_DisplayBuf, m_ArrSubGrabBuf[SubIdx]) )
		{
			g_MILCTRL_CS.Unlock()	;
			return	FALSE			;
		}

		if( m_bIsColor )
		{
			MILCTRL_IF_FAILED( MbufCopy(m_DispBufR, m_ArrSubGrabBufR[SubIdx]) )
			{
				g_MILCTRL_CS.Unlock()	;
				return	FALSE			;
			}

			MILCTRL_IF_FAILED( MbufCopy(m_DispBufG, m_ArrSubGrabBufG[SubIdx]) )
			{
				g_MILCTRL_CS.Unlock()	;
				return	FALSE			;
			}

			MILCTRL_IF_FAILED( MbufCopy(m_DispBufB, m_ArrSubGrabBufB[SubIdx]) )
			{
				g_MILCTRL_CS.Unlock()	;
				return	FALSE			;
			}
		}
	}
	else
	{
		if( !m_bIsColor )
		{
			g_MILCTRL_CS.Unlock()	;
			return	FALSE			;
		}

		switch( ColorBuffer )
		{
			case	1	:
				MILCTRL_IF_FAILED( MbufCopy(m_DispBufR, m_ArrSubGrabBuf[SubIdx]) )
				{
					g_MILCTRL_CS.Unlock()	;
					return	FALSE			;
				}
				break	;
			case	2	:
				MILCTRL_IF_FAILED( MbufCopy(m_DispBufG, m_ArrSubGrabBuf[SubIdx]) )
				{
					g_MILCTRL_CS.Unlock()	;
					return	FALSE			;
				}
				break	;
			case	3	:
				MILCTRL_IF_FAILED( MbufCopy(m_DispBufB, m_ArrSubGrabBuf[SubIdx]) )
				{
					g_MILCTRL_CS.Unlock()	;
					return	FALSE			;
				}
				break	;
		}
	}

	g_MILCTRL_CS.Unlock()	;
	return	TRUE			;
}

BOOL	MILCTRL::SetSubToMain( int SubIdx, int ColorBuffer /* 0 */ )
{
	if( !m_bIsCreated || SubIdx < 0 || SubIdx >= m_nArrGrabBufSize || ColorBuffer < 0 || ColorBuffer > 3 )
		return	FALSE	;

	if( m_bIsFreePathMIL )
		return	TRUE	;

	g_MILCTRL_CS.Lock()	;
	if( ColorBuffer == 0 )
	{
		MILCTRL_IF_FAILED( MbufCopy(m_ArrSubGrabBuf[SubIdx], m_DisplayBuf) )
		{
			g_MILCTRL_CS.Unlock()	;
			return	FALSE			;
		}

		if( m_bIsColor )
		{
			MILCTRL_IF_FAILED( MbufCopy(m_ArrSubGrabBufR[SubIdx], m_DispBufR) )
			{
				g_MILCTRL_CS.Unlock()	;
				return	FALSE			;
			}

			MILCTRL_IF_FAILED( MbufCopy(m_ArrSubGrabBufG[SubIdx], m_DispBufG) )
			{
				g_MILCTRL_CS.Unlock()	;
				return	FALSE			;
			}

			MILCTRL_IF_FAILED( MbufCopy(m_ArrSubGrabBufB[SubIdx], m_DispBufB) )
			{
				g_MILCTRL_CS.Unlock()	;
				return	FALSE			;
			}
		}
	}
	else
	{
		if( !m_bIsColor )
		{
			g_MILCTRL_CS.Unlock()	;
			return	FALSE			;
		}

		switch( ColorBuffer )
		{
			case	1	:
				MILCTRL_IF_FAILED( MbufCopy(m_ArrSubGrabBufR[SubIdx], m_DisplayBuf) )
				{
					g_MILCTRL_CS.Unlock()	;
					return	FALSE			;
				}
				break	;
			case	2	:
				MILCTRL_IF_FAILED( MbufCopy(m_ArrSubGrabBufG[SubIdx], m_DisplayBuf) )
				{
					g_MILCTRL_CS.Unlock()	;
					return	FALSE			;
				}
				break	;
			case	3	:
				MILCTRL_IF_FAILED( MbufCopy(m_ArrSubGrabBufB[SubIdx], m_DisplayBuf) )
				{
					g_MILCTRL_CS.Unlock()	;
					return	FALSE			;
				}
				break	;
		}
	}

	g_MILCTRL_CS.Unlock()	;
	return	TRUE			;
}

BOOL	MILCTRL::ClearSubBuffer( int SubIdx /*-1*/ )
{
	if( !m_bIsCreated )
		return	FALSE	;

	if( m_bIsFreePathMIL )
		return	TRUE	;

	if( SubIdx >= m_nArrGrabBufSize )
		return	FALSE	;

	if( SubIdx < 0 )
	{
		for( int i = 0 ; i < m_nArrGrabBufSize ; i++ )
		{
			if( !ClearSubBuffer(i) )
				return	FALSE	;
		}
	}
	else
	{
		g_MILCTRL_CS.Lock()	;
		MILCTRL_IF_FAILED( MbufClear(m_ArrSubGrabBuf[SubIdx], 0x00) )
		{
			g_MILCTRL_CS.Unlock()	;
			return	FALSE			;
		}

		MILCTRL_IF_FAILED( MbufClear(m_ArrSubGrabBufR[SubIdx], 0x00) )
		{
			g_MILCTRL_CS.Unlock()	;
			return	FALSE			;
		}

		MILCTRL_IF_FAILED( MbufClear(m_ArrSubGrabBufG[SubIdx], 0x00) )
		{
			g_MILCTRL_CS.Unlock()	;
			return	FALSE			;
		}

		MILCTRL_IF_FAILED( MbufClear(m_ArrSubGrabBufB[SubIdx], 0x00) )
		{
			g_MILCTRL_CS.Unlock()	;
			return	FALSE			;
		}

		g_MILCTRL_CS.Unlock()	;
	}

	return	TRUE	;


}

BOOL	MILCTRL::SetWhiteBalance( P_MILCTRL_WB pWhiteBalance )
{
	if( !m_bIsCreated || pWhiteBalance == NULL )
		return	FALSE	;

	if( m_bIsFreePathMIL )
		return	TRUE	;

	g_MILCTRL_CS.Lock()	;
	if( m_IdWBCoefficients != M_NULL )
	{
		MILCTRL_IF_FAILED( MbufPut(m_IdWBCoefficients, (void *)pWhiteBalance->m_WhiteBalance) )
		{
			g_MILCTRL_CS.Unlock()	;
			return	FALSE			;
		}

		m_ArrWBCoefficients[0]	=	pWhiteBalance->m_WhiteBalance[0]	;
		m_ArrWBCoefficients[1]	=	pWhiteBalance->m_WhiteBalance[1]	;
		m_ArrWBCoefficients[2]	=	pWhiteBalance->m_WhiteBalance[2]	;
	}
	else
	{
		g_MILCTRL_CS.Unlock()	;
		return	FALSE			;
	}

	g_MILCTRL_CS.Unlock()	;
	return	TRUE	;
}

BOOL	MILCTRL::GetWhiteBalance( P_MILCTRL_WB pWhiteBalance )
{
	if( !m_bIsCreated || pWhiteBalance == NULL )
		return	FALSE	;

	if( m_bIsFreePathMIL )
		return	TRUE	;

	g_MILCTRL_CS.Lock()	;
	if( m_IdWBCoefficients != M_NULL )
	{
		pWhiteBalance->m_WhiteBalance[0]	=	m_ArrWBCoefficients[0]	;
		pWhiteBalance->m_WhiteBalance[1]	=	m_ArrWBCoefficients[1]	;
		pWhiteBalance->m_WhiteBalance[2]	=	m_ArrWBCoefficients[2]	;
	}
	else
	{
		g_MILCTRL_CS.Unlock()	;
		return	FALSE			;
	}

	g_MILCTRL_CS.Unlock()	;
	return	TRUE	;
}

BOOL	MILCTRL::GetCalcAutoWB( P_MILCTRL_WB pWhiteBalance )
{
	if( !m_bIsCreated || !m_bIsColor || pWhiteBalance == NULL )
		return	FALSE	;

	if( m_bIsFreePathMIL )
		return	TRUE	;

	MIL_INT	Flag	;

	g_MILCTRL_CS.Lock()	;
	if( m_IdWBCoefficients != M_NULL )
	{
		switch( m_ColorPtn )
		{
			case MILCTRL_COLOR_PTN_GB	:	Flag	=	M_BAYER_GB + M_WHITE_BALANCE_CALCULATE	;	break;
			case MILCTRL_COLOR_PTN_BG	:	Flag	=	M_BAYER_BG + M_WHITE_BALANCE_CALCULATE	;	break;
			case MILCTRL_COLOR_PTN_GR	:	Flag	=	M_BAYER_GR + M_WHITE_BALANCE_CALCULATE	;	break;
			case MILCTRL_COLOR_PTN_RG	:	Flag	=	M_BAYER_RG + M_WHITE_BALANCE_CALCULATE	;	break;
			default :
				g_MILCTRL_CS.Unlock()	;
				return	FALSE			;
		}

		MILCTRL_IF_FAILED( MbufBayer(m_DisplayBuf, m_DisplayBuf, m_IdWBCoefficients, Flag) )
		{
			g_MILCTRL_CS.Unlock()	;
			return	FALSE			;
		}

		MILCTRL_IF_FAILED( MbufGet(m_IdWBCoefficients, (void *)pWhiteBalance->m_WhiteBalance) )
		{
			g_MILCTRL_CS.Unlock()	;
			return	FALSE			;
		}
	}
	else
	{
		g_MILCTRL_CS.Unlock()	;
		return	FALSE			;
	}

	g_MILCTRL_CS.Unlock()	;
	return	TRUE	;
}

VOID	MILCTRL::SetColorPattern( MILCTRL_COLOR_PTN ColorPattern )
{
	g_MILCTRL_CS.Lock()				;
	m_ColorPtn	=	ColorPattern	;
	g_MILCTRL_CS.Unlock()			;
}

BOOL	MILCTRL::GetFocusValue( double *pFocusVal, int *pPixelVal, RECT *pArea /*NULL*/ )
{
	if( !m_bIsCreated || pFocusVal == NULL || pPixelVal == NULL )
		return	FALSE	;

	if( m_bIsFreePathMIL )
	{
		*pFocusVal	=	0.0	;
		*pPixelVal	=	0	;
		return	TRUE		;
	}

	MIL_ID		MonoBuf	;
	long		left, right, top, bottom, wd, ht	;
	long long	pitch = 0, pitch2 = 0	;
	double		m_edit_multi = 1.0	;
	BYTE		*by_source_r = NULL, *by_source_g = NULL, *by_source_b = NULL, *by_source = NULL, *by_dest = NULL	;

	if( pArea == NULL )
	{
		left	=	(m_CamWidth / 2) - 50			;
		right	=	(m_CamWidth / 2) + 50			;
		top		=	(m_CamHeight / 2 + 150) - 50	;
		bottom	=	(m_CamHeight / 2 + 150) + 50	;
	}
	else
	{
		left	=	pArea->left		;
		right	=	pArea->right	;
		top		=	pArea->top		;
		bottom	=	pArea->bottom	;
	}
	wd		=	right - left	;
	ht		=	bottom - top	;

	g_MILCTRL_CS.Lock()				;
	if( MbufAlloc2d(m_Sys, wd, ht, 8L+M_UNSIGNED,M_IMAGE+M_PROC, &MonoBuf) == M_NULL )
	{
		g_MILCTRL_CS.Unlock()	;
		return	FALSE			;
	}

	MILCTRL_IF_FAILED( MbufClear(MonoBuf, 0L) )
	{
		MbufFree( MonoBuf )		;
		g_MILCTRL_CS.Unlock()	;
		return	FALSE			;
	}

	if( MbufInquire(MonoBuf, M_HOST_ADDRESS, &by_dest) == M_NULL )
	{
		MbufFree( MonoBuf )		;
		g_MILCTRL_CS.Unlock()	;
		return	FALSE			;
	}

	MbufInquire( MonoBuf, M_PITCH, &pitch)	;

	if( m_bIsColor )
	{
		if( MbufInquire(m_DispBufR, M_HOST_ADDRESS, &by_source_r) == M_NULL )
		{
			MbufFree( MonoBuf )		;
			g_MILCTRL_CS.Unlock()	;
			return	FALSE			;
		}

		if( MbufInquire(m_DispBufG, M_HOST_ADDRESS, &by_source_g) == M_NULL )
		{
			MbufFree( MonoBuf )		;
			g_MILCTRL_CS.Unlock()	;
			return	FALSE			;
		}

		if( MbufInquire(m_DispBufB, M_HOST_ADDRESS, &by_source_b) == M_NULL )
		{
			MbufFree( MonoBuf )		;
			g_MILCTRL_CS.Unlock()	;
			return	FALSE			;
		}

		MbufInquire(m_DispBufR, M_PITCH, &pitch2);

		*pPixelVal = RGBtoGrayF( (const long)(wd), (const long)(ht), (const long)(pitch), (const long)(pitch2), left, top, by_source_r, by_source_g, by_source_b, by_dest )	;
	}
	else
	{
		if( MbufInquire(m_DisplayBuf, M_HOST_ADDRESS, &by_source) == M_NULL )
		{
			MbufFree( MonoBuf )		;
			g_MILCTRL_CS.Unlock()	;
			return	FALSE			;
		}


		MbufInquire(m_DisplayBuf, M_PITCH, &pitch2);

		*pPixelVal = MonotoGrayF( (const long)(wd), (const long)(ht), (const long)(pitch), (const long)(pitch2), left, top, by_source, by_dest )	;
	}

	double	*by_source_dest = new double[pitch * ht]	;
	*pFocusVal = EdgeRoberts_D( (long)(wd), (long)(ht), (long)(pitch), by_dest, by_source_dest, 1.0 )	;


	
	MbufFree( MonoBuf )			;
	delete[]	by_source_dest	;

	*pFocusVal	=	_finite(*pFocusVal) ? *pFocusVal : 0;	//20170831 by 무한대수의 일 경우 0을 리턴

	g_MILCTRL_CS.Unlock()			;
	return	TRUE	;
}

BOOL	MILCTRL::GetColorPixelInfo( int *pMinData, int *pMaxData, double *pAvrgData, MILCTRL_COLOR_PTN Bayer )
{
	if( !m_bIsCreated || pMinData == NULL || pMaxData == NULL || pAvrgData == NULL || Bayer == MILCTRL_COLOR_PTN_NONE )
		return	FALSE	;

	if( m_bIsFreePathMIL )
		return	FALSE	;

	long long	Pitch, ColorSum[3]	;
	long		Idx, ColorCount[3], ColorIdx	;
	BYTE		*pGrabRawData	;
	g_MILCTRL_CS.Lock()				;
	MbufInquire( m_GrabBuf, M_PITCH, &Pitch)	;

	if( MbufInquire(m_GrabBuf, M_HOST_ADDRESS, &pGrabRawData) == M_NULL )
	{
		g_MILCTRL_CS.Unlock()	;
		return	FALSE			;
	}

	for( int i = 0 ; i < 3 ; i++ )
	{
		ColorCount[i]	=	ColorSum[i]	=	0	;
		pMinData[i]	=	0xFFFFFF	;
		pMaxData[i]	=	0			;
	}

	for( int i = 0 ; i < m_CamWidth ; i++ )
	{
		for( int j = 0 ; j < m_CamHeight ; j++ )
		{
			Idx	=	j * Pitch + i	;
			switch( Bayer )
			{
				case	MILCTRL_COLOR_PTN_GB	:
					//	GB
					//	RG
					if( i % 2 == 0 )
					{
						if( j % 2 == 0 )
							ColorIdx	=	1	;	//	G
						else
							ColorIdx	=	0	;	//	R
					}
					else
					{
						if( j % 2 == 0 )
							ColorIdx	=	2	;	//	B
						else
							ColorIdx	=	1	;	//	G
					}
					break	;
				case	MILCTRL_COLOR_PTN_BG	:
					//	BG
					//	GR
					if( i % 2 == 0 )
					{
						if( j % 2 == 0 )
							ColorIdx	=	2	;	//	B
						else
							ColorIdx	=	1	;	//	G
					}
					else
					{
						if( j % 2 == 0 )
							ColorIdx	=	1	;	//	G
						else
							ColorIdx	=	0	;	//	R
					}
					break	;
				case	MILCTRL_COLOR_PTN_GR	:
					//	GR
					//	BG
					if( i % 2 == 0 )
					{
						if( j % 2 == 0 )
							ColorIdx	=	1	;	//	G
						else
							ColorIdx	=	2	;	//	B
					}
					else
					{
						if( j % 2 == 0 )
							ColorIdx	=	0	;	//	R
						else
							ColorIdx	=	1	;	//	G
					}
					break	;
				case	MILCTRL_COLOR_PTN_RG	:
					//	RG
					//	GB
					if( i % 2 == 0 )
					{
						if( j % 2 == 0 )
							ColorIdx	=	0	;	//	R
						else
							ColorIdx	=	1	;	//	G
					}
					else
					{
						if( j % 2 == 0 )
							ColorIdx	=	1	;	//	G
						else
							ColorIdx	=	2	;	//	B
					}
					break	;
			}

			if( pMinData[ColorIdx] > pGrabRawData[Idx] )
				pMinData[ColorIdx]	=	pGrabRawData[Idx]	;

			if( pMaxData[ColorIdx] < pGrabRawData[Idx] )
				pMaxData[ColorIdx]	=	pGrabRawData[Idx]	;

			ColorSum[ColorIdx]	+=	pGrabRawData[Idx]	;
			ColorCount[ColorIdx]++	;
		}
	}

	g_MILCTRL_CS.Unlock()			;

	for( int i = 0 ; i < 3 ; i++ )
	{
		pAvrgData[i]	=	ColorSum[i] / ColorCount[i]	;
	}

	return	TRUE	;
}

BOOL	MILCTRL::CreateNewOverlay()
{
	if( !m_bIsCreated )
		return	FALSE	;

	if( MdispInquire(m_MainDisp, M_OVERLAY_ID, &m_OverlayBuf) == M_NULL )
	{
		return	FALSE	;
	}

	return	TRUE	;
}

MIL_ID	MILCTRL::AllocMaskingImage( MIL_ID SrcID, P_MILCTRL_MASKING_AREA pMaskingArea )
{
	if( pMaskingArea == NULL || SrcID == M_NULL )
		return	M_NULL	;

	MIL_ID	ReturnBufID	;

	g_MILCTRL_CS.Lock()	;
	if( m_bIsColor )
	{
		if( MbufAllocColor(m_Sys, 3, m_CamWidth, m_CamHeight, 8L+M_UNSIGNED,M_IMAGE+M_DISP+M_PLANAR, &ReturnBufID) == M_NULL )
		{
			g_MILCTRL_CS.Unlock()	;
			return	M_NULL			;
		}
	}
	else
	{
		if( MbufAlloc2d(m_Sys, m_CamWidth, m_CamHeight, 8L+M_UNSIGNED,M_IMAGE+M_DISP+M_PLANAR, &ReturnBufID) == M_NULL )
		{
			g_MILCTRL_CS.Unlock()	;
			return	M_NULL			;
		}
	}

	MbufCopy( SrcID, ReturnBufID )	;
	g_MILCTRL_CS.Unlock()	;

	MILCTRL_IF_FAILED( MbufCopy(SrcID, ReturnBufID) )
	{
		MbufFree( ReturnBufID )	;
		return	M_NULL	;
	}

	MILCTRL_IF_FAILED( MgraControl(M_DEFAULT, M_BACKGROUND_MODE, M_TRANSPARENT) )
	{
		MbufFree( ReturnBufID )	;
		return	M_NULL	;
	}

	MILCTRL_IF_FAILED( MgraColor(M_DEFAULT, M_COLOR_BLACK) )
	{
		MbufFree( ReturnBufID )	;
		return	M_NULL	;
	}

	RECT	MaskingRect	;
	LONG	Margin		;

	Margin	=	5		;	//	중심점 기준 바깥쪽 마스킹 영역의 마진

	//	좌상단 영역의 바깥 상단 영역
	MaskingRect.left	=	pMaskingArea->m_LTPos.x	-	(pMaskingArea->m_LTLen.x/2)	;
	MaskingRect.top		=	pMaskingArea->m_LTPos.y	-	(pMaskingArea->m_LTLen.y/2)	;
	MaskingRect.right	=	MaskingRect.left		+	pMaskingArea->m_LTLen.x		;
	MaskingRect.bottom	=	pMaskingArea->m_LTPos.y	-	Margin						;
	MILCTRL_IF_FAILED( MgraRectFill(M_DEFAULT, ReturnBufID, MaskingRect.left, MaskingRect.top, MaskingRect.right, MaskingRect.bottom) )
	{
		MbufFree( ReturnBufID )	;
		return	M_NULL	;
	}

	//	좌상단 영역의 바깥 좌측 하단 영역
	MaskingRect.top		=	MaskingRect.bottom		;
	MaskingRect.right	=	pMaskingArea->m_LTPos.x	-	Margin	;
	MaskingRect.bottom	=	pMaskingArea->m_LTPos.y	+	(pMaskingArea->m_LTLen.y/2)	;
	MILCTRL_IF_FAILED( MgraRectFill(M_DEFAULT, ReturnBufID, MaskingRect.left, MaskingRect.top, MaskingRect.right, MaskingRect.bottom) )
	{
		MbufFree( ReturnBufID )	;
		return	M_NULL	;
	}

	//	우하단 영역의 바깥 하단 영역
	MaskingRect.left	=	pMaskingArea->m_RBPos.x	-	(pMaskingArea->m_RBLen.x/2)	;
	MaskingRect.top		=	pMaskingArea->m_RBPos.y	+	Margin						;
	MaskingRect.right	=	MaskingRect.left		+	pMaskingArea->m_RBLen.x		;
	MaskingRect.bottom	=	pMaskingArea->m_RBPos.y	+	(pMaskingArea->m_RBLen.y/2)	;
	MILCTRL_IF_FAILED( MgraRectFill(M_DEFAULT, ReturnBufID, MaskingRect.left, MaskingRect.top, MaskingRect.right, MaskingRect.bottom) )
	{
		MbufFree( ReturnBufID )	;
		return	M_NULL	;
	}

	//	우하단 영역의 바깥 우측 상단 영역
	MaskingRect.bottom	=	MaskingRect.top											;
	MaskingRect.left	=	pMaskingArea->m_RBPos.x	+	Margin						;
	MaskingRect.top		=	pMaskingArea->m_RBPos.y	-	(pMaskingArea->m_RBLen.y/2)	;
	MILCTRL_IF_FAILED( MgraRectFill(M_DEFAULT, ReturnBufID, MaskingRect.left, MaskingRect.top, MaskingRect.right, MaskingRect.bottom) )
	{
		MbufFree( ReturnBufID )	;
		return	M_NULL	;
	}

	return	ReturnBufID	;
}

BOOL	MILCTRL::CalcExposureTime( HWND hWndRecvResult, UINT UserMessage, LONG DesAvrgtLumi, LONG LumiMargin, RECT *pROI )
{
	if( pROI == NULL )
		return	FALSE	;

	g_MILCTRL_AutoExpData.m_hWndRecvResult	=	hWndRecvResult	;
	g_MILCTRL_AutoExpData.m_UserMessage		=	UserMessage		;
	g_MILCTRL_AutoExpData.m_DestLumi		=	DesAvrgtLumi	;
	g_MILCTRL_AutoExpData.m_LumiMargin		=	LumiMargin		;
	memcpy( &g_MILCTRL_AutoExpData.m_ROI, pROI, sizeof(RECT) )	;

	CWinThread	*pThread	=	AfxBeginThread(MILCTRL_AUTOEXP_THREAD, NULL, THREAD_PRIORITY_NORMAL);
	if( pThread == NULL )
		return	FALSE	;

	pThread->m_bAutoDelete			=	TRUE	;

	return	TRUE	;
}


BOOL	MILCTRL::GetImage(P_MILCTRL_IMGINFO pImage, BOOL AllocNewBuffer /*FALSE*/)
{
	if (!m_bIsCreated || pImage == NULL)
		return	FALSE;

	LONG	Pitch;
	Pitch = GetGrabBufPitch();

	if (AllocNewBuffer)
	{
		if (pImage->m_pData != NULL)
		{
			return	FALSE;
		}

		if (m_CamBpp <= 8)
		{
			pImage->m_pData = new	BYTE[m_CamWidth*m_CamHeight];
		}
		else if (m_CamBpp <= 16)
		{
			pImage->m_pData = new	BYTE[m_CamWidth*m_CamHeight *2];
		}
	}
	else
	{
		if (pImage->m_pData == NULL)
		{
			return	FALSE;
		}

	}

	pImage->m_Width		=	m_CamWidth		;
	pImage->m_Height	= m_CamHeight;
	pImage->m_Bpp		=	m_CamBpp		;
	if (m_bIsColor)
		pImage->m_Channel = 3;
	else
		pImage->m_Channel = 1;
	pImage->m_ColorType = m_ColorPtn	;
	if (m_CamBpp <= 8)
	{
		pImage->m_DataSize = m_CamWidth*m_CamHeight;
	}
	else if (m_CamBpp <= 16)
	{
		pImage->m_DataSize = m_CamWidth*m_CamHeight *2;
	}

	BYTE	*pImgBuf;
	
	GetGrabBuffer((VOID**)&pImgBuf);
	
	for (int i = 0; i < m_CamHeight; i++)
	{
		if (m_CamBpp <= 8)
		{
			memcpy(pImage->m_pData + (i*m_CamWidth), pImgBuf + (i*Pitch), m_CamWidth);
		}
		else if (m_CamBpp <= 16)
		{
			memcpy(pImage->m_pData + (i*m_CamWidth *2), pImgBuf + (i*Pitch*2), m_CamWidth);
		}
	}

	return	TRUE;
}

BOOL MILCTRL::InitLineScanGrabThread()
{
	BOOL rslt = TRUE;

	m_bGrabThreadStart = TRUE;
	m_bGrabThreadEnd = FALSE;

	m_pThreadLineScanGrab = AfxBeginThread(LIneScanGrabThread, this, THREAD_PRIORITY_TIME_CRITICAL);
	m_pThreadLineScanGrab->m_bAutoDelete = TRUE;

	if (m_pThreadLineScanGrab == NULL)
	{
		rslt = FALSE;
	}

	bGrabCopyStart = FALSE;
	bGrabCopyEnd = FALSE;

	if (rslt)
	{
		m_pThreadGrabCopy = AfxBeginThread(GrabCopyThread, this, THREAD_PRIORITY_NORMAL);
		m_pThreadGrabCopy->m_bAutoDelete = TRUE;

		if (m_pThreadGrabCopy == NULL)
		{
			rslt = FALSE;
		}
	}
	memset(m_bGrabCopy, TRUE, sizeof(BOOL) * 100);

	return rslt;
}

UINT MILCTRL::GrabCopyThread(LPVOID pParam)
{
	MILCTRL *pdlg = (MILCTRL *)pParam;

	pdlg->GrabCopyThread();

	return 0;
}

void MILCTRL::GrabCopyThread()
{
	while (TRUE)
	{
		if (bGrabCopyStart)
		{
			if (!m_bGrabCopy[m_nGrabCopyIdx])
			{
				//bGrabCopyStart = FALSE;
				//m_VisionGrab_CS.Lock();
				MbufCopy(m_TempBuf, m_GrabBuf);
				m_bGrabCopy[m_nGrabCopyIdx] = TRUE;
			}
		}

		if (bGrabCopyEnd)
		{
			break;
		}
		Sleep(1);
	}
}

UINT MILCTRL::LIneScanGrabThread(LPVOID pParam)
{
	MILCTRL *pdlg = (MILCTRL *)pParam;
	pdlg->SetGrabThreadExit(FALSE);
	while (TRUE)
	{
		if (pdlg->GetGrabThreadStart())
		{
			if (!pdlg->GetGrabEnd() && pdlg->GetGrabStart())
			{
				pdlg->SetLineScanGrab();
			}
		}

		if (pdlg->GetGrabThreadEnd())
		{
			break;
		}

		Sleep(1);
	}

	pdlg->SetGrabThreadExit(TRUE);
	return 0;
}

BOOL MILCTRL::SetLineScanGrab()
{
	BOOL rslt = TRUE;
	m_nGrabLineCnt = 0;
	m_bGrabWait = FALSE;
	if (m_bLineScan)
	{
		m_nCamErrorType = 0;

		//af on
		m_cAtcCtrl->SetAfOnOff(TRUE);
		if (!LineScanGrab(m_Disp))
		{
			rslt = FALSE;
			ErrorTypeView(m_nCamErrorType);
		}

		//af off
		m_cAtcCtrl->SetAfOnOff(FALSE);
	}
	m_bGrabWait = FALSE;
	SetGrabStart(FALSE);
	SetGrabEnd(TRUE);

	//MbufExport(_T("d:\\temp2.bmp"), M_BMP, m_TempBuf);

	return rslt;
}

void MILCTRL::ErrorTypeView(int nErrorType)
{
	CString strText;
	switch (nErrorType)
	{
	case 1:
		strText.Format(_T("MIL Create Error !!"));
		break;
	case 2:
		strText.Format(_T("MIL Free Path Error !!"));
		break;
	case 3:
		strText.Format(_T("MIL Grab TimeOut Error !!"));
		break;
	}
	AfxMessageBox(strText);
}

void MILCTRL::SetGrab()
{
	m_nGrabLineCnt = 0;
	m_nGrabCopyIdx = 0;
	memset(m_bGrabCopy, TRUE, sizeof(BOOL) * 100);
	//MbufExport(_T("d:\\temp1.bmp"), M_BMP, m_TempBuf);
	//MbufClear(m_TempBuf, 0x00);
	//MbufClear(m_GrabBuf, 0x00);
	SetGrabStart(TRUE);
	SetGrabEnd(FALSE);
}

BOOL MILCTRL::LineScanGrab(long disp)
{
	BOOL rslt = TRUE;
	
	if (!m_bIsCreated)
	{
		m_nCamErrorType = 1;
		return	FALSE;
	}
	if (m_bIsFreePathMIL)
	{
		m_nCamErrorType = 2;
		return	TRUE;
	}
	//g_MILCTRL_CS.Lock();
	m_bGrabEnd = FALSE;
	MILCTRL_IF_FAILED(MdigControl(m_Dig, M_GRAB_ABORT, M_DEFAULT))
	{
		m_nCamErrorType = 3;
		//g_MILCTRL_CS.Unlock();
		return	FALSE;
	}

	if (!m_bDoubleBuf)
	{
		MILCTRL_IF_FAILED(MdigGrab(m_Dig, m_GrabBuf))
		{
			m_nCamErrorType = 4;
			//g_MILCTRL_CS.Unlock();

			if (m_bGrabThreadEnd || m_bGrabEnd)
			{
				return FALSE;
			}
			return	FALSE;
		}

		MILCTRL_IF_FAILED(MdigGrabWait(m_Dig, M_GRAB_END))
		{
			m_nCamErrorType = 3;
			//g_MILCTRL_CS.Unlock();
			if (m_bGrabThreadEnd)
			{
				return FALSE;
			}
			return	FALSE;
		}
	}
	else
	{
		bGrabCopyStart = TRUE;
		MdigGrabWait(m_Dig, M_GRAB_END);

		int i = 0, cnt = MAX_GRAB_HEIGHT / m_lGrabHeight, x = 0;
		MILCTRL_IF_FAILED(MdigGrab(m_Dig, m_MilGrabBuf[0]))
		{
			m_nCamErrorType = 3;
			//g_MILCTRL_CS.Unlock();
			//AfxMessageBox(_T("Grab TimeOut!!!"));
			MbufCopyClip(m_MilGrabBuf[0], m_TempBuf, 0, (m_lGrabHeight * 0));
			//bGrabCopyStart = TRUE;
			if (m_bGrabThreadEnd || m_bGrabEnd)
			{
				return FALSE;
			}
			return	FALSE;
		}
		m_bGrabWait = TRUE;
		m_nGrabLineCnt++;
		//bGrabCopyStart = TRUE;
		//Grab
		for (i = 0; i < (cnt - 1); i++)
		{
			x = 1 - (i % 2);
			//Grab
			MILCTRL_IF_FAILED(MdigGrab(m_Dig, m_MilGrabBuf[x]))
			{
				m_nCamErrorType = 3;
				//g_MILCTRL_CS.Unlock();
				//AfxMessageBox(_T("Grab TimeOut!!!"));
				MbufCopyClip(m_MilGrabBuf[i % 2], m_TempBuf, 0, (m_lGrabHeight * i));
				//bGrabCopyStart = TRUE;
				if (m_bGrabThreadEnd || m_bGrabEnd)
				{
					return FALSE;
				}
				return	FALSE;
			}
			MbufCopyClip(m_MilGrabBuf[i % 2], m_TempBuf, 0, (m_lGrabHeight * i));

			m_nGrabLineCnt++;
			//bGrabCopyStart = TRUE;

			if (m_bGrabThreadEnd || m_bGrabEnd)
			{
				//g_MILCTRL_CS.Unlock();
				return FALSE;
			}
		}
		MdigGrabWait(m_Dig, M_GRAB_END);
		MbufCopyClip(m_MilGrabBuf[i % 2], m_TempBuf, 0, (m_lGrabHeight * i));
		m_nGrabLineCnt++;
		//bGrabCopyStart = TRUE;
	}

	//long pitch;
	//MbufInquire(m_GrabBuf, M_PITCH, &pitch);

	MdigControl(m_Dig, M_GRAB_ABORT, M_DEFAULT);
	MbufCopy(m_TempBuf, m_GrabBuf);
	//g_MILCTRL_CS.Unlock();
	if (m_bGrabThreadEnd)
	{
		return FALSE;
	}
	m_bGrabEnd = TRUE;

	return rslt;
}

BOOL MILCTRL::GetATFStatus()
{
	//for (int i = 0; i < 16; i++)
	{
		m_cAtcCtrl->GetAFCState(0);
	}

	return TRUE;
}

BOOL MILCTRL::SetATFLaserOn()
{
	return m_cAtcCtrl->SetAFCLaserOnOff(TRUE);
}
BOOL MILCTRL::SetATFLaserOff()
{
	return m_cAtcCtrl->SetAFCLaserOnOff(FALSE);
}
BOOL MILCTRL::SetATFAFOn()
{
	return m_cAtcCtrl->SetAfOnOff(TRUE);
}
BOOL MILCTRL::SetATFAFOff()
{
	return m_cAtcCtrl->SetAfOnOff(FALSE);
}

void MILCTRL::LoadImage(CString strPath)
{
	MbufImport(strPath, M_BMP, M_LOAD, m_Sys, &m_GrabBuf);
}