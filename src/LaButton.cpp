#include "StdAfx.h"
#include "LaButton.h"

CLaButton::CLaButton(IPaintableParent* pParent, const char* pText, const RECT* pRect, ButtonStyle nStyle/* = BT_NORMAL */, CFont* pFont/* = NULL */)
{
	InitButton(pParent, pText, pRect, nStyle, pFont);
}

void CLaButton::InitButton(IPaintableParent* pParent, const char* pText, const RECT* pRect, ButtonStyle nStyle, CFont* pFont)
{
	m_nIdxTxt = 0;
	m_pParent = pParent;
	m_oRect = *pRect;
	m_nStyle = nStyle;
	m_pFont = pFont;

	m_nState = BS_UP;
	m_bIsDown = FALSE;
	m_bMouseOver = FALSE;
	m_bMouseHold = FALSE;

	m_pKeyboardEventListener = NULL;
	m_pMouseMoveEventListener = NULL;
	m_pMouseKeyEventListener = NULL;

	m_nWantKeyCode = 0;

	ResolveTextList(pText);
}

CLaButton::~CLaButton(void)
{
	while (0 < m_vText.size())
	{
		delete[] m_vText[m_vText.size() - 1];
		m_vText.pop_back();
	}
}

void CLaButton::AddKeyboardEventListener(IKeyboardEventListener* pListener)
{
	m_pKeyboardEventListener = pListener;
}

void CLaButton::AddMouseMoveEventListener(IMouseMoveEventListener* pListener)
{
	m_pMouseMoveEventListener = pListener;
}

void CLaButton::AddMouseKeyEventListener(IMouseKeyEventListener* pListener)
{
	m_pMouseKeyEventListener = pListener;
}

BOOL CLaButton::IsRelated(int x, int y)
{
	RECT rect = m_oRect;
	BOOL r = IControl::PointInRect(&rect, x, y);
	if (r != m_bMouseOver)
	{
		m_bMouseOver = r;
		m_pParent->Invalidate(&rect);
	}

	return m_bMouseHold || r;
}

void CLaButton::GetRect(RECT* r)
{
	*r = m_oRect;
}

void CLaButton::SetRect(const RECT* r)
{
	m_oRect = *r;
	m_pParent->Invalidate(r);
}

void CLaButton::SetText(const char* pNewText)
{
	while (0 < m_vText.size())
	{
		delete[] m_vText[m_vText.size() - 1];
		m_vText.pop_back();
	}

	ResolveTextList(pNewText);
	m_pParent->Invalidate(&m_oRect);
}

BOOL CLaButton::OnPaint(void* owner, CGraphics* g, const RECT* pRect)
{
	RECT r = m_oRect;
	OffsetRect(&r, -r.left, -r.top);
	
	if (BS_UP == m_nState)
	{
		g->SetBgColor(RGB(255, 255, 255));

		if (m_bMouseOver)
		{
			g->SetColor(RGB(0, 0, 0));
		}
		else
		{
			g->SetColor(RGB(127, 127, 127));
		}
	}
	else
	{
		g->SetColor(RGB(0, 0, 0));
		g->SetBgColor(RGB(240, 240, 240));
	}

	int rw = (r.right - r.left) / 2;
	int rh = (r.bottom - r.top) / 2;
	if (rw > 16) rw = 16;
	if (rh > 16) rh = 16;
	int rd = (rw > rh ? rh : rw);

	g->DrawRoundRect(&r, rd, rd);

	if (m_pFont)
		g->SetFont(m_pFont);

	g->DrawText(&r, m_vText[m_nIdxTxt], DT_VCENTER|DT_CENTER|DT_SINGLELINE);

	return TRUE;
}

void CLaButton::ResolveTextList(const char* pText)
{
	m_nCntTxt = 0;
	for (int i = 0, j; '\0' != pText[i]; i = j, ++m_nCntTxt)
	{
		for (j = i; '\0' != pText[j] && ',' != pText[j]; ++j)
		{
		}

		m_vText.push_back(new char[j - i + 1]);
		strncpy(m_vText[m_nCntTxt], pText + i, j - i);
		m_vText[m_nCntTxt][j - i] = '\0';

		if (',' == pText[j])
			++j;
	}

	m_nIdxTxt = 0;
}

BOOL CLaButton::OnKeyDown(void* owner, int nKeyCode)
{
	if (m_nWantKeyCode == nKeyCode)
	{
		m_nState = BS_DOWN;

		m_pParent->Invalidate(&m_oRect);

		if (m_pKeyboardEventListener)
			m_pKeyboardEventListener->OnKeyDown(this, nKeyCode);

		return TRUE;
	}

	return FALSE;
}

BOOL CLaButton::OnKeyUp(void* owner, int nKeyCode)
{
	if (m_nWantKeyCode == nKeyCode)
	{
		if (BT_LOCKABLE == m_nStyle)
		{
			m_bIsDown = !m_bIsDown;
			m_nState = (m_bIsDown ? BS_DOWN : BS_UP);
		}
		else
			m_nState = BS_UP;

		m_pParent->Invalidate(&m_oRect);

		if (m_pKeyboardEventListener)
			m_pKeyboardEventListener->OnKeyUp(this, nKeyCode);

		OnClick(this);

		return TRUE;
	}

	return FALSE;
}

BOOL CLaButton::OnMouseMove(void* owner, int x, int y)
{
	if (m_pMouseMoveEventListener)
	{
		return m_pMouseMoveEventListener->OnMouseMove(this, x, y);
	}

	return FALSE;
}

BOOL CLaButton::OnMouseKeyDown(void* owner, MouseKeyType nMkt, int x, int y)
{
	m_bMouseHold = TRUE;
	m_nState = BS_DOWN;
	m_pParent->Invalidate(&m_oRect);

	if (m_pMouseKeyEventListener)
		m_pMouseKeyEventListener->OnMouseKeyDown(this, nMkt, x, y);

	return TRUE;
}

BOOL CLaButton::OnMouseKeyUp(void* owner, MouseKeyType nMkt, int x, int y)
{
	m_bMouseHold = FALSE;
	if (BT_LOCKABLE == m_nStyle)
	{
		m_bIsDown = !m_bIsDown;
		m_nState = (m_bIsDown ? BS_DOWN : BS_UP);
	}
	else
		m_nState = BS_UP;

	if (m_nCntTxt == ++m_nIdxTxt)
		m_nIdxTxt = 0;

	m_pParent->Invalidate(&m_oRect);

	if (m_pMouseKeyEventListener)
		m_pMouseKeyEventListener->OnMouseKeyUp(this, nMkt, x, y);

	if (IControl::PointInRect(&m_oRect, x + m_oRect.left, y + m_oRect.top))
		OnClick(this);

	return TRUE;
}

BOOL CLaButton::OnClick(void* owner)
{
	BOOL bProcess = FALSE;
	if (m_pKeyboardEventListener)
		bProcess = m_pKeyboardEventListener->OnClick(this);

	if (m_pMouseKeyEventListener && !bProcess)
		bProcess = m_pMouseKeyEventListener->OnClick(this);

	return bProcess;
}

