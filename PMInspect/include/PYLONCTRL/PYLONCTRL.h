#ifndef	__PYLONCTRL_H__
#define	__PYLONCTRL_H__

#include <pylon/PylonIncludes.h>

using namespace Pylon;

#define	g_PylonCtrl	PYLONCTRL::GetInstance()

const	enum PYLONCTRL_COLORTYPE
{
	PYLONCTRL_COLORTYPE_MONO,		//	RAW ��� �̹���(20180718 ���� �̰͸� ����)
	PYLONCTRL_COLORTYPE_BAYERGR,	//	RAW ���̾� ���� �̹���
	PYLONCTRL_COLORTYPE_BAYERRG,	//	RAW ���̾� ���� �̹���
	PYLONCTRL_COLORTYPE_BAYERGB,	//	RAW ���̾� ���� �̹���
	PYLONCTRL_COLORTYPE_BAYERBG,	//	RAW ���̾� ���� �̹���
	PYLONCTRL_COLORTYPE_RGB,		//	�÷� �̹���(RGB ����)
	PYLONCTRL_COLORTYPE_BGR,		//	�÷� �̹���(BGR ����)
	PYLONCTRL_COLORTYPE_RGBA,		//	�÷� �̹���(RGBA ����)
	PYLONCTRL_COLORTYPE_BGRA,		//	�÷� �̹���(BGRA ����)
	PYLONCTRL_COLORTYPE_YUV411,
	PYLONCTRL_COLORTYPE_YUV422,
	PYLONCTRL_COLORTYPE_YUV444
};

typedef	struct PYLONCTRL_IMG
{
	int					m_Width		;	//	���� ������
	int					m_Height	;	//	���� ������
	int					m_Bpp		;	//	Bit per Pixel
	int					m_Channel	;	//	ä��(MONO, BAYER -> 1, RGB, BGR -> 3)
	PYLONCTRL_COLORTYPE	m_ColorType	;	//	�÷� Ÿ��
	int					m_DataSize	;	//	��ü ������ ������(����Ʈ ��)
	BYTE*				m_pData		;	//	�̹��� ������ ���� ������
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
	Grab �̹����� ǥ���� ������ �ڵ��� ����
	*/
	BOOL	SetDisplayWindow( HWND hWndDisplay )	;

	/*
	���� ����Ǿ��ִ� Grab �̹����� ����
	AllocNewBuffer�� TRUE�� ��� pImage->m_pData�� ���� ���۸� �Ҵ��Ͽ� ������ ����
	�̶� pImage->m_pData�� NULL�� �ƴϸ� FALSE ��ȯ
	AllocNewBuffer�� FALSE�� ��� pImage->m_pData�� �ٷ� ������ ����
	�̶� pImage->m_pData�� NULL�̸� FALSE ��ȯ
	*/
	BOOL	GetImage( P_PYLONCTRL_IMGINFO pImage, BOOL AllocNewBuffer = FALSE )	;

}PYLONCTRL, *P_PYLONCTRL	;

#endif