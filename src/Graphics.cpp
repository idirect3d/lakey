#include "StdAfx.h"
#include "graphics.h"
#include "stdio.h"

CGraphics::CGraphics(HWND hWnd)
{
	m_hWnd = hWnd;
	GetClientRect(m_hWnd, &m_oSize);
	HDC hDc = GetDC(m_hWnd);
	m_hDc = ::CreateCompatibleDC(hDc);
	m_hBgBm = ::CreateCompatibleBitmap(hDc, m_oSize.right, m_oSize.bottom);
	SelectObject(m_hDc, m_hBgBm);
	ReleaseDC(m_hWnd, hDc);

	SetOrigin(0, 0);

	m_nColor = RGB(0, 0, 0);
	//m_nBgColor = RGB(224, 224, 224);
	m_nBgColor = RGB(255, 255, 255);

	m_hPen = CreatePen(PS_SOLID, 1, m_nColor);
	m_hPenDot = CreatePen(PS_DASH, 1, m_nColor);
	m_hBrush = CreateSolidBrush(m_nBgColor);

	DrawRect(&m_oSize);
//	SetBkMode(m_hDc, TRANSPARENT);
}

CGraphics::CGraphics(HDC hParentDc, int w, int h)
{
	m_oSize.left = m_oSize.top = 0;
	m_oSize.right = w;
	m_oSize.bottom = h;

	m_hWnd = NULL;
	m_hDc = ::CreateCompatibleDC(hParentDc);
	m_hBgBm = ::CreateCompatibleBitmap(hParentDc, m_oSize.right, m_oSize.bottom);
	SelectObject(m_hDc, m_hBgBm);

	SetOrigin(0, 0);

	m_nColor = RGB(0, 0, 0);
	m_nBgColor = RGB(255, 255, 255);

	m_hPen = ::CreatePen(PS_SOLID, 1, m_nColor);
	m_hPenDot = CreatePen(PS_DASH, 1, m_nColor);
	m_hBrush = ::CreateSolidBrush(m_nBgColor);

//	SetBkMode(m_hDc, TRANSPARENT);
}

CGraphics::~CGraphics(void)
{
	::DeleteObject(m_hBrush);
	::DeleteObject(m_hPen);
	::DeleteObject(m_hPenDot);
	::DeleteObject(m_hBgBm);

	if (m_hDc)
		::DeleteDC(m_hDc);
}

void CGraphics::BeginPaint()
{
	if (m_hWnd)
	{
		m_hWndDc = ::BeginPaint(m_hWnd, &m_ps);
		::SetBkMode(m_hDc, TRANSPARENT);
	}
}

void CGraphics::EndPaint()
{
	if (m_hWnd)
	{
		//HDC hWndDc = ::BeginPaint(m_hWnd, &m_ps);

		::BitBlt( m_hWndDc, m_ps.rcPaint.left, m_ps.rcPaint.top
			, m_ps.rcPaint.right - m_ps.rcPaint.left, m_ps.rcPaint.bottom - m_ps.rcPaint.top
			, m_hDc, m_ps.rcPaint.left, m_ps.rcPaint.top, SRCCOPY);

		//printf("%d,%d,%d,%d", m_ps.rcPaint.left, m_ps.rcPaint.right, m_ps.rcPaint.top, m_ps.rcPaint.bottom);
/*		
		::BitBlt( m_hWndDc, m_oSize.left, m_oSize.top
			, m_oSize.right - m_oSize.left, m_oSize.bottom - m_oSize.top
			, m_hDc, 0, 0, SRCCOPY);
*/
		::EndPaint(m_hWnd, &m_ps);
	}
}

const RECT* CGraphics::GetPaintRect()
{
	if (m_hDc)
		return &m_ps.rcPaint;

	return NULL;
}

void CGraphics::SetOrigin(int x, int y)
{
	m_nOriginX = x;
	m_nOriginY = y;
}

void CGraphics::SetColor(int rgb)
{
	m_nColor = rgb;
	::DeleteObject(m_hPen);
	::DeleteObject(m_hPenDot);
	m_hPen = ::CreatePen(PS_SOLID, 1, m_nColor);
	m_hPenDot = ::CreatePen(PS_DOT, 1, m_nColor);
}

void CGraphics::SetBgColor(int rgb)
{
	m_nBgColor = rgb;
	DeleteObject(m_hBrush);
	m_hBrush = CreateSolidBrush(m_nBgColor);
	::SetBkColor(m_hDc, rgb);
}

