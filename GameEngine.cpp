#include "stdafx.h"
#include "GameEngine.h"

GameEngine * GameEngine :: m_pGameEngine = NULL;

//------
//主事件循環
//------

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprevinstance, LPSTR lpcmdline, int ncmdshow)
{
	MSG msg;
	static int iTickTrigger = 0;
	int iTickCount;

	if(GameInitialize(hinstance))
	{
		if(!GameEngine :: GetEngine() -> Initialize(ncmdshow))
		{
			return FALSE;
		}
		while(TRUE)
		{ 
			if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				if(msg.message == WM_QUIT)
				{
					break;
				}
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				if(!(GameEngine :: GetEngine() -> GetSleep()))
				{
					iTickCount = GetTickCount();
					if(iTickCount > iTickTrigger)
					{
						//------
						//每經過一定時間執行一次遊戲循環 即幀數
						//------
						iTickTrigger = iTickCount + GameEngine :: GetEngine() -> GetFramDelay();
						//------
						//遊戲循環
						//------
						HandleKeys();
						GameCycle();
					}
				}
			}
		}
		return msg.wParam;
	}
	GameEnd();

	return TRUE;
}

//------
//消息處理
//------

LRESULT CALLBACK WindowProc(HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return GameEngine :: GetEngine() -> HandleEvent(hWindow, msg, wParam, lParam);
}

//------
//消息處理實現
//------

LRESULT GameEngine :: HandleEvent(HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case WM_CREATE:
		{
			SetWindow(hWindow);
//			PlaySound(MAKEINTRESOURCE(IDW_WAVE), NULL, SND_SYNC | SND_RESOURCE);
			GameStart(hWindow);
//			PlaySound(MAKEINTRESOURCE(ID_WAVE2), NULL, SND_ASYNC | SND_LOOP | SND_RESOURCE);
			return 0;
		}
		break;
	case WM_SETFOCUS:
		{
			GameActivate(hWindow);
			SetSleep(FALSE);
			return 0;
		}
		break;
	case WM_KILLFOCUS:
		{
			GameDeactivate(hWindow);
			SetSleep(TRUE);
			return 0;
		}
		break;
//------
//實現循環播放MIDI
//------
	case MM_MCINOTIFY:
		{
			if(wParam == MCI_NOTIFY_SUCCESSFUL)
			{
				MCI_PLAY_PARMS mciPlayParms;
				mciPlayParms.dwFrom = 0;
				mciPlayParms.dwCallback = (DWORD)m_hWindow;
				mciSendCommand(m_uiMIDIPlayerID, MCI_SEEK, MCI_SEEK_TO_START, NULL);
				mciSendCommand(m_uiMIDIPlayerID, MCI_PLAY, MCI_NOTIFY, (DWORD_PTR)&mciPlayParms);
			}
			return 0;
		}
		break;
	case WM_COMMAND:
		{
			switch(LOWORD(wParam))
			{			
			case MENU_FILE_ID_EXIT:
				{
					PostMessage(hWindow, WM_DESTROY, 0, 0);			
					return 0;
				}
				break;
			case MENU_HELP_ID_ABOUT:
				{
					MessageBox(hWindow, "Produced by OZX\nMeteorDefense Ver 1.0", "Brainiac", MB_OK | MB_ICONINFORMATION);
				}
				break;
			default:
				break;
			}
		}
	case WM_LBUTTONDOWN:
		{
			MouseButtonDown(LOWORD(lParam), HIWORD(lParam), TRUE);
			return 0;
		}
		break;
/*	case WM_LBUTTONUP:
		{
			MouseButtonUp(LOWORD(lParam), HIWORD(lParam), TRUE);
			return 0;
		}
	case WM_MOUSEMOVE:
		{
			MouseMove(LOWORD(lParam), HIWORD(lParam));
			return 0;
		}	*/
	case WM_PAINT:
		{
			HDC hDC;
			PAINTSTRUCT ps;
			hDC = BeginPaint(hWindow, &ps);
			GamePaint(hDC);
			EndPaint(hWindow, &ps);
			return 0;	
		}
		break;
	case WM_DESTROY:
		{
			GameEnd();
			PostQuitMessage(0);
			return 0;
		}
		break;
	default:
		break;
	}

	return DefWindowProc(hWindow, msg, wParam, lParam);
}

