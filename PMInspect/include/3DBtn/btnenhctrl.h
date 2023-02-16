#pragma once
#include "_Font.h"

// 컴퓨터에서 Microsoft Visual C++를 사용하여 생성한 IDispatch 래퍼 클래스입니다.

// 참고: 이 파일의 내용을 수정하지 마십시오. Microsoft Visual C++에서
//  이 클래스를 다시 생성할 때 수정한 내용을 덮어씁니다.

/////////////////////////////////////////////////////////////////////////////
// CBtnenhctrl 래퍼 클래스입니다.

class CBtnenhctrl : public CWnd
{
protected:
	DECLARE_DYNCREATE(CBtnenhctrl)
public:
	CLSID const& GetClsid()
	{
		static CLSID const clsid
			= { 0x40B5CE81, 0xC5A8, 0x11D2, { 0x81, 0x83, 0x0, 0x0, 0x24, 0x40, 0xDF, 0xD8 } };
		return clsid;
	}
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle,
						const RECT& rect, CWnd* pParentWnd, UINT nID, 
						CCreateContext* pContext = NULL)
	{ 
		return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID); 
	}

    BOOL Create(LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, 
				UINT nID, CFile* pPersist = NULL, BOOL bStorage = FALSE,
				BSTR bstrLicKey = NULL)
	{ 
		return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID,
		pPersist, bStorage, bstrLicKey); 
	}

// 특성입니다.
public:
enum
{
    COLOR_GRADIENT_CLASSIC = 0,
    COLOR_GRADIENT_LINEAR = 1
}enumGradientType;
enum
{
    COLOR_RENDER_BEST_MATCH = 0,
    COLOR_RENDER_BEST_3D = 1,
    COLOR_RENDER_LINEAR_MATCH = 2,
    COLOR_RENDER_HYPER_3D = 3,
    COLOR_RENDER_2_COLORS = 4
}enumGradientRender;
enum
{
    EFFECT_NONE = 0,
    EFFECT_RAISED = 1,
    EFFECT_SUNKEN = 2,
    EFFECT_FLOATING = 3,
    EFFECT_HALO = 4,
    EFFECT_RAISEDHALO = 5,
    EFFECT_SUNKENHALO = 6,
    EFFECT_FLOATINGHALO = 7
}enumTextEffects;
enum
{
    TEXTURE_NONE = 0,
    TEXTURE_WHOLE = 1,
    TEXTURE_OUTSIDE = 2,
    TEXTURE_INSIDE = 3
}enumTextureModes;
enum
{
    CELL_SCALE_UNIT_PERCENTAGE = 0,
    CELL_SCALE_UNIT_PIXELS = 1
}enumScaleUnits;
enum
{
    PICTURE_NORMAL = 0,
    PICTURE_PRESSED = 1,
    PICTURE_MOUSEOVER = 2,
    PICTURE_CUSTOM = 3,
    PICTURE_TEXTURE = 4,
    PICTURE_DISABLED = 5
}enumPictures;
enum
{
    HALIGN_LEFT = 0,
    HALIGN_CENTER = 1,
    HALIGN_RIGHT = 2
}enumHorzAlign;
enum
{
    VALIGN_TOP = 0,
    VALIGN_CENTER = 1,
    VALIGN_BOTTOM = 2
}enumVertAlign;
enum
{
    SHAPE_RECTANGLE = 0,
    SHAPE_ROUNDRECT = 1,
    SHAPE_PARALLEL = 2,
    //SHAPE_ELLIPSE = 3,
    SHAPE_GEM = 4,
    SHAPE_CUSTOM = 5,
    SHAPE_TRIANGLE = 6,
    SHAPE_TRAPEZE = 7,
    SHAPE_ARROW = 8,
    SHAPE_HONEYCOMB_V = 9,
    SHAPE_HONEYCOMB_H = 10,
    SHAPE_HONEYCOMB_45 = 11,
    SHAPE_HONEYCOMB_135 = 12,
    SHAPE_DIAMOND = 13,
    SHAPE_KEYHOLE = 14,
    SHAPE_4LEAFCLOVER_V = 15,
    SHAPE_4LEAFCLOVER_D = 16,
    SHAPE_ALFALFA = 17
}enumShapes;
enum
{
    SURFACE_HARD = 0,
    SURFACE_SOFT = 1,
    SURFACE_FLAT = 2,
    SURFACE_FLATPIL = 3,
    SURFACE_SUNKEN = 4,
    SURFACE_PLASTIC = 5,
    SURFACE_LUCID = 6,
    SURFACE_NEON = 7,
    SURFACE_AQUA = 8,
    SURFACE_GEL = 9,
    SURFACE_PLASTIC_SHAPED = 10,
    SURFACE_NEON_SHAPED = 11,
    SURFACE_AQUA_SHAPED = 12,
    SURFACE_GEL_SHAPED = 13,
    SURFACE_VISTA_AERO = 14,
    SURFACE_OFFICE_2003 = 15,
    SURFACE_OFFICE_2007 = 16
}enumSurfaces;
enum
{
    SPECIAL_EFFECT_NONE = 0,
    SPECIAL_EFFECT_RAISED = 1,
    SPECIAL_EFFECT_SUNKEN = 2,
    SPECIAL_EFFECT_SUNKEN_SIMPLE = 3
}enumSpecEffects;
enum
{
    FRAME_EFFECT_RAISED = 0,
    FRAME_EFFECT_SUNKEN = 1,
    FRAME_EFFECT_SUNKEN_SIMPLE = 2
}enumFrameEffects;
enum
{
    TRANSPMODE_USECOLOR = 0,
    TRANSPMODE_USELEFTTOP_PIXEL = 1
}enumPictTranspModes;
enum
{
    GRD_VERYHIGH = 0,
    GRD_HIGH = 1,
    GRD_AVERAGE = 2,
    GRD_LOW = 3,
    GRD_VERYLOW = 4
}enumGrds;
enum
{
    DIR_ALL = 0,
    DIR_WEST = 1,
    DIR_NORTH = 2,
    DIR_EAST = 3,
    DIR_SOUTH = 4,
    DIR_NW = 5,
    DIR_NE = 6,
    DIR_SE = 7,
    DIR_SW = 8
}enumOrientations;
enum
{
    SMOOTH_NONE = 0,
    SMOOTH_LOW = 1,
    SMOOTH_HIGH = 2
}enumSmooths;
enum
{
    SHADOW_NONE = 0,
    SHADOW_SOLID = 1,
    SHADOW_HALFBLURRED = 2,
    SHADOW_BLURRED = 3
}enumShadowModes;
enum
{
    STYLE_PUSHBUTTON = 0,
    STYLE_CHECKBOX = 1,
    STYLE_RADIOBUTTON = 2,
    STYLE_FRAME = 3,
    STYLE_PICTURE = 4,
    STYLE_CHECKBOX_STD = 5,
    STYLE_RADIOBUTTON_STD = 6,
    STYLE_LABEL = 7,
    STYLE_PROGRESS_BAR = 8,
    STYLE_SPLIT_BUTTON = 9
}enumStyles;
enum
{
    ALIGN_LEFT = 0,
    ALIGN_CENTER = 1,
    ALIGN_RIGHT = 2
}enumCaptionAlignment;
enum
{
    FOCUS_NONE = 0,
    FOCUS_BORDER = 1,
    FOCUS_COLOR = 2
}enumFocusModes;
enum
{
    PICTPOS_CM = 0,
    PICTPOS_LT = 1,
    PICTPOS_CT = 2,
    PICTPOS_RT = 3,
    PICTPOS_LM = 4,
    PICTPOS_RM = 5,
    PICTPOS_LB = 6,
    PICTPOS_CB = 7,
    PICTPOS_RB = 8,
    PICTPOS_TILE = 9,
    PICTPOS_STRETCH = 10,
    PICTPOS_SIZETOCONTENT = 11
}enumPicturePositions;
enum
{
    SOUND_DOWN = 0,
    SOUND_UP = 1,
    SOUND_MOUSEOVER = 2,
    SOUND_MOUSEOUT = 3
}enumSounds;
enum
{
    RT_TPL_TYPE_PICTURE = 0,
    RT_TPL_TYPE_TEXTURE = 1,
    RT_TPL_TYPE_SOUND = 2,
    RT_TPL_TYPE_CURSOR = 3
}enumTplBinaryTypes;
enum
{
    LIGHT_DIR_NW = 0,
    LIGHT_DIR_NORTH = 1,
    LIGHT_DIR_NE = 2,
    LIGHT_DIR_EAST = 3,
    LIGHT_DIR_SE = 4,
    LIGHT_DIR_SOUTH = 5,
    LIGHT_DIR_SW = 6,
    LIGHT_DIR_WEST = 7,
    LIGHT_DIR_ABOVE = 8
}enumLightDirections;
enum
{
    EXPORT_FORMAT_BMP = 0,
    EXPORT_FORMAT_PNG = 1,
    EXPORT_FORMAT_JPG = 2,
    EXPORT_FORMAT_CLIPBOARD = 3,
    EXPORT_FORMAT_GIF = 4,
    EXPORT_FORMAT_GIF_TRANSP = 5,
    EXPORT_FORMAT_PNG_TRANSP = 6
}enumExportFormats;
enum
{
    EXPORT_IMAGE_NORMAL = 0,
    EXPORT_IMAGE_MOUSEOVER = 1,
    EXPORT_IMAGE_PRESSED = 2,
    EXPORT_IMAGE_FOCUS = 3,
    EXPORT_IMAGE_DISABLED = 4
}enumExportImages;
enum
{
    TRANSP_ZONE_WHOLE = 0,
    TRANSP_ZONE_OUTSIDE = 1,
    TRANSP_ZONE_INSIDE = 2
}enumTransparentZones;
enum
{
    VISTA_LOOK_STANDARD = 0,
    VISTA_LOOK_HOVER = 1,
    VISTA_LOOK_CHECKED = 2,
    VISTA_LOOK_FLAT = 3
}enumVistaLooks;
enum
{
    VISTA_GLOSSY_SHAPE_RECT = 0,
    VISTA_GLOSSY_SHAPE_ELLIPSE_LARGE = 1,
    VISTA_GLOSSY_SHAPE_ELLIPSE_MIDDLE = 2,
    VISTA_GLOSSY_SHAPE_ELLIPSE_SMALL = 3
}enumVistaGlossyShapes;
enum
{
    SPOTLIGHT_TYPE_RADIAL = 0,
    SPOTLIGHT_TYPE_LINEAR = 1,
    SPOTLIGHT_TYPE_BELL_SHAPED = 2
}enumSpotlightTypes;
enum
{
    PROGRESS_BAR_HORIZONTAL_WE = 0,
    PROGRESS_BAR_HORIZONTAL_EW = 1,
    PROGRESS_BAR_VERTICAL_SN = 2,
    PROGRESS_BAR_VERTICAL_NS = 3,
    PROGRESS_BAR_RADIAL_0_CW = 4,
    PROGRESS_BAR_RADIAL_90_CW = 5,
    PROGRESS_BAR_RADIAL_180_CW = 6,
    PROGRESS_BAR_RADIAL_270_CW = 7,
    PROGRESS_BAR_RADIAL_0_ACW = 8,
    PROGRESS_BAR_RADIAL_90_ACW = 9,
    PROGRESS_BAR_RADIAL_180_ACW = 10,
    PROGRESS_BAR_RADIAL_270_ACW = 11
}enumProgressBarTypes;
enum
{
    PROGRESS_BAR_FILL_WHOLE = 0,
    PROGRESS_BAR_FILL_OUTSIDE = 1,
    PROGRESS_BAR_FILL_INSIDE = 2
}enumProgressBarFillModes;
enum
{
    SPLIT_BUTTON_TYPE_VERTICAL_LEFT = 0,
    SPLIT_BUTTON_TYPE_HORIZONTAL_BOTTOM = 1
}enumSplitButtonTypes;
enum
{
    TEXTPOS_CAPTION = 0,
    TEXTPOS_LT = 1,
    TEXTPOS_CT = 2,
    TEXTPOS_RT = 3,
    TEXTPOS_LM = 4,
    TEXTPOS_RM = 5,
    TEXTPOS_LB = 6,
    TEXTPOS_CB = 7,
    TEXTPOS_RB = 8
}enumTextPositions;


