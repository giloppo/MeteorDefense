#pragma once

#include "Bitmap.h"
#include "BackgroundLayer.h"
#include <stdlib.h>
#include <windows.h>
//------
//1.�ΑԔw�i
//------
class Background
{
protected:

	int m_iWidth, m_iHeight;
	COLORREF m_crColor;
	Bitmap * m_pBitmap;

public:

	Background(int iWidth, int iHeight, COLORREF crColor);
	Background(Bitmap * pBitmap);
	virtual ~ Background();

	virtual void Update();
	virtual void Draw(HDC hDC);

	int GetWidth()
	{
		return m_iWidth;
	}
	int GetHeight()
	{
		return m_iHeight;
	}
};
//------
//2.�Mࡐ���w�i
//------
class StarryBackground : public Background
{
protected:

	int m_iNumStars;
	int m_iTwinkleDelay;
	POINT m_ptStars[100];
	COLORREF m_crStarColors[100];

public:

	StarryBackground(int iWidth, int iHeight, int iNumStars = 100, int iTwinkleDelay = 50);
	virtual ~ StarryBackground();

	virtual void Update();
	virtual void Draw(HDC hDC);
};
//------
//3.���w�����w�i
//------
class ScrollingBackground : public Background
{
protected:

	int m_iNumLayers;
	BackgroundLayer * m_pLayers[10];//�ő���10���w �������v�C��

public:

	ScrollingBackground(int iWidth, int iHeight);
	virtual ~ ScrollingBackground();

	virtual void Update();
	virtual void Draw(HDC hDC, BOOL bTrans = FALSE, COLORREF crTransColor = RGB(0, 0, 0));
	void AddLayer(BackgroundLayer * pLayer);
};