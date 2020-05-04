#include "StdAfx.h"
#include "LaLabel.h"

#include <assert.h>

CLaLabel::CLaLabel(IPaintableParent* pParent, const RECT* pRect, const char* pText, STYLE* pStyle, const char* pMutexName/* = NULL */)
{
	m_pParent = pParent;
	m_oRect = *pRect;
	m_pStyle = pStyle;

	if (pText)
	{
		strncpy(m_vText, pText, MAX_LABEL_TEXT_LEN);
		m_vText[MAX_LABEL_TEXT_LEN] = '\0';
	}
	else
		m_vText[0] = '\0';

	m_hMutex = (pMutexName ? CreateMutex(NULL, FALSE, pMutexName) : NULL);
}

CLaLabel::~CLaLabel(void)
{
}

BOOL CLaLabel::IsRelated(int x, int y)
{
	return IControl::PointInRect(&m_oRect, x, y);
}

void CLaLabel::GetRect(RECT* r)
{
	*r = m_oRect;
}

BOOL CLaLabel::OnPaint(void* owner, CGraphics* g, const RECT* pRect)
{
	RECT r = m_oRect;
	OffsetRect(&r, -r.left, -r.top);
	g->SetBgColor(m_pStyle->nBgColor);
	g->DrawRect(&r);
	g->SetFont(m_pStyle->pFont);
	g->SetColor(m_pStyle->nColor);
	g->DrawText(&r, m_vText, m_pStyle->nTextFormat);

	return TRUE;
}

void CLaLabel::SetText(const char* pText)
{
	if (pText)
	{
		strncpy(m_vText, pText, MAX_LABEL_TEXT_LEN);
		m_vText[MAX_LABEL_TEXT_LEN] = '\0';
	}
	else
		m_vText[0] = '\0';

	m_pParent->Invalidate(&m_oRect);
}

void CLaLabel::GetText(char* pText, int nSize)
{
	strncpy(pText, m_vText, nSize);
}

const char* CLaLabel::GetText()
{
	return (const char *)m_vText;
}

BOOL CLaLabel::PushChar(char ch)
{
	assert(m_hMutex);
	BOOL r = FALSE;

	if (WAIT_OBJECT_0 == WaitForSingleObject(m_hMutex, 1000L))
	{
		for (int i = 0; i < MAX_LABEL_TEXT_LEN; ++i)
		{
			if (!m_vText[i])
			{
				m_vText[i] = ch;
				m_vText[i + 1] = '\0';
				r = TRUE;
				break;
			}
		}
		ReleaseMutex(m_hMutex);
		m_pParent->Invalidate(&m_oRect);
	}

	return r;
}

char CLaLabel::PopChar()
{
	assert(m_hMutex);

	char r = '\0';
	if (WAIT_OBJECT_0 == WaitForSingleObject(m_hMutex, 1000L))
	{
		r = m_vText[0];
		for (int i = 0; i < MAX_LABEL_TEXT_LEN; ++i)
		{
			if (m_vText[i])
				m_vText[i] = m_vText[i + 1];
			else
				break;
		}

		ReleaseMutex(m_hMutex);
		m_pParent->Invalidate(&m_oRect);
	}

	return r;
}
