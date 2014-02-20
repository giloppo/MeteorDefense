#pragma once

#include "Bitmap.h"
#include <windows.h>

typedef WORD SCROLLDIR;
const SCROLLDIR SD_UP = 0, SD_RIGHT = 1, SD_DOWN = 2, SD_LEFT = 3;
//------
//Ÿö“®”wŒi
//------
class BackgroundLayer : public Bitmap
{
protected:

	RECT m_rcViewport;
	int m_iSpeed;
	SCROLLDIR m_sdDirection;

public:

	BackgroundLayer(HDC hDC, LPTSTR szFileName, int iSpeed, SCROLLDIR sdDirection);
	BackgroundLayer(HDC hDC, UINT uiResID, HINSTANCE hInstance, int iSpeed = 0, SCROLLDIR sdDirection = SD_DOWN);
	virtual ~ BackgroundLayer();

	virtual void Update();
	virtual void Draw(HDC hDC, int x, int y, BOOL bTrans = FALSE, COLORREF crTransColor = RGB(0, 0, 0));
	void SetSpeed(int iSpeed)
	{
		m_iSpeed = iSpeed;
	}
	void SetDirection(SCROLLDIR sdDirection)
	{
		m_sdDirection = sdDirection;
	}
	void SetViewport(RECT & rcViewport)
	{
		CopyRect(&m_rcViewport, &rcViewport);
	}
	RECT GetViewport()
	{
		return m_rcViewport;
	}
};