#pragma once

#include "EventManagerWin32.h"
#include "MorseParser.h"

class CLaJournalPanel
	: public IPaintEventControl
	, public IParseEventListener
{
public:
	CLaJournalPanel(IPaintableParent* pParent, const RECT* pRect, const MORSECODE* pMorseList, int nMaxShortCount, STYLE* pStyle, IParseEventListener* pParseLsnr);
	virtual ~CLaJournalPanel(void);

	virtual BOOL IsRelated(int x, int y);
	virtual void GetRect(RECT* r);
	virtual BOOL OnPaint(void* owner, CGraphics* g, const RECT* pRect);
	virtual void OnWorkOut(void* owner, const MORSECODE* pResult);

	virtual void Sample(int nState);
	void SetMaxShortCount(int nCount);

private:
	IPaintableParent*		m_pParent;
	IParseEventListener*	m_pParseLsnr;
	STYLE*		m_pStyle;
	RECT	m_oRect;
	CGraphics*	m_pPaintBoard;
	int		m_nCurrPos;
	int		m_nSampleDiv;
	int		m_nJitter;
	int		m_nStateCount;
	int		m_nStep;

	CMorseParser*	m_pMorseParser;

	void SampleRender(int nState);
};
