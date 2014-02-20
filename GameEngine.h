#pragma once
#pragma comment (lib, "winmm.lib")
#pragma comment (lib, "msimg32.lib")

#include "resource.h"
#include "Sprite.h"
#include <vector>
#include <windows.h>
#include <mmsystem.h>

using namespace std;

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprevinstance, LPSTR lpcmdline, int ncmdshow);

LRESULT CALLBACK WindowProc(HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam);

//------
//遊戲專有方法，定義在遊戲實現內
//------

BOOL GameInitialize(HINSTANCE hinstance);
void GameStart(HWND hWindow);
void GameEnd();
void GameActivate(HWND hWindow);
void GameDeactivate(HWND hWindow);
void GamePaint(HDC hDC);
void GameCycle();
BOOL SpriteCollision(Sprite * pSpriteHitter, Sprite * pSpriteHittee);
void SpriteDying(Sprite * pSpriteDying);
void HandleKeys();
void MouseButtonDown(int x, int y, BOOL bLeft);
void MouseButtonUp(int x, int y, BOOL bLeft);
void MouseMove(int x, int y);

//------
//遊戲引擎類
//------

class GameEngine
{
protected:
	
	static GameEngine * m_pGameEngine;
	HINSTANCE m_hInstance;
	HWND m_hWindow;
	TCHAR m_szWindowClass[32];
	TCHAR m_szTitle[32];
	WORD m_wIcon, m_wSmallIcon;
	int m_iWidth, m_iHeight;
	int m_iFramDelay;
	BOOL m_bSleep;
	vector<Sprite *> m_vSprites;
	UINT m_uiMIDIPlayerID;

	BOOL CheckSpriteCollision(Sprite * pTestSprite);

public:
	
	GameEngine(HINSTANCE hInstance, LPTSTR szWindowClass, LPTSTR szTitle, WORD wIcon, WORD wSmallIcon, int iWidth = 1280, int iHeight = 720);
	virtual ~ GameEngine()
	{

	};

	static GameEngine * GetEngine()
	{
		return m_pGameEngine;
	}
	BOOL Initialize(int iCmdShow);
	LRESULT HandleEvent(HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam);
	void AddSprite(Sprite * pSprite);
	void DrawSprites(HDC hDC);
	void UpdateSprites();
	void CleanupSprites();
	Sprite * IsPointInSprite(int x, int y);
	void PlayMIDISong(LPTSTR szMIDIFileName = " ", BOOL bRestart = TRUE);
	void PauseMIDISong();
	void CloseMIDIPlayer();

	HINSTANCE GetInstance()
	{
		return m_hInstance;
	}
	HWND GetWindow()
	{
		return m_hWindow;
	}
	void SetWindow(HWND hWindow)
	{
		m_hWindow = hWindow;
	}
	LPTSTR GetTitle()
	{
		return m_szTitle;
	}
	WORD GetIcon()
	{
		return m_wIcon;
	}
	WORD GetSmallIcon()
	{
		return m_wSmallIcon;
	}
	int GetWidth()
	{
		return m_iWidth;
	}
	int GetHeight()
	{
		return m_iHeight;
	}
	int GetFramDelay()
	{
		return m_iFramDelay;
	}
	void SetFramRate(int iFremRate)
	{
		m_iFramDelay = 1000 / iFremRate;
	}
	BOOL GetSleep()
	{
		return m_bSleep;
	}
	void SetSleep(BOOL bSleep)
	{
		m_bSleep = bSleep;
	}
};