#include "stdafx.h"
#include "Background.h"
//------
//1.靜態背景
//------
Background :: Background(int iWidth, int iHeight, COLORREF crColor)
{
	m_iWidth = iWidth;
	m_iHeight = iHeight;
	m_crColor = crColor;
	m_pBitmap = NULL;
}

Background :: Background(Bitmap * pBitmap)
{
	m_iWidth = pBitmap -> GetWidth();
	m_iHeight = pBitmap -> GetHeight();
	m_crColor = NULL;
	m_pBitmap = pBitmap;
}

Background :: ~ Background()
{

}
//------
//靜態背景不需要更新
//------
void Background :: Update()
{

}
//------
//繪製靜態背景 純色或圖像
//------
void Background :: Draw(HDC hDC)
{
	if(m_pBitmap != NULL)
	{
		m_pBitmap -> Draw(hDC, 0, 0);
	}
	else
	{
		RECT rcRect = {0, 0, m_iWidth, m_iHeight};
		HBRUSH hBrush = CreateSolidBrush(m_crColor);
		FillRect(hDC, &rcRect, hBrush);
		DeleteObject(hBrush);
	}
}
//------
//2.閃爍星空背景
//------
//------
//初始化并在窗口隨機中散佈星星
//------
StarryBackground :: StarryBackground(int iWidth, int iHeight, int iNumStars, int iTwinkleDelay) : Background(iWidth, iHeight, 0)
{
	m_iNumStars = min(iNumStars, 100);
	m_iTwinkleDelay = iTwinkleDelay;

	for(int i = 0; i < iNumStars; ++i)
	{
		m_ptStars[i].x = rand() % iWidth;
		m_ptStars[i].y = rand() % iHeight;
		m_crStarColors[i] = RGB(128, 128, 128);
	}
}

StarryBackground :: ~ StarryBackground()
{

}
//------
//遍歷所有星星 每顆星星有一定機率變成一種隨機灰色
//------
void StarryBackground :: Update()
{
	int iRGB;
	for(int i = 0; i < m_iNumStars; ++i)
	{
		if((rand() % m_iTwinkleDelay) == 0)
		{
			iRGB = rand() % 256;
			m_crStarColors[i] = RGB(iRGB, iRGB, iRGB);
		}
	}
}
//------
//繪製黑色背景和星星
//------
void StarryBackground :: Draw(HDC hDC)
{
	RECT rcRect = {0, 0, m_iWidth, m_iHeight};
	HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0));
	FillRect(hDC, &rcRect, hBrush);
	DeleteObject(hBrush);

	for(int i = 0; i < m_iNumStars; ++i)
	{
		SetPixel(hDC, m_ptStars[i].x, m_ptStars[i].y, m_crStarColors[i]);
		SetPixel(hDC, m_ptStars[i].x + 1, m_ptStars[i].y, m_crStarColors[i]);
		SetPixel(hDC, m_ptStars[i].x, m_ptStars[i].y + 1, m_crStarColors[i]);
		SetPixel(hDC, m_ptStars[i].x + 1, m_ptStars[i].y + 1, m_crStarColors[i]);
	}
}
//------
//3.多層滾動背景
//------
ScrollingBackground :: ScrollingBackground(int iWidth, int iHeight) : Background(iWidth, iHeight, NULL), m_iNumLayers(0)
{

}

ScrollingBackground :: ~ ScrollingBackground()
{

}
//------
//遍歷更新每一層背景
//------
void ScrollingBackground :: Update()
{
	for(int i = 0; i < m_iNumLayers; ++i)
	{
		m_pLayers[i] -> Update();
	}
}
//------
//遍歷繪製每一層背景
//------
void ScrollingBackground :: Draw(HDC hDC, BOOL bTrans, COLORREF crTransColor)
{
	for(int i = 0; i < m_iNumLayers; ++i)
	{
		m_pLayers[i] -> Draw(hDC, bTrans, crTransColor);
	}
}
//------
//添加背景圖層
//------
void ScrollingBackground :: AddLayer(BackgroundLayer * pLayer)
{
	if(m_iNumLayers < 10)
	{
		m_pLayers[m_iNumLayers] = pLayer;
		++m_iNumLayers;
	}
}