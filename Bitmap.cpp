#include "stdafx.h"
#include "Bitmap.h"

Bitmap :: Bitmap() : m_hBitmap(NULL), m_iWidth(0), m_iHeight(0)
{

}

Bitmap :: Bitmap(HDC hDC, LPTSTR szFileName) : m_hBitmap(NULL), m_iWidth(0), m_iHeight(0)
{
	Create(hDC, szFileName);
}

Bitmap :: Bitmap(HDC hDC, UINT uiResID, HINSTANCE hInstance) : m_hBitmap(NULL), m_iWidth(0), m_iHeight(0)
{
	Create(hDC, uiResID, hInstance);
}

Bitmap :: Bitmap(HDC hDC, int iWidth, int iHeight, COLORREF crColor) : m_hBitmap(NULL), m_iWidth(0), m_iHeight(0)
{
	Create(hDC, iWidth, iHeight, crColor);
}

Bitmap :: ~ Bitmap()
{
	Free();
}

void Bitmap :: Free()
{
	if(m_hBitmap != NULL)
	{
		DeleteObject(m_hBitmap);
		m_hBitmap = NULL;
	}
}

//------
//�ʉߕ������n������
//------

BOOL Bitmap :: Create(HDC hDC, LPTSTR szFileName)
{
	Free();
//------
//�n������
//------
	HANDLE hFile = CreateFile(szFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
//	if(hFile = INVALID_HANDLE_VALUE)
//	{
//		return FALSE;
//	}
//------
//椎�BMP������
//------
	BITMAPFILEHEADER bmfHeader;
	DWORD dwBytesRead;
	BOOL bOK = ReadFile(hFile, &bmfHeader, sizeof(BITMAPFILEHEADER), &dwBytesRead, NULL);
	if((!bOK) || (dwBytesRead != sizeof(BITMAPFILEHEADER)) || (bmfHeader.bfType != 0x4d42))
	{
		CloseHandle(hFile);
		return FALSE;
	}
//------
//椎�BMP�M����
//------
	BITMAPINFO * pBitmapInfo = new BITMAPINFO;
	if(pBitmapInfo != NULL)
	{
		bOK = ReadFile(hFile, pBitmapInfo, sizeof(BITMAPINFOHEADER), &dwBytesRead, NULL);
		if((!bOK) || (dwBytesRead != sizeof(BITMAPINFOHEADER)))
		{
			CloseHandle(hFile);
			Free();
			return FALSE;
		}
	}
//------
//�l�撷�x�a���x
//------
	m_iWidth = pBitmapInfo -> bmiHeader.biWidth;
	m_iHeight = pBitmapInfo -> bmiHeader.biHeight;
//------
//�n���ݔ����BMP
//------
	PBYTE pBitmapBits;
	m_hBitmap = CreateDIBSection(hDC, pBitmapInfo, DIB_RGB_COLORS, (PVOID *)&pBitmapBits, NULL, 0);
	if((m_hBitmap != NULL) && (pBitmapBits != NULL))
	{
		SetFilePointer(hFile, bmfHeader.bfOffBits, NULL, FILE_BEGIN);
		if(bOK = ReadFile(hFile, pBitmapBits, pBitmapInfo -> bmiHeader.biSizeImage, &dwBytesRead, NULL))
		{
			return TRUE;
		}
	}

	Free();

	return FALSE;	
}

//------
//�ʉ�ID�n������
//------

BOOL Bitmap :: Create(HDC hDC, UINT uiResID, HINSTANCE hInstance)
{
	Free();
//------
//�l�掑��
//------	
	HRSRC hResInfo = FindResource(hInstance, MAKEINTRESOURCE(uiResID), RT_BITMAP);
	if(hResInfo == NULL)
	{
		return FALSE;
	}
//------
//���ڎ���
//------
	HGLOBAL hMemBitmap = LoadResource(hInstance, hResInfo);
	if(hMemBitmap == NULL)
	{
		return FALSE;
	}
//------
//���莑��
//------
	PBYTE pBitmapImage = (BYTE *)LockResource(hMemBitmap);
	if(pBitmapImage == NULL)
	{
		FreeResource(hMemBitmap);
		return FALSE;
	}
//------
//�l�����x�a���x
//------
	BITMAPINFO * pBitmapInfo = (BITMAPINFO *)pBitmapImage;
	m_iWidth = pBitmapInfo -> bmiHeader.biWidth;
	m_iHeight = pBitmapInfo -> bmiHeader.biHeight;
//------
//�n���ݔ����BMP
//------
	PBYTE pBitmapBits;
	m_hBitmap = CreateDIBSection(hDC, pBitmapInfo, DIB_RGB_COLORS, (PVOID *)&pBitmapBits, NULL, 0);
	if((m_hBitmap != NULL) && (pBitmapBits != NULL))
	{
		const PBYTE pTempBits = pBitmapImage + pBitmapInfo -> bmiHeader.biSize + pBitmapInfo -> bmiHeader.biClrUsed * sizeof(RGBQUAD);
		CopyMemory(pBitmapBits, pTempBits, pBitmapInfo -> bmiHeader.biSizeImage);

		UnlockResource(hMemBitmap);
		FreeResource(hMemBitmap);
		return TRUE;
	}

	UnlockResource(hMemBitmap);
	FreeResource(hMemBitmap);
	Free();
	return FALSE;	
}

//------
//�n�����F����
//------

BOOL Bitmap :: Create(HDC hDC, int iWidth, int iHeight, COLORREF crColor)
{
	m_hBitmap = CreateCompatibleBitmap(hDC, iWidth, iHeight);
	if(m_hBitmap == NULL)
	{
		return FALSE;
	}

	m_iWidth = iWidth;
	m_iHeight = iHeight;

	HDC hMemDC = CreateCompatibleDC(hDC);
	HBRUSH hBrush = CreateSolidBrush(crColor);
	HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, m_hBitmap);
	RECT rcBitmap = {0, 0, m_iHeight, m_iWidth};

	FillRect(hMemDC, &rcBitmap, hBrush);

	SelectObject(hMemDC, hOldBitmap);
	DeleteDC(hMemDC);
	DeleteObject(hBrush);

	return TRUE;
}

//------
//㉐�����
//------

void Bitmap :: Draw(HDC hDC, int x, int y, BOOL bTrans, COLORREF crTransColor)
{
	DrawPart(hDC, x, y, 0, 0, GetWidth(), GetHeight(), bTrans, crTransColor);
}

//------
//㉐��������s�������� �x���ꚤ��㉐��i�ʉߙB稍��W�����j
//------

void Bitmap :: DrawPart(HDC hDC, int x, int y, int xPart, int yPart, int wPart, int hPart, BOOL bTrans, COLORREF crTransColor)
{
	if(m_hBitmap != NULL)
	{
		HDC hMemDC = CreateCompatibleDC(hDC);	
		HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, m_hBitmap);
		RECT rect;
		HWND hWnd = WindowFromDC(hDC);	
		
		GetClientRect(hWnd, &rect);

		SetStretchBltMode(hDC, HALFTONE);
		if(bTrans == TRUE)
		{
			TransparentBlt(hDC, x, y, wPart, hPart, hMemDC, xPart, yPart, wPart, hPart, crTransColor);
		}
		else
		{
			StretchBlt(hDC, x, y, wPart, hPart, hMemDC, xPart, yPart, wPart, hPart, SRCCOPY);
		}

		SelectObject(hMemDC, hOldBitmap);
		DeleteDC(hMemDC);
	}
}	