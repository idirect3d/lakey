#pragma once

#include "EventManagerWin32.h"
#include "FFT.h"
#include "dsound.h"

//log(32768)
//#define LOG_SAMPLE_RANGE	4.51545	
#define LOG_SAMPLE_RANGE	4.51545f

class ICwEventListener;

class CLaSpectrogram
	: public IPaintEventControl
	, public IMouseKeyEventControl
	, public IFilterDFT<short int, float>
{
public:
	CLaSpectrogram(IPaintableParent* pParent, const RECT* pRect, int nSamplePerSec, int nAnalyzeSample);
	~CLaSpectrogram(void);

	virtual BOOL IsRelated(int x, int y);
	virtual void GetRect(RECT* r);
	virtual BOOL OnPaint(void* owner, CGraphics* g, const RECT* pRect);

	virtual BOOL Initialize();
	virtual BOOL Refresh();

	virtual BOOL OnMouseKeyDown(void* owner, MouseKeyType nMkt, int x, int y);
	virtual BOOL OnMouseKeyUp(void* owner, MouseKeyType nMkt, int x, int y);
	virtual BOOL OnClick(void* owner) { return FALSE; };

	virtual short int OnEncodeFilter(float val, int i);
	virtual short int OnDecodeFilter(float val, int i);

	void SetBackgroundColor(int nColor);
	void SetFrontColor(int nColor);

	void SetBrightness(float scale);

private:
	int AnalyzeData();
	DWORD GetSyncPos(short int* pBuff, DWORD nBuffSize);

	void DrawRuler();
	void DrawBorder();
	void DrawGrid();

	IPaintableParent*		m_pParent;
	CFont*		m_pFont;
	RECT		m_oRect;	// by parent
	RECT		m_oPaintRect;	// by self
	CGraphics*	m_pPaintBoard;
	int			m_nAnalyzeSamples;

	int			m_nBorderWidth;

	int			m_nBgColor;
	int			m_nFrColor;

	BOOL		m_bFraze;

	float*		m_vFilterBuf;
	float		m_rPaintIdx;
	int			m_nCurrPos;
	float		m_rPaintScale;	// m_nAnalyzeSamples / canvos height
	float		m_rPaintScaleNext;
	int			m_nPhases;
	int			m_nPhasesDiv;
	float		m_rBrightnessScale;
	BOOL		m_bOrgSize;
};
