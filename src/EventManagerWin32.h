#pragma once

#include <vector>

#include "Graphics.h"
#include "EventListener.h"

using namespace std;

class IControl
{
public:
	virtual BOOL IsRelated(int x, int y) = 0;
	virtual void GetRect(RECT* r) = 0;

	static BOOL PointInRect(const RECT* pRect, int x, int y);
	static BOOL IntersectRect(RECT* pDest, const RECT* pSrc1, const RECT* pSrc2);
};

class IPaintEventControl : public IControl, public IPaintEventListener
{
};

class IMouseMoveEventControl : public IControl, public IMouseMoveEventListener
{
};

class IKeyboardEventControl : public IKeyboardEventListener
{
};

class IMouseKeyEventControl : public IControl, public IMouseKeyEventListener
{
};

class ITimerEventControl : public ITimerEventListener
{
};

class ICommandEventControl : public ICommandEventListener
{
};

class IUserData
{
public:
	virtual void SetUserData(void* pUserData) = 0;
	virtual void* GetUserData() = 0;
};

class IPaintableParent
{
public:
	virtual void Invalidate(const RECT* pRect) = 0;
	virtual CGraphics* NewGraphics(int w, int h) = 0;
};

class CEventDispatcherWin32
	: public IPaintableParent
{
public:
	CEventDispatcherWin32(HWND hWnd);
	virtual ~CEventDispatcherWin32(void);

	virtual BOOL OnMessage(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);

	virtual void AddPaintEventControl(IPaintEventControl* pCtrl);
	virtual void AddMouseMoveEventControl(IMouseMoveEventControl* pCtrl);
	virtual void AddKeyboardEventControl(IKeyboardEventControl* pCtrl);
	virtual void AddMouseKeyEventControl(IMouseKeyEventControl* pCtrl);
	virtual void AddTimerEventControl(ITimerEventControl* pCtrl, int nTimerId, int nInterval);
	virtual void AddCommandEventControl(ICommandEventControl* pCtrl);

	virtual void Invalidate(const RECT* pRect);
	virtual CGraphics* NewGraphics(int w, int h);

	virtual BOOL EraseProc();
	virtual BOOL PaintProc();
	virtual BOOL MouseMoveProc(int x, int y);
	virtual BOOL KeyDownProc(int nKeyCode);
	virtual BOOL KeyUpProc(int nKeyCode);
	virtual BOOL MouseKeyDownProc(IMouseKeyEventListener::MouseKeyType nMkt, int x, int y);
	virtual BOOL MouseKeyUpProc(IMouseKeyEventListener::MouseKeyType nMkt, int x, int y);
	virtual BOOL TimerProc(int nTimerId);
	virtual BOOL CommandProc(int nCommId);

	virtual void RemovePaintEventControl(IPaintEventControl* pCtrl);
	virtual void RemoveMouseMoveEventControl(IMouseMoveEventControl* pCtrl);
	virtual void RemoveKeyboardEventControl(IKeyboardEventControl* pCtrl);
	virtual void RemoveMouseKeyEventControl(IMouseKeyEventControl* pCtrl);
	virtual void RemoveTimerEventControl(ITimerEventControl* pCtrl);
	virtual void RemoveCommandEventControl(ICommandEventControl* pCtrl);

protected:
	HWND GetHWnd() { return m_hWnd; };
	CGraphics* GetGraphics() { return m_pGraphics; };

private:
	vector<IPaintEventControl *>		m_oPaintEventControls;
	vector<IMouseMoveEventControl *>	m_oMouseMoveEventControls;
	vector<IKeyboardEventControl *>		m_oKeyboardEventControls;
	vector<IMouseKeyEventControl *>		m_oMouseKeyEventControls;
	vector<ITimerEventControl *>		m_oTimerEventControls;
	vector<ICommandEventControl *>		m_oCommandEventControls;

	CGraphics*		m_pGraphics;
	HWND			m_hWnd;
};

