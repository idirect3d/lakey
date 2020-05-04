#include "StdAfx.h"
#include "lawavecapture.h"
#include "sinsound.h"

CLaWaveCapture::CLaWaveCapture(IPaintableParent* pParent, const RECT* pRect, int nSamplePerSec, int nAnalyzeSamples, IFilterDFT<short int, float>* pNextFilter, ICwEventListener* pCwEventListener/* = NULL */)
{
	m_bRulerInvalid = TRUE;
	m_nBorderWidth = 1;
	m_nRularH = 9;
	m_nRularV = 16;
	m_pFont = new CFont("Arial", 8, CFont::THIN);
	m_nWaveState = WAVE_ORIGINAL;
	m_nDctState = DCT_LOG10_FLOAT;
	m_nWaveStateTip = 0;
	m_nDctStateTip = 0;
	m_pParent = pParent;
	m_oRect = *pRect;
	int w = m_oRect.right - m_oRect.left;
	int h = m_oRect.bottom - m_oRect.top;
	SetRect(&m_oPaintRect, m_nBorderWidth, m_nBorderWidth, w - m_nRularV - m_nBorderWidth, h - m_nRularH - m_nBorderWidth);
	SetRect(&m_oRulerRectH, m_nBorderWidth, h - m_nRularH - m_nBorderWidth, w - m_nRularV - m_nBorderWidth, h - m_nBorderWidth);
	SetRect(&m_oRulerRectV, m_oPaintRect.right, m_oPaintRect.top, w - m_nBorderWidth, h - m_nRularH - m_nBorderWidth);
	m_nRulerWidthH = w - m_nRularV - m_nBorderWidth * 2;
	m_nRulerWidthV = h - m_nRularH - m_nBorderWidth * 2;
	m_nRulerGridsV = (100 < m_nRulerWidthV ? 8 : 4);
	m_nRulerDbMax = 80;

	m_nBgColor = 0x000000;
	m_nWaveColor = 0x00f000;
	m_nFreqAnaColor = 0x808080;
	m_nFloatColor = 0xf0f0f0;
	m_nRulerColorH = 0x008000;
	m_nRulerColorV = 0x008080;
	m_nGridColorH = 0x000080;
	m_nGridColorV = 0x004040;
	m_nEnabledColor = 0x00f000;
	m_nDisabledColor = 0x808080;

	m_pPaintBoard = pParent->NewGraphics(pRect->right - pRect->left, pRect->bottom - pRect->top);

	m_pCwEventListener = pCwEventListener;

	m_pDsCap = NULL;
	m_pDsCapBuff = NULL;

	memset(&m_oFormat, 0, sizeof(m_oFormat)); 
	m_oFormat.wFormatTag = WAVE_FORMAT_PCM;
	m_oFormat.nChannels = 1;
	m_oFormat.nSamplesPerSec = nSamplePerSec;
	m_oFormat.wBitsPerSample = 16;
	m_oFormat.nAvgBytesPerSec = m_oFormat.nSamplesPerSec * (m_oFormat.wBitsPerSample / 8) * m_oFormat.nChannels;
	m_oFormat.nBlockAlign = m_oFormat.wBitsPerSample / 8 * m_oFormat.nChannels;
	m_oFormat.cbSize = 0;

	memset(&m_oCaptureBufferDesc, 0, sizeof(m_oCaptureBufferDesc));
	m_oCaptureBufferDesc.dwSize = sizeof(m_oCaptureBufferDesc);
	m_oCaptureBufferDesc.dwFlags = 0;
	m_oCaptureBufferDesc.dwBufferBytes = m_oFormat.nAvgBytesPerSec * 2;	// 2 sec buffer
	m_oCaptureBufferDesc.lpwfxFormat = &m_oFormat;
	m_oCaptureBufferDesc.dwFXCount = 0;
	m_oCaptureBufferDesc.lpDSCFXDesc = NULL;

	m_nAnalyzeSamples = nAnalyzeSamples;
	m_pWavBuff = new short int[m_nAnalyzeSamples * 2];
	m_pDctBuff = new short int[m_nAnalyzeSamples];
	m_pFreqRuler = new short int[m_nRulerWidthH];
	m_pFloatLine = new short int[m_nAnalyzeSamples];
	m_pFloatClear = new short int[m_nAnalyzeSamples];
	memset(m_pFloatLine, 0, sizeof(short int) * m_nAnalyzeSamples);
	memset(m_pFloatClear, 0, sizeof(short int) * m_nAnalyzeSamples);
	//m_nFloatClear = 0;

	//DrawRuler();
	DrawBorder();

	m_nRecvThreshold = (short int)(32768 * 0.5l);
	m_nRecvFreqStart = 850;
	m_nRecvFreqEnd = 1250;

//	m_pFft = new TCosDFT<short int, float>(m_nAnalyzeSamples, this);
	m_pFft = new TFastFT<short int, float>(m_nAnalyzeSamples, this);
	m_pNextFilter = pNextFilter;

	m_bFraze = FALSE;

	InitTestBuffer();
	m_nTestIndex = 0;
}

