#include "stdafx.h"
#include "Sprite.h"

Sprite :: Sprite(Bitmap * pBitmap)
{
	m_pBitmap = pBitmap;
	SetRect(&m_rcPosition, 0, 0, pBitmap -> GetWidth(), pBitmap -> GetHeight());
	m_ptVelocity.x = m_ptVelocity.y = 0;
	m_iZOrder = 0;
	SetRect(&m_rcBounds, 0, 0, 616, 659);
	m_baBoundsAction = BA_STOP;
	m_bHidden = FALSE;
	m_iNumFrames = 1;
	m_iCurFrame = 0;
	m_iFrameDelay = 0;
	m_iFrameTrigger = 0;
	m_bDying = FALSE;
	m_bOneCycle = FALSE;
}

Sprite :: Sprite(Bitmap * pBitmap, RECT &rcBounds, BOUNDSACTION baBoundsAction)
{
	int iXPos = rand() % (rcBounds.right - rcBounds.left);
	int iYPos = rand() % (rcBounds.bottom - rcBounds.top);

	m_pBitmap = pBitmap;
	SetRect(&m_rcPosition, iXPos, iYPos,  iXPos + pBitmap -> GetWidth(),  iYPos + pBitmap -> GetHeight());
	m_ptVelocity.x = m_ptVelocity.y = 0;
	m_iZOrder = 0;
	CopyRect(&m_rcBounds, &rcBounds);
	m_baBoundsAction = baBoundsAction;
	m_bHidden = FALSE;
	m_iNumFrames = 1;
	m_iCurFrame = 0;
	m_iFrameDelay = 0;
	m_iFrameTrigger = 0;
	m_bDying = FALSE;
	m_bOneCycle = FALSE;
}

Sprite :: Sprite(Bitmap * pBitmap, POINT ptPosition, POINT ptVelocity, int iZOrder, RECT &rcBounds, BOUNDSACTION baBoundsAction)
{
	m_pBitmap = pBitmap;
	SetRect(&m_rcPosition, ptPosition.x, ptPosition.y,  ptPosition.x + pBitmap -> GetWidth(),  ptPosition.y + pBitmap -> GetHeight());
	m_ptVelocity = ptVelocity;
	m_iZOrder = iZOrder;
	CopyRect(&m_rcBounds, &rcBounds);
	m_baBoundsAction = baBoundsAction;
	m_bHidden = FALSE;
	m_iNumFrames = 1;
	m_iCurFrame = 0;
	m_iFrameDelay = 0;
	m_iFrameTrigger = 0;
	m_bDying = FALSE;
	m_bOneCycle = FALSE;
}

Sprite :: ~ Sprite()
{

}

//------
//更新子畫面 返回邊界動作 全項目最重要的方法
//------

