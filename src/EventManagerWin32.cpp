#include "StdAfx.h"
#include "eventmanagerwin32.h"

CEventDispatcherWin32::CEventDispatcherWin32(HWND hWnd)
{
	m_hWnd = hWnd;
	m_pGraphics = new CGraphics(hWnd);
}

CEventDispatcherWin32::~CEventDispatcherWin32(void)
{
	delete m_pGraphics;
}

BOOL CEventDispatcherWin32::OnMessage(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	BOOL r = FALSE;

	switch (message) 
	{
		case WM_ERASEBKGND:
			r = EraseProc(); break;
		case WM_PAINT:
			r = PaintProc(); break;
		case WM_MOUSEMOVE:
			r = MouseMoveProc(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); break;
		case WM_KEYDOWN:
			r = KeyDownProc((int)wParam); break;
		case WM_KEYUP:
			r = KeyUpProc((int)wParam); break;
		case WM_LBUTTONDOWN:
			r = MouseKeyDownProc(IMouseKeyEventListener::LBUTTON, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); break;
		case WM_LBUTTONUP:
			r = MouseKeyUpProc(IMouseKeyEventListener::LBUTTON, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); break;
		case WM_RBUTTONDOWN:
			r = MouseKeyDownProc(IMouseKeyEventListener::RBUTTON, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); break;
		case WM_RBUTTONUP:
			r = MouseKeyUpProc(IMouseKeyEventListener::RBUTTON, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); break;
		case WM_TIMER:
			r = TimerProc((int)wParam); break;
		case WM_COMMAND:
		{
			int nCtrlId = LOWORD(wParam);
			if (nCtrlId)
				r = CommandProc(nCtrlId);
			break;
		}
	}

	return r;
}

void CEventDispatcherWin32::AddPaintEventControl(IPaintEventControl* pCtrl)
{
	m_oPaintEventControls.push_back(pCtrl);
}

void CEventDispatcherWin32::AddMouseMoveEventControl(IMouseMoveEventControl* pCtrl)
{
	m_oMouseMoveEventControls.push_back(pCtrl);
}

void CEventDispatcherWin32::AddKeyboardEventControl(IKeyboardEventControl* pCtrl)
{
	m_oKeyboardEventControls.push_back(pCtrl);
}

void CEventDispatcherWin32::AddMouseKeyEventControl(IMouseKeyEventControl* pCtrl)
{
	m_oMouseKeyEventControls.push_back(pCtrl);
}

void CEventDispatcherWin32::AddTimerEventControl(ITimerEventControl* pCtrl, int nTimerId, int nInterval)
{
	m_oTimerEventControls.push_back(pCtrl);
	SetTimer(m_hWnd, nTimerId, nInterval, NULL);
}

void CEventDispatcherWin32::AddCommandEventControl(ICommandEventControl* pCtrl)
{
	m_oCommandEventControls.push_back(pCtrl);
}

void CEventDispatcherWin32::RemovePaintEventControl(IPaintEventControl* pCtrl)
{
	for (vector<IPaintEventControl *>::iterator it = m_oPaintEventControls.begin();
		it != m_oPaintEventControls.end();
		++it)
	{
		if (pCtrl == (*it))
		{
			m_oPaintEventControls.erase(it);
			break;
		}
	}
}

void CEventDispatcherWin32::RemoveMouseMoveEventControl(IMouseMoveEventControl* pCtrl)
{
	for (vector<IMouseMoveEventControl *>::iterator it = m_oMouseMoveEventControls.begin();
		it != m_oMouseMoveEventControls.end();
		++it)
	{
		if (pCtrl == (*it))
		{
			m_oMouseMoveEventControls.erase(it);
			break;
		}
	}
}

void CEventDispatcherWin32::RemoveKeyboardEventControl(IKeyboardEventControl* pCtrl)
{
	for (vector<IKeyboardEventControl *>::iterator it = m_oKeyboardEventControls.begin();
		it != m_oKeyboardEventControls.end();
		++it)
	{
		if (pCtrl == (*it))
		{
			m_oKeyboardEventControls.erase(it);
			break;
		}
	}
}

void CEventDispatcherWin32::RemoveMouseKeyEventControl(IMouseKeyEventControl* pCtrl)
{
	for (vector<IMouseKeyEventControl *>::iterator it = m_oMouseKeyEventControls.begin();
		it != m_oMouseKeyEventControls.end();
		++it)
	{
		if (pCtrl == (*it))
		{
			m_oMouseKeyEventControls.erase(it);
			break;
		}
	}
}

void CEventDispatcherWin32::RemoveTimerEventControl(ITimerEventControl* pCtrl)
{
	for (vector<ITimerEventControl *>::iterator it = m_oTimerEventControls.begin();
		it != m_oTimerEventControls.end();
		++it)
	{
		if (pCtrl == (*it))
		{
			m_oTimerEventControls.erase(it);
			break;
		}
	}
}

void CEventDispatcherWin32::RemoveCommandEventControl(ICommandEventControl* pCtrl)
{
	for (vector<ICommandEventControl *>::iterator it = m_oCommandEventControls.begin();
		it != m_oCommandEventControls.end();
		++it)
	{
		if (pCtrl == (*it))
		{
			m_oCommandEventControls.erase(it);
			break;
		}
	}
}

