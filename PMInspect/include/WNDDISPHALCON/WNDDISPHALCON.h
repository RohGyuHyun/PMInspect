#ifndef __WNDDISPHALCON_H__
#define	__WNDDISPHALCON_H__

class WNDDISPHALCON
{
	Hlong	m_WindowHandle;
	LONG	m_CanvasWidth;
	LONG	m_CanvasHeight;

	VOID	SetDrawType(COLORREF Ref, BOOL bFill, LONG LineWidth);

public:
	WNDDISPHALCON();
	virtual ~WNDDISPHALCON();

	BOOL	Create(HWND hWnd, LONG CanvasWidth, LONG CanvasHeight);
	VOID	Release();

	BOOL	ClearDisplay();
	BOOL	DisplayObject(Hobject *pObj, COLORREF Ref = RGB(0, 0, 0), BOOL bFill = TRUE);
	BOOL	DisplayCross( POINT CrossCenter, LONG CrossSize = 0, COLORREF Ref = RGB(0, 0, 0), BOOL bFill = TRUE);
	BOOL	SetDisplayArea(LPRECT pDispPartArea = NULL);

	BOOL	IsCreate();
};

#endif // !__WNDDISPHALCON_H__
