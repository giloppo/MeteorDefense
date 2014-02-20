#include "stdafx.h"
#include "MeteorDefense.h"

//------
//遊戲引擎初始化
//------

BOOL GameInitialize(HINSTANCE hinstance)
{
	g_pGame = new GameEngine(hinstance, "MeteorDefense", "MeterorDefense", IDI_ICON, IDI_ICON, 800, 600);
	if(g_pGame == NULL)
	{
		return FALSE;
	}

	g_pGame -> SetFramRate(60);
	
	g_hInstance = hinstance;

	return TRUE;
}

//------
//遊戲初始化
//------

void GameStart(HWND hWindow)
{
	srand(GetTickCount());

	g_hOffscreenDC = CreateCompatibleDC(GetDC(hWindow));
	g_hOffscreenBitmap = CreateCompatibleBitmap(GetDC(hWindow), g_pGame -> GetWidth(), g_pGame -> GetHeight());
	SelectObject(g_hOffscreenDC, g_hOffscreenBitmap);

	HDC hDC = GetDC(hWindow);
	
	g_pGroundBitmap = new Bitmap(hDC, IDB_GROUND, g_hInstance);
	g_pCityBitmap = new Bitmap(hDC, IDB_CITY, g_hInstance);
	g_pMeteorBitmap = new Bitmap(hDC, IDB_METEOR, g_hInstance);
	g_pBeamBitmap = new Bitmap(hDC, IDB_BEAM, g_hInstance);
	g_pExplosionBitmap = new Bitmap(hDC, IDB_EXPLOSION, g_hInstance);
	g_pGameOverBitmap = new Bitmap(hDC, IDB_GAMEOVER, g_hInstance);
	g_pShipBitmap = new Bitmap(hDC, IDB_SHIP, g_hInstance);
//	g_pBackBitmapTest = new Bitmap(hDC, IDB_BACK, g_hInstance);

//	g_pBackTest = new Background(g_pBackBitmapTest);
	g_pScrollingBackground = new ScrollingBackground(g_pGame -> GetWidth(), g_pGame -> GetHeight());
	g_pBackgroundLayer = new BackgroundLayer(hDC, IDB_BACK, g_hInstance, 5, SD_DOWN);
	RECT rcViewport;
	GetClientRect(g_pGame -> GetWindow(), &rcViewport);
	g_pBackgroundLayer -> SetViewport(rcViewport);
	g_pScrollingBackground -> AddLayer(g_pBackgroundLayer);

	g_pGame -> PlayMIDISong("D:\\Work\\C++\\testPJ\\MeteorDefense\\Res\\BGM.mid");

	NewGame();
}

//------
//遊戲結束 清理各種對象
//------

void GameEnd()
{
	g_pGame -> CloseMIDIPlayer();

	DeleteObject(g_hOffscreenBitmap);
	DeleteDC(g_hOffscreenDC);
	
	delete g_pGroundBitmap;
	delete g_pCityBitmap;
	delete g_pMeteorBitmap;
	delete g_pBeamBitmap;
	delete g_pExplosionBitmap;
	delete g_pGameOverBitmap;
	delete g_pShipBitmap;

	delete g_pBackgroundLayer;
	
	delete g_pShipSprite;
	g_pGame -> CleanupSprites();

	delete g_pGame;
}

void GameActivate(HWND hWindow)
{
	g_pGame -> PlayMIDISong("", FALSE);
}

void GameDeactivate(HWND hWindow)
{
	g_pGame -> PauseMIDISong();
}

void GamePaint(HDC hDC)
{
	g_pScrollingBackground -> Draw(hDC);

	g_pGroundBitmap -> Draw(hDC, 0, 500, TRUE, RGB(0, 0, 0));
//	g_pBackTest -> Draw(hDC);
	
	g_pGame -> DrawSprites(hDC);

	TCHAR szText[64];
	RECT rcRect = {5, 5, 100, 50};
	wsprintf(szText, "SCORE: %d", g_iScore);
	SetBkMode(hDC, TRANSPARENT);
	SetTextColor(hDC, RGB(255, 255, 255));
	DrawText(hDC, szText, -1, &rcRect, DT_SINGLELINE);

	if(g_bGameOver)
	{
		g_pGameOverBitmap -> Draw(hDC, 255, 190, TRUE, RGB(0, 0, 0));
	}
}

//------
//遊戲循環
//------

