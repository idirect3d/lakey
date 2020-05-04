#pragma once

#include "Graphics.h"

class IEventListener
{
};

class IClickEventListener
{
public:
	virtual BOOL OnClick(void* owner) = 0;
};

class IPaintEventListener
{
public:
//	virtual BOOL OnErase(void* owner, CGraphics* g, const RECT* pRect) = 0;
	virtual BOOL OnPaint(void* owner, CGraphics* g, const RECT* pRect) = 0;
};

class IMouseMoveEventListener
{
public:
	virtual BOOL OnMouseMove(void* owner, int x, int y) = 0;
};

class IKeyboardEventListener : public IClickEventListener
{
public:
	virtual BOOL OnKeyDown(void* owner, int nKeyCode) = 0;
	virtual BOOL OnKeyUp(void* owner, int nKeyCode) = 0;
};

class IMouseKeyEventListener : public IClickEventListener
{
public:
	typedef enum { LBUTTON = 1, CBUTTON, RBUTTON } MouseKeyType;

public:
	virtual BOOL OnMouseKeyDown(void* owner, MouseKeyType nMkt, int x, int y) = 0;
	virtual BOOL OnMouseKeyUp(void* owner, MouseKeyType nMkt, int x, int y) = 0;
};

class ITimerEventListener
{
public:
	virtual BOOL OnTimer(void* owner, int nTimerId) = 0;
};

class ICommandEventListener
{
public:
	virtual BOOL OnCommand(void* owner, int nCommId) = 0;
};