//------
//遊戲引擎初始化
//------

GameEngine :: GameEngine(HINSTANCE hInstance, LPTSTR szWindowClass, LPTSTR szTitle, WORD wIcon, WORD wSmallIcon, int iWidth, int iHeight)
{
	m_pGameEngine = this;
	m_hInstance = hInstance;
	m_hWindow = NULL;
	if(strlen(szWindowClass) > 0)
	{
		strcpy_s(m_szWindowClass, szWindowClass);
	}
	if(strlen(szTitle) > 0)
	{
		strcpy_s(m_szTitle, szTitle);
	}
	m_wIcon = wIcon;
	m_wSmallIcon = wSmallIcon;
	m_iWidth = iWidth;
	m_iHeight = iHeight;
	m_iFramDelay = 50;
	m_bSleep = TRUE;
	m_vSprites.reserve(100);
	m_uiMIDIPlayerID = 0;
}

//------
//創建窗口
//------

BOOL GameEngine :: Initialize(int iCmdshow)
{
	WNDCLASSEX wndclass;

	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WindowProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = m_hInstance;
	wndclass.hIcon = LoadIcon(m_hInstance, MAKEINTRESOURCE(GetIcon()));
	wndclass.hIconSm = LoadIcon(m_hInstance, MAKEINTRESOURCE(GetSmallIcon()));
	wndclass.hCursor = LoadCursor(m_hInstance, MAKEINTRESOURCE(IDC_CURSOR));
	wndclass.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
	wndclass.lpszMenuName = MAKEINTRESOURCE(MAIN_MENU);
	wndclass.lpszClassName = m_szWindowClass;

	if(!RegisterClassEx(&wndclass))
	{
		return FALSE;
	}
//------
//使創口居中
//------
	int iXPos = (GetSystemMetrics(SM_CXSCREEN) - GetWidth()) / 2;
	int iYPos = (GetSystemMetrics(SM_CYSCREEN) - GetHeight()) / 2;

	if(!CreateWindowEx(NULL, m_szWindowClass, m_szTitle, WS_POPUPWINDOW | WS_CAPTION | WS_MINIMIZEBOX | WS_VISIBLE, iXPos, iYPos, GetWidth(), GetHeight(), NULL, NULL, m_hInstance, NULL))
	{
		return FALSE;
	}

	return TRUE;
}

//------
//子畫面相關
//------

//------
//一次遍歷所有子畫面 使用Sprite類的TestCollision方法檢測碰撞 若檢測到則返回TRUE
//------

BOOL GameEngine :: CheckSpriteCollision(Sprite * pTestSprite)
{
	vector<Sprite *> :: iterator siSprite;
	for(siSprite = m_vSprites.begin(); siSprite != m_vSprites.end(); ++siSprite)
	{
		if(pTestSprite == * siSprite)
		{
			continue;
		}
		if(pTestSprite -> TestCollision(* siSprite))
		{
			return SpriteCollision(* siSprite, pTestSprite);
		}
	}
}

//------
//添加子畫面 如果設置了Z軸順序則可以添加到適宜的位置
//------

void GameEngine :: AddSprite(Sprite * pSprite)
{
	if(pSprite != NULL)
	{
		if(m_vSprites.size() > 0)
		{
			vector<Sprite * > :: iterator siSprite;
			for(siSprite = m_vSprites.begin(); siSprite != m_vSprites.end(); ++siSprite)
			{
				if(pSprite -> GetZOrder() < (* siSprite) -> GetZOrder())
				{
					m_vSprites.insert(siSprite, pSprite);
					return;
				}
			}
		}
		m_vSprites.push_back(pSprite);
	}
}

//------
//調用Sprite類的Draw方法一次繪製所有子畫面
//------

void GameEngine :: DrawSprites(HDC hDC)
{
	vector<Sprite * > :: iterator siSprite;
	for(siSprite = m_vSprites.begin(); siSprite != m_vSprites.end(); ++siSprite)
	{
		(* siSprite) ->Draw(hDC);
	}
}

//------
//一次遍歷所有子畫面 使用Sprite類的Update方法更新子畫面 并記住碰撞時的位置
//------

