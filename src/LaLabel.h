#pragma once

#include "EventManagerWin32.h"

#define MAX_LABEL_TEXT_LEN		8193

class CLaLabel
	: public IPaintEventControl
{
public:
	CLaLabel(IPaintableParent* pParent, const RECT* pRect, const char* pText, STYLE* pStyle, const char* pMutexName = NULL);
	virtual ~CLaLabel(void);

	virtual BOOL IsRelated(int x, int y);
	virtual void GetRect(RECT* r);
	virtual BOOL OnPaint(void* owner, CGraphics* g, const RECT* pRect);

	virtual void SetText(const char* pText);
	virtual void GetText(char* pText, int nSize);
	virtual const char* GetText();

	virtual BOOL PushChar(char ch);
	virtual char PopChar();

private:
	IPaintableParent*		m_pParent;
	STYLE*		m_pStyle;
	RECT		m_oRect;

	char		m_vText[MAX_LABEL_TEXT_LEN + 1];

	HANDLE		m_hMutex;
};
