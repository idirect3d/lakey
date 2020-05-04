#pragma once

#include "EventManagerWin32.h"
#include "FFT.h"
#include "dsound.h"

//log(32768)
//#define LOG_SAMPLE_RANGE	4.51545	
#define LOG_SAMPLE_RANGE	4.51545f

// CLaWaveCapture.m_nWaveState values
#define WAVE_INVISIBLE		0
#define WAVE_ORIGINAL		1
#define WAVE_GAIN_X2		2
#define WAVE_GAIN_X4		3
#define WAVE_GAIN_X8		4
#define WAVE_GAIN_X16		5
#define WAVE_GAIN_X32		6

// CLaWaveCapture.m_nDctState values
#define DCT_INVISIBLE		0
#define DCT_LOG10_FLOAT		1
#define DCT_LOG10			2
#define DCT_LINEAR_FLOAT	3
#define DCT_LINEAR			4

class ICwEventListener
{
public:
	virtual void OnCwEvent(BOOL bCwDown) = 0;
};

class CLaWaveCapture
	: public IPaintEventControl
	, public IMouseKeyEventControl
	, public IFilterDFT<short int, float>
{
public:
	CLaWaveCapture(IPaintableParent* pParent, const RECT* pRect, int nSamplePerSec, int nAnalyzeSample, IFilterDFT<short int, float>* pNextFilter, ICwEventListener* pCwEventListener = NULL);
	~CLaWaveCapture(void);

	virtual BOOL IsRelated(int x, int y);
	virtual void GetRect(RECT* r);
	virtual BOOL OnPaint(void* owner, CGraphics* g, const RECT* pRect);

	virtual BOOL Initialize();
	virtual BOOL Refresh();

	virtual void SetThresholdLevel(double rThresholdLevel);
	virtual void SetFreqRange(int nLowFreq, int nHighFreq);

	virtual BOOL OnMouseKeyDown(void* owner, MouseKeyType nMkt, int x, int y);
	virtual BOOL OnMouseKeyUp(void* owner, MouseKeyType nMkt, int x, int y);
	virtual BOOL OnClick(void* owner) { return FALSE; };

	virtual short int OnEncodeFilter(float val, int i);
	virtual short int OnDecodeFilter(float val, int i);

	void SetBackgroundColor(int nColor);
	void SetWaveColor(int nColor);
	void SetFloatColor(int nColor);
	void SetFreqAnaColor(int nColor);

	BOOL GetFrazeState() { return m_bFraze; };
	void SetFrazeState(BOOL bFraze) { m_bFraze = bFraze; }

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
	RECT		m_oRulerRectH;	// by self
	RECT		m_oRulerRectV;	// by self
	int		m_nRulerWidthH;
	int		m_nRulerWidthV;
	int		m_nRulerGridsV;
	int		m_nRulerDbMax;
	CGraphics*	m_pPaintBoard;

	IDirectSoundCapture*		m_pDsCap;
	IDirectSoundCaptureBuffer*	m_pDsCapBuff;

	WAVEFORMATEX		m_oFormat;
	DSCBUFFERDESC		m_oCaptureBufferDesc;
	int		m_nNextPeriodStartPos;

	short int*	m_pWavBuff; //[m_nAnalyzeSamples * 2];
	short int*	m_pDctBuff; //[m_nAnalyzeSamples];
	short int*	m_pFreqRuler; //[m_oRulerWidth];
	short int*	m_pFloatLine; //[m_nAnalyzeSamples];
	short int*  m_pFloatClear;

//	TCosDFT<short int, float>*		m_pFft;
	TFastFT<short int, float>*		m_pFft;
	IFilterDFT<short int, float>*	m_pNextFilter;

	ICwEventListener* m_pCwEventListener;
	DWORD		m_nAnalyzeSamples;
	int			m_nRecvThreshold;
	int			m_nRecvFreqStart;
	int			m_nRecvFreqEnd;

	BOOL		m_nCwState;

	short int	m_nWaveState;
	short int	m_nDctState;
	int			m_nWaveStateTip;
	int			m_nDctStateTip;

	int			m_nBorderWidth;
	int			m_nRularH;
	int			m_nRularV;

	int			m_nBgColor;
	int			m_nWaveColor;
	int			m_nFloatColor;
	int			m_nFreqAnaColor;
	int			m_nRulerColorH;
	int			m_nRulerColorV;
	int			m_nGridColorH;
	int			m_nGridColorV;
	int			m_nEnabledColor;
	int			m_nDisabledColor;

	BOOL		m_bRulerInvalid;
	BOOL		m_bFraze;


	/* for test only */
	int			m_nTestBuffLen;
	int			m_nTestIndex;
	short int*	m_pTestBuff;
	void		InitTestBuffer();
	int			m_nTestInt;
};