void CEventDispatcherWin32::Invalidate(const RECT* pRect)
{
	::InvalidateRect(m_hWnd, pRect, TRUE);
}

CGraphics* CEventDispatcherWin32::NewGraphics(int w, int h)
{
	HDC hDc = GetDC(m_hWnd);
	CGraphics* g = new CGraphics(hDc, w, h);
	ReleaseDC(m_hWnd, hDc);
	return g;
}

BOOL CEventDispatcherWin32::EraseProc()
{
//	m_pGraphics->SetErase();
	return TRUE;
}

BOOL CEventDispatcherWin32::PaintProc()
{
	BOOL r = FALSE;
	m_pGraphics->BeginPaint();
	for (vector<IPaintEventControl *>::iterator it = m_oPaintEventControls.begin();
		it != m_oPaintEventControls.end();
		++it)
	{
		RECT cr, dr;
		(*it)->GetRect(&cr);
		const RECT* pRect = m_pGraphics->GetPaintRect();
		if (NULL == pRect || IntersectRect(&dr, pRect, &cr))
		{
			OffsetRect(&dr, -cr.left, -cr.top);
			m_pGraphics->SetOrigin(cr.left, cr.top);
			r |= (*it)->OnPaint((*it), m_pGraphics, &dr);
		}
	}
	m_pGraphics->EndPaint();

	return r;
}

BOOL CEventDispatcherWin32::MouseMoveProc(int x, int y)
{
	BOOL r = FALSE;
	for (vector<IMouseMoveEventControl *>::iterator it = m_oMouseMoveEventControls.begin();
		it != m_oMouseMoveEventControls.end();
		++it)
	{
		if ((*it)->IsRelated(x, y))
		{
			RECT cr;
			(*it)->GetRect(&cr);
			r |= (*it)->OnMouseMove((*it), x - cr.left, y - cr.top);
		}
	}

	return r;
}

BOOL CEventDispatcherWin32::KeyDownProc(int nKeyCode)
{
	BOOL r = FALSE;
	for (vector<IKeyboardEventControl *>::iterator it = m_oKeyboardEventControls.begin();
		it != m_oKeyboardEventControls.end();
		++it)
	{
		r |= (*it)->OnKeyDown((*it), nKeyCode);
	}

	return r;
}

BOOL CEventDispatcherWin32::KeyUpProc(int nKeyCode)
{
	BOOL r = FALSE;
	for (vector<IKeyboardEventControl *>::iterator it = m_oKeyboardEventControls.begin();
		it != m_oKeyboardEventControls.end();
		++it)
	{
		r |= (*it)->OnKeyUp((*it), nKeyCode);
	}

	return r;
}

BOOL CEventDispatcherWin32::MouseKeyDownProc(IMouseKeyEventControl::MouseKeyType nMkt, int x, int y)
{
	BOOL r = FALSE;
	for (vector<IMouseKeyEventControl *>::iterator it = m_oMouseKeyEventControls.begin();
		it != m_oMouseKeyEventControls.end();
		++it)
	{
		if ((*it)->IsRelated(x, y))
		{
			RECT cr;
			(*it)->GetRect(&cr);
			r |= (*it)->OnMouseKeyDown((*it), nMkt, x - cr.left, y - cr.top);
		}
	}

	return r;
}

BOOL CEventDispatcherWin32::MouseKeyUpProc(IMouseKeyEventListener::MouseKeyType nMkt, int x, int y)
{
	BOOL r = FALSE;
	for (vector<IMouseKeyEventControl *>::iterator it = m_oMouseKeyEventControls.begin();
		it != m_oMouseKeyEventControls.end();
		++it)
	{
		if ((*it)->IsRelated(x, y))
		{
			RECT cr;
			(*it)->GetRect(&cr);
			r |= (*it)->OnMouseKeyUp((*it), nMkt, x - cr.left, y - cr.top);
		}
	}

	return r;
}

BOOL CEventDispatcherWin32::TimerProc(int nTimerId)
{
	BOOL r = FALSE;
	for (vector<ITimerEventControl *>::iterator it = m_oTimerEventControls.begin();
		it != m_oTimerEventControls.end();
		++it)
	{
		r |= (*it)->OnTimer((*it), nTimerId);
	}

	return r;
}

BOOL CEventDispatcherWin32::CommandProc(int nCommId)
{
	BOOL r = FALSE;
	for (vector<ICommandEventControl *>::iterator it = m_oCommandEventControls.begin();
	it != m_oCommandEventControls.end();
		++it)
	{
		r |= (*it)->OnCommand((*it), nCommId);
	}

	return r;
}

BOOL IControl::PointInRect(const RECT* pRect, int x, int y)
{
	POINT pt;
	pt.x = x;
	pt.y = y;

	return ::PtInRect(pRect, pt);
}

BOOL IControl::IntersectRect(RECT* pDest, const RECT* pSrc1, const RECT* pSrc2)
{
	return ::IntersectRect(pDest, pSrc1, pSrc2);
}