SPRITEACTION Sprite :: Update()
{
//------
//判斷是否消失
//------
	if(m_bDying)
	{
		return SA_KILL;
	}
//------
//更新幀
//------
	UpdateFrame();

	POINT ptNewPosition, ptSpriteSize, ptBoundsSizes;
//------
//通過原始座標加上一個值（速度）實現移動子畫面 并設置子畫面大小和活動範圍大小（一般為窗口大小）
//------	
	ptNewPosition.x = m_rcPosition.left + m_ptVelocity.x;
	ptNewPosition.y = m_rcPosition.top + m_ptVelocity.y;
	ptSpriteSize.x = m_rcPosition.right - m_rcPosition.left;
	ptSpriteSize.y = m_rcPosition.bottom - m_rcPosition.top;
	ptBoundsSizes.x = m_rcBounds.right - m_rcBounds.left;
	ptBoundsSizes.y = m_rcBounds.bottom - m_rcBounds.top;
//------
//邊界動作為循環 則在到達邊界時將新的座標設置為對面的座標
//------
	if(m_baBoundsAction == BA_WRAP)
	{
		if((ptNewPosition.x + ptSpriteSize.x) < m_rcBounds.left)
		{
			ptNewPosition.x = m_rcBounds.right;
		}
		else if(ptNewPosition.x > m_rcBounds.right)
		{
			ptNewPosition.x = m_rcBounds.left - ptSpriteSize.x;
		}
		if((ptNewPosition.y + ptSpriteSize.y) < m_rcBounds.top)
		{
			ptNewPosition.y = m_rcBounds.bottom;
		}
		else if(ptNewPosition.y > m_rcBounds.bottom)
		{
			ptNewPosition.y = m_rcBounds.top - ptSpriteSize.y;
		}		
	}
//------
//邊界動作為反彈 通過將速度反向實現
//------
	else if(m_baBoundsAction == BA_BOUNCE)
	{
		BOOL bBounce = FALSE;
		POINT ptNewVelocity = m_ptVelocity;

		if(ptNewPosition.x < m_rcBounds.left)
		{
			bBounce = TRUE;
			ptNewPosition.x = m_rcBounds.left;
			ptNewVelocity.x = -ptNewVelocity.x;
		}
		else if((ptNewPosition.x + ptSpriteSize.x) > m_rcBounds.right)
		{
			bBounce = TRUE;
			ptNewPosition.x = m_rcBounds.right - ptSpriteSize.x;
			ptNewVelocity.x = -ptNewVelocity.x;
		}
		if(ptNewPosition.y < m_rcBounds.top)
		{
			bBounce = TRUE;
			ptNewPosition.y = m_rcBounds.top;
			ptNewVelocity.y = -ptNewVelocity.y;
		}
		else if((ptNewPosition.y + ptSpriteSize.y) > m_rcBounds.bottom)
		{
			bBounce = TRUE;
			ptNewPosition.y = m_rcBounds.bottom - ptSpriteSize.y;
			ptNewVelocity.y = -ptNewVelocity.y;
		}
		if(bBounce)
		{
			SetVelocity(ptNewVelocity);
		}
	}
	else if(m_baBoundsAction == BA_DIE)
	{
		if((ptNewPosition.x + ptSpriteSize.x) < m_rcBounds.left || ptNewPosition.x > m_rcBounds.right || (ptNewPosition.y + ptSpriteSize.y) < m_rcBounds.top || ptNewPosition.y > m_rcBounds.bottom)
		{
			return SA_KILL;
		}
	}
//------
//默認為停止 將速度設置為零
//------
	else
	{
		if((ptNewPosition.x < m_rcBounds.left) || (ptNewPosition.x > (m_rcBounds.right - ptSpriteSize.x)))
		{
			ptNewPosition.x = max(m_rcBounds.left, min(ptNewPosition.x, m_rcBounds.right - ptSpriteSize.x));
			SetVelocity(0, 0);
		}
		if((ptNewPosition.y < m_rcBounds.top) || (ptNewPosition.y > (m_rcBounds.bottom - ptSpriteSize.y)))
		{
			ptNewPosition.y = max(m_rcBounds.top, min(ptNewPosition.y, m_rcBounds.bottom - ptSpriteSize.y));
			SetVelocity(0, 0);
		}
	}
//------
//設置更新後的位置
//------
	SetPosition(ptNewPosition);

	return SA_NONE;
}

//------
//繪製子畫面 幀數為1則繪製普通圖像 否則繪製幀圖像
//------

void Sprite :: Draw(HDC hDC)
{
	if((m_pBitmap != NULL) && !m_bHidden)
	{
		if(m_iNumFrames == 1)
		{
			m_pBitmap -> Draw(hDC, m_rcPosition.left, m_rcPosition.top, TRUE, RGB(0, 0, 0));
		}
		else
		{
			m_pBitmap -> DrawPart(hDC, m_rcPosition.left, m_rcPosition.top, 0, m_iCurFrame * GetHeight(), GetWidth(), GetHeight(), TRUE, RGB(0, 0, 0));
		}
	}
}