void CGraphics::DrawRoundRect(const RECT* pRect, int nEllipseWidth, int nEllipseHeight)
{
	::SelectObject(m_hDc, m_hPen);
	::SelectObject(m_hDc, m_hBrush);
	::RoundRect(m_hDc, pRect->left + m_nOriginX, pRect->top + m_nOriginY, pRect->right + m_nOriginX, pRect->bottom + m_nOriginY, nEllipseWidth, nEllipseHeight);
}

void CGraphics::DrawEllipse(const RECT* pRect)
{
	::SelectObject(m_hDc, m_hPen);
	::SelectObject(m_hDc, m_hBrush);
	::Ellipse(m_hDc, pRect->left + m_nOriginX, pRect->top + m_nOriginY, pRect->right + m_nOriginX, pRect->bottom + m_nOriginY);
}

void CGraphics::DrawText(const RECT* pRect, char* pText, int nFormat, BOOL bTransparent /* = NULL */)
{
	if (bTransparent)
	{
		if (TRANSPARENT != ::GetBkMode(m_hDc))
			::SetBkMode(m_hDc, TRANSPARENT);
	}
	else
	{
		if (OPAQUE != ::GetBkMode(m_hDc))
			::SetBkMode(m_hDc, OPAQUE);
	}

	::SetTextColor(m_hDc, m_nColor);
	RECT r = *pRect;
	::OffsetRect(&r, m_nOriginX, m_nOriginY);
	::DrawText(m_hDc, pText, (int)strlen(pText), &r, nFormat);
}

void CGraphics::SetFont(CFont* pFont)
{
	if (pFont)
		::SelectObject(m_hDc, pFont->GetFont());
}

void CGraphics::Copy(const RECT* pDestRect, CGraphics* pSrc, int x, int y)
{
	::BitBlt( m_hDc, pDestRect->left + m_nOriginX, pDestRect->top + m_nOriginY
		, pDestRect->right - pDestRect->left, pDestRect->bottom - pDestRect->top
		, pSrc->m_hDc, x, y, SRCCOPY);
}

void CGraphics::DrawRect(const RECT* pRect)
{
	RECT r = *pRect;
	::OffsetRect(&r, m_nOriginX, m_nOriginY);
	::FillRect(m_hDc, &r, m_hBrush);
}

void CGraphics::DrawLine(int x1, int y1, int x2, int y2)
{
	BOOL r;
	HGDIOBJ old = ::SelectObject(m_hDc, m_hPen);
	r = ::MoveToEx(m_hDc, x1 + m_nOriginX, y1 + m_nOriginY, NULL);
	r = ::LineTo(m_hDc, x2 + m_nOriginX, y2 + m_nOriginY);
}

void CGraphics::DrawLineDot(int x1, int y1, int x2, int y2)
{
	BOOL r;
	HGDIOBJ old = ::SelectObject(m_hDc, m_hPenDot);
	r = ::MoveToEx(m_hDc, x1 + m_nOriginX, y1 + m_nOriginY, NULL);
	r = ::LineTo(m_hDc, x2 + m_nOriginX, y2 + m_nOriginY);
}

CFont::CFont(const char* pFaceName, int nHeight, FontWeight tWeight)
{
	memset(&m_oLogFont, 0, sizeof(m_oLogFont));

	strcpy(m_oLogFont.lfFaceName, pFaceName);
	m_oLogFont.lfHeight = nHeight;
	SetWeight(tWeight);
	m_oLogFont.lfQuality = CLEARTYPE_QUALITY;
}

void CFont::SetFace(const char* pFaceName)
{
	ReleaseHandle();
	strcpy(m_oLogFont.lfFaceName, pFaceName);
}

void CFont::SetHeight(int h)
{
	ReleaseHandle();
	m_oLogFont.lfHeight = h;
}

void CFont::SetWeight(FontWeight tWeight)
{
	ReleaseHandle();
	switch(tWeight)
	{
		case THIN:
			m_oLogFont.lfWeight = 100; break;
		case NORMAL:
			m_oLogFont.lfWeight = 400; break;
		case BOLD:
			m_oLogFont.lfWeight = 700; break;
	}
}

void CFont::SetItalic(BOOL bFlag)
{
	ReleaseHandle();
	m_oLogFont.lfItalic = bFlag;
}

void CFont::SetUnderline(BOOL bFlag)
{
	ReleaseHandle();
	m_oLogFont.lfUnderline = bFlag;
}

void CFont::SetStrikeOut(BOOL bFlag)
{
	ReleaseHandle();
	m_oLogFont.lfStrikeOut = bFlag;
}

HFONT CFont::GetFont()
{
	if (NULL == m_hFont)
		m_hFont = CreateFontIndirect(&m_oLogFont);

	return m_hFont;
}

void CFont::ReleaseHandle()
{
	if (NULL != m_hFont)
	{
		DeleteObject(m_hFont);
		m_hFont = NULL;
	}
}