CLaWaveCapture::~CLaWaveCapture(void)
{
	delete m_pFont;
	delete m_pPaintBoard;

	if (NULL != m_pDsCap)
	{
		if (NULL != m_pDsCapBuff)
		{
			m_pDsCapBuff->Release();
			m_pDsCapBuff = NULL;
		}

		m_pDsCap->Release();
		m_pDsCap = NULL;
	}

	delete[] m_pWavBuff;
	delete[] m_pDctBuff;
	delete[] m_pFreqRuler;
	delete[] m_pFloatLine;
	delete[] m_pFloatClear;

	delete[] m_pTestBuff;
}

void CLaWaveCapture::SetThresholdLevel(double rThresholdLevel)
{
	m_nRecvThreshold = (short int)(32768 * rThresholdLevel);
}

void CLaWaveCapture::SetFreqRange(int nLowFreq, int nHighFreq)
{
	m_nRecvFreqStart = nLowFreq;
	m_nRecvFreqEnd = nHighFreq;
}

void CLaWaveCapture::DrawRuler()
{
	m_bRulerInvalid = FALSE;
	double rTotalLogNum = log10(16000.0l) - 2;
	for (int i = 0; i < m_nRulerWidthH; ++i)
	{
		m_pFreqRuler[i] = (short int)pow(10, 2 + (i * rTotalLogNum / m_nRulerWidthH));
	}

	m_pPaintBoard->SetBgColor(m_nBgColor);
	m_pPaintBoard->DrawRect(&m_oRulerRectH);
	m_pPaintBoard->SetColor(m_nRulerColorH);
	m_pPaintBoard->DrawLine(m_oRulerRectH.left, m_oRulerRectH.top, m_oRulerRectH.right, m_oRulerRectH.top);
	m_pPaintBoard->SetFont(m_pFont);
	double d = m_nRulerWidthH / rTotalLogNum;
	RECT r = m_oRulerRectH;
//	RECT tr;
	char grad[8];
	r.top += 1;
	for (int i = 0; i < rTotalLogNum; ++i)
	{
		r.left = (int)(d * i);

		if (0 == i)
			sprintf(grad, "%.0lfHz", pow(10.0, i + 2));
		else if (3 > i + 2)
			sprintf(grad, "%.0lf", pow(10.0, i + 2));
		else
			sprintf(grad, "%.0lfK", pow(10.0, i + 2) / 1000);

		//m_pPaintBoard->SetColor(RGB(192, 192, 192));
		r.left += 2;
		m_pPaintBoard->DrawText(&r, grad, DT_LEFT);
		r.left -= 2;
		//m_pPaintBoard->SetColor(RGB(224, 224, 224));
		m_pPaintBoard->DrawLine(r.left, 0 == i ? 0 : m_oRulerRectH.top + 1, r.left, m_oRulerRectH.bottom - 1);

		if (4 > i)
		{
			for (int j = 2; j < 6 && r.left < m_nRulerWidthH; ++j)
			{
				r.left = (int)(d * (i + log10((double)j)));

				sprintf(grad, "%d", j);

//				m_pPaintBoard->SetColor(m_nFreqAnaColor);
				r.left += 2;
				m_pPaintBoard->DrawText(&r, grad, DT_LEFT);
				r.left -= 2;
//				m_pPaintBoard->SetColor(m_nFreqAnaColor);
				m_pPaintBoard->DrawLine(r.left, m_oRulerRectH.top + 1, r.left, m_oRulerRectH.bottom - 2);
			}
		}
	}

	// draw rular-V
	m_pPaintBoard->SetBgColor(m_nBgColor);
	m_pPaintBoard->DrawRect(&m_oRulerRectV);
	m_pPaintBoard->DrawLine(m_oRulerRectV.left, m_oRulerRectV.top, m_oRulerRectV.left, m_oRulerRectV.bottom);

	switch(m_nDctState)
	{
	case DCT_LOG10_FLOAT:
	case DCT_LOG10:
		m_pPaintBoard->SetColor(m_nRulerColorV);
		r = m_oRulerRectV;
		r.bottom = r.top + m_nRularV;
		m_pPaintBoard->DrawText(&r, "dB", DT_CENTER);
		//r.top -= 3;

		for (int i = 1; i <= m_nRulerGridsV; ++i)
		{
			r.top = m_oRulerRectV.top + i * m_nRulerWidthV / m_nRulerGridsV - 3;
			r.bottom = r.top + m_nRularV;
			//OffsetRect(&r, 0, m_nRulerWidthV / m_nRulerGridsV);
			sprintf(grad, "-%d", i * m_nRulerDbMax / m_nRulerGridsV);
			m_pPaintBoard->DrawText(&r, grad, DT_CENTER);
		}
		break;
	case DCT_LINEAR_FLOAT:
	case DCT_LINEAR:
		m_pPaintBoard->SetColor(m_nRulerColorV);
		r = m_oRulerRectV;
		r.bottom = m_nRularV;
		m_pPaintBoard->DrawText(&r, "%", DT_CENTER);
		r.top -= 3;

		for (int i = 2; i <= m_nRulerGridsV; i+=2)
		{
			OffsetRect(&r, 0, m_nRulerWidthV * 2 / m_nRulerGridsV);
			sprintf(grad, "%d", 100 - i * 100 / m_nRulerGridsV);
			m_pPaintBoard->DrawText(&r, grad, DT_CENTER);
		}
		break;
	}
}

void CLaWaveCapture::DrawBorder()
{
	int w = m_oRect.right - m_oRect.left - 1;
	int h = m_oRect.bottom - m_oRect.top - 1;
	m_pPaintBoard->SetColor(0x808080);
	m_pPaintBoard->DrawLine(0, 0, 0, h + 1);
	m_pPaintBoard->DrawLine(0, 0, w + 1, 0);
	m_pPaintBoard->SetColor(0xffffff);
	m_pPaintBoard->DrawLine(w, 0, w, h + 1);
	m_pPaintBoard->DrawLine(0, h, w + 1, h);
}

void CLaWaveCapture::DrawGrid()
{
	m_pPaintBoard->SetColor(m_nGridColorV);
	for (int i = 1; i < m_nRulerGridsV; ++i)
	{
		m_pPaintBoard->DrawLineDot(m_oPaintRect.left, m_nBorderWidth + i * m_nRulerWidthV / m_nRulerGridsV, m_oPaintRect.right, m_nBorderWidth + i * m_nRulerWidthV / m_nRulerGridsV);
	}
}
BOOL CLaWaveCapture::Initialize()
{
	m_nNextPeriodStartPos = -1;

	if (FAILED(DirectSoundCaptureCreate8(NULL, &m_pDsCap, NULL)))
		return FALSE;

	if (FAILED(m_pDsCap->CreateCaptureBuffer(&m_oCaptureBufferDesc, &m_pDsCapBuff, NULL)))
		return FALSE;

	return FAILED(m_pDsCapBuff->Start(DSCBSTART_LOOPING));
}

BOOL CLaWaveCapture::IsRelated(int x, int y)
{
	return IControl::PointInRect(&m_oRect, x, y);
}

void CLaWaveCapture::GetRect(RECT* r)
{
	*r = m_oRect;
}

BOOL CLaWaveCapture::OnPaint(void* owner, CGraphics* g, const RECT* pRect)
{
	int w = m_oRect.right - m_oRect.left;
	int h = m_oRect.bottom - m_oRect.top;

	RECT r;
	r.top = 0; r.bottom = h;
	r.left = 0; r.right = w;
	g->Copy(&r, m_pPaintBoard, 0, 0);

	return TRUE;
}

#define TRANSFORM_SAMPLE(f, t)				\
	int x = f;								\
	x <<= (m_nWaveState - WAVE_ORIGINAL);	\
	if (x > 32767)							\
		x = 32767;							\
	else if (x < -32767)					\
		x = -32767;							\
	t = m_oPaintRect.top + (m_oPaintRect.bottom - m_oPaintRect.top) / 2 - x * (m_oPaintRect.bottom - m_oPaintRect.top) / 65536;

int CLaWaveCapture::AnalyzeData()
{
	DWORD nStatus = 0;
	m_pDsCapBuff->GetStatus(&nStatus);
	if (!((DSCBSTATUS_CAPTURING & nStatus) && (DSCBSTATUS_LOOPING & nStatus)))
		return -1;

	DWORD nReadPosEnd;
	DWORD nCapturePos;
	int nFrameBytes = m_nAnalyzeSamples * sizeof(short int);
	if (FAILED(m_pDsCapBuff->GetCurrentPosition(&nCapturePos, &nReadPosEnd)))
		return -1;

	// get read start pos
	//int nReadPosStart = m_nNextPeriodStartPos;
	if (0 > m_nNextPeriodStartPos)
	{
		// always read 1 frame data at first time since buffer be init
		m_nNextPeriodStartPos = nReadPosEnd - nFrameBytes * 2;
		if (0 > m_nNextPeriodStartPos)
			m_nNextPeriodStartPos += m_oCaptureBufferDesc.dwBufferBytes;
	}

	// calc bytes should be read
	int nReadBytes = nReadPosEnd - m_nNextPeriodStartPos;
	if (nReadBytes < 0)
		nReadBytes += m_oCaptureBufferDesc.dwBufferBytes;

	int nReadTimes = nReadBytes / (nFrameBytes * 2);
	if (0 == nReadTimes)
		return -1;

	// correct read bytes
	nReadBytes = nReadTimes * nFrameBytes * 2;

	short int *pHeadPortion, *pTailPortion;
	DWORD nHpBytes, nTpBytes;
	if (FAILED(m_pDsCapBuff->Lock(m_nNextPeriodStartPos, nReadBytes, (LPVOID *)&pHeadPortion, &nHpBytes, (LPVOID *)&pTailPortion, &nTpBytes, 0)))
		return -1;

	DWORD i, nBase = 0;
	BOOL bTestFlg = FALSE; //(0 == (++m_nTestInt / 10) % 2);
	int s = 0;
	for (int nFrame = 0; nFrame < nReadTimes; ++nFrame)
	{
		for (i = 0; i < nFrameBytes && (nBase + i) < nHpBytes / sizeof(short int); ++i)
		{
			m_pWavBuff[i] = pHeadPortion[nBase + i];
			s += abs(m_pWavBuff[i]);

			if (bTestFlg)
			{
				if (++m_nTestIndex == m_nTestBuffLen)
					m_nTestIndex = 0;

				int t = (int)m_pWavBuff[i] + (int)m_pTestBuff[m_nTestIndex];
				m_pWavBuff[i] = min(32767, t);
				m_pWavBuff[i] = max(-32767, t);
			}
		}

		// nFrameBytes same as (nFrameBytes * 2 / sizeof(short int))
		for (; i < nFrameBytes; ++i)
		{
			m_pWavBuff[i] = pTailPortion[nBase + i];
			s += abs(m_pWavBuff[i]);

			if (bTestFlg)
			{
				if (++m_nTestIndex == m_nTestBuffLen)
					m_nTestIndex = 0;

				int t = (int)m_pWavBuff[i] + (int)m_pTestBuff[m_nTestIndex];
				m_pWavBuff[i] = min(32767, t);
				m_pWavBuff[i] = max(-32767, t);
			}
		}

		static int dddd = 100;
		_ASSERT(dddd != 0 || 0 != s);
		if (0 < dddd)
			--dddd;

		m_nCwState = FALSE;
		m_pFft->Encode(m_pDctBuff, m_pWavBuff);
		//m_pFft->Decode(m_pWavBuff, m_pDctBuff);
		if (NULL != m_pCwEventListener)
			m_pCwEventListener->OnCwEvent(m_nCwState);

		m_nCwState = FALSE;
		m_pFft->Encode(m_pDctBuff, m_pWavBuff + m_nAnalyzeSamples);
		//m_pFft->Decode(m_pWavBuff + m_nAnalyzeSamples, m_pDctBuff);
		if (NULL != m_pCwEventListener)
			m_pCwEventListener->OnCwEvent(m_nCwState);

		nBase += nFrameBytes;
	}

	m_pDsCapBuff->Unlock(pHeadPortion, nHpBytes, pTailPortion, nTpBytes);

	if ((m_nNextPeriodStartPos += nReadBytes) >= m_oCaptureBufferDesc.dwBufferBytes)
		m_nNextPeriodStartPos -= m_oCaptureBufferDesc.dwBufferBytes;

	return GetSyncPos(m_pWavBuff, m_nAnalyzeSamples);
}

short int CLaWaveCapture::OnEncodeFilter(float val, int i)
{
	if (!m_nCwState && m_nRecvThreshold <= abs(val))
	{
		int nFreq = i * m_oFormat.nSamplesPerSec / 2 / m_nAnalyzeSamples;
		if (m_nRecvFreqStart <= nFreq && nFreq <= m_nRecvFreqEnd)
			m_nCwState = TRUE;
	}

	if (m_pNextFilter)
		return m_pNextFilter->OnEncodeFilter(val, i);

	return val;
}

short int CLaWaveCapture::OnDecodeFilter(float val, int i)
{
	return val;
}

/***
 * 刷新控件，包括：重画标尺，读取音频数据，分析并输出
 *
 */
BOOL CLaWaveCapture::Refresh()
{
	if (m_bRulerInvalid)
		DrawRuler();

	int nSyncStart = AnalyzeData();

	if (0 > nSyncStart)
		return FALSE;

	if (m_bFraze)
		return TRUE;
	// redraw data if need

	m_pPaintBoard->SetColor(m_nBgColor);
	//m_oPaintRect.left += 1;
	m_pPaintBoard->DrawRect(&m_oPaintRect);
	//m_oPaintRect.left -= 1;

	DrawGrid();

	m_pPaintBoard->SetColor(m_nWaveColor);
	short int* pSyncWavBuff = m_pWavBuff + nSyncStart;

	int last, curr;
	TRANSFORM_SAMPLE(pSyncWavBuff[0], last);

	int i, x1, x2 = m_nBorderWidth;
	for (i = 1; m_nWaveState && i < m_nAnalyzeSamples; ++i)
	{
		int v = pSyncWavBuff[i];
		TRANSFORM_SAMPLE(v, curr);
		x1 = x2;
		x2 = m_nBorderWidth + m_nRulerWidthH * i / m_nAnalyzeSamples;
		m_pPaintBoard->DrawLine(x1, last, x2, curr);
		last = curr;
	}

	m_pPaintBoard->SetColor(m_nFreqAnaColor);

	//FFT
	//m_pFft->Compute(m_pDctBuff, pSyncWavBuff);

	//m_pPaintBoard->SetColor(RGB(0, 0, 0));
	int fd, v, l = 1;
	float db;
//	BOOL bState = FALSE;
	for (i = 0; i < m_nAnalyzeSamples; i += 2)
	{
		short int nFreq = (short int)(i * m_oFormat.nSamplesPerSec / 2 / m_nAnalyzeSamples);
		for (; m_nDctState && l < m_nRulerWidthH; ++l)
		{
			if (m_pFreqRuler[l] >= nFreq)
			{
				v = 0;
				switch (m_nDctState)
				{
					case DCT_LINEAR:
					case DCT_LINEAR_FLOAT:
						v = abs(m_pDctBuff[i]) * m_oPaintRect.bottom / 32768;
						break;
					case DCT_LOG10:
					case DCT_LOG10_FLOAT:
						db = log10f((float)abs(m_pDctBuff[i]));
						if (db > LOG_SAMPLE_RANGE - 4)
							v = (int)((log10f((float)(abs(m_pDctBuff[i]))) - (LOG_SAMPLE_RANGE - 4)) * m_oPaintRect.bottom / 4);
						break;
				}

				if (v > m_oPaintRect.bottom)
					v = m_oPaintRect.bottom;
				m_pPaintBoard->DrawLine(l, m_oPaintRect.bottom - v, l, m_oPaintRect.bottom);

				switch (m_nDctState)
				{
				case DCT_LINEAR_FLOAT:
				case DCT_LOG10_FLOAT:
					fd = (int)(0.005f * m_pFloatClear[i] * m_pFloatClear[i] * m_pFloatClear[i]);

					if (m_pFloatLine[i] - fd < v)
					{
						m_pFloatLine[i] = v;
						m_pFloatClear[i] = 0;
					}
					else if (1 < m_pFloatLine[i] - fd)
					{
						m_pPaintBoard->SetColor(m_nFloatColor);
						fd = m_oPaintRect.bottom - (m_pFloatLine[i] - fd);
						m_pPaintBoard->DrawLine(l, fd, l, fd + 2);
						++m_pFloatClear[i];
						m_pPaintBoard->SetColor(m_nFreqAnaColor);
					}
				}

				break;
			}
		}

//		if (!bState && m_nRecvThreshold <= abs(m_pDctBuff[i]))
//			if (m_nRecvFreqStart <= nFreq && nFreq <= m_nRecvFreqEnd)
//				bState = TRUE;
	} // end of for (i = 0; i < m_nAnalyzeSamples; ++i)

	RECT r = m_oPaintRect;
	r.left += 2;
	r.top += 2;
	r.right = r.left + 32;
	r.bottom = r.top + 12;
	
	switch(m_nWaveState)
	{
	case WAVE_INVISIBLE:
		m_pPaintBoard->SetColor(m_nDisabledColor);
		m_pPaintBoard->DrawText(&r, "W", DT_LEFT); break;
	case WAVE_ORIGINAL:
		m_pPaintBoard->SetColor(m_nEnabledColor);
		m_pPaintBoard->DrawText(&r, "W", DT_LEFT); break;
	default:
		char szBuff[16];
		m_pPaintBoard->SetColor(m_nEnabledColor);
		sprintf(szBuff, "W +%ddB", 6 * (m_nWaveState - WAVE_ORIGINAL));
		m_pPaintBoard->DrawText(&r, szBuff, DT_LEFT); break;
	}

	OffsetRect(&r, 100, 0);

	switch(m_nDctState)
	{
	case DCT_INVISIBLE:
	case DCT_LOG10:
	case DCT_LINEAR:
		m_pPaintBoard->SetColor(m_nDisabledColor);
		m_pPaintBoard->DrawText(&r, "~~~", DT_LEFT); break;
	case DCT_LOG10_FLOAT:
	case DCT_LINEAR_FLOAT:
		m_pPaintBoard->SetColor(m_nEnabledColor);
		m_pPaintBoard->DrawText(&r, "~~~", DT_LEFT); break;
	}

	OffsetRect(&r, 32, 0);

	switch(m_nDctState)
	{
	case DCT_INVISIBLE:
		m_pPaintBoard->SetColor(m_nDisabledColor);
		m_pPaintBoard->DrawText(&r, "LOG", DT_LEFT);
		OffsetRect(&r, 32, 0);
		m_pPaintBoard->DrawText(&r, "LINEAR", DT_LEFT); break;
	case DCT_LOG10_FLOAT:
	case DCT_LOG10:
		m_pPaintBoard->SetColor(m_nEnabledColor);
		m_pPaintBoard->DrawText(&r, "LOG", DT_LEFT);
		OffsetRect(&r, 32, 0);
		m_pPaintBoard->SetColor(m_nDisabledColor);
		m_pPaintBoard->DrawText(&r, "LINEAR", DT_LEFT); break;
	case DCT_LINEAR_FLOAT:
	case DCT_LINEAR:
		m_pPaintBoard->SetColor(m_nDisabledColor);
		m_pPaintBoard->DrawText(&r, "LOG", DT_LEFT);
		OffsetRect(&r, 32, 0);
		m_pPaintBoard->SetColor(m_nEnabledColor);
		m_pPaintBoard->DrawText(&r, "LINEAR", DT_LEFT); break;
	}

	//r = m_oPaintRect;
	//OffsetRect(&r, m_oRect.left, m_oRect.top);
	//m_pParent->Invalidate(&r);
	m_pParent->Invalidate(&m_oRect);

//	if (NULL != m_pCwEventListener)
//		m_pCwEventListener->OnCwEvent(bState);

	return TRUE;
}

DWORD CLaWaveCapture::GetSyncPos(short int* pBuff, DWORD nBuffSize)
{
	//synchronize the wave
	int nStep = 1;
	DWORD nSyncStart;
	for (nSyncStart = 0; nSyncStart < nBuffSize - 1 && 0 < nStep; ++nSyncStart)
	{
		switch(nStep)
		{
			case 1:
				if (pBuff[nSyncStart] < pBuff[nSyncStart + 1])
					nStep = 2;
				break;
			case 2:
				if (pBuff[nSyncStart] >= 0 && pBuff[nSyncStart] > pBuff[nSyncStart + 1])
					nStep = 0;
			/* for sin wave
			case 1: // look for negative
				if (m_vWavBuff[nSyncStart] < 0)
					nStep = 2;
				break;
			case 2: // through the X axis
				if (m_vWavBuff[nSyncStart] >= 0)
					nStep = 0;
				break;
			*/
		}
	}

	return nSyncStart;
}

BOOL CLaWaveCapture::OnMouseKeyDown(void* owner, MouseKeyType nMkt, int x, int y)
{
	return FALSE;
}

BOOL CLaWaveCapture::OnMouseKeyUp(void* owner, MouseKeyType nMkt, int x, int y)
{
	switch(nMkt)
	{
		case LBUTTON:
		{
			if (WAVE_GAIN_X32 < ++m_nWaveState)
				m_nWaveState = 0;

			m_nWaveStateTip = 50;
			return TRUE;
		}
		case RBUTTON:
		{
			if (DCT_LINEAR < ++m_nDctState)
				m_nDctState = 0;

			m_nDctStateTip = 50;
			m_bRulerInvalid = TRUE;
			return TRUE;
		}
	}

	return FALSE;
}

void CLaWaveCapture::SetBackgroundColor(int nColor)
{
	m_nBgColor = nColor;
}

void CLaWaveCapture::SetWaveColor(int nColor)
{
	m_nWaveColor = nColor;
}

void CLaWaveCapture::SetFloatColor(int nColor)
{
	m_nFloatColor = nColor;
}

void CLaWaveCapture::SetFreqAnaColor(int nColor)
{
	m_nFreqAnaColor = nColor;
}

void CLaWaveCapture::InitTestBuffer()
{
	REAL f = 1000.0f;
	REAL w = 1.0f / f;
	REAL d = 1.0f / 44100.0f;
	int nTimes;
	for (nTimes = 100; nTimes <= 1000; ++nTimes)
	{
		REAL rRemScale = fmod(w * nTimes, d) / w;

		if (rRemScale < 0.001l || rRemScale > 0.998l)
			break;
	}

	REAL s = nTimes * w;
	m_nTestBuffLen = s * 44100;
	REAL DW = 2 * PI / m_oFormat.nSamplesPerSec;
	m_pTestBuff = new short int[m_nTestBuffLen];

	for (DWORD i = 0; i < m_nTestBuffLen; ++i)
	{
		REAL v = sin(DW * i * f * 3) * 0.3 + sin(DW * i * f * 2) * 0.4 + sin(DW * i * f) * 0.5;	// -1.0 ~ +1.0
		m_pTestBuff[i] = (short int)(v * 32767);
	}
}