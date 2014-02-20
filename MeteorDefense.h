#pragma once

#include "resource.h"
#include "GameEngine.h"
#include "Bitmap.h"
#include "Sprite.h"
#include "Background.h"
#include "BackgroundLayer.h"
#include <windows.h>
#include <stdlib.h>

HINSTANCE g_hInstance;
GameEngine * g_pGame;
HDC g_hOffscreenDC;
HBITMAP g_hOffscreenBitmap;

Bitmap * g_pGroundBitmap;
Bitmap * g_pCityBitmap;
Bitmap * g_pMeteorBitmap;
Bitmap * g_pBeamBitmap;
Bitmap * g_pExplosionBitmap;
Bitmap * g_pGameOverBitmap;
Bitmap * g_pShipBitmap;
//Bitmap * g_pBackBitmapTest;

Sprite * g_pShipSprite;

//Background * g_pBackTest;
BackgroundLayer * g_pBackgroundLayer;
ScrollingBackground * g_pScrollingBackground;

int g_iNumCities, g_iScore, g_iDifficulty, g_iShootDelay;
BOOL g_bGameOver;

void NewGame();
void AddMeteor();
void Shoot();