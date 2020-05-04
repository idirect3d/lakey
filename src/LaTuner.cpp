#include "StdAfx.h"
#include "latuner.h"
#include "FFT.h"

CLaTuner::CLaTuner(IPaintableParent* pParent, const char* pText, const RECT* pRect, LASCALE nScaleL, LASCALE nScaleR, LASCALE nScale, CFont* pFont)
{
	InitButton(pParent, pRect, pText, nScaleL, nScaleR, nScale, pFont);
}

CLaTuner::~CLaTuner()
{
}

void CLaTuner::SetScale(LASCALE nScale)
{
	m_nScaleCurr = nScale;
	m_pParent->Invalidate(&m_oRect);
}

void CLaTuner::InitButton(IPaintableParent* pParent, const RECT* pRect, const char* pText, LASCALE nScaleL, LASCALE nScaleR, LASCALE nScale, CFont* pFont)
{
	m_nIdxTxt = 0;
	m_pParent = pParent;
	m_oRect = *pRect;
	m_pFont = pFont;

	m_nScaleL = nScaleL;
	m_nScaleR = nScaleR;
	m_nScaleDiv = (nScaleR - nScaleL) / 256;
	m_nScaleDivD = 0;
	m_nScaleCurr = nScale;

	m_bMouseOver = FALSE;
	m_bRbuttonHold = FALSE;
	m_bLbuttonHold = FALSE;

//	m_pKeyboardEventListener = NULL;
	m_pMouseMoveEventListener = NULL;
	m_pMouseKeyEventListener = NULL;
	m_pTunerEventListener = NULL;

	ResolveTextList(pText);
	
	((CEventDispatcherWin32 *)pParent)->AddTimerEventControl(this, LA_TUNER_HEARTBEAT_TIMER_ID, 60);
}

BOOL CLaTuner::IsRelated(int x, int y)
{
	RECT rect = m_oRect;
	BOOL r = IControl::PointInRect(&rect, x, y);
	if (r != m_bMouseOver)
	{
		m_bMouseOver = r;
		m_pParent->Invalidate(&rect);
	}

	return m_bRbuttonHold || m_bLbuttonHold || r;
}

void CLaTuner::GetRect(RECT* r)
{
	*r = m_oRect;
}

BOOL CLaTuner::OnPaint(void* owner, CGraphics* g, const RECT* pRect)
{
	char buff[16];
	RECT a = m_oRect;
	OffsetRect(&a, -a.left, -a.top);
	
	g->SetBgColor(RGB(255, 255, 255));

	if (m_bMouseOver)
	{
		g->SetColor(RGB(0, 0, 0));
	}
	else
	{
		g->SetColor(RGB(127, 127, 127));
	}

	if (m_pFont)
		g->SetFont(m_pFont);

	RECT t = a;
	int r = (int)((a.bottom < a.right ? a.bottom : a.right) / 3.2f);
	int ox = a.right / 2;
	int oy = a.bottom / 2;
	t.left = ox - r;
	t.right = ox + r;
	t.top = oy - r;
	t.bottom = oy + r;

	g->DrawEllipse(&t);
	if (m_bMouseOver || m_bLbuttonHold || m_bRbuttonHold)
	{
		if (abs(m_nScaleR - m_nScaleL) > 20)
			sprintf(buff, "%0.0f", m_nScaleCurr);
		else if (abs(m_nScaleR - m_nScaleL) > 2)
			sprintf(buff, "%0.1f", m_nScaleCurr);
		else
			sprintf(buff, "%0.2f", m_nScaleCurr);

		g->DrawText(&t, buff, DT_VCENTER|DT_CENTER|DT_SINGLELINE);
	}

	int nScaleLen = r / 5;
	int nSplitWidth = nScaleLen / 2;
	float rx, ry;
	int nScaleCount = 10;
	float srad = 0.75f / nScaleCount;
	for (int i = 0; i <= nScaleCount; ++i)
	{
		rx = cosf(2 * PI * (i * srad + 0.375f));
		ry = sinf(2 * PI * (i * srad + 0.375f));

		if (0 == i || i == nScaleCount)
		{
			int tx = (int)(ox + (r + nScaleLen * 2 + nSplitWidth) * rx);
			int ty = (int)(oy + (r + nScaleLen * 2 + nSplitWidth) * ry);
			g->DrawLine((int)(ox + (r + nSplitWidth) * rx), (int)(oy + (r + nSplitWidth) * ry)
				, tx, ty);
			
			g->DrawLine(tx, ty, 0 == i ? 0 : a.right, ty);
		}
		else
		{
			g->DrawLine((int)(ox + (r + nSplitWidth) * rx), (int)(oy + (r + nSplitWidth) * ry)
				, (int)(ox + (r + nScaleLen + nSplitWidth) * rx), (int)(oy + (r + nScaleLen + nSplitWidth) * ry));
		}
	}

	t.left = 0;
	t.right = ox - r;
	t.top = 0;
	t.bottom = oy + r + 0;
	sprintf(buff, "%0.0f", m_nScaleL);
	g->DrawText(&t, buff, DT_BOTTOM|DT_CENTER|DT_SINGLELINE, FALSE);

	t.left = ox + r + 2;
	t.right = a.right;
	sprintf(buff, "%0.0f", m_nScaleR);
	g->DrawText(&t, buff, DT_BOTTOM|DT_CENTER|DT_SINGLELINE, FALSE);

	LASCALE rd = 2 * PI * 0.75f * (m_nScaleCurr - m_nScaleL) / (m_nScaleR - m_nScaleL) + PI * 0.75f;
	int px = (int)((r - 8) * cosf(rd) + ox);
	int py = (int)((r - 8) * sinf(rd) + oy);
	t.left = px - 3;
	t.right = px + 3;
	t.top = py - 3;
	t.bottom = py + 3;

	g->DrawEllipse(&t);

	g->DrawText(&a, m_vText[m_nIdxTxt], DT_BOTTOM|DT_CENTER|DT_SINGLELINE, FALSE);

	return TRUE;
}

BOOL CLaTuner::OnMouseMove(void* owner, int x, int y)
{
	return FALSE;
}

BOOL CLaTuner::OnMouseKeyDown(void* owner, MouseKeyType nMkt, int x, int y)
{
	switch(nMkt)
	{
	case LBUTTON:
		m_bLbuttonHold = TRUE; break;
	case RBUTTON:
		m_bRbuttonHold = TRUE; break;
	default:
		return FALSE;
	}

	m_nScaleDivD = 0;

	return TRUE;
}

BOOL CLaTuner::OnMouseKeyUp(void* owner, MouseKeyType nMkt, int x, int y)
{
	switch(nMkt)
	{
	case LBUTTON:
		m_bLbuttonHold = FALSE; break;
	case RBUTTON:
		m_bRbuttonHold = FALSE; break;
	default:
		return FALSE;
	}

	m_nScaleDivD = 0;

	m_pParent->Invalidate(&m_oRect);

	return TRUE;
}

BOOL CLaTuner::OnTimer(void* owner, int nTimerId)
{
	if (LA_TUNER_HEARTBEAT_TIMER_ID == nTimerId
		&& m_bMouseOver)
	{
		if (m_bRbuttonHold)
			m_nScaleCurr += (m_nScaleDiv + m_nScaleDivD);
		else if (m_bLbuttonHold)
			m_nScaleCurr -= (m_nScaleDiv + m_nScaleDivD);
		else
			return FALSE;

		m_nScaleDivD += m_nScaleDiv;

		if (m_nScaleDiv < 0)
		{
			if (m_nScaleCurr < this->m_nScaleR)
				m_nScaleCurr = m_nScaleR;
			else if (m_nScaleCurr > this->m_nScaleL)
				m_nScaleCurr = m_nScaleL;
		}
		else
		{
			if (m_nScaleCurr < this->m_nScaleL)
				m_nScaleCurr = m_nScaleL;
			else if (m_nScaleCurr > this->m_nScaleR)
				m_nScaleCurr = m_nScaleR;
		}
		
		m_pParent->Invalidate(&m_oRect);

		if (m_pTunerEventListener)
			m_pTunerEventListener->OnTune(this, m_nScaleCurr);

		return TRUE;
	}

	return FALSE;
}

/*void CLaTuner::AddKeyboardEventListener(IKeyboardEventListener* pListener)
{
	m_pKeyboardEventListener = pListener;
}*/

void CLaTuner::AddMouseMoveEventListener(IMouseMoveEventListener* pListener)
{
	m_pMouseMoveEventListener = pListener;
}

void CLaTuner::AddMouseKeyEventListener(IMouseKeyEventListener* pListener)
{
	m_pMouseKeyEventListener = pListener;
}

void CLaTuner::AddTunerEventListener(ITunerEventListener* pListener)
{
	m_pTunerEventListener = pListener;
}

void CLaTuner::ResolveTextList(const char* pText)
{
	m_nCntTxt = 0;
	for (int i = 0, j; '\0' != pText[i]; i = j, ++m_nCntTxt)
	{
		for (j = i; '\0' != pText[j] && ',' != pText[j]; ++j)
		{
		}

		m_vText.push_back(new char[j - i + 1]);
		strncpy(m_vText[m_nCntTxt], pText + i, j - i);
		m_vText[m_nCntTxt][j - i] = '\0';

		if (',' == pText[j])
			++j;
	}

	m_nIdxTxt = 0;
}