// 작업입니다.
public:

// _DBtnEnh

// Functions
//

	void SetAuxText(short position, LPCTSTR Text, unsigned long Color, unsigned long ColorMouseOver, short xOffset, short yOffset, short iEffect, short nFactor)
	{
		static BYTE parms[] = VTS_I2 VTS_BSTR VTS_UI4 VTS_UI4 VTS_I2 VTS_I2 VTS_I2 VTS_I2 ;
		InvokeHelper(0x31, DISPATCH_METHOD, VT_EMPTY, NULL, parms, position, Text, Color, ColorMouseOver, xOffset, yOffset, iEffect, nFactor);
	}
	LPUNKNOWN GetControlPtr()
	{
		LPUNKNOWN result;
		InvokeHelper(0x32, DISPATCH_METHOD, VT_UNKNOWN, (void*)&result, NULL);
		return result;
	}
	void EnableSound(short nSound, BOOL bEnable)
	{
		static BYTE parms[] = VTS_I2 VTS_BOOL ;
		InvokeHelper(0x33, DISPATCH_METHOD, VT_EMPTY, NULL, parms, nSound, bEnable);
	}
	void DoClick()
	{
		InvokeHelper(DISPID_DOCLICK, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	void Refresh()
	{
		InvokeHelper(DISPID_REFRESH, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	void SetNSContainerColor(unsigned long Color)
	{
		static BYTE parms[] = VTS_UI4 ;
		InvokeHelper(0x34, DISPATCH_METHOD, VT_EMPTY, NULL, parms, Color);
	}
	CString GetAuxText(short position, unsigned long * pcolor, unsigned long * pcolorMouseOver, short * pxOffset, short * pyOffset, short * piEffect, short * pnFactor)
	{
		CString result;
		static BYTE parms[] = VTS_I2 VTS_PUI4 VTS_PUI4 VTS_PI2 VTS_PI2 VTS_PI2 VTS_PI2 ;
		InvokeHelper(0x35, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, position, pcolor, pcolorMouseOver, pxOffset, pyOffset, piEffect, pnFactor);
		return result;
	}
	long SetPictureFromHandle(long index, long hBmp)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_I4 ;
		InvokeHelper(0x36, DISPATCH_METHOD, VT_I4, (void*)&result, parms, index, hBmp);
		return result;
	}
	long SetPictureFromMemory(long index, VARIANT pData, long sizeData)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_VARIANT VTS_I4 ;
		InvokeHelper(0x37, DISPATCH_METHOD, VT_I4, (void*)&result, parms, index, &pData, sizeData);
		return result;
	}
	long SetAnimFromMemory(long index, VARIANT pData, long sizeData)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_VARIANT VTS_I4 ;
		InvokeHelper(0x38, DISPATCH_METHOD, VT_I4, (void*)&result, parms, index, &pData, sizeData);
		return result;
	}
	long SetCursorFromHandle(long hCursor)
	{
		long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x39, DISPATCH_METHOD, VT_I4, (void*)&result, parms, hCursor);
		return result;
	}
	long SetSoundFromMemory(long index, VARIANT pData, long sizeData)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_VARIANT VTS_I4 ;
		InvokeHelper(0x3a, DISPATCH_METHOD, VT_I4, (void*)&result, parms, index, &pData, sizeData);
		return result;
	}
	void SetAutomaticRefresh(BOOL bAutoRefresh)
	{
		static BYTE parms[] = VTS_BOOL ;
		InvokeHelper(0x3b, DISPATCH_METHOD, VT_EMPTY, NULL, parms, bAutoRefresh);
	}
	void SetSoundSync(long sound, BOOL flag)
	{
		static BYTE parms[] = VTS_I4 VTS_BOOL ;
		InvokeHelper(0x3c, DISPATCH_METHOD, VT_EMPTY, NULL, parms, sound, flag);
	}
	void EnableAcceleratorForVB()
	{
		InvokeHelper(0x3d, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	void SetTextVisible(short nText, BOOL bVisible)
	{
		static BYTE parms[] = VTS_I2 VTS_BOOL ;
		InvokeHelper(0x3e, DISPATCH_METHOD, VT_EMPTY, NULL, parms, nText, bVisible);
	}
	void SetCaptionText(LPCTSTR Text, unsigned long Color, unsigned long ColorMouseOver, short xOffset, short yOffset, short iEffect, short nFactor, short nAngle)
	{
		static BYTE parms[] = VTS_BSTR VTS_UI4 VTS_UI4 VTS_I2 VTS_I2 VTS_I2 VTS_I2 VTS_I2 ;
		InvokeHelper(0x3f, DISPATCH_METHOD, VT_EMPTY, NULL, parms, Text, Color, ColorMouseOver, xOffset, yOffset, iEffect, nFactor, nAngle);
	}
	CString GetCaptionText(unsigned long * pcolor, unsigned long * pcolorMouseOver, short * pxOffset, short * pyOffset, short * piEffect, short * pnFactor, short * pnAngle)
	{
		CString result;
		static BYTE parms[] = VTS_PUI4 VTS_PUI4 VTS_PI2 VTS_PI2 VTS_PI2 VTS_PI2 VTS_PI2 ;
		InvokeHelper(0x40, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, pcolor, pcolorMouseOver, pxOffset, pyOffset, piEffect, pnFactor, pnAngle);
		return result;
	}
	long SetPictureFromNet(long index, LPCTSTR strAddress, LPCTSTR strUsername, LPCTSTR strPassword)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_BSTR VTS_BSTR VTS_BSTR ;
		InvokeHelper(0x41, DISPATCH_METHOD, VT_I4, (void*)&result, parms, index, strAddress, strUsername, strPassword);
		return result;
	}
	long SetSoundFromNet(long index, LPCTSTR strAddress, LPCTSTR strUsername, LPCTSTR strPassword)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_BSTR VTS_BSTR VTS_BSTR ;
		InvokeHelper(0x42, DISPATCH_METHOD, VT_I4, (void*)&result, parms, index, strAddress, strUsername, strPassword);
		return result;
	}
	BOOL SetAnimFromNet(long index, LPCTSTR strAddress, LPCTSTR strUsername, LPCTSTR strPassword)
	{
		BOOL result;
		static BYTE parms[] = VTS_I4 VTS_BSTR VTS_BSTR VTS_BSTR ;
		InvokeHelper(0x43, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, index, strAddress, strUsername, strPassword);
		return result;
	}
	void SetSurfaceColor(short nElement, unsigned long Color, short nGradientFactor, short nRender3D)
	{
		static BYTE parms[] = VTS_I2 VTS_UI4 VTS_I2 VTS_I2 ;
		InvokeHelper(0x44, DISPATCH_METHOD, VT_EMPTY, NULL, parms, nElement, Color, nGradientFactor, nRender3D);
	}
	void MouseCapture(BOOL bCapture)
	{
		static BYTE parms[] = VTS_BOOL ;
		InvokeHelper(0x45, DISPATCH_METHOD, VT_EMPTY, NULL, parms, bCapture);
	}
	void GetSurfaceColor(short nElement, unsigned long * pcolor, short * pnGradientFactor, short * pnRender3D)
	{
		static BYTE parms[] = VTS_I2 VTS_PUI4 VTS_PI2 VTS_PI2 ;
		InvokeHelper(0x46, DISPATCH_METHOD, VT_EMPTY, NULL, parms, nElement, pcolor, pnGradientFactor, pnRender3D);
	}
	void InternalRefresh()
	{
		InvokeHelper(0x47, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	void SetAuxTextString(short position, LPCTSTR Text)
	{
		static BYTE parms[] = VTS_I2 VTS_BSTR ;
		InvokeHelper(0x48, DISPATCH_METHOD, VT_EMPTY, NULL, parms, position, Text);
	}
	BOOL ExportBtnImage(long nImage, LPCTSTR strFilename, long nFormat)
	{
		BOOL result;
		static BYTE parms[] = VTS_I4 VTS_BSTR VTS_I4 ;
		InvokeHelper(0x55, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, nImage, strFilename, nFormat);
		return result;
	}
	void EditProperties(unsigned long colorText, unsigned long colorBack, long x, long y)
	{
		static BYTE parms[] = VTS_UI4 VTS_UI4 VTS_I4 VTS_I4 ;
		InvokeHelper(0x56, DISPATCH_METHOD, VT_EMPTY, NULL, parms, colorText, colorBack, x, y);
	}
	short LoadTemplate(LPCTSTR strFilename)
	{
		short result;
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x59, DISPATCH_METHOD, VT_I2, (void*)&result, parms, strFilename);
		return result;
	}
	short SaveTemplate(LPCTSTR strFilename)
	{
		short result;
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x5a, DISPATCH_METHOD, VT_I2, (void*)&result, parms, strFilename);
		return result;
	}
	short LoadTemplateEx(LPCTSTR strFilename)
	{
		short result;
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x5b, DISPATCH_METHOD, VT_I2, (void*)&result, parms, strFilename);
		return result;
	}
	short SaveTemplateEx(LPCTSTR strFilename)
	{
		short result;
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x5c, DISPATCH_METHOD, VT_I2, (void*)&result, parms, strFilename);
		return result;
	}
	void SetHelpLink(short nPage, LPCTSTR strLink)
	{
		static BYTE parms[] = VTS_I2 VTS_BSTR ;
		InvokeHelper(0x5d, DISPATCH_METHOD, VT_EMPTY, NULL, parms, nPage, strLink);
	}
	void ShowEditingButtons(short lFlags, short bShow)
	{
		static BYTE parms[] = VTS_I2 VTS_I2 ;
		InvokeHelper(0x5e, DISPATCH_METHOD, VT_EMPTY, NULL, parms, lFlags, bShow);
	}
	void SetTextOnPos(short position, LPCTSTR strText)
	{
		static BYTE parms[] = VTS_I2 VTS_BSTR ;
		InvokeHelper(0x63, DISPATCH_METHOD, VT_EMPTY, NULL, parms, position, strText);
	}
	CString GetTextOnPos(short position)
	{
		CString result;
		static BYTE parms[] = VTS_I2 ;
		InvokeHelper(0x64, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, position);
		return result;
	}
	void SetTextSettingOnPos(short position, long pdesc, short nFlag)
	{
		static BYTE parms[] = VTS_I2 VTS_I4 VTS_I2 ;
		InvokeHelper(0x65, DISPATCH_METHOD, VT_EMPTY, NULL, parms, position, pdesc, nFlag);
	}
	void GetTextSettingOnPos(short position, long pdesc)
	{
		static BYTE parms[] = VTS_I2 VTS_I4 ;
		InvokeHelper(0x66, DISPATCH_METHOD, VT_EMPTY, NULL, parms, position, pdesc);
	}
	void SetSurfaceColorDesc(short nElement, long pdesc)
	{
		static BYTE parms[] = VTS_I2 VTS_I4 ;
		InvokeHelper(0x67, DISPATCH_METHOD, VT_EMPTY, NULL, parms, nElement, pdesc);
	}
	void GetSurfaceColorDesc(short nElement, long pdesc)
	{
		static BYTE parms[] = VTS_I2 VTS_I4 ;
		InvokeHelper(0x68, DISPATCH_METHOD, VT_EMPTY, NULL, parms, nElement, pdesc);
	}
	void SetEditingTplMode(short nMode)
	{
		static BYTE parms[] = VTS_I2 ;
		InvokeHelper(0x6a, DISPATCH_METHOD, VT_EMPTY, NULL, parms, nMode);
	}
	void SetTemplateBinDir(long nType, LPCTSTR strDirPath)
	{
		static BYTE parms[] = VTS_I4 VTS_BSTR ;
		InvokeHelper(0x6b, DISPATCH_METHOD, VT_EMPTY, NULL, parms, nType, strDirPath);
	}
	CString GetHelperString(short dispid, short nValue)
	{
		CString result;
		static BYTE parms[] = VTS_I2 VTS_I2 ;
		InvokeHelper(0x7d, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, dispid, nValue);
		return result;
	}
	void ResetProperties()
	{
		InvokeHelper(0x7e, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	long GetBackPicture(long hWndDest)
	{
		long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x81, DISPATCH_METHOD, VT_I4, (void*)&result, parms, hWndDest);
		return result;
	}
	void SetBackPicture(long hBitmap)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x82, DISPATCH_METHOD, VT_EMPTY, NULL, parms, hBitmap);
	}
	short AnimGetFramesRate()
	{
		short result;
		InvokeHelper(0x84, DISPATCH_METHOD, VT_I2, (void*)&result, NULL);
		return result;
	}
	long AnimSetFramesRate(short nRate)
	{
		long result;
		static BYTE parms[] = VTS_I2 ;
		InvokeHelper(0x85, DISPATCH_METHOD, VT_I4, (void*)&result, parms, nRate);
		return result;
	}
	short AnimGetFramesCount()
	{
		short result;
		InvokeHelper(0x86, DISPATCH_METHOD, VT_I2, (void*)&result, NULL);
		return result;
	}
	long AnimShowFrame(short nFrame)
	{
		long result;
		static BYTE parms[] = VTS_I2 ;
		InvokeHelper(0x87, DISPATCH_METHOD, VT_I4, (void*)&result, parms, nFrame);
		return result;
	}
	long AnimPause()
	{
		long result;
		InvokeHelper(0x88, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	long AnimResume()
	{
		long result;
		InvokeHelper(0x89, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	long AnimInvertPlay()
	{
		long result;
		InvokeHelper(0x8a, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	void SetBackPictureVisible(BOOL bVisible)
	{
		static BYTE parms[] = VTS_BOOL ;
		InvokeHelper(0x8b, DISPATCH_METHOD, VT_EMPTY, NULL, parms, bVisible);
	}
	void ClearButtonCache()
	{
		InvokeHelper(0x93, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	short GetValueEx()
	{
		short result;
		InvokeHelper(0x94, DISPATCH_METHOD, VT_I2, (void*)&result, NULL);
		return result;
	}
	void SetValueEx(short nValue)
	{
		static BYTE parms[] = VTS_I2 ;
		InvokeHelper(0x95, DISPATCH_METHOD, VT_EMPTY, NULL, parms, nValue);
	}
	short LoadTplExInternal(LPCTSTR strFilename, long cxMax, long cyMax, long * pcxOrig, long * pcyOrig)
	{
		short result;
		static BYTE parms[] = VTS_BSTR VTS_I4 VTS_I4 VTS_PI4 VTS_PI4 ;
		InvokeHelper(0x96, DISPATCH_METHOD, VT_I2, (void*)&result, parms, strFilename, cxMax, cyMax, pcxOrig, pcyOrig);
		return result;
	}
	short SaveTplExInternal(LPCTSTR strFilename, long cx, long cy)
	{
		short result;
		static BYTE parms[] = VTS_BSTR VTS_I4 VTS_I4 ;
		InvokeHelper(0x97, DISPATCH_METHOD, VT_I2, (void*)&result, parms, strFilename, cx, cy);
		return result;
	}
	short CellCreate(short left, short top, short width, short height)
	{
		short result;
		static BYTE parms[] = VTS_I2 VTS_I2 VTS_I2 VTS_I2 ;
		InvokeHelper(0x98, DISPATCH_METHOD, VT_I2, (void*)&result, parms, left, top, width, height);
		return result;
	}
	short CellGetCount()
	{
		short result;
		InvokeHelper(0x99, DISPATCH_METHOD, VT_I2, (void*)&result, NULL);
		return result;
	}
	long CellDelete(short nIndex)
	{
		long result;
		static BYTE parms[] = VTS_I2 ;
		InvokeHelper(0x9a, DISPATCH_METHOD, VT_I4, (void*)&result, parms, nIndex);
		return result;
	}
	long CellShow(short nIndex, BOOL bShow)
	{
		long result;
		static BYTE parms[] = VTS_I2 VTS_BOOL ;
		InvokeHelper(0x9b, DISPATCH_METHOD, VT_I4, (void*)&result, parms, nIndex, bShow);
		return result;
	}
	long CellMove(short nIndex, short left, short top)
	{
		long result;
		static BYTE parms[] = VTS_I2 VTS_I2 VTS_I2 ;
		InvokeHelper(0x9c, DISPATCH_METHOD, VT_I4, (void*)&result, parms, nIndex, left, top);
		return result;
	}
	long CellResize(short nIndex, short width, short height)
	{
		long result;
		static BYTE parms[] = VTS_I2 VTS_I2 VTS_I2 ;
		InvokeHelper(0x9d, DISPATCH_METHOD, VT_I4, (void*)&result, parms, nIndex, width, height);
		return result;
	}
	long CellSetText(short nIndex, LPCTSTR strText)
	{
		long result;
		static BYTE parms[] = VTS_I2 VTS_BSTR ;
		InvokeHelper(0x9e, DISPATCH_METHOD, VT_I4, (void*)&result, parms, nIndex, strText);
		return result;
	}
	LPDISPATCH CellGetTextDescriptor(short nIndex)
	{
		LPDISPATCH result;
		static BYTE parms[] = VTS_I2 ;
		InvokeHelper(0x9f, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms, nIndex);
		return result;
	}
	long CellSetTextFont(short nIndex, LPCTSTR strFacename, short nHeight, BOOL bBold, BOOL bItalic, BOOL bUnder, BOOL bStrike)
	{
		long result;
		static BYTE parms[] = VTS_I2 VTS_BSTR VTS_I2 VTS_BOOL VTS_BOOL VTS_BOOL VTS_BOOL ;
		InvokeHelper(0xa0, DISPATCH_METHOD, VT_I4, (void*)&result, parms, nIndex, strFacename, nHeight, bBold, bItalic, bUnder, bStrike);
		return result;
	}
	long CellSetTextWordWrap(short nIndex, BOOL bWordWrap)
	{
		long result;
		static BYTE parms[] = VTS_I2 VTS_BOOL ;
		InvokeHelper(0xa1, DISPATCH_METHOD, VT_I4, (void*)&result, parms, nIndex, bWordWrap);
		return result;
	}
	long CellSetTextHorzAlign(short nIndex, long nAlign)
	{
		long result;
		static BYTE parms[] = VTS_I2 VTS_I4 ;
		InvokeHelper(0xa2, DISPATCH_METHOD, VT_I4, (void*)&result, parms, nIndex, nAlign);
		return result;
	}
	long CellSetTextVertAlign(short nIndex, long nAlign)
	{
		long result;
		static BYTE parms[] = VTS_I2 VTS_I4 ;
		InvokeHelper(0xa3, DISPATCH_METHOD, VT_I4, (void*)&result, parms, nIndex, nAlign);
		return result;
	}
	long CellSetPicture(short nIndex, LPCTSTR strFilename)
	{
		long result;
		static BYTE parms[] = VTS_I2 VTS_BSTR ;
		InvokeHelper(0xa4, DISPATCH_METHOD, VT_I4, (void*)&result, parms, nIndex, strFilename);
		return result;
	}
	long CellSetPictureDisabled(short nIndex, LPCTSTR strFilename)
	{
		long result;
		static BYTE parms[] = VTS_I2 VTS_BSTR ;
		InvokeHelper(0xa5, DISPATCH_METHOD, VT_I4, (void*)&result, parms, nIndex, strFilename);
		return result;
	}
	long CellSetPictureHorzAlign(short nIndex, long nAlign)
	{
		long result;
		static BYTE parms[] = VTS_I2 VTS_I4 ;
		InvokeHelper(0xa6, DISPATCH_METHOD, VT_I4, (void*)&result, parms, nIndex, nAlign);
		return result;
	}
	long CellSetPictureVertAlign(short nIndex, long nAlign)
	{
		long result;
		static BYTE parms[] = VTS_I2 VTS_I4 ;
		InvokeHelper(0xa7, DISPATCH_METHOD, VT_I4, (void*)&result, parms, nIndex, nAlign);
		return result;
	}
	long CellSetPictureStatic(short nIndex, BOOL bStatic)
	{
		long result;
		static BYTE parms[] = VTS_I2 VTS_BOOL ;
		InvokeHelper(0xa8, DISPATCH_METHOD, VT_I4, (void*)&result, parms, nIndex, bStatic);
		return result;
	}
	long CellSetTestMode(short nIndex, BOOL bTestMode)
	{
		long result;
		static BYTE parms[] = VTS_I2 VTS_BOOL ;
		InvokeHelper(0xa9, DISPATCH_METHOD, VT_I4, (void*)&result, parms, nIndex, bTestMode);
		return result;
	}
	long GetButtonCacheSize()
	{
		long result;
		InvokeHelper(0xaa, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	short GetHelperStringsNum(short dispid)
	{
		short result;
		static BYTE parms[] = VTS_I2 ;
		InvokeHelper(0xac, DISPATCH_METHOD, VT_I2, (void*)&result, parms, dispid);
		return result;
	}
	short LoadCellsInternal(LPCTSTR strFilename)
	{
		short result;
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0xad, DISPATCH_METHOD, VT_I2, (void*)&result, parms, strFilename);
		return result;
	}
	short SaveCellsInternal(LPCTSTR strFilename)
	{
		short result;
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0xae, DISPATCH_METHOD, VT_I2, (void*)&result, parms, strFilename);
		return result;
	}
	CString GetVersion()
	{
		CString result;
		InvokeHelper(0xb6, DISPATCH_METHOD, VT_BSTR, (void*)&result, NULL);
		return result;
	}
	CString GetEdition()
	{
		CString result;
		InvokeHelper(0xb7, DISPATCH_METHOD, VT_BSTR, (void*)&result, NULL);
		return result;
	}
	void SetTotalTransparentFactor(short nNewValue)
	{
		static BYTE parms[] = VTS_I2 ;
		InvokeHelper(0xb9, DISPATCH_METHOD, VT_EMPTY, NULL, parms, nNewValue);
	}
	void RefreshTransparentControl()
	{
		InvokeHelper(0xba, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	void EnableButtonCache(BOOL bEnable)
	{
		static BYTE parms[] = VTS_BOOL ;
		InvokeHelper(0xbe, DISPATCH_METHOD, VT_EMPTY, NULL, parms, bEnable);
	}
	void SetCustomPaintFunction(long pFunction)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0xbf, DISPATCH_METHOD, VT_EMPTY, NULL, parms, pFunction);
	}
	void SetClipChildren(BOOL bSet)
	{
		static BYTE parms[] = VTS_BOOL ;
		InvokeHelper(0xc0, DISPATCH_METHOD, VT_EMPTY, NULL, parms, bSet);
	}
	long GetControlBitmap(long nImage)
	{
		long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0xde, DISPATCH_METHOD, VT_I4, (void*)&result, parms, nImage);
		return result;
	}
	void EnableDrawGdiPlus(BOOL bEnable)
	{
		static BYTE parms[] = VTS_BOOL ;
		InvokeHelper(0x112, DISPATCH_METHOD, VT_EMPTY, NULL, parms, bEnable);
	}
	void SetFontPropertiesEx(long nTextPos, LPCTSTR strFacename, float nSize, BOOL bBold, BOOL bItalic, BOOL bUnderline, BOOL bStrikethrough)
	{
		static BYTE parms[] = VTS_I4 VTS_BSTR VTS_R4 VTS_BOOL VTS_BOOL VTS_BOOL VTS_BOOL ;
		InvokeHelper(0x113, DISPATCH_METHOD, VT_EMPTY, NULL, parms, nTextPos, strFacename, nSize, bBold, bItalic, bUnderline, bStrikethrough);
	}
	void SetTemplateBinDirSingleBtn(long nType, LPCTSTR strDirPath)
	{
		static BYTE parms[] = VTS_I4 VTS_BSTR ;
		InvokeHelper(0x114, DISPATCH_METHOD, VT_EMPTY, NULL, parms, nType, strDirPath);
	}
	void AboutBox()
	{
		InvokeHelper(DISPID_ABOUTBOX, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}

// Properties
//

CString GetCaption()
{
	CString result;
	GetProperty(DISPID_CAPTION, VT_BSTR, (void*)&result);
	return result;
}
void SetCaption(CString propVal)
{
	SetProperty(DISPID_CAPTION, VT_BSTR, propVal);
}
long GetShape()
{
	long result;
	GetProperty(0xb, VT_I4, (void*)&result);
	return result;
}
void SetShape(long propVal)
{
	SetProperty(0xb, VT_I4, propVal);
}
short GetRoundness()
{
	short result;
	GetProperty(0xc, VT_I2, (void*)&result);
	return result;
}
void SetRoundness(short propVal)
{
	SetProperty(0xc, VT_I2, propVal);
}
long GetSlant()
{
	long result;
	GetProperty(0xd, VT_I4, (void*)&result);
	return result;
}
void SetSlant(long propVal)
{
	SetProperty(0xd, VT_I4, propVal);
}
long GetSurface()
{
	long result;
	GetProperty(0xe, VT_I4, (void*)&result);
	return result;
}
void SetSurface(long propVal)
{
	SetProperty(0xe, VT_I4, propVal);
}
unsigned long GetPictureTranspColor()
{
	unsigned long result;
	GetProperty(0xf, VT_UI4, (void*)&result);
	return result;
}
void SetPictureTranspColor(unsigned long propVal)
{
	SetProperty(0xf, VT_UI4, propVal);
}
CString GetSoundDown()
{
	CString result;
	GetProperty(0x1, VT_BSTR, (void*)&result);
	return result;
}
void SetSoundDown(CString propVal)
{
	SetProperty(0x1, VT_BSTR, propVal);
}
CString GetSoundUp()
{
	CString result;
	GetProperty(0x2, VT_BSTR, (void*)&result);
	return result;
}
void SetSoundUp(CString propVal)
{
	SetProperty(0x2, VT_BSTR, propVal);
}
CString GetCursor()
{
	CString result;
	GetProperty(0x3, VT_BSTR, (void*)&result);
	return result;
}
void SetCursor(CString propVal)
{
	SetProperty(0x3, VT_BSTR, propVal);
}
CString GetSoundMouseOver()
{
	CString result;
	GetProperty(0x4, VT_BSTR, (void*)&result);
	return result;
}
void SetSoundMouseOver(CString propVal)
{
	SetProperty(0x4, VT_BSTR, propVal);
}
CString GetSoundMouseOut()
{
	CString result;
	GetProperty(0x5, VT_BSTR, (void*)&result);
	return result;
}
void SetSoundMouseOut(CString propVal)
{
	SetProperty(0x5, VT_BSTR, propVal);
}
COleFont GetFontTextLT()
{
	LPDISPATCH result;
	GetProperty(0x10, VT_DISPATCH, (void*)&result);
	return COleFont(result);
}
void SetFontTextLT(LPDISPATCH propVal)
{
	SetProperty(0x10, VT_DISPATCH, propVal);
}
COleFont GetFontTextCT()
{
	LPDISPATCH result;
	GetProperty(0x11, VT_DISPATCH, (void*)&result);
	return COleFont(result);
}
void SetFontTextCT(LPDISPATCH propVal)
{
	SetProperty(0x11, VT_DISPATCH, propVal);
}
COleFont GetFontTextRT()
{
	LPDISPATCH result;
	GetProperty(0x12, VT_DISPATCH, (void*)&result);
	return COleFont(result);
}
void SetFontTextRT(LPDISPATCH propVal)
{
	SetProperty(0x12, VT_DISPATCH, propVal);
}
COleFont GetFontTextLM()
{
	LPDISPATCH result;
	GetProperty(0x13, VT_DISPATCH, (void*)&result);
	return COleFont(result);
}
void SetFontTextLM(LPDISPATCH propVal)
{
	SetProperty(0x13, VT_DISPATCH, propVal);
}
COleFont GetFontTextRM()
{
	LPDISPATCH result;
	GetProperty(0x14, VT_DISPATCH, (void*)&result);
	return COleFont(result);
}
void SetFontTextRM(LPDISPATCH propVal)
{
	SetProperty(0x14, VT_DISPATCH, propVal);
}
COleFont GetFontTextLB()
{
	LPDISPATCH result;
	GetProperty(0x15, VT_DISPATCH, (void*)&result);
	return COleFont(result);
}
void SetFontTextLB(LPDISPATCH propVal)
{
	SetProperty(0x15, VT_DISPATCH, propVal);
}
COleFont GetFontTextCB()
{
	LPDISPATCH result;
	GetProperty(0x16, VT_DISPATCH, (void*)&result);
	return COleFont(result);
}
void SetFontTextCB(LPDISPATCH propVal)
{
	SetProperty(0x16, VT_DISPATCH, propVal);
}
COleFont GetFontTextRB()
{
	LPDISPATCH result;
	GetProperty(0x17, VT_DISPATCH, (void*)&result);
	return COleFont(result);
}
void SetFontTextRB(LPDISPATCH propVal)
{
	SetProperty(0x17, VT_DISPATCH, propVal);
}
CString GetPicture()
{
	CString result;
	GetProperty(0x6, VT_BSTR, (void*)&result);
	return result;
}
void SetPicture(CString propVal)
{
	SetProperty(0x6, VT_BSTR, propVal);
}
CString GetPicturePressed()
{
	CString result;
	GetProperty(0x7, VT_BSTR, (void*)&result);
	return result;
}
void SetPicturePressed(CString propVal)
{
	SetProperty(0x7, VT_BSTR, propVal);
}
CString GetPictureMouseOver()
{
	CString result;
	GetProperty(0x8, VT_BSTR, (void*)&result);
	return result;
}
void SetPictureMouseOver(CString propVal)
{
	SetProperty(0x8, VT_BSTR, propVal);
}
CString GetPictureCustom()
{
	CString result;
	GetProperty(0x9, VT_BSTR, (void*)&result);
	return result;
}
void SetPictureCustom(CString propVal)
{
	SetProperty(0x9, VT_BSTR, propVal);
}
long GetPicturePosition()
{
	long result;
	GetProperty(0x18, VT_I4, (void*)&result);
	return result;
}
void SetPicturePosition(long propVal)
{
	SetProperty(0x18, VT_I4, propVal);
}
short GetPictureXOffset()
{
	short result;
	GetProperty(0x19, VT_I2, (void*)&result);
	return result;
}
void SetPictureXOffset(short propVal)
{
	SetProperty(0x19, VT_I2, propVal);
}
short GetPictureYOffset()
{
	short result;
	GetProperty(0x1a, VT_I2, (void*)&result);
	return result;
}
void SetPictureYOffset(short propVal)
{
	SetProperty(0x1a, VT_I2, propVal);
}
BOOL GetEnabled()
{
	BOOL result;
	GetProperty(DISPID_ENABLED, VT_BOOL, (void*)&result);
	return result;
}
void SetEnabled(BOOL propVal)
{
	SetProperty(DISPID_ENABLED, VT_BOOL, propVal);
}
long GetPictureTranspMode()
{
	long result;
	GetProperty(0x1b, VT_I4, (void*)&result);
	return result;
}
void SetPictureTranspMode(long propVal)
{
	SetProperty(0x1b, VT_I4, propVal);
}
BOOL GetButtonCustomRaised()
{
	BOOL result;
	GetProperty(0x1c, VT_BOOL, (void*)&result);
	return result;
}
void SetButtonCustomRaised(BOOL propVal)
{
	SetProperty(0x1c, VT_BOOL, propVal);
}
unsigned long GetBackColorContainer()
{
	unsigned long result;
	GetProperty(0x1d, VT_UI4, (void*)&result);
	return result;
}
void SetBackColorContainer(unsigned long propVal)
{
	SetProperty(0x1d, VT_UI4, propVal);
}
BOOL GetMovePictureOnPress()
{
	BOOL result;
	GetProperty(0x1e, VT_BOOL, (void*)&result);
	return result;
}
void SetMovePictureOnPress(BOOL propVal)
{
	SetProperty(0x1e, VT_BOOL, propVal);
}
short GetButtonRaiseFactor()
{
	short result;
	GetProperty(0x1f, VT_I2, (void*)&result);
	return result;
}
void SetButtonRaiseFactor(short propVal)
{
	SetProperty(0x1f, VT_I2, propVal);
}
unsigned long GetHighlightColor()
{
	unsigned long result;
	GetProperty(0x20, VT_UI4, (void*)&result);
	return result;
}
void SetHighlightColor(unsigned long propVal)
{
	SetProperty(0x20, VT_UI4, propVal);
}
unsigned long GetShadowColor()
{
	unsigned long result;
	GetProperty(0x21, VT_UI4, (void*)&result);
	return result;
}
void SetShadowColor(unsigned long propVal)
{
	SetProperty(0x21, VT_UI4, propVal);
}
BOOL GetButtonCustomFitPicture()
{
	BOOL result;
	GetProperty(0x22, VT_BOOL, (void*)&result);
	return result;
}
void SetButtonCustomFitPicture(BOOL propVal)
{
	SetProperty(0x22, VT_BOOL, propVal);
}
OLE_HANDLE GethWnd()
{
	OLE_HANDLE result;
	GetProperty(DISPID_HWND, VT_I4, (void*)&result);
	return result;
}
void SethWnd(OLE_HANDLE propVal)
{
	SetProperty(DISPID_HWND, VT_I4, propVal);
}
COleFont GetFontTextCaption()
{
	LPDISPATCH result;
	GetProperty(0x23, VT_DISPATCH, (void*)&result);
	return COleFont(result);
}
void SetFontTextCaption(LPDISPATCH propVal)
{
	SetProperty(0x23, VT_DISPATCH, propVal);
}
CString GetHyperlink()
{
	CString result;
	GetProperty(0x24, VT_BSTR, (void*)&result);
	return result;
}
void SetHyperlink(CString propVal)
{
	SetProperty(0x24, VT_BSTR, propVal);
}
BOOL GetEnableInternetLoadAtDT()
{
	BOOL result;
	GetProperty(0xa, VT_BOOL, (void*)&result);
	return result;
}
void SetEnableInternetLoadAtDT(BOOL propVal)
{
	SetProperty(0xa, VT_BOOL, propVal);
}
long GetSmoothEdges()
{
	long result;
	GetProperty(0x25, VT_I4, (void*)&result);
	return result;
}
void SetSmoothEdges(long propVal)
{
	SetProperty(0x25, VT_I4, propVal);
}
CString GetTagEx()
{
	CString result;
	GetProperty(0x26, VT_BSTR, (void*)&result);
	return result;
}
void SetTagEx(CString propVal)
{
	SetProperty(0x26, VT_BSTR, propVal);
}
BOOL GetPictureKeepRatio()
{
	BOOL result;
	GetProperty(0x27, VT_BOOL, (void*)&result);
	return result;
}
void SetPictureKeepRatio(BOOL propVal)
{
	SetProperty(0x27, VT_BOOL, propVal);
}
long GetFocusMode()
{
	long result;
	GetProperty(0x28, VT_I4, (void*)&result);
	return result;
}
void SetFocusMode(long propVal)
{
	SetProperty(0x28, VT_I4, propVal);
}
long GetSpecialEffect()
{
	long result;
	GetProperty(0x29, VT_I4, (void*)&result);
	return result;
}
void SetSpecialEffect(long propVal)
{
	SetProperty(0x29, VT_I4, propVal);
}
short GetCaptionWordWrapPerc()
{
	short result;
	GetProperty(0x2a, VT_I2, (void*)&result);
	return result;
}
void SetCaptionWordWrapPerc(short propVal)
{
	SetProperty(0x2a, VT_I2, propVal);
}
short GetOptimization()
{
	short result;
	GetProperty(0x2b, VT_I2, (void*)&result);
	return result;
}
void SetOptimization(short propVal)
{
	SetProperty(0x2b, VT_I2, propVal);
}
BOOL GetClickable()
{
	BOOL result;
	GetProperty(0x2c, VT_BOOL, (void*)&result);
	return result;
}
void SetClickable(BOOL propVal)
{
	SetProperty(0x2c, VT_BOOL, propVal);
}
unsigned long GetBackColor()
{
	unsigned long result;
	GetProperty(0x2d, VT_UI4, (void*)&result);
	return result;
}
void SetBackColor(unsigned long propVal)
{
	SetProperty(0x2d, VT_UI4, propVal);
}
unsigned long GetForeColor()
{
	unsigned long result;
	GetProperty(0x2e, VT_UI4, (void*)&result);
	return result;
}
void SetForeColor(unsigned long propVal)
{
	SetProperty(0x2e, VT_UI4, propVal);
}
BOOL GetIgnoreSpaceBar()
{
	BOOL result;
	GetProperty(0x2f, VT_BOOL, (void*)&result);
	return result;
}
void SetIgnoreSpaceBar(BOOL propVal)
{
	SetProperty(0x2f, VT_BOOL, propVal);
}
long GetSpecialEffectFactor()
{
	long result;
	GetProperty(0x30, VT_I4, (void*)&result);
	return result;
}
void SetSpecialEffectFactor(long propVal)
{
	SetProperty(0x30, VT_I4, propVal);
}
CString GetTexture()
{
	CString result;
	GetProperty(0x49, VT_BSTR, (void*)&result);
	return result;
}
void SetTexture(CString propVal)
{
	SetProperty(0x49, VT_BSTR, propVal);
}
short GetTextureMode()
{
	short result;
	GetProperty(0x4a, VT_I2, (void*)&result);
	return result;
}
void SetTextureMode(short propVal)
{
	SetProperty(0x4a, VT_I2, propVal);
}
short GetTextureLightIntensity()
{
	short result;
	GetProperty(0x4b, VT_I2, (void*)&result);
	return result;
}
void SetTextureLightIntensity(short propVal)
{
	SetProperty(0x4b, VT_I2, propVal);
}
unsigned long GetTextureColorMix()
{
	unsigned long result;
	GetProperty(0x4c, VT_UI4, (void*)&result);
	return result;
}
void SetTextureColorMix(unsigned long propVal)
{
	SetProperty(0x4c, VT_UI4, propVal);
}
short GetTextureColorMixPerc()
{
	short result;
	GetProperty(0x4d, VT_I2, (void*)&result);
	return result;
}
void SetTextureColorMixPerc(short propVal)
{
	SetProperty(0x4d, VT_I2, propVal);
}
short GetTextureBevelFactor()
{
	short result;
	GetProperty(0x4e, VT_I2, (void*)&result);
	return result;
}
void SetTextureBevelFactor(short propVal)
{
	SetProperty(0x4e, VT_I2, propVal);
}
unsigned long GetForeColorDisabled()
{
	unsigned long result;
	GetProperty(0x4f, VT_UI4, (void*)&result);
	return result;
}
void SetForeColorDisabled(unsigned long propVal)
{
	SetProperty(0x4f, VT_UI4, propVal);
}
unsigned long GetBackColorDisabled()
{
	unsigned long result;
	GetProperty(0x50, VT_UI4, (void*)&result);
	return result;
}
void SetBackColorDisabled(unsigned long propVal)
{
	SetProperty(0x50, VT_UI4, propVal);
}
unsigned long GetBackColorMouseOver()
{
	unsigned long result;
	GetProperty(0x51, VT_UI4, (void*)&result);
	return result;
}
void SetBackColorMouseOver(unsigned long propVal)
{
	SetProperty(0x51, VT_UI4, propVal);
}
unsigned long GetBackColorFocus()
{
	unsigned long result;
	GetProperty(0x52, VT_UI4, (void*)&result);
	return result;
}
void SetBackColorFocus(unsigned long propVal)
{
	SetProperty(0x52, VT_UI4, propVal);
}
unsigned long GetBackColorInterior()
{
	unsigned long result;
	GetProperty(0x53, VT_UI4, (void*)&result);
	return result;
}
void SetBackColorInterior(unsigned long propVal)
{
	SetProperty(0x53, VT_UI4, propVal);
}
unsigned long GetForeColorMouseOver()
{
	unsigned long result;
	GetProperty(0x54, VT_UI4, (void*)&result);
	return result;
}
void SetForeColorMouseOver(unsigned long propVal)
{
	SetProperty(0x54, VT_UI4, propVal);
}
long GetFlatPillowFactor()
{
	long result;
	GetProperty(0x57, VT_I4, (void*)&result);
	return result;
}
void SetFlatPillowFactor(long propVal)
{
	SetProperty(0x57, VT_I4, propVal);
}
long GetShadowMode()
{
	long result;
	GetProperty(0x58, VT_I4, (void*)&result);
	return result;
}
void SetShadowMode(long propVal)
{
	SetProperty(0x58, VT_I4, propVal);
}
long GetStyle()
{
	long result;
	GetProperty(0x5f, VT_I4, (void*)&result);
	return result;
}
void SetStyle(long propVal)
{
	SetProperty(0x5f, VT_I4, propVal);
}
CString GetPictureDisabled()
{
	CString result;
	GetProperty(0x60, VT_BSTR, (void*)&result);
	return result;
}
void SetPictureDisabled(CString propVal)
{
	SetProperty(0x60, VT_BSTR, propVal);
}
unsigned long GetForeColorPressed()
{
	unsigned long result;
	GetProperty(0x61, VT_UI4, (void*)&result);
	return result;
}
void SetForeColorPressed(unsigned long propVal)
{
	SetProperty(0x61, VT_UI4, propVal);
}
unsigned long GetBackColorPressed()
{
	unsigned long result;
	GetProperty(0x62, VT_UI4, (void*)&result);
	return result;
}
void SetBackColorPressed(unsigned long propVal)
{
	SetProperty(0x62, VT_UI4, propVal);
}
short GetValue()
{
	short result;
	GetProperty(0x69, VT_I2, (void*)&result);
	return result;
}
void SetValue(short propVal)
{
	SetProperty(0x69, VT_I2, propVal);
}
long GetOrientation()
{
	long result;
	GetProperty(0x6c, VT_I4, (void*)&result);
	return result;
}
void SetOrientation(long propVal)
{
	SetProperty(0x6c, VT_I4, propVal);
}

long GetFrameEffect()
{
	long result;
	GetProperty(0x7f, VT_I4, (void*)&result);
	return result;
}
void SetFrameEffect(long propVal)
{
	SetProperty(0x7f, VT_I4, propVal);
}
BOOL GetOptionGroupDelimiter()
{
	BOOL result;
	GetProperty(0x80, VT_BOOL, (void*)&result);
	return result;
}
void SetOptionGroupDelimiter(BOOL propVal)
{
	SetProperty(0x80, VT_BOOL, propVal);
}
BOOL GetPictureVisible()
{
	BOOL result;
	GetProperty(0x83, VT_BOOL, (void*)&result);
	return result;
}
void SetPictureVisible(BOOL propVal)
{
	SetProperty(0x83, VT_BOOL, propVal);
}
long GetTextCaptionAlignment()
{
	long result;
	GetProperty(0x8c, VT_I4, (void*)&result);
	return result;
}
void SetTextCaptionAlignment(long propVal)
{
	SetProperty(0x8c, VT_I4, propVal);
}
short GetSpotlightOffsetX()
{
	short result;
	GetProperty(0x8d, VT_I2, (void*)&result);
	return result;
}
void SetSpotlightOffsetX(short propVal)
{
	SetProperty(0x8d, VT_I2, propVal);
}
short GetSpotlightOffsetY()
{
	short result;
	GetProperty(0x8e, VT_I2, (void*)&result);
	return result;
}
void SetSpotlightOffsetY(short propVal)
{
	SetProperty(0x8e, VT_I2, propVal);
}
short GetSpotlightResizeWidth()
{
	short result;
	GetProperty(0x8f, VT_I2, (void*)&result);
	return result;
}
void SetSpotlightResizeWidth(short propVal)
{
	SetProperty(0x8f, VT_I2, propVal);
}
short GetSpotlightResizeHeight()
{
	short result;
	GetProperty(0x90, VT_I2, (void*)&result);
	return result;
}
void SetSpotlightResizeHeight(short propVal)
{
	SetProperty(0x90, VT_I2, propVal);
}
short GetCornerFactor()
{
	short result;
	GetProperty(0x91, VT_I2, (void*)&result);
	return result;
}
void SetCornerFactor(short propVal)
{
	SetProperty(0x91, VT_I2, propVal);
}
BOOL GetUseAntialias()
{
	BOOL result;
	GetProperty(0x92, VT_BOOL, (void*)&result);
	return result;
}
void SetUseAntialias(BOOL propVal)
{
	SetProperty(0x92, VT_BOOL, propVal);
}

BOOL GetHollowFrame()
{
	BOOL result;
	GetProperty(0xaf, VT_BOOL, (void*)&result);
	return result;
}
void SetHollowFrame(BOOL propVal)
{
	SetProperty(0xaf, VT_BOOL, propVal);
}
long GetLightDirection()
{
	long result;
	GetProperty(0xb0, VT_I4, (void*)&result);
	return result;
}
void SetLightDirection(long propVal)
{
	SetProperty(0xb0, VT_I4, propVal);
}
BOOL GetSpotlightVisible()
{
	BOOL result;
	GetProperty(0xb1, VT_BOOL, (void*)&result);
	return result;
}
void SetSpotlightVisible(BOOL propVal)
{
	SetProperty(0xb1, VT_BOOL, propVal);
}
unsigned long GetSpotlightColor()
{
	unsigned long result;
	GetProperty(0xb2, VT_UI4, (void*)&result);
	return result;
}
void SetSpotlightColor(unsigned long propVal)
{
	SetProperty(0xb2, VT_UI4, propVal);
}
short GetPictureTranspFactor()
{
	short result;
	GetProperty(0xb3, VT_I2, (void*)&result);
	return result;
}
void SetPictureTranspFactor(short propVal)
{
	SetProperty(0xb3, VT_I2, propVal);
}
short GetShapeHeadFactor()
{
	short result;
	GetProperty(0xb4, VT_I2, (void*)&result);
	return result;
}
void SetShapeHeadFactor(short propVal)
{
	SetProperty(0xb4, VT_I2, propVal);
}
short GetShapeLineFactor()
{
	short result;
	GetProperty(0xb5, VT_I2, (void*)&result);
	return result;
}
void SetShapeLineFactor(short propVal)
{
	SetProperty(0xb5, VT_I2, propVal);
}
short GetSurfaceTransparentFactor()
{
	short result;
	GetProperty(0xb8, VT_I2, (void*)&result);
	return result;
}
void SetSurfaceTransparentFactor(short propVal)
{
	SetProperty(0xb8, VT_I2, propVal);
}
BOOL GetRectHardEdges()
{
	BOOL result;
	GetProperty(0xbb, VT_BOOL, (void*)&result);
	return result;
}
void SetRectHardEdges(BOOL propVal)
{
	SetProperty(0xbb, VT_BOOL, propVal);
}
BOOL GetSwapEffectsColors()
{
	BOOL result;
	GetProperty(0xbc, VT_BOOL, (void*)&result);
	return result;
}
void SetSwapEffectsColors(BOOL propVal)
{
	SetProperty(0xbc, VT_BOOL, propVal);
}
long GetSurfaceTransparentZone()
{
	long result;
	GetProperty(0xbd, VT_I4, (void*)&result);
	return result;
}
void SetSurfaceTransparentZone(long propVal)
{
	SetProperty(0xbd, VT_I4, propVal);
}
unsigned long GetVistaColorGlossyEffectUpper()
{
	unsigned long result;
	GetProperty(0xc1, VT_UI4, (void*)&result);
	return result;
}
void SetVistaColorGlossyEffectUpper(unsigned long propVal)
{
	SetProperty(0xc1, VT_UI4, propVal);
}
short GetVistaOpacityGlossyEffectUpper()
{
	short result;
	GetProperty(0xc2, VT_I2, (void*)&result);
	return result;
}
void SetVistaOpacityGlossyEffectUpper(short propVal)
{
	SetProperty(0xc2, VT_I2, propVal);
}
unsigned long GetVistaColorGlossyEffectLower()
{
	unsigned long result;
	GetProperty(0xc3, VT_UI4, (void*)&result);
	return result;
}
void SetVistaColorGlossyEffectLower(unsigned long propVal)
{
	SetProperty(0xc3, VT_UI4, propVal);
}
short GetVistaOpacityGlossyEffectLower()
{
	short result;
	GetProperty(0xc4, VT_I2, (void*)&result);
	return result;
}
void SetVistaOpacityGlossyEffectLower(short propVal)
{
	SetProperty(0xc4, VT_I2, propVal);
}
unsigned long GetVistaColorOuterBorder()
{
	unsigned long result;
	GetProperty(0xc5, VT_UI4, (void*)&result);
	return result;
}
void SetVistaColorOuterBorder(unsigned long propVal)
{
	SetProperty(0xc5, VT_UI4, propVal);
}
unsigned long GetVistaColorInnerBorder()
{
	unsigned long result;
	GetProperty(0xc6, VT_UI4, (void*)&result);
	return result;
}
void SetVistaColorInnerBorder(unsigned long propVal)
{
	SetProperty(0xc6, VT_UI4, propVal);
}
unsigned long GetVistaColorInnerBorderPressed()
{
	unsigned long result;
	GetProperty(0xc7, VT_UI4, (void*)&result);
	return result;
}
void SetVistaColorInnerBorderPressed(unsigned long propVal)
{
	SetProperty(0xc7, VT_UI4, propVal);
}
unsigned long GetVistaColorMiddleBorder()
{
	unsigned long result;
	GetProperty(0xc8, VT_UI4, (void*)&result);
	return result;
}
void SetVistaColorMiddleBorder(unsigned long propVal)
{
	SetProperty(0xc8, VT_UI4, propVal);
}
unsigned long GetVistaColorGlossyEmphUpper()
{
	unsigned long result;
	GetProperty(0xc9, VT_UI4, (void*)&result);
	return result;
}
void SetVistaColorGlossyEmphUpper(unsigned long propVal)
{
	SetProperty(0xc9, VT_UI4, propVal);
}
short GetVistaOpacityGlossyEmphUpper()
{
	short result;
	GetProperty(0xca, VT_I2, (void*)&result);
	return result;
}
void SetVistaOpacityGlossyEmphUpper(short propVal)
{
	SetProperty(0xca, VT_I2, propVal);
}
unsigned long GetVistaColorGlossyEmphLower()
{
	unsigned long result;
	GetProperty(0xcb, VT_UI4, (void*)&result);
	return result;
}
void SetVistaColorGlossyEmphLower(unsigned long propVal)
{
	SetProperty(0xcb, VT_UI4, propVal);
}
short GetVistaOpacityGlossyEmphLower()
{
	short result;
	GetProperty(0xcc, VT_I2, (void*)&result);
	return result;
}
void SetVistaOpacityGlossyEmphLower(short propVal)
{
	SetProperty(0xcc, VT_I2, propVal);
}
unsigned long GetVistaColorGlossyEmphUpperPressed()
{
	unsigned long result;
	GetProperty(0xcd, VT_UI4, (void*)&result);
	return result;
}
void SetVistaColorGlossyEmphUpperPressed(unsigned long propVal)
{
	SetProperty(0xcd, VT_UI4, propVal);
}
short GetVistaOpacityGlossyEmphUpperPressed()
{
	short result;
	GetProperty(0xce, VT_I2, (void*)&result);
	return result;
}
void SetVistaOpacityGlossyEmphUpperPressed(short propVal)
{
	SetProperty(0xce, VT_I2, propVal);
}
unsigned long GetVistaColorGlossyEmphLowerPressed()
{
	unsigned long result;
	GetProperty(0xcf, VT_UI4, (void*)&result);
	return result;
}
void SetVistaColorGlossyEmphLowerPressed(unsigned long propVal)
{
	SetProperty(0xcf, VT_UI4, propVal);
}
short GetVistaOpacityGlossyEmphLowerPressed()
{
	short result;
	GetProperty(0xd0, VT_I2, (void*)&result);
	return result;
}
void SetVistaOpacityGlossyEmphLowerPressed(short propVal)
{
	SetProperty(0xd0, VT_I2, propVal);
}
unsigned long GetVistaColorGlowBack()
{
	unsigned long result;
	GetProperty(0xd1, VT_UI4, (void*)&result);
	return result;
}
void SetVistaColorGlowBack(unsigned long propVal)
{
	SetProperty(0xd1, VT_UI4, propVal);
}
unsigned long GetVistaColorGlow()
{
	unsigned long result;
	GetProperty(0xd2, VT_UI4, (void*)&result);
	return result;
}
void SetVistaColorGlow(unsigned long propVal)
{
	SetProperty(0xd2, VT_UI4, propVal);
}
unsigned long GetVistaColorGlowChecked()
{
	unsigned long result;
	GetProperty(0xd3, VT_UI4, (void*)&result);
	return result;
}
void SetVistaColorGlowChecked(unsigned long propVal)
{
	SetProperty(0xd3, VT_UI4, propVal);
}
unsigned long GetVistaColorGlowCheckedMouseOver()
{
	unsigned long result;
	GetProperty(0xd4, VT_UI4, (void*)&result);
	return result;
}
void SetVistaColorGlowCheckedMouseOver(unsigned long propVal)
{
	SetProperty(0xd4, VT_UI4, propVal);
}
unsigned long GetVistaColorBackgroundChecked()
{
	unsigned long result;
	GetProperty(0xd5, VT_UI4, (void*)&result);
	return result;
}
void SetVistaColorBackgroundChecked(unsigned long propVal)
{
	SetProperty(0xd5, VT_UI4, propVal);
}
unsigned long GetVistaColorBackgroundCheckedMouseOver()
{
	unsigned long result;
	GetProperty(0xd6, VT_UI4, (void*)&result);
	return result;
}
void SetVistaColorBackgroundCheckedMouseOver(unsigned long propVal)
{
	SetProperty(0xd6, VT_UI4, propVal);
}
long GetVistaLook()
{
	long result;
	GetProperty(0xd7, VT_I4, (void*)&result);
	return result;
}
void SetVistaLook(long propVal)
{
	SetProperty(0xd7, VT_I4, propVal);
}
long GetVistaGlossyShape()
{
	long result;
	GetProperty(0xd8, VT_I4, (void*)&result);
	return result;
}
void SetVistaGlossyShape(long propVal)
{
	SetProperty(0xd8, VT_I4, propVal);
}
short GetVistaGlossySmoothFactor()
{
	short result;
	GetProperty(0xd9, VT_I2, (void*)&result);
	return result;
}
void SetVistaGlossySmoothFactor(short propVal)
{
	SetProperty(0xd9, VT_I2, propVal);
}
long GetSpotlightType()
{
	long result;
	GetProperty(0xda, VT_I4, (void*)&result);
	return result;
}
void SetSpotlightType(long propVal)
{
	SetProperty(0xda, VT_I4, propVal);
}
BOOL GetPictureReflectivityEnabled()
{
	BOOL result;
	GetProperty(0xdb, VT_BOOL, (void*)&result);
	return result;
}
void SetPictureReflectivityEnabled(BOOL propVal)
{
	SetProperty(0xdb, VT_BOOL, propVal);
}
short GetPictureReflectivityPerc()
{
	short result;
	GetProperty(0xdc, VT_I2, (void*)&result);
	return result;
}
void SetPictureReflectivityPerc(short propVal)
{
	SetProperty(0xdc, VT_I2, propVal);
}
short GetPictureReflectHeightPerc()
{
	short result;
	GetProperty(0xdd, VT_I2, (void*)&result);
	return result;
}
void SetPictureReflectHeightPerc(short propVal)
{
	SetProperty(0xdd, VT_I2, propVal);
}
long GetProgressBarType()
{
	long result;
	GetProperty(0xdf, VT_I4, (void*)&result);
	return result;
}
void SetProgressBarType(long propVal)
{
	SetProperty(0xdf, VT_I4, propVal);
}
long GetProgressBarMin()
{
	long result;
	GetProperty(0xe0, VT_I4, (void*)&result);
	return result;
}
void SetProgressBarMin(long propVal)
{
	SetProperty(0xe0, VT_I4, propVal);
}
long GetProgressBarMax()
{
	long result;
	GetProperty(0xe1, VT_I4, (void*)&result);
	return result;
}
void SetProgressBarMax(long propVal)
{
	SetProperty(0xe1, VT_I4, propVal);
}
long GetProgressBarValue()
{
	long result;
	GetProperty(0xe2, VT_I4, (void*)&result);
	return result;
}
void SetProgressBarValue(long propVal)
{
	SetProperty(0xe2, VT_I4, propVal);
}
unsigned long GetProgressBarColor()
{
	unsigned long result;
	GetProperty(0xe3, VT_UI4, (void*)&result);
	return result;
}
void SetProgressBarColor(unsigned long propVal)
{
	SetProperty(0xe3, VT_UI4, propVal);
}
long GetProgressBarFillMode()
{
	long result;
	GetProperty(0xe4, VT_I4, (void*)&result);
	return result;
}
void SetProgressBarFillMode(long propVal)
{
	SetProperty(0xe4, VT_I4, propVal);
}
unsigned long GetVistaColorGlowBottom()
{
	unsigned long result;
	GetProperty(0xe5, VT_UI4, (void*)&result);
	return result;
}
void SetVistaColorGlowBottom(unsigned long propVal)
{
	SetProperty(0xe5, VT_UI4, propVal);
}
unsigned long GetOffice2007ColorInnerBorder()
{
	unsigned long result;
	GetProperty(0xe6, VT_UI4, (void*)&result);
	return result;
}
void SetOffice2007ColorInnerBorder(unsigned long propVal)
{
	SetProperty(0xe6, VT_UI4, propVal);
}
unsigned long GetOffice2007ColorOuterBorder()
{
	unsigned long result;
	GetProperty(0xe7, VT_UI4, (void*)&result);
	return result;
}
void SetOffice2007ColorOuterBorder(unsigned long propVal)
{
	SetProperty(0xe7, VT_UI4, propVal);
}
unsigned long GetOffice2007ColorNorthTop()
{
	unsigned long result;
	GetProperty(0xe8, VT_UI4, (void*)&result);
	return result;
}
void SetOffice2007ColorNorthTop(unsigned long propVal)
{
	SetProperty(0xe8, VT_UI4, propVal);
}
unsigned long GetOffice2007ColorNorthBottom()
{
	unsigned long result;
	GetProperty(0xe9, VT_UI4, (void*)&result);
	return result;
}
void SetOffice2007ColorNorthBottom(unsigned long propVal)
{
	SetProperty(0xe9, VT_UI4, propVal);
}
unsigned long GetOffice2007ColorSouthTop()
{
	unsigned long result;
	GetProperty(0xea, VT_UI4, (void*)&result);
	return result;
}
void SetOffice2007ColorSouthTop(unsigned long propVal)
{
	SetProperty(0xea, VT_UI4, propVal);
}
unsigned long GetOffice2007ColorSouthBottom()
{
	unsigned long result;
	GetProperty(0xeb, VT_UI4, (void*)&result);
	return result;
}
void SetOffice2007ColorSouthBottom(unsigned long propVal)
{
	SetProperty(0xeb, VT_UI4, propVal);
}
unsigned long GetOffice2007ColorInnerBorderMouseOver()
{
	unsigned long result;
	GetProperty(0xec, VT_UI4, (void*)&result);
	return result;
}
void SetOffice2007ColorInnerBorderMouseOver(unsigned long propVal)
{
	SetProperty(0xec, VT_UI4, propVal);
}
unsigned long GetOffice2007ColorOuterBorderMouseOver()
{
	unsigned long result;
	GetProperty(0xed, VT_UI4, (void*)&result);
	return result;
}
void SetOffice2007ColorOuterBorderMouseOver(unsigned long propVal)
{
	SetProperty(0xed, VT_UI4, propVal);
}
unsigned long GetOffice2007ColorNorthTopMouseOver()
{
	unsigned long result;
	GetProperty(0xee, VT_UI4, (void*)&result);
	return result;
}
void SetOffice2007ColorNorthTopMouseOver(unsigned long propVal)
{
	SetProperty(0xee, VT_UI4, propVal);
}
unsigned long GetOffice2007ColorNorthBottomMouseOver()
{
	unsigned long result;
	GetProperty(0xef, VT_UI4, (void*)&result);
	return result;
}
void SetOffice2007ColorNorthBottomMouseOver(unsigned long propVal)
{
	SetProperty(0xef, VT_UI4, propVal);
}
unsigned long GetOffice2007ColorSouthTopMouseOver()
{
	unsigned long result;
	GetProperty(0xf0, VT_UI4, (void*)&result);
	return result;
}
void SetOffice2007ColorSouthTopMouseOver(unsigned long propVal)
{
	SetProperty(0xf0, VT_UI4, propVal);
}
unsigned long GetOffice2007ColorSouthBottomMouseOver()
{
	unsigned long result;
	GetProperty(0xf1, VT_UI4, (void*)&result);
	return result;
}
void SetOffice2007ColorSouthBottomMouseOver(unsigned long propVal)
{
	SetProperty(0xf1, VT_UI4, propVal);
}
unsigned long GetOffice2007ColorInnerBorderPressed()
{
	unsigned long result;
	GetProperty(0xf2, VT_UI4, (void*)&result);
	return result;
}
void SetOffice2007ColorInnerBorderPressed(unsigned long propVal)
{
	SetProperty(0xf2, VT_UI4, propVal);
}
unsigned long GetOffice2007ColorOuterBorderPressed()
{
	unsigned long result;
	GetProperty(0xf3, VT_UI4, (void*)&result);
	return result;
}
void SetOffice2007ColorOuterBorderPressed(unsigned long propVal)
{
	SetProperty(0xf3, VT_UI4, propVal);
}
unsigned long GetOffice2007ColorNorthTopPressed()
{
	unsigned long result;
	GetProperty(0xf4, VT_UI4, (void*)&result);
	return result;
}
void SetOffice2007ColorNorthTopPressed(unsigned long propVal)
{
	SetProperty(0xf4, VT_UI4, propVal);
}
unsigned long GetOffice2007ColorNorthBottomPressed()
{
	unsigned long result;
	GetProperty(0xf5, VT_UI4, (void*)&result);
	return result;
}
void SetOffice2007ColorNorthBottomPressed(unsigned long propVal)
{
	SetProperty(0xf5, VT_UI4, propVal);
}
unsigned long GetOffice2007ColorSouthTopPressed()
{
	unsigned long result;
	GetProperty(0xf6, VT_UI4, (void*)&result);
	return result;
}
void SetOffice2007ColorSouthTopPressed(unsigned long propVal)
{
	SetProperty(0xf6, VT_UI4, propVal);
}
unsigned long GetOffice2007ColorSouthBottomPressed()
{
	unsigned long result;
	GetProperty(0xf7, VT_UI4, (void*)&result);
	return result;
}
void SetOffice2007ColorSouthBottomPressed(unsigned long propVal)
{
	SetProperty(0xf7, VT_UI4, propVal);
}
unsigned long GetOffice2003ColorNorth()
{
	unsigned long result;
	GetProperty(0xf8, VT_UI4, (void*)&result);
	return result;
}
void SetOffice2003ColorNorth(unsigned long propVal)
{
	SetProperty(0xf8, VT_UI4, propVal);
}
unsigned long GetOffice2003ColorSouth()
{
	unsigned long result;
	GetProperty(0xf9, VT_UI4, (void*)&result);
	return result;
}
void SetOffice2003ColorSouth(unsigned long propVal)
{
	SetProperty(0xf9, VT_UI4, propVal);
}
unsigned long GetOffice2003ColorBorder()
{
	unsigned long result;
	GetProperty(0xfa, VT_UI4, (void*)&result);
	return result;
}
void SetOffice2003ColorBorder(unsigned long propVal)
{
	SetProperty(0xfa, VT_UI4, propVal);
}
unsigned long GetOffice2003ColorNorthMouseOver()
{
	unsigned long result;
	GetProperty(0xfb, VT_UI4, (void*)&result);
	return result;
}
void SetOffice2003ColorNorthMouseOver(unsigned long propVal)
{
	SetProperty(0xfb, VT_UI4, propVal);
}
unsigned long GetOffice2003ColorSouthMouseOver()
{
	unsigned long result;
	GetProperty(0xfc, VT_UI4, (void*)&result);
	return result;
}
void SetOffice2003ColorSouthMouseOver(unsigned long propVal)
{
	SetProperty(0xfc, VT_UI4, propVal);
}
unsigned long GetOffice2003ColorBorderMouseOver()
{
	unsigned long result;
	GetProperty(0xfd, VT_UI4, (void*)&result);
	return result;
}
void SetOffice2003ColorBorderMouseOver(unsigned long propVal)
{
	SetProperty(0xfd, VT_UI4, propVal);
}
unsigned long GetOffice2003ColorNorthPressed()
{
	unsigned long result;
	GetProperty(0xfe, VT_UI4, (void*)&result);
	return result;
}
void SetOffice2003ColorNorthPressed(unsigned long propVal)
{
	SetProperty(0xfe, VT_UI4, propVal);
}
unsigned long GetOffice2003ColorSouthPressed()
{
	unsigned long result;
	GetProperty(0xff, VT_UI4, (void*)&result);
	return result;
}
void SetOffice2003ColorSouthPressed(unsigned long propVal)
{
	SetProperty(0xff, VT_UI4, propVal);
}
unsigned long GetOffice2003ColorBorderPressed()
{
	unsigned long result;
	GetProperty(0x100, VT_UI4, (void*)&result);
	return result;
}
void SetOffice2003ColorBorderPressed(unsigned long propVal)
{
	SetProperty(0x100, VT_UI4, propVal);
}
BOOL GetToolTipEnabled()
{
	BOOL result;
	GetProperty(0x101, VT_BOOL, (void*)&result);
	return result;
}
void SetToolTipEnabled(BOOL propVal)
{
	SetProperty(0x101, VT_BOOL, propVal);
}
CString GetToolTipTextNormal()
{
	CString result;
	GetProperty(0x102, VT_BSTR, (void*)&result);
	return result;
}
void SetToolTipTextNormal(CString propVal)
{
	SetProperty(0x102, VT_BSTR, propVal);
}
CString GetToolTipTextChecked()
{
	CString result;
	GetProperty(0x103, VT_BSTR, (void*)&result);
	return result;
}
void SetToolTipTextChecked(CString propVal)
{
	SetProperty(0x103, VT_BSTR, propVal);
}
short GetVistaOuterBorderWidth()
{
	short result;
	GetProperty(0x104, VT_I2, (void*)&result);
	return result;
}
void SetVistaOuterBorderWidth(short propVal)
{
	SetProperty(0x104, VT_I2, propVal);
}
short GetVistaMiddleBorderWidth()
{
	short result;
	GetProperty(0x105, VT_I2, (void*)&result);
	return result;
}
void SetVistaMiddleBorderWidth(short propVal)
{
	SetProperty(0x105, VT_I2, propVal);
}
short GetVistaInnerBorderWidth()
{
	short result;
	GetProperty(0x106, VT_I2, (void*)&result);
	return result;
}
void SetVistaInnerBorderWidth(short propVal)
{
	SetProperty(0x106, VT_I2, propVal);
}
short GetOffice2007OuterBorderWidth()
{
	short result;
	GetProperty(0x107, VT_I2, (void*)&result);
	return result;
}
void SetOffice2007OuterBorderWidth(short propVal)
{
	SetProperty(0x107, VT_I2, propVal);
}
short GetOffice2007InnerBorderWidth()
{
	short result;
	GetProperty(0x108, VT_I2, (void*)&result);
	return result;
}
void SetOffice2007InnerBorderWidth(short propVal)
{
	SetProperty(0x108, VT_I2, propVal);
}
short GetOffice2003BorderWidth()
{
	short result;
	GetProperty(0x109, VT_I2, (void*)&result);
	return result;
}
void SetOffice2003BorderWidth(short propVal)
{
	SetProperty(0x109, VT_I2, propVal);
}
CString GetSplitButtonPicture()
{
	CString result;
	GetProperty(0x10a, VT_BSTR, (void*)&result);
	return result;
}
void SetSplitButtonPicture(CString propVal)
{
	SetProperty(0x10a, VT_BSTR, propVal);
}
long GetSplitButtonType()
{
	long result;
	GetProperty(0x10b, VT_I4, (void*)&result);
	return result;
}
void SetSplitButtonType(long propVal)
{
	SetProperty(0x10b, VT_I4, propVal);
}
short GetSplitButtonArrowOffsetX()
{
	short result;
	GetProperty(0x10c, VT_I2, (void*)&result);
	return result;
}
void SetSplitButtonArrowOffsetX(short propVal)
{
	SetProperty(0x10c, VT_I2, propVal);
}
short GetSplitButtonArrowOffsetY()
{
	short result;
	GetProperty(0x10d, VT_I2, (void*)&result);
	return result;
}
void SetSplitButtonArrowOffsetY(short propVal)
{
	SetProperty(0x10d, VT_I2, propVal);
}
unsigned long GetSplitButtonArrowColor()
{
	unsigned long result;
	GetProperty(0x10e, VT_UI4, (void*)&result);
	return result;
}
void SetSplitButtonArrowColor(unsigned long propVal)
{
	SetProperty(0x10e, VT_UI4, propVal);
}
unsigned long GetSplitButtonHighlightColor()
{
	unsigned long result;
	GetProperty(0x10f, VT_UI4, (void*)&result);
	return result;
}
void SetSplitButtonHighlightColor(unsigned long propVal)
{
	SetProperty(0x10f, VT_UI4, propVal);
}
unsigned long GetSplitButtonShadowColor()
{
	unsigned long result;
	GetProperty(0x110, VT_UI4, (void*)&result);
	return result;
}
void SetSplitButtonShadowColor(unsigned long propVal)
{
	SetProperty(0x110, VT_UI4, propVal);
}
short GetSplitButtonSizePerc()
{
	short result;
	GetProperty(0x111, VT_I2, (void*)&result);
	return result;
}
void SetSplitButtonSizePerc(short propVal)
{
	SetProperty(0x111, VT_I2, propVal);
}


};
