#pragma once

#include "EventManagerWin32.h"

class CLaButton 
	: public IPaintEventControl
	, public IMouseMoveEventControl
	, public IMouseKeyEventControl
	, public IKeyboardEventControl
	, public IUserData
{
public:
	typedef enum { BS_UP, BS_DOWN } ButtonState;
	typedef enum { BT_NORMAL, BT_LOCKABLE } ButtonStyle;

public:
	CLaButton(IPaintableParent* pParent, const char* pText, const RECT* pRect, ButtonStyle nStyle = BT_NORMAL, CFont* pFont = NULL);
	virtual ~CLaButton(void);

	virtual BOOL IsRelated(int x, int y);
	virtual void GetRect(RECT* pRect);
	virtual BOOL OnPaint(void* owner, CGraphics* g, const RECT* pRect);
	virtual BOOL OnKeyDown(void* owner, int nKeyCode);
	virtual BOOL OnKeyUp(void* owner, int nKeyCode);
	virtual BOOL OnMouseMove(void* owner, int x, int y);
	virtual BOOL OnMouseKeyDown(void* owner, MouseKeyType nMkt, int x, int y);
	virtual BOOL OnMouseKeyUp(void* owner, MouseKeyType nMkt, int x, int y);
	virtual BOOL OnClick(void* owner);

	virtual void SetRect(const RECT* pRect);
	virtual void SetText(const char* pNewText);

	virtual void SetUserData(void* pUserData) { m_pUserData = pUserData; };
	virtual void* GetUserData() { return m_pUserData; };

	virtual BOOL IsPressed() { return m_bIsDown; };

	void AddKeyboardEventListener(IKeyboardEventListener* pListener);
	void AddMouseMoveEventListener(IMouseMoveEventListener* pListener);
	void AddMouseKeyEventListener(IMouseKeyEventListener* pListener);

	void SetWantKeyCode(int nKeyCode) { m_nWantKeyCode = nKeyCode; };
	int GetWantKeyCode() { return m_nWantKeyCode; };

private:
	IPaintableParent*		m_pParent;
	CFont*		m_pFont;
	vector<char *>		m_vText;
	RECT	m_oRect;
	ButtonStyle		m_nStyle;
	ButtonState		m_nState;
	BOOL	m_bMouseOver;
	int		m_nWantKeyCode;
	BOOL	m_bIsDown;
	BOOL	m_bMouseHold;
	int		m_nIdxTxt;
	int		m_nCntTxt;

	IKeyboardEventListener*		m_pKeyboardEventListener;
	IMouseMoveEventListener*	m_pMouseMoveEventListener;
	IMouseKeyEventListener*		m_pMouseKeyEventListener;

	void*	m_pUserData;

	void InitButton(IPaintableParent* pParent, const char* pText, const RECT* pRect, ButtonStyle nStyle, CFont* pFont);
	void ResolveTextList(const char* pText);
};
