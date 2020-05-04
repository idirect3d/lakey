#pragma once

#include "EventManagerWin32.h"

#define LA_TUNER_HEARTBEAT_TIMER_ID		10060
typedef float	LASCALE;

class ITunerEventListener
{
public:
	virtual BOOL OnTune(void* owner, LASCALE nScale) = 0;
};

class CLaTuner
	: public IPaintEventControl
	, public IMouseMoveEventControl
	, public IMouseKeyEventControl
	, public ITimerEventControl
{
public:
	CLaTuner(IPaintableParent* pParent, const char* pText, const RECT* pRect, LASCALE nScaleL, LASCALE nScaleR, LASCALE nScale, CFont* pFont);
	virtual ~CLaTuner();

	virtual BOOL IsRelated(int x, int y);
	virtual void GetRect(RECT* r);
	virtual BOOL OnPaint(void* owner, CGraphics* g, const RECT* pRect);
	virtual BOOL OnMouseMove(void* owner, int x, int y);
	virtual BOOL OnMouseKeyDown(void* owner, MouseKeyType nMkt, int x, int y);
	virtual BOOL OnMouseKeyUp(void* owner, MouseKeyType nMkt, int x, int y);
	virtual BOOL OnTimer(void* owner, int nTimerId);
	virtual BOOL OnClick(void* owner) { return FALSE; };

//	void AddKeyboardEventListener(IKeyboardEventListener* pListener);
	void AddMouseMoveEventListener(IMouseMoveEventListener* pListener);
	void AddMouseKeyEventListener(IMouseKeyEventListener* pListener);
	void AddTunerEventListener(ITunerEventListener* pListener);
	void SetScale(LASCALE nScale);
	LASCALE GetScale() { return m_nScaleCurr; };

private:
	IPaintableParent*		m_pParent;

	CFont*		m_pFont;
	vector<char *>		m_vText;
	RECT	m_oRect;
	LASCALE	m_nScaleL;
	LASCALE	m_nScaleR;
	LASCALE	m_nScaleDiv;
	LASCALE	m_nScaleDivD;
	LASCALE	m_nScaleCurr;

	BOOL	m_bMouseOver;
	BOOL	m_bRbuttonHold;
	BOOL	m_bLbuttonHold;
	int		m_nIdxTxt;
	int		m_nCntTxt;

//	IKeyboardEventListener*		m_pKeyboardEventListener;
	IMouseMoveEventListener*	m_pMouseMoveEventListener;
	IMouseKeyEventListener*		m_pMouseKeyEventListener;
	ITunerEventListener*		m_pTunerEventListener;

	void*	m_pUserData;

	void InitButton(IPaintableParent* pParent, const RECT* pRect, const char* pText, LASCALE nScaleL, LASCALE nScaleR, LASCALE nScale, CFont* pFont);
	void ResolveTextList(const char* pText);
};