void GameCycle()
{
	if(!g_bGameOver)
	{
		if((rand() % g_iDifficulty) == 0)
		{
			AddMeteor();
		}
		
/*		if(g_pBackgroundLayer -> GetViewport().bottom < 0)
		{
			g_pBackgroundLayer -> SetSpeed(0);
		}	要實現滾動到某一座標停止加上該段代碼（該段代碼為滾動至頂部停止，可根據需要修改）*/
		
		g_pScrollingBackground -> Update();

		g_pGame -> UpdateSprites();

		HWND hWindow = g_pGame -> GetWindow();
		HDC hDC = GetDC(hWindow);

		GamePaint(g_hOffscreenDC);

		BitBlt(hDC, 0, 0, g_pGame -> GetWidth(), g_pGame -> GetHeight(), g_hOffscreenDC, 0, 0, SRCCOPY);

		ReleaseDC(hWindow, hDC);
	}
}

//------
//碰撞后刪除子畫面并計算得分
//------

BOOL SpriteCollision(Sprite * pSpriteHitter, Sprite * pSpriteHittee)
{
	if((pSpriteHitter -> GetBitmap() == g_pBeamBitmap && pSpriteHittee -> GetBitmap() == g_pMeteorBitmap) || (pSpriteHitter -> GetBitmap() == g_pMeteorBitmap && pSpriteHittee -> GetBitmap() == g_pBeamBitmap))
	{
		pSpriteHitter -> Kill();
		pSpriteHittee -> Kill();

		g_iScore += 10;
		g_iDifficulty = max(50 - (g_iScore / 10), 10);
	}
	if(pSpriteHitter -> GetBitmap() == g_pMeteorBitmap && pSpriteHittee -> GetBitmap() == g_pCityBitmap)
	{
		PlaySound(MAKEINTRESOURCE(IDW_BIGEXPLODE), NULL, SND_ASYNC | SND_RESOURCE);

		pSpriteHitter -> Kill();
		pSpriteHittee -> Kill();

		if(--g_iNumCities == 0)
		{
			g_bGameOver = TRUE;
		}
	}
	if((pSpriteHitter -> GetBitmap() == g_pMeteorBitmap && pSpriteHittee -> GetBitmap() == g_pShipBitmap) || (pSpriteHitter -> GetBitmap() == g_pShipBitmap && pSpriteHittee -> GetBitmap() == g_pMeteorBitmap))
	{
		PlaySound(MAKEINTRESOURCE(IDW_BIGEXPLODE), NULL, SND_ASYNC | SND_RESOURCE);

		pSpriteHitter -> Kill();
		pSpriteHittee -> Kill();

		g_bGameOver = TRUE;
	}
	return FALSE;
}

void SpriteDying(Sprite * pSpriteDying)
{
	if(pSpriteDying -> GetBitmap() == g_pMeteorBitmap)
	{
		PlaySound(MAKEINTRESOURCE(IDW_EXPLODE), NULL, SND_ASYNC | SND_RESOURCE);

		RECT rcBounds = {0, 0, g_pGame -> GetWidth(), g_pGame -> GetHeight()};
		RECT rcPos = pSpriteDying -> GetPosition();
		Sprite * pSprite = new Sprite(g_pExplosionBitmap, rcBounds);
		pSprite -> SetNumFrames(12, TRUE);
		pSprite -> SetPosition(rcPos.left, rcPos.top);
		g_pGame -> AddSprite(pSprite);
	}
}

void HandleKeys()
{
	if(!g_bGameOver)
	{
		if(GetAsyncKeyState('A') < 0)
		{
			g_pShipSprite -> OffsetPosition(-10, 0);
		}
		if(GetAsyncKeyState('D') < 0)
		{
			g_pShipSprite -> OffsetPosition(10, 0);
		}
		if(GetAsyncKeyState('W') < 0)
		{
			g_pShipSprite -> OffsetPosition(0, -10);
		}
		if(GetAsyncKeyState('S') < 0)
		{
			g_pShipSprite -> OffsetPosition(0, 10);
		}
		if((GetAsyncKeyState('K') < 0) && (++g_iShootDelay > 3))
		{
			Shoot();
			g_iShootDelay = 0;
		}
	}
}

//------
//單擊鼠標時捕獲對象
//------

void MouseButtonDown(int x, int y, BOOL bLeft)
{	
/*	if(bLeft && !g_bGameOver)
	{
		RECT rcBounds = {0, 0, g_pGame -> GetWidth(), g_pGame -> GetHeight()};
		int iXpos = (x < 300) ? 144 : 449;
		Sprite * pSprite = new Sprite(g_pBeamBitmap, rcBounds, BA_DIE);
		pSprite -> SetNumFrames(3);
		pSprite -> SetPosition(iXpos, 365);

		int iXVel, iYVel = -6;
		y = min(y, 300);
		iXVel = (iYVel * (iXpos + 8 - x)) / (365 - y);
		pSprite -> SetVelocity(iXVel, iYVel);

		g_pGame -> AddSprite(pSprite);

		PlaySound(MAKEINTRESOURCE(IDW_SHOOT), NULL, SND_ASYNC | SND_RESOURCE);

		g_iScore = max(--g_iScore, 0);
	} 
	else*/ if(bLeft && g_bGameOver)
	{
		NewGame();
	}	
}	

