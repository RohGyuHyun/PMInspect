#include "stdafx.h"
#include "WNDDISPHALCON.h"


WNDDISPHALCON::WNDDISPHALCON()
{
	m_WindowHandle	=	0	;
	m_CanvasWidth	=	0	;
	m_CanvasHeight	=	0	;
}


WNDDISPHALCON::~WNDDISPHALCON()
{
	Release();
}

BOOL	WNDDISPHALCON::Create(HWND hWnd, LONG CanvasWidth, LONG CanvasHeight)
{
	Release();

	if (hWnd == NULL)
		return	FALSE;

	CRect	WndSize;
	GetWindowRect(hWnd, &WndSize);

	set_window_attr("border_width", 0);
	set_check("~father");
	open_window(WndSize.top, WndSize.left, WndSize.Width(), WndSize.Height(), (Hlong)hWnd, "visible", "", &m_WindowHandle);
	set_window_extents(m_WindowHandle, 0, 0, WndSize.Width(), WndSize.Height());
	set_check("father");
	clear_window(m_WindowHandle);

	m_CanvasWidth = CanvasWidth;
	m_CanvasHeight = CanvasHeight;
	SetDisplayArea();

	return	TRUE;
}

VOID	WNDDISPHALCON::Release()
{
	if (m_WindowHandle != 0)
	{
		close_window(m_WindowHandle);
		m_WindowHandle = 0;
		m_CanvasWidth = 0;
		m_CanvasHeight = 0;
	}
}

BOOL	WNDDISPHALCON::ClearDisplay()
{
	if (m_WindowHandle == 0)
		return	FALSE;

	clear_window(m_WindowHandle);
	return	TRUE;
}

BOOL	WNDDISPHALCON::DisplayObject(Hobject *pObj, COLORREF Ref/*RGB(0, 0, 0)*/, BOOL bFill/*TRUE*/)
{
	if (m_WindowHandle == 0 || pObj == NULL )
		return	FALSE;

	SetDrawType(Ref, bFill, 1);
	disp_obj(*pObj, m_WindowHandle);

	return	TRUE;
}

BOOL	WNDDISPHALCON::DisplayCross(POINT CrossCenter, LONG CrossSize/*0*/, COLORREF Ref/*RGB(0, 0, 0)*/, BOOL bFill/*TRUE*/)
{
	if (m_WindowHandle == 0)
		return	FALSE;

	SetDrawType(Ref, bFill, 1);

	if (CrossSize <= 0)
	{
		if (m_CanvasWidth < m_CanvasHeight)
			CrossSize = m_CanvasHeight;
		else
			CrossSize = m_CanvasWidth;
	}

	disp_cross(m_WindowHandle, CrossCenter.y, CrossCenter.x, CrossSize, 0.0);

	return	TRUE;
}

BOOL	WNDDISPHALCON::SetDisplayArea(LPRECT pDispPartArea/*NULL*/)
{
	if (m_WindowHandle == 0)
		return	FALSE;

	if (pDispPartArea != NULL)
	{
		set_part(m_WindowHandle, pDispPartArea->top, pDispPartArea->left, pDispPartArea->bottom, pDispPartArea->right);
	}
	else
	{
		set_part(m_WindowHandle, 0, 0, m_CanvasHeight, m_CanvasWidth);
	}
	return	TRUE;
}

BOOL	WNDDISPHALCON::IsCreate()
{
	if (m_WindowHandle == 0)
		return	FALSE;

	return	TRUE;
}

VOID	WNDDISPHALCON::SetDrawType(COLORREF Ref, BOOL bFill, LONG LineWidth)
{
	if (m_WindowHandle == 0)
		return;

	char	strColor[10];
	sprintf(strColor, "#%02x%02x%02x", GetRValue(Ref), GetGValue(Ref), GetBValue(Ref));
	set_color(m_WindowHandle, strColor);

	if (bFill)
		set_draw(m_WindowHandle, "fill");
	else
	{
		if (LineWidth > 0)
			set_line_width(m_WindowHandle, LineWidth);

		set_draw(m_WindowHandle, "margin");
	}
}