void GameEngine :: UpdateSprites()
{
	if(m_vSprites.size() >= (m_vSprites.capacity() / 2))
	{
		m_vSprites.reserve(m_vSprites.capacity() * 2);
	}

	RECT rcOldSpritePos;
	SPRITEACTION saSpriteAction;
	vector<Sprite * > :: iterator siSprite;
	for(siSprite = m_vSprites.begin(); siSprite != m_vSprites.end(); ++siSprite)
	{
		rcOldSpritePos = (* siSprite) -> GetPosition();
		saSpriteAction = (* siSprite) -> Update();
		if(saSpriteAction == SA_KILL)
		{
			SpriteDying(* siSprite);
			delete(* siSprite);
			m_vSprites.erase(siSprite);
			return;/*--siSprite;continue; <--將return;替換為這個會崩潰*/
		}
		if(CheckSpriteCollision(* siSprite))
		{
			(* siSprite) -> SetPosition(rcOldSpritePos);
		}
	}
}

//------
//清理子畫面
//------ 

void GameEngine :: CleanupSprites()
{
	vector<Sprite * > :: iterator siSprite;
	for(siSprite = m_vSprites.begin(); siSprite != m_vSprites.end(); ++siSprite)
	{
		if(* siSprite != NULL)
		{
			delete(* siSprite);
			m_vSprites.erase(siSprite);
			return;/*--siSprite; <--將return;替換為這個會崩潰*/
		}
		else
		{
			continue;
		}
	}
}

//------
//一次遍歷所有子畫面 使用Sprite類的IsPointInside方法檢測一個點是否在一個子畫面內 若是則返回該子畫面
//------

Sprite * GameEngine :: IsPointInSprite(int x, int y)
{
	vector<Sprite * > :: iterator siSprite;
	for(siSprite = m_vSprites.begin(); siSprite != m_vSprites.end(); ++siSprite)
	{
		if(!((* siSprite) -> IsHidden()) && (* siSprite) -> IsPointInside(x, y))
		{
			return * siSprite;
		}
	}
	return NULL;
}

//------
//MIDI播放相關
//------

//------
//打開MIDI設備并開始播放 該方法為循環播放
//------

void GameEngine :: PlayMIDISong(LPTSTR szMIDIFileName, BOOL bRestart)
{
	if(m_uiMIDIPlayerID == 0)
	{
		MCI_OPEN_PARMS mciOpenPrams;
		mciOpenPrams.lpstrDeviceType = "sequencer";
		mciOpenPrams.lpstrElementName = szMIDIFileName;
		if(mciSendCommand(NULL, MCI_OPEN, MCI_OPEN_TYPE | MCI_OPEN_ELEMENT, (DWORD_PTR)&mciOpenPrams) == 0)//0表示成功
		{
			m_uiMIDIPlayerID = mciOpenPrams.wDeviceID;
		}
		else
		{
			return;
		}
	}
	if(bRestart)
	{
		MCI_SEEK_PARMS mciSeekParms;
		if(mciSendCommand(m_uiMIDIPlayerID, MCI_SEEK, MCI_SEEK_TO_START, (DWORD_PTR)&mciSeekParms) != 0)
		{
			CloseMIDIPlayer();
		}
	}
	MCI_PLAY_PARMS mciPlayParms;
	if(mciSendCommand(m_uiMIDIPlayerID, MCI_PLAY, 0, (DWORD_PTR)&mciPlayParms) != 0)
	{
		CloseMIDIPlayer();
	}
	else
	{
		PostMessage(m_hWindow, MM_MCINOTIFY, MCI_NOTIFY_SUCCESSFUL, 0);//原創，以MCI_NOTIFY_SUCCESSFUL為參數強制發送MM_MCINOTIFY消息，實現循環播放，但是會卡（原因不明）
	}
}

//------
//暫停播放MIDI
//------

void GameEngine :: PauseMIDISong()
{
	if(m_uiMIDIPlayerID != 0)
	{
		mciSendCommand(m_uiMIDIPlayerID, MCI_PAUSE, 0, NULL);
	}
}

//------
//關閉MIDI設備
//------

void GameEngine :: CloseMIDIPlayer()
{
	if(m_uiMIDIPlayerID != 0)
	{
		mciSendCommand(m_uiMIDIPlayerID, MCI_CLOSE, 0, NULL);
		m_uiMIDIPlayerID = 0;
	}
}