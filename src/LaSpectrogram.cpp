#include "StdAfx.h"
#include "LaSpectrogram.h"

CLaSpectrogram::CLaSpectrogram(IPaintableParent* pParent, const RECT* pRect, int nSamplePerSec, int nAnalyzeSamples)
{
	m_nBorderWidth = 1;
	m_pFont = new CFont("Arial", 8, CFont::THIN);
	m_pParent = pParent;
	m_oRect = *pRect;
	int w = m_oRect.right - m_oRect.left;
	int h = m_oRect.bottom - m_oRect.top;
	SetRect(&m_oPaintRect, m_nBorderWidth, m_nBorderWidth, w - m_nBorderWidth, h - m_nBorderWidth);

	m_nBgColor = 0x000000;
	m_nFrColor = 0xffff00;

	m_pPaintBoard = pParent->NewGraphics(m_oPaintRect.right - m_oPaintRect.left, m_oPaintRect.bottom - m_oPaintRect.top);

	m_nAnalyzeSamples = nAnalyzeSamples;

	m_bFraze = FALSE;

	h = m_oPaintRect.bottom - m_oPaintRect.top;
	m_rPaintScale = (float)(h) / (float)m_nAnalyzeSamples;
	m_rPaintScaleNext = m_rPaintScale;
	m_rPaintIdx = 0;
	m_nCurrPos = 0;
	m_vFilterBuf = new float[h];
	m_nPhases = -1;
	m_nPhasesDiv = 512 / m_nAnalyzeSamples;
	m_bOrgSize = FALSE;
}

CLaSpectrogram::~CLaSpectrogram(void)
{
	delete[] m_vFilterBuf;
	delete m_pFont;
	delete m_pPaintBoard;
}

BOOL CLaSpectrogram::Initialize()
{
	return TRUE;
}

BOOL CLaSpectrogram::IsRelated(int x, int y)
{
	return IControl::PointInRect(&m_oRect, x, y);
}

void CLaSpectrogram::GetRect(RECT* r)
{
	*r = m_oRect;
}

BOOL CLaSpectrogram::OnPaint(void* owner, CGraphics* g, const RECT* pRect)
{
	int w = m_oPaintRect.right - m_oPaintRect.left;
	int h = m_oPaintRect.bottom - m_oPaintRect.top;

	RECT r = m_oPaintRect;
	r.right -= (m_nCurrPos + 1);

	if (m_nCurrPos < w - 1)
	{
		g->Copy(&r, m_pPaintBoard, m_nCurrPos + 1, 0);
	}

	r.left = r.right; r.right = r.left + m_nCurrPos + 1;
	g->Copy(&r, m_pPaintBoard, 0, 0);

	w = m_oRect.right - m_oRect.left - 1;
	h = m_oRect.bottom - m_oRect.top - 1;
	g->SetColor(0x808080);
	g->DrawLine(0, 0, 0, h + 1);
	g->DrawLine(0, 0, w + 1, 0);
	g->SetColor(0xffffff);
	g->DrawLine(w, 0, w, h + 1);
	g->DrawLine(0, h, w + 1, h);

	return TRUE;
}

short int CLaSpectrogram::OnEncodeFilter(float val, int i)
{
	if (0 == i)
	{
		if (++m_nPhases > m_nPhasesDiv)
		{
			Refresh();
			if (++m_nCurrPos >= m_oPaintRect.right - m_oPaintRect.left)
				m_nCurrPos = 0;

			m_nPhases = 0;
			m_rPaintScale = m_rPaintScaleNext;
		}

		m_rPaintIdx = 0.0f;
	}

	float currIdx = m_rPaintIdx;
	m_rPaintIdx += m_rPaintScale;
	float w = abs(val) * m_rPaintScale / 256.0f * m_rBrightnessScale;
	int j = (int)currIdx + 1;

	if (j < m_oPaintRect.bottom - m_oPaintRect.top)
	{
		for (; j <= (int)m_rPaintIdx; currIdx = j++)
		{
			m_vFilterBuf[j - 1] += w * (j - currIdx);
		}

		m_vFilterBuf[j - 1] += w * (m_rPaintIdx - currIdx);
	}


	return val;
}

void CLaSpectrogram::SetBrightness(float scale)
{
	m_rBrightnessScale = pow(10.0f, scale / 20.0f);
}

short int CLaSpectrogram::OnDecodeFilter(float val, int i)
{
	return val;
}

/***
 * 刷新控件，包括：重画标尺，读取音频数据，分析并输出
 *
 */
BOOL CLaSpectrogram::Refresh()
{
	int h = m_oPaintRect.bottom - m_oPaintRect.top;
	int lval = min(abs(m_vFilterBuf[0]), 255);
	int lpos = 0, i;

	for (i = lpos + 1; i < h; ++i)
	{
		if (lval != m_vFilterBuf[i])
		{
			m_pPaintBoard->SetColor(RGB(lval, lval, lval));
			m_pPaintBoard->DrawLine(m_nCurrPos, m_oPaintRect.bottom - (lpos + m_oPaintRect.top) - 1, m_nCurrPos, m_oPaintRect.bottom - (i + m_oPaintRect.top) - 1);
			lval = min(abs(m_vFilterBuf[i]), 255);
			lpos = i;
		}

		m_vFilterBuf[i] = 0;
	}

	if (i - 1 != lpos)
	{
		m_pPaintBoard->SetColor(RGB(0, 0, 0));
		m_pPaintBoard->DrawLine(m_nCurrPos, lpos, m_nCurrPos, h);
	}

	m_vFilterBuf[0] = 0;
	//RECT r = m_oPaintRect;
	//OffsetRect(&r, m_oRect.left, m_oRect.top);
	m_pParent->Invalidate(&m_oRect);

	return TRUE;
}

BOOL CLaSpectrogram::OnMouseKeyDown(void* owner, MouseKeyType nMkt, int x, int y)
{
	switch(nMkt)
	{
		case MouseKeyType::RBUTTON:
		{
			if (m_bOrgSize = !m_bOrgSize)
			{
				m_rPaintScaleNext = 1.0f;
			}
			else
			{
				m_rPaintScaleNext = (m_oPaintRect.bottom - m_oPaintRect.top) / (float)m_nAnalyzeSamples;
			}
			return TRUE;
		}
		case MouseKeyType::LBUTTON:
		{

			return TRUE;
		}
	}

	return FALSE;
}

BOOL CLaSpectrogram::OnMouseKeyUp(void* owner, MouseKeyType nMkt, int x, int y)
{
	return FALSE;
}

void CLaSpectrogram::SetBackgroundColor(int nColor)
{
	m_nBgColor = nColor;
}

