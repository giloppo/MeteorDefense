#pragma once

#include "Bitmap.h"
#include <stdlib.h>
#include <windows.h>

//------
//定義邊界行為
//------

typedef WORD BOUNDSACTION;
const BOUNDSACTION BA_STOP = 0, BA_WRAP = 1, BA_BOUNCE = 2, BA_DIE = 3;
typedef WORD SPRITEACTION;
const SPRITEACTION SA_NONE = 0x0000L, SA_KILL = 0x0001L;

//------
//子畫面類
//------

class Sprite
{
protected:
	
	Bitmap * m_pBitmap;
	RECT m_rcPosition, m_rcBounds, m_rcCollision;
	POINT m_ptVelocity;
	int m_iZOrder;
	BOOL m_bHidden;
	BOUNDSACTION m_baBoundsAction;
	int m_iNumFrames, m_iCurFrame, m_iFrameDelay, m_iFrameTrigger;
	BOOL m_bDying, m_bOneCycle;
//------
//設置碰撞檢測邊界
//------
	virtual void CalcCollisionRect()
	{
		int iXShrink = (m_rcPosition.left - m_rcPosition.right) / 12;
		int iYShrink = (m_rcPosition.top - m_rcPosition.bottom) / 12;
		CopyRect(&m_rcCollision, &m_rcPosition);
		InflateRect(&m_rcCollision, iXShrink, iYShrink);
	}
//------
//每次將幀計時器減一 減到零則跳至下一幀 到達最後一幀時跳至第一幀
//------
	void UpdateFrame()
	{
		if((m_iFrameDelay >= 0) && (--m_iFrameTrigger <= 0))
		{
			m_iFrameTrigger = m_iFrameDelay;
			if(++m_iCurFrame >= m_iNumFrames)
			{
				if(m_bOneCycle)
				{
					m_bDying = TRUE;
				}
				else
				{
					m_iCurFrame = 0;
				}			
			}
		}
	}

public:
	
	Sprite(Bitmap * pBitmap);
	Sprite(Bitmap * pBitmap, RECT &rcBounds, BOUNDSACTION baBoundsAction = BA_STOP);
	Sprite(Bitmap * pBitmap, POINT ptPosition, POINT ptVelocity, int iZOrder, RECT &rcBounds, BOUNDSACTION baBoundsAction = BA_STOP);
	virtual ~ Sprite();

	virtual SPRITEACTION Update();
	void Draw(HDC hDC);
//------
//判斷是否檢測到碰撞
//------
	BOOL TestCollision(Sprite * pTestSprite)
	{
		RECT & rcTest = pTestSprite -> GetCollision();
		return (m_rcCollision.left <= rcTest.right && m_rcCollision.right >= rcTest.left && m_rcCollision.top <= rcTest.bottom && m_rcCollision.bottom >= rcTest.top);
	}
//------
//判斷一個點是否在一個矩形內 很有用的方法
//------
	BOOL IsPointInside(int x, int y)
	{
		POINT ptPoint;
		ptPoint.x = x;
		ptPoint.y = y;
		return PtInRect(&m_rcPosition, ptPoint);
	}
	void Kill()
	{
		m_bDying = TRUE;
	}

	RECT & GetCollision()
	{
		return m_rcCollision;
	}
	RECT & GetPosition()
	{
		return m_rcPosition;
	}
	void SetPosition(int x, int y)
	{
		OffsetRect(&m_rcPosition, x - m_rcPosition.left, y - m_rcPosition.top);
		CalcCollisionRect();
	}
	void SetPosition(POINT ptPosition)
	{
		OffsetRect(&m_rcPosition, ptPosition.x - m_rcPosition.left, ptPosition.y - m_rcPosition.top);
		CalcCollisionRect();
	}
	void SetPosition(RECT & rcPosition)
	{
		CopyRect(&m_rcPosition, &rcPosition);
	}
	void OffsetPosition(int x, int y)
	{
		OffsetRect(&m_rcPosition, x, y);
		CalcCollisionRect();
	}
	POINT GetVelocity()
	{
		return m_ptVelocity;
	}
	void SetVelocity(int x, int y)
	{
		m_ptVelocity.x = x;
		m_ptVelocity.y = y;
	}
	void SetVelocity(POINT ptVelocity)
	{
		m_ptVelocity.x = ptVelocity.x;
		m_ptVelocity.y = ptVelocity.y;
	}
	BOOL GetZOrder()
	{
		return m_iZOrder;
	}
	void SetZOrder(int iZOrder)
	{
		m_iZOrder = iZOrder;
	}
	void SetBounds(RECT & rcBounds)
	{
		CopyRect(&m_rcBounds, &rcBounds);
	}
	void SetBoundsAction(BOUNDSACTION ba)
	{
		m_baBoundsAction = ba;
	}
	BOOL IsHidden()
	{
		return m_bHidden;
	}
	void SetHidden(BOOL bHidden)
	{
		m_bHidden = bHidden;
	}
	Bitmap * GetBitmap()
	{
		return m_pBitmap;
	}
	int GetWidth()
	{
		return m_pBitmap -> GetWidth();
	}
	int GetHeight()
	{
		return (m_pBitmap -> GetHeight() / m_iNumFrames);
	}
//------
//設置幀數并將圖像大小設置為每一幀的大小
//------
	void SetNumFrames(int iNumFreams, BOOL bOneCycle = FALSE)
	{
		m_iNumFrames = iNumFreams;
		m_bOneCycle = bOneCycle;
		RECT rcRect = GetPosition();
		rcRect.bottom = rcRect.top + ((rcRect.bottom - rcRect.top) / iNumFreams);
		SetPosition(rcRect);
	}
	void SetFrameDelay(int iFrameDelay)
	{
		m_iFrameDelay = iFrameDelay;
	}
};