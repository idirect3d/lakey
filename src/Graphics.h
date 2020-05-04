#pragma once

#include <windows.h>

class CFont;

typedef struct tagSTYLE
{
	int		nColor;
	int		nBgColor;
	int		nTextFormat;	// win32 DrawText macro: DT_...
	CFont*	pFont;
}STYLE;

class CGraphics
{
public:
	CGraphics(HWND hWnd);
	CGraphics(HDC hParentDc, int w, int h);
	~CGraphics(void);

	//void SetErase() { m_bNeedErase = TRUE; };
	void BeginPaint();
	void EndPaint();
	const RECT* GetPaintRect();

	void SetOrigin(int x, int y);
	void DrawRoundRect(const RECT* pRect, int nEllipseWidth, int nEllipseHeight);
	void DrawText(const RECT* pRect, char* pText, int nFormat, BOOL bTransparent = TRUE);
	void SetColor(int rgb);
	void SetBgColor(int rgb);
	void SetFont(CFont* pFont);
	void Copy(const RECT* pDestRect, CGraphics* pSrc, int x, int y);
	void DrawRect(const RECT* pRect);
	void DrawLine(int x1, int y1, int x2, int y2);
	void DrawLineDot(int x1, int y1, int x2, int y2);
	void DrawEllipse(const RECT* pRect);

private:
	HWND			m_hWnd;
	HDC				m_hWndDc;
	HDC				m_hDc;
	PAINTSTRUCT		m_ps;
	HBITMAP			m_hBgBm;

	int				m_nOriginX;
	int				m_nOriginY;

	int				m_nColor;
	int				m_nBgColor;

	HBRUSH			m_hBrush;
	HPEN			m_hPen;
	HPEN			m_hPenDot;

	//BOOL			m_bNeedErase;
	RECT			m_oSize;
};

class CFont
{
public:
	typedef enum { THIN, NORMAL, BOLD } FontWeight;

	CFont(const char* pFaceName, int nSize, FontWeight tWeight);
	~CFont() { ReleaseHandle(); };

	void SetFace(const char* pFaceName);
	void SetHeight(int nSize);
	void SetWeight(FontWeight tWeight);
	void SetItalic(BOOL bFlag);
	void SetUnderline(BOOL bFlag);
	void SetStrikeOut(BOOL bFlag);

	HFONT	GetFont();

private:
	void ReleaseHandle();

	LOGFONT		m_oLogFont;
	HFONT		m_hFont;
};