void Shoot()
{
	if(!g_bGameOver)
	{
		RECT rcBounds = {0, 0, g_pGame -> GetWidth(), g_pGame -> GetHeight()};
		Sprite * pSprite = new Sprite(g_pBeamBitmap, rcBounds, BA_DIE);
		pSprite -> SetNumFrames(3);
		int iShootPosX = g_pShipSprite -> GetPosition().left + (g_pShipSprite -> GetPosition().right - g_pShipSprite -> GetPosition().left) / 2 - 4;
		int iShootPosY = g_pShipSprite -> GetPosition().top - (g_pShipSprite -> GetPosition().bottom - g_pShipSprite -> GetPosition().top) / 2;
		pSprite -> SetPosition(iShootPosX, iShootPosY);
		pSprite -> SetVelocity(0, -30);
		g_pGame -> AddSprite(pSprite);

		PlaySound(MAKEINTRESOURCE(IDW_SHOOT), NULL, SND_ASYNC | SND_RESOURCE);
	}
}

/*void MouseButtonUp(int x, int y, BOOL bLeft)
{
	ReleaseCapture();
	g_pDragSprite = NULL;
}

void MouseMove(int x, int y)
{
	if(g_pDragSprite != NULL)
	{
		g_pDragSprite -> SetPosition(x - (g_pDragSprite -> GetWidth() / 2), y - (g_pDragSprite -> GetHeight() / 2));
		InvalidateRect(g_pGame -> GetWindow(), NULL, FALSE);
	}
}	*/

void NewGame()
{	
	if(g_pShipSprite != NULL)
	{
		delete g_pShipSprite;
	}
	g_pGame -> CleanupSprites();

	RECT rcBounds = {0, 0, g_pGame -> GetWidth(), g_pGame -> GetHeight()};
	RECT rcShipBounds = {0, 0, g_pGame -> GetWidth() - 15, g_pGame -> GetHeight() - 60};

	Sprite * pSprite = new Sprite(g_pCityBitmap, rcBounds);
	pSprite -> SetPosition(2, 475);
	g_pGame -> AddSprite(pSprite);
	pSprite = new Sprite(g_pCityBitmap, rcBounds);
	pSprite -> SetPosition(200, 475);
	g_pGame -> AddSprite(pSprite);
	pSprite = new Sprite(g_pCityBitmap, rcBounds);
	pSprite -> SetPosition(450, 475);
	g_pGame -> AddSprite(pSprite);
	pSprite = new Sprite(g_pCityBitmap, rcBounds);
	pSprite -> SetPosition(600, 475);
	g_pGame -> AddSprite(pSprite);
	g_pShipSprite = new Sprite(g_pShipBitmap, rcShipBounds, BA_STOP);
	g_pShipSprite -> SetPosition(350, 450);
	g_pShipSprite -> SetNumFrames(6);
	g_pGame -> AddSprite(g_pShipSprite);

	g_iScore = 0;
	g_iNumCities = 4;
	g_iDifficulty = 50;
	g_iShootDelay = 0;
	g_bGameOver = FALSE;

	g_pGame -> PlayMIDISong();
}

void AddMeteor()
{
	RECT rcBounds = {0, 0, g_pGame -> GetWidth(), g_pGame -> GetHeight() - 100};
	int iXPos = rand() % g_pGame -> GetWidth();
	Sprite * pSprite = new Sprite(g_pMeteorBitmap, rcBounds, BA_DIE);
	pSprite -> SetNumFrames(14);
	pSprite -> SetPosition(iXPos, 0);

	int iXVel, iYVel = rand() % 4 + 3;
	switch (rand() % 4)
	{
	case 0:
		iXVel = iYVel * (56 - (iXPos + 50)) / 400;
		break;
	case 1:
		iXVel = iYVel * (240 - (iXPos + 50)) / 400;
		break;
	case 2:
		iXVel = iYVel * (360 - (iXPos + 50)) / 400;
		break;
	case 3:
		iXVel = iYVel * (546 - (iXPos + 50)) / 400;
		break;
	}
	pSprite -> SetVelocity(iXVel, iYVel);

	g_pGame -> AddSprite(pSprite);
}