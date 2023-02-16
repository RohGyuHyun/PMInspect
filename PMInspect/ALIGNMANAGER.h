#pragma once
#include	"include\IMGALIGN\IMGALIGN.h"

class ALIGNMANAGER
{
private:
	IMGALIGN	m_ImgAlign;
	HWND		m_hWndAlignView[2];

public:
	ALIGNMANAGER();
	virtual ~ALIGNMANAGER();


};

