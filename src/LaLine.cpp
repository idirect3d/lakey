#include "StdAfx.h"
#include "LaLine.h"

CLaLine::CLaLine(IPaintableParent* pParent, int x1, int y1, int x2, int y2)
{
	m_pParent = pParent;

	m_x1 = x1;
	m_x2 = x2;
	m_y1 = y1;
	m_y2 = y2;

	if (x1 > x2) x1 ^= x2 ^= x1 ^= x2;
	if (y1 > y2) y1 ^= y2 ^= y1 ^= y2;

	m_oRect.left = x1;
	m_oRect.top = y1;
	m_oRect.right = x2 + 1;
	m_oRect.bottom = y2 + 1;

	m_x1 -= m_oRect.left;
	m_x2 -= m_oRect.left;
	m_y1 -= m_oRect.top;
	m_y2 -= m_oRect.top;
}

CLaLine::~CLaLine()
{
}

BOOL CLaLine::IsRelated(int x, int y)
{
	return IControl::PointInRect(&m_oRect, x, y);
}

void CLaLine::GetRect(RECT* r)
{
	*r = m_oRect;
}

BOOL CLaLine::OnPaint(void* owner, CGraphics* g, const RECT* pRect)
{
	g->SetColor(0);
	g->DrawLine(m_x1, m_y1, m_x2, m_y2);

	return TRUE;
}
