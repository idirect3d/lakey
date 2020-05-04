#include "StdAfx.h"
#include "lajournalpanel.h"

CLaJournalPanel::CLaJournalPanel(IPaintableParent* pParent, const RECT* pRect, const MORSECODE* pMorseList
	, int nMaxShortCount, STYLE* pStyle, IParseEventListener* pParseLsnr)
{
	m_pParent = pParent;
	m_pParseLsnr = pParseLsnr;
	m_oRect = *pRect;
	m_pStyle = pStyle;
	m_pPaintBoard = pParent->NewGraphics(pRect->right - pRect->left, pRect->bottom - pRect->top);
	RECT r;
	SetRect(&r, 0, 0, pRect->right - pRect->left, pRect->bottom - pRect->top);
	m_pPaintBoard->DrawRect(&r);
	//m_pPaintBoard->DrawLine(0, pRect->bottom - pRect->top - 1, pRect->right - pRect->left, pRect->bottom - pRect->top - 1);
	m_nCurrPos = 0;
	m_pMorseParser = new CMorseParser(pMorseList, MORSECODECOUNT, nMaxShortCount, this);
	SetMaxShortCount(nMaxShortCount);
	m_nJitter = 0;
	m_nStateCount = 0;
	m_nStep = 1;
}

CLaJournalPanel::~CLaJournalPanel(void)
{
	delete m_pMorseParser;
	delete m_pPaintBoard;
}

BOOL CLaJournalPanel::IsRelated(int x, int y)
{
	return IControl::PointInRect(&m_oRect, x, y);
}

void CLaJournalPanel::GetRect(RECT* r)
{
	*r = m_oRect;
}

BOOL CLaJournalPanel::OnPaint(void* owner, CGraphics* g, const RECT* pRect)
{
	int w = m_oRect.right - m_oRect.left;
	int h = m_oRect.bottom - m_oRect.top;

	RECT r;
	r.top = 0; r.bottom = h;

	r.left = 0; r.right = w - m_nCurrPos;
	g->Copy(&r, m_pPaintBoard, m_nCurrPos, 0);

	if (0 < m_nCurrPos)
	{
		r.left = w - m_nCurrPos; r.right = w;
		g->Copy(&r, m_pPaintBoard, 0, 0);
	}

	return TRUE;
}

void CLaJournalPanel::Sample(int nState)
{
	// sample filter
	switch(m_nStep)
	{
		case 1:	// init
		{
			if (3 > m_nSampleDiv)
			{
			}
			else if (nState)	// key down
			{
				m_nStateCount = 1;
				m_nStep = 2;
				return;
			}

			if (m_nSampleDiv <= ++m_nStateCount)
			{
				SampleRender(nState);
				m_pMorseParser->Sample(nState);
				m_nStateCount = 0;
			}
			
			break;
		}
		case 2:	// jitter(L) test
		{
			++m_nStateCount;

			if (!nState)
			{
				m_nStep = 3;
			}
			else if (m_nSampleDiv <= m_nStateCount)
			{
				SampleRender(nState);
				m_pMorseParser->Sample(nState);
				m_nStep = 1;
				break;
			}

			return;
		}
		case 3: // jitter(H) test
		{
			if (nState)
			{
				if (m_nSampleDiv <= ++m_nStateCount)
				{
					SampleRender(nState);
					m_pMorseParser->Sample(nState);
					m_nStateCount = 0;
					m_nStep = 1;
					break;
				}
				else
					m_nStep = 2;
			}
			else
			{
				m_nStateCount = 2;
				m_nStep = 1;	// Á¬Ðøkey up
			}

			return;
		}
	}

	m_pParent->Invalidate(&m_oRect);
}

void CLaJournalPanel::SampleRender(int nState)
{
	if (nState)
		m_pPaintBoard->SetColor(m_pStyle->nColor);
	else
		m_pPaintBoard->SetColor(m_pStyle->nBgColor);

	int y = 2;

	m_pPaintBoard->DrawLine(m_nCurrPos, y, m_nCurrPos, y + 5);

	m_pPaintBoard->SetColor(m_pStyle->nBgColor);
	m_pPaintBoard->DrawLine(m_nCurrPos, m_oRect.bottom - m_oRect.top - 16, m_nCurrPos, m_oRect.bottom - m_oRect.top - 1);

	if (++m_nCurrPos >= m_oRect.right - m_oRect.left)
		m_nCurrPos = 0;
}

void CLaJournalPanel::OnWorkOut(void* owner, const MORSECODE* pResult)
{
	char buff[] = { '#', '\0' };

	if (pResult)
		buff[0] = pResult->nAscCode;

	m_pPaintBoard->SetColor(m_pStyle->nColor);
	m_pPaintBoard->SetFont(m_pStyle->pFont);

	RECT r;
	r.right = m_nCurrPos;
	r.left = r.right - 12;
	r.bottom = m_oRect.bottom - m_oRect.top;
	r.top = r.bottom - 16;
	m_pPaintBoard->DrawText(&r, buff, DT_VCENTER|DT_CENTER|DT_SINGLELINE);

	if (r.left < 0)
	{
		int w = m_oRect.right - m_oRect.left;
		r.right += w;
		r.left += w;
		m_pPaintBoard->DrawText(&r, buff, DT_VCENTER|DT_CENTER|DT_SINGLELINE);
	}

	if (m_pParseLsnr)
		m_pParseLsnr->OnWorkOut(this, pResult);
}

void CLaJournalPanel::SetMaxShortCount(int nCount)
{
	m_nSampleDiv = nCount / 3;
	if (0 == m_nSampleDiv)
		m_nSampleDiv = 1;

	m_pMorseParser->SetMaxShortCount(nCount / m_nSampleDiv);
}
