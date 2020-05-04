#pragma once

#include "EventManagerWin32.h"

class CLaLine
	: public IPaintEventControl
{
public:
	CLaLine(IPaintableParent* pParent, int x1, int y1, int x2, int y2);
	virtual ~CLaLine();

	virtual BOOL IsRelated(int x, int y);
	virtual void GetRect(RECT* r);
	virtual BOOL OnPaint(void* owner, CGraphics* g, const RECT* pRect);

private:
	IPaintableParent*		m_pParent;
	RECT	m_oRect;
	int m_x1;
	int m_x2;
	int m_y1;
	int m_y2;

};
