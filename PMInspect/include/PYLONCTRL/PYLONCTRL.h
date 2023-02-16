#ifndef	__PYLONCTRL_H__
#define	__PYLONCTRL_H__

#include <pylon/PylonIncludes.h>

using namespace Pylon;

#define	g_PylonCtrl	PYLONCTRL::GetInstance()

const	enum PYLONCTRL_COLORTYPE
{
	PYLONCTRL_COLORTYPE_MONO,		//	RAW 모노 이미지(20180718 현재 이것만 지원)
	PYLONCTRL_COLORTYPE_BAYERGR,	//	RAW 베이어 패턴 이미지
	PYLONCTRL_COLORTYPE_BAYERRG,	//	RAW 베이어 패턴 이미지
	PYLONCTRL_COLORTYPE_BAYERGB,	//	RAW 베이어 패턴 이미지
	PYLONCTRL_COLORTYPE_BAYERBG,	//	RAW 베이어 패턴 이미지
	PYLONCTRL_COLORTYPE_RGB,		//	컬러 이미지(RGB 순서)
	PYLONCTRL_COLORTYPE_BGR,		//	컬러 이미지(BGR 순서)
	PYLONCTRL_COLORTYPE_RGBA,		//	컬러 이미지(RGBA 순서)
	PYLONCTRL_COLORTYPE_BGRA,		//	컬러 이미지(BGRA 순서)
	PYLONCTRL_COLORTYPE_YUV411,
	PYLONCTRL_COLORTYPE_YUV422,
	PYLONCTRL_COLORTYPE_YUV444
};

typedef	struct PYLONCTRL_IMG
{
	int					m_Width		;	//	가로 사이즈
	int					m_Height	;	//	세로 사이즈
	int					m_Bpp		;	//	Bit per Pixel
	int					m_Channel	;	//	채널(MONO, BAYER -> 1, RGB, BGR -> 3)
	PYLONCTRL_COLORTYPE	m_ColorType	;	//	컬러 타입
	int					m_DataSize	;	//	전체 데이터 사이즈(바이트 수)
	BYTE*				m_pData		;	//	이미지 데이터 시작 포인터
}PYLONCTRL_IMGINFO, *P_PYLONCTRL_IMGINFO	;

typedef	class PYLONCTRL
{
	////////////////////////////////////////////////////////////////////////
	//	Singlton
private:
	static	PYLONCTRL	m_Instance;

	PYLONCTRL();
	~PYLONCTRL();
	PYLONCTRL(const PYLONCTRL&) {}
	void operator=(const PYLONCTRL&) {}

public:
	static	PYLONCTRL&	GetInstance() { return	m_Instance; }
	/////////////////////////////////////////////////////////////////////////


private:
	BOOL			m_bCamConnected	;
	CInstantCamera	m_Camera		;
	HWND			m_hWndDisplay	;
	PYLONCTRL_IMG	m_Image			;

public:
	BOOL	Initialize()	;
	BOOL	Grab()			;
	BOOL	Terminate()		;

	/*
	Grab 이미지를 표시할 윈도우 핸들을 설정
	*/
	BOOL	SetDisplayWindow( HWND hWndDisplay )	;

	/*
	현재 저장되어있는 Grab 이미지를 복사
	AllocNewBuffer가 TRUE일 경우 pImage->m_pData에 새로 버퍼를 할당하여 데이터 복사
	이때 pImage->m_pData가 NULL이 아니면 FALSE 반환
	AllocNewBuffer가 FALSE일 경우 pImage->m_pData에 바로 데이터 복사
	이때 pImage->m_pData가 NULL이면 FALSE 반환
	*/
	BOOL	GetImage( P_PYLONCTRL_IMGINFO pImage, BOOL AllocNewBuffer = FALSE )	;

}PYLONCTRL, *P_PYLONCTRL	;

#endif