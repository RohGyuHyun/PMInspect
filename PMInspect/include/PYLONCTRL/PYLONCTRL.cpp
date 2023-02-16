#include "stdafx.h"
#include "PYLONCTRL.h"

PYLONCTRL	PYLONCTRL::m_Instance;

PYLONCTRL::PYLONCTRL()
{
	m_bCamConnected	=	FALSE	;
	m_hWndDisplay	=	NULL	;
	ZeroMemory(&m_Image, sizeof(PYLONCTRL_IMG));
}

PYLONCTRL::~PYLONCTRL()
{
	Terminate();
}

BOOL	PYLONCTRL::Initialize()
{
	PylonInitialize()	;

	DeviceInfoList_t	devices	;	//	연결된 카메라 정보 목록

	devices.clear();
	int	NumCam = CTlFactory::GetInstance().EnumerateDevices(devices);

	if (NumCam < 1)	//	연결된 카메라가 없으면 FALSE 반환
		return	FALSE;
	else if (NumCam > 1)	//	연결된 카메라가 2개 이상이면 FALSE 반환
	{
		devices.clear();
		return	FALSE;
	}

	m_Camera.Attach(CTlFactory::GetInstance().CreateFirstDevice());

	GenApi::INodeMap&	NodeMap = m_Camera.GetNodeMap();

	m_Camera.Open();
	GenApi::CIntegerPtr		Width = NodeMap.GetNode("Width");
	GenApi::CIntegerPtr		Height = NodeMap.GetNode("Height");
	GenApi::CEnumerationPtr	PixelFormat = NodeMap.GetNode("PixelFormat");

	m_Image.m_Width = Width->GetValue();
	m_Image.m_Height = Height->GetValue();

	EPixelType	PixelType = (EPixelType)PixelFormat->GetIntValue();

	//	베이어, 모노, 컬러, 팩 등은 별도 구분할 방법이 없어 하드 코딩
	switch (PixelType)
	{
		case	PixelType_Undefined		:
		case	PixelType_Mono1packed	:
		case	PixelType_Mono2packed	:
		case	PixelType_Mono4packed	:
		case	PixelType_Mono8			:
		case	PixelType_Mono8signed	:
		case	PixelType_Mono10		:
		case	PixelType_Mono10packed	:
		case	PixelType_Mono10p		:
		case	PixelType_Mono12		:
		case	PixelType_Mono12packed	:
		case	PixelType_Mono12p		:
		case	PixelType_Mono16		:
		case	PixelType_Double		:
			m_Image.m_ColorType	=	PYLONCTRL_COLORTYPE_MONO	;
			m_Image.m_Channel	=	1							;
			break;

		case	PixelType_BayerGR8			:
		case	PixelType_BayerGR10			:
		case	PixelType_BayerGR12			:
		case	PixelType_BayerGR12Packed	:
		case	PixelType_BayerGR10p		:
		case	PixelType_BayerGR12p		:
		case	PixelType_BayerGR16			:
			m_Image.m_ColorType	=	PYLONCTRL_COLORTYPE_BAYERGR	;
			m_Image.m_Channel	=	1							;
			break;

		case	PixelType_BayerRG8			:
		case	PixelType_BayerRG10			:
		case	PixelType_BayerRG12			:
		case	PixelType_BayerRG12Packed	:
		case	PixelType_BayerRG10p		:
		case	PixelType_BayerRG12p		:
		case	PixelType_BayerRG16			:
			m_Image.m_ColorType	=	PYLONCTRL_COLORTYPE_BAYERRG	;
			m_Image.m_Channel	=	1							;
			break;

		case	PixelType_BayerGB8			:
		case	PixelType_BayerGB10			:
		case	PixelType_BayerGB12			:
		case	PixelType_BayerGB12Packed	:
		case	PixelType_BayerGB10p		:
		case	PixelType_BayerGB12p		:
		case	PixelType_BayerGB16			:
			m_Image.m_ColorType	=	PYLONCTRL_COLORTYPE_BAYERGB	;
			m_Image.m_Channel	=	1							;
			break;

		case	PixelType_BayerBG8			:
		case	PixelType_BayerBG10			:
		case	PixelType_BayerBG12			:
		case	PixelType_BayerBG12Packed	:
		case	PixelType_BayerBG10p		:
		case	PixelType_BayerBG12p		:
		case	PixelType_BayerBG16			:
			m_Image.m_ColorType	=	PYLONCTRL_COLORTYPE_BAYERBG	;
			m_Image.m_Channel	=	1							;
			break;

		case	PixelType_RGB8packed	:
		case	PixelType_RGB10packed	:
		case	PixelType_RGB12packed	:
		case	PixelType_RGB16packed	:
		case	PixelType_RGB8planar	:
		case	PixelType_RGB10planar	:
		case	PixelType_RGB12planar	:
		case	PixelType_RGB16planar	:
		case	PixelType_RGB12V1packed	:
			m_Image.m_ColorType	=	PYLONCTRL_COLORTYPE_RGB	;
			m_Image.m_Channel	=	1						;
			break;

		case	PixelType_BGR8packed	:
		case	PixelType_BGR10packed	:
		case	PixelType_BGR12packed	:
		case	PixelType_BGR10V1packed	:
		case	PixelType_BGR10V2packed	:
			m_Image.m_ColorType	=	PYLONCTRL_COLORTYPE_BGR	;
			m_Image.m_Channel	=	1						;
			break;

		case	PixelType_RGBA8packed	:
			m_Image.m_ColorType	=	PYLONCTRL_COLORTYPE_RGBA	;
			m_Image.m_Channel	=	1							;
			break;

		case	PixelType_BGRA8packed	:
			m_Image.m_ColorType	=	PYLONCTRL_COLORTYPE_BGRA	;
			m_Image.m_Channel	=	1							;
			break;

		case	PixelType_YUV411packed	:
			m_Image.m_ColorType	=	PYLONCTRL_COLORTYPE_YUV411	;
			m_Image.m_Channel	=	1							;
			break;

		case	PixelType_YUV422packed			:
		case	PixelType_YUV422_YUYV_Packed	:
			m_Image.m_ColorType	=	PYLONCTRL_COLORTYPE_YUV422	;
			m_Image.m_Channel	=	1							;
			break;

		case	PixelType_YUV444packed	:
			m_Image.m_ColorType	=	PYLONCTRL_COLORTYPE_YUV444	;
			m_Image.m_Channel	=	1							;
			break;
	}

	m_Image.m_ColorType = PYLONCTRL_COLORTYPE_MONO;

	m_Image.m_Bpp = (PixelType & 0x00FF0000) >> 16;

	int	BytesPerPixel	= m_Image.m_Bpp / 8	;
	if (m_Image.m_Bpp % 8 > 0)
		BytesPerPixel++;
	m_Image.m_DataSize = m_Image.m_Width * m_Image.m_Height * BytesPerPixel;

	m_Image.m_pData = new	BYTE[m_Image.m_DataSize];

	m_bCamConnected = TRUE;
	m_Camera.Close();

	return	m_bCamConnected;
}

BOOL	PYLONCTRL::Grab()
{
	if (!m_bCamConnected)
		return	FALSE;

	CGrabResultPtr ptrGrabResult;
	if (m_Camera.GrabOne(500, ptrGrabResult, TimeoutHandling_ThrowException))
	{
		memcpy(m_Image.m_pData, (BYTE*)ptrGrabResult->GetBuffer(), m_Image.m_Width*m_Image.m_Height);
	}
	else
	{
		return	FALSE;
	}

	return	TRUE;
}

BOOL	PYLONCTRL::Terminate()
{
	if (m_Camera.IsOpen())
	{
		m_Camera.Close();
		m_bCamConnected = FALSE;
	}

	m_Camera.DetachDevice();

	PylonTerminate();

	m_hWndDisplay = NULL;
	if (m_Image.m_pData != NULL)
		delete[]	m_Image.m_pData	;
	ZeroMemory(&m_Image, sizeof(PYLONCTRL_IMG));

	return	TRUE;
}

BOOL	PYLONCTRL::SetDisplayWindow(HWND hWndDisplay)
{
	return	FALSE;
}

BOOL	PYLONCTRL::GetImage(P_PYLONCTRL_IMGINFO pImage, BOOL AllocNewBuffer/*FALSE*/)
{
	if (!m_bCamConnected || pImage == NULL )
		return	FALSE;

	if (AllocNewBuffer)
	{
		if (pImage->m_pData != NULL)
		{
			return	FALSE;
		}

		if (m_Image.m_Bpp <= 8)
		{
			pImage->m_pData = new	BYTE[m_Image.m_DataSize];
		}
		else if (m_Image.m_Bpp <= 16)
		{
			pImage->m_pData = new	BYTE[m_Image.m_DataSize];
		}
	}
	else
	{
		if (pImage->m_pData == NULL)
		{
			return	FALSE;
		}

	}

	pImage->m_Width = m_Image.m_Width;
	pImage->m_Height = m_Image.m_Height;
	pImage->m_Bpp = m_Image.m_Bpp;
	pImage->m_Channel = m_Image.m_Channel;
	pImage->m_ColorType = m_Image.m_ColorType;
	pImage->m_DataSize = m_Image.m_DataSize;
	memcpy(pImage->m_pData, m_Image.m_pData, m_Image.m_DataSize);

	return	TRUE;
}
