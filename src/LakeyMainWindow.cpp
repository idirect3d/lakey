#include "StdAfx.h"
#include "lakeymainwindow.h"

#include "Mmreg.h"
#include "dsound.h"
#include "math.h"
#include "stdio.h"

#include "LakeySetting.h"
#include "SinSound.h"
#include "EventManagerWin32.h"
#include "LaButton.h"
#include "LaTuner.h"
#include "LaJournalPanel.h"
#include "LaLabel.h"
#include "LaLine.h"
#include "LaWaveCapture.h"
#include "LaHwControl.h"
#include "LaNetwork.h"
#include "LaSpectrogram.h"

#include "Resource.h"

#include <assert.h>

#define SAFE_DELETE(x)		if (x) { delete x; x = NULL; }
#define SAFE_DELETE_DC(h)	if (h) { DeleteDC(h); h = NULL; }
#define SEND_SHORT_COUNT_EXPR	(m_pSetting->cust.m_nLongHit * 2 / (m_pSetting->cust.m_nSendJournalPeriod * 3))
#define RECV_SHORT_COUNT_EXPR	(m_pSetting->cust.m_nShortHit * 1.5 * 44.100 / m_pSetting->cust.m_nRecvAnalyzeSamples)

#define JOIN_SAFELY(ht)								\
{													\
	if (NULL != ht)									\
	{												\
		DWORD nExitCode_j = -1;						\
		if (GetExitCodeThread(ht, &nExitCode_j))	\
			if (STILL_ACTIVE == nExitCode_j)		\
				WaitForSingleObject(ht, INFINITE);	\
	}												\
}

CLakeyMainWindow::CLakeyMainWindow(HWND hWnd, CLakeySetting* pSetting)
: CEventDispatcherWin32(hWnd)
{
	m_pSetting = pSetting;

	m_pFont = new CFont("Arial", 12, CFont::THIN);
	m_pLogoFont = new CFont("Courier New", 20, CFont::THIN);
	m_pLogoFont->SetItalic(TRUE);
	//m_pLogoFont->SetUnderline(TRUE);

	m_oBtStyle.nTextFormat = DT_END_ELLIPSIS|DT_LEFT;
	m_oBtStyle.pFont = m_pFont;
	
	m_oLabelStyle.nTextFormat = DT_END_ELLIPSIS|DT_LEFT;
	m_oLabelStyle.pFont = m_pFont;
	m_oLabelStyle.nBgColor = RGB(255, 255, 255);
	m_oLabelStyle.nColor = RGB(0, 0, 0);

	m_oScqlStyle.nTextFormat = DT_END_ELLIPSIS|DT_LEFT;
	m_oScqlStyle.pFont = m_pFont;
	m_oScqlStyle.nBgColor = RGB(255, 255, 255);
	m_oScqlStyle.nColor = RGB(0, 0, 0);

	m_oLogoStyle.nTextFormat = DT_END_ELLIPSIS|DT_RIGHT|DT_BOTTOM|DT_SINGLELINE;
	m_oLogoStyle.pFont = m_pLogoFont;
	m_oLogoStyle.nBgColor = RGB(255, 255, 255);
	m_oLogoStyle.nColor = RGB(32, 32, 32);

	m_oVersionStyle.nTextFormat = DT_END_ELLIPSIS|DT_RIGHT|DT_BOTTOM|DT_SINGLELINE;
	m_oVersionStyle.pFont = m_pFont;
	m_oVersionStyle.nBgColor = RGB(255, 255, 255);
	m_oVersionStyle.nColor = RGB(100, 100, 100);

	AddPaintEventControl(this);

	m_pBtCw = CreateButton(&m_pSetting->cust.m_oCwKeyButton);
	m_pBtSendPause = CreateButton(&m_pSetting->cust.m_oSendPauseButton, CLaButton::BT_LOCKABLE);
	m_pBtSendFile = CreateButton(&m_pSetting->cust.m_oSendFileButton);
	m_pBtAutoKeySwitch = CreateButton(&m_pSetting->cust.m_oAutoKeySwitchButton);
	m_pBtNoiseSwitch = CreateButton(&m_pSetting->cust.m_oNoiseSwitchButton, CLaButton::BT_LOCKABLE);

	m_pJpSend = CreateJournalPanel(&m_pSetting->cust.m_oSendJournalRect, SEND_SHORT_COUNT_EXPR, &m_oLabelStyle, this);
	m_pJpRecv = CreateJournalPanel(&m_pSetting->cust.m_oRecvJournalRect, (int)RECV_SHORT_COUNT_EXPR, &m_oLabelStyle, NULL);
	m_pRecvSpectrogram = CreateSpectrogram(&m_pSetting->cust.m_oSpectrogramRect, NULL);
	m_pRecvSpectrogram->SetBrightness(m_pSetting->cust.m_oSpecBrightnessTuner.v);
	m_pRecvSpectrogram->Initialize();
	m_pRecvDctMonitor = CreateWaveCapture(&m_pSetting->cust.m_oRecvMonitorRect, m_pRecvSpectrogram);

	m_pSendCharQueueLabel = CreateLabel(&m_pSetting->cust.m_oSendCharQueueRect, NULL, &m_oScqlStyle, "HHH");
	RECT r = m_pSetting->cust.m_oSendJournalRect;
	OffsetRect(&r, m_pSetting->cust.m_oSendLabelOffset.cx, m_pSetting->cust.m_oSendLabelOffset.cy);
	r.bottom = r.top + 12;
	r.right = r.left + 40;
	m_pSendLabel = CreateLabel(&r, m_pSetting->cust.m_vSendLabel, &m_oLabelStyle);
	r = m_pSetting->cust.m_oRecvJournalRect;
	OffsetRect(&r, m_pSetting->cust.m_oRecvLabelOffset.cx, m_pSetting->cust.m_oRecvLabelOffset.cy);
	r.bottom = r.top + 12;
	r.right = r.left + 40;
	m_pRecvLabel = CreateLabel(&r, m_pSetting->cust.m_vRecvLabel, &m_oLabelStyle);

	m_pSendJpBorder = CreateLine(m_pSetting->cust.m_oSendCharQueueRect.left, m_pSetting->cust.m_oSendCharQueueRect.bottom + 1,
			m_pSetting->cust.m_oSendCharQueueRect.right, m_pSetting->cust.m_oSendCharQueueRect.bottom + 1);
	m_pRecvJpBorder = CreateLine(m_pSetting->cust.m_oRecvJournalRect.left, m_pSetting->cust.m_oRecvJournalRect.bottom + 1,
			m_pSetting->cust.m_oRecvJournalRect.right, m_pSetting->cust.m_oRecvJournalRect.bottom + 1);

	m_hMorseQueueThread = NULL;
	m_nMorseQueueThreadID = 0;
	m_hMorseJournalThread = NULL;
	m_nMorseJournalThreadID = 0;
	m_hRecvMonitorThread = NULL;
	m_nRecvMonitorThreadID = 0;

	BuildMorseButtons();

	m_pTnSendVol = CreateTuner(&m_pSetting->cust.m_oSendVolTuner);
	m_pTnBgVol = CreateTuner(&m_pSetting->cust.m_oNoiseVolTuner);
	m_pTnSendSpeed = CreateTuner(&m_pSetting->cust.m_oSendSpeedTuner);
	m_pTnSpecBrightness = CreateTuner(&m_pSetting->cust.m_oSpecBrightnessTuner);

	GetRect(&r);
	r.right -= r.left;
	r.bottom -= r.top + 20;
	r.left = r.right - 80;
	r.top = r.bottom - 20;
	// m_pLogo = CreateLabel(&r, "Lakey ", &m_oLogoStyle);

	r.top += 20;
	r.bottom += 16;
	r.right -= 10;
	// m_pVersion = CreateLabel(&r, "b0019", &m_oVersionStyle);

	m_pSound = new CSinSound();
	m_pSound->SetFrequency((float)m_pSetting->cust.m_rBeepFreq);
	m_pSound->SetScale((float)m_pSetting->cust.m_rBeepVol);

	m_bNeedExit = FALSE;
	m_bCwFlag = FALSE;
	m_nRecvIdleCount = 0;
	m_bPause = FALSE;
	m_bClear = FALSE;
	m_bNwSndState = FALSE;
	m_nAutoKey = 0;
	m_nEncryptTrngTotalChars = 0;
	m_hSendJrnFile = NULL;

	m_pExtPort = new CLaHwControl(m_pSetting->cust.m_nExtPortAddr, m_pSetting->cust.m_nCloseByte);

	m_pNetwork = new CLaNetwork(0);
	m_pNetwork->BindEventListener(this);
	m_pNetwork->BindFrameListener(this);

	AddCommandEventControl(this);
	AddKeyboardEventControl(this);
}

CLakeyMainWindow::~CLakeyMainWindow(void)
{
	// save current win rect
	GetWindowRect(GetHWnd(), &m_pSetting->cust.m_oWindowRect);

	m_bNeedExit = TRUE;
	ResumeThread(m_hMorseQueueThread);
	JOIN_SAFELY(m_hMorseQueueThread);
	JOIN_SAFELY(m_hMorseJournalThread);
	JOIN_SAFELY(m_hRecvMonitorThread);

	m_pSetting->cust.m_oSendVolTuner.v = this->m_pTnSendVol->GetScale();
	m_pSetting->cust.m_oNoiseVolTuner.v = this->m_pTnBgVol->GetScale();
	m_pSetting->cust.m_oSendSpeedTuner.v = this->m_pTnSendSpeed->GetScale();
	m_pSetting->cust.m_oSpecBrightnessTuner.v = this->m_pTnSpecBrightness->GetScale();

	// delete m_pVersion;
	// delete m_pLogo;
	delete m_pTnSpecBrightness;
	delete m_pTnSendSpeed;
	delete m_pTnBgVol;
	delete m_pTnSendVol;
	delete m_pNetwork;
	delete m_pExtPort;
	delete m_pRecvDctMonitor;
	delete m_pRecvJpBorder;
	delete m_pJpRecv;
	delete m_pSendJpBorder;
	delete m_pSendCharQueueLabel;
	delete m_pJpSend;
	delete m_pBtCw;
	delete m_pBtSendPause;
	delete m_pBtSendFile;
	delete m_pBtAutoKeySwitch;
	delete m_pBtNoiseSwitch;
	delete m_pSendLabel;
	delete m_pRecvLabel;
	delete m_pSound;
	delete m_pLogoFont;
	delete m_pFont;
	delete m_pRecvSpectrogram;

	for (int i = 0; i < MORSECODECOUNT; ++i)
		delete m_vBtMorse[i];
}

void CLakeyMainWindow::BuildMorseButtons()
{
	RECT r;
	GetWindowRect(GetHWnd(), &r);

	int x = 10;
	int oy = 300;
	int y = oy;
	int h = 12;
	int w = 58;
	int nMaxY = r.bottom - r.top - 6 * h;
	char txt[] = { ' ', ' ', ' ', ' ', ' ', '#', '#', '#', '#', '#', '#', '\0' };

	for (int i = 0; i < MORSECODECOUNT; ++i)
	{
		MORSECODE* p = &(m_pSetting->cust.m_vMorseCode[i]);
		txt[0] = p->nAscCode;
		int nWin = 0x0020;
		for (int j = 0; j < 6; ++j, nWin >>= 1)
		{
			if (nWin & p->nMask)
				txt[j + 5] = (nWin & p->nMorseCode ? '.' : '-');
			else
				txt[j + 5] = ' ';
		}

		m_vBtMorse[i] = CreateButton(txt, x, y, w, h, p->nKeyCode);
		m_vBtMorse[i]->SetUserData(p);

		if ((y += (h + 2)) > nMaxY)
		{
			y = oy;
			x += (6 * w / 5);
		}
	}
}

BOOL CLakeyMainWindow::OnTune(void* owner, LASCALE nScale)
{
	if (m_pTnSendVol == owner)
		m_pSound->SetVolume((LONG)(nScale * 100));
	else if (m_pTnBgVol == owner)
		m_pSound->SetNoiseVolume((LONG)(nScale * 100));
	else if (m_pTnSendSpeed == owner)
	{
		WPMS wpms;
		wpms.wpm = nScale;
		wpmToParams(&wpms);
		m_pSetting->cust.m_nShortHit = wpms.di;
		m_pSetting->cust.m_nLongHit = wpms.dah;
		m_pSetting->cust.m_nHitDelay = wpms.hdelay;
		m_pSetting->cust.m_nLetterDelay = wpms.ldelay;
		m_pSetting->cust.m_nWordDelay = wpms.wdelay;
		
		m_pJpSend->SetMaxShortCount(SEND_SHORT_COUNT_EXPR);
		m_pJpRecv->SetMaxShortCount((int)RECV_SHORT_COUNT_EXPR);
	}
	else if (m_pTnSpecBrightness == owner)
	{
		m_pRecvSpectrogram->SetBrightness(nScale);
	}
	else
		return FALSE;

	return TRUE;
}

MORSECODE* CLakeyMainWindow::GetMorseCode(char ch)
{
	for (int i = 0; i < MORSECODECOUNT; ++i)
	{
		MORSECODE* p = &(m_pSetting->cust.m_vMorseCode[i]);
		if (ch == p->nAscCode || ch + ('A' - 'a') == p->nAscCode)
			return p;
	}

	// '_' is default
	return &(m_pSetting->cust.m_vMorseCode[MORSECODECOUNT - 1]);
}

CLaTuner* CLakeyMainWindow::CreateTuner(const TUNERMAPPING* pMapping)
{
	return CreateTuner(pMapping->vLabel, pMapping->x, pMapping->y, pMapping->w, pMapping->h, pMapping->vl, pMapping->vr, pMapping->v);
}

CLaTuner* CLakeyMainWindow::CreateTuner(const char* pText, int x, int y, int w, int h, LASCALE nScaleL, LASCALE nScaleR, LASCALE nScale)
{
	RECT r;
	r.left = x; r.top = y; r.right = x + w; r.bottom = y + h;

	CLaTuner* pTuner = new CLaTuner(this, pText, &r, nScaleL, nScaleR, nScale, m_pFont);

	AddPaintEventControl(pTuner);
	AddMouseMoveEventControl(pTuner);
	AddMouseKeyEventControl(pTuner);
//	AddKeyboardEventControl(pButton);
	pTuner->AddMouseKeyEventListener(this);
//	pTuner->AddKeyboardEventListener(this);
	pTuner->AddTunerEventListener(this);

	return pTuner;
}

CLaButton* CLakeyMainWindow::CreateButton(const char* pText, int x, int y, int w, int h, int nWantKeyCode, CLaButton::ButtonStyle nStyle)
{
	RECT r;
	r.left = x; r.top = y; r.right = x + w; r.bottom = y + h;

	CLaButton* pButton = new CLaButton(this, pText, &r, nStyle, m_pFont);
	pButton->SetWantKeyCode(nWantKeyCode);

	AddPaintEventControl(pButton);
	AddMouseMoveEventControl(pButton);
	AddMouseKeyEventControl(pButton);
	AddKeyboardEventControl(pButton);
	pButton->AddMouseKeyEventListener(this);
	pButton->AddKeyboardEventListener(this);

	return pButton;
}

CLaButton* CLakeyMainWindow::CreateButton(const BUTTONMAPPING* pMapping, CLaButton::ButtonStyle nStyle)
{
	return CreateButton(pMapping->vLabel, pMapping->x, pMapping->y, pMapping->w, pMapping->h, pMapping->nKeyCode, nStyle);
}

CLaJournalPanel* CLakeyMainWindow::CreateJournalPanel(const RECT* pRect, int nMaxShortCount
	, STYLE* pStyle, IParseEventListener* pParseListener)
{
	CLaJournalPanel* jp = new CLaJournalPanel(this, pRect, m_pSetting->cust.m_vMorseCode, nMaxShortCount, pStyle, pParseListener);

	AddPaintEventControl(jp);

	return jp;
}

CLaWaveCapture* CLakeyMainWindow::CreateWaveCapture(const RECT* pRect, IFilterDFT<short int, float>* pNextFilter)
{
	CLaWaveCapture* wc = new CLaWaveCapture(this, pRect, 44100, m_pSetting->cust.m_nRecvAnalyzeSamples, pNextFilter, this);
	wc->SetFreqRange(m_pSetting->cust.m_nRecvFreqStart, m_pSetting->cust.m_nRecvFreqEnd);
	wc->SetThresholdLevel(m_pSetting->cust.m_rRecvThreshold);

	AddPaintEventControl(wc);
	AddMouseKeyEventControl(wc);

	return wc;
}

CLaSpectrogram* CLakeyMainWindow::CreateSpectrogram(const RECT* pRect, IFilterDFT<short int, float>* pNextFilter)
{
	CLaSpectrogram* p_spectrogram = new CLaSpectrogram(this, pRect, 44100, m_pSetting->cust.m_nRecvAnalyzeSamples);

	AddPaintEventControl(p_spectrogram);
	AddMouseKeyEventControl(p_spectrogram);

	return p_spectrogram;
}

CLaLabel* CLakeyMainWindow::CreateLabel(const RECT* pRect, const char* pText, STYLE* pStyle, const char* pMutexName/* = NULL */)
{
	CLaLabel* label = new CLaLabel(this, pRect, pText, pStyle, pMutexName);

	AddPaintEventControl(label);

	return label;
}

CLaLine* CLakeyMainWindow::CreateLine(int x1, int y1, int x2, int y2)
{
	CLaLine* line = new CLaLine(this, x1, y1, x2, y2);

	AddPaintEventControl(line);

	return line;
}

BOOL CLakeyMainWindow::Initialize()
{
	m_hMorseQueueThread = CreateThread(NULL, 0, MorseQueueThreadProc, this, CREATE_SUSPENDED, &m_nMorseQueueThreadID);
	if (!m_hMorseQueueThread)
		return FALSE;

	m_hMorseJournalThread = CreateThread(NULL, 0, MorseJournalThreadProc, this, 0, &m_nMorseJournalThreadID);
	if (!m_hMorseJournalThread)
		return FALSE;

	///*
	m_pRecvDctMonitor->Initialize();
	m_hRecvMonitorThread = CreateThread(NULL, 0, RecvMonitorThreadProc, this, 0, &m_nRecvMonitorThreadID);
	if (!m_hRecvMonitorThread)
		return FALSE;
	//	*/

	if (!m_pSound->Initialize(GetHWnd()))
		return FALSE;

	m_pSound->SetVolume((LONG)(m_pTnSendVol->GetScale() * 100));
	m_pSound->SetNoiseVolume((LONG)(m_pTnBgVol->GetScale() * 100));

	AddTimerEventControl(m_pSound, 1, 500);

	return TRUE;
}

void CLakeyMainWindow::GetRect(RECT* r)
{
	GetClientRect(GetHWnd(), r);
}

BOOL CLakeyMainWindow::OnPaint(void* owner, CGraphics* g, const RECT* pRect)
{
	//g->SetBgColor(RGB(127,127,127));
	//g->DrawRect(pRect);
	return TRUE;
}

BOOL CLakeyMainWindow::OnMouseMove(void* owner, int x, int y)
{
	return FALSE;
}

BOOL CLakeyMainWindow::OnKeyDown(void* owner, int nKeyCode)
{
	if (m_pBtCw == owner)
	{
		CwDown();
		return TRUE;
	}
	/*else if (' ' == nKeyCode)
	{
		m_pRecvDctMonitor->Refresh();
		return TRUE;
	}*/
	
	return FALSE;
}

BOOL CLakeyMainWindow::OnKeyUp(void* owner, int nKeyCode)
{
	if (m_pBtCw == owner)
	{
		CwUp();
		return TRUE;
	}
	else if (m_pBtSendFile == owner)
	{
		ChooseSendFile();
		return TRUE;
	}
	else if (VK_PAUSE == nKeyCode)
	{
		m_pRecvDctMonitor->SetFrazeState(!m_pRecvDctMonitor->GetFrazeState());
	}

	return FALSE;
}

BOOL CLakeyMainWindow::OnMouseKeyDown(void* owner, MouseKeyType nMkt, int x, int y)
{
	if (m_pBtCw == owner)
	{
		if (m_nAutoKey)
		{
			m_nAutoKeyState = (m_nAutoKey == nMkt);
			m_oAutoKeyQueue.push(m_nAutoKeyState);
			ResumeThread(m_hMorseQueueThread);
		}
		else
		{
			CwDown();
		}

		return FALSE;
	}
	else if (this == owner)
	{
	}

	return FALSE;
}

BOOL CLakeyMainWindow::OnMouseKeyUp(void* owner, MouseKeyType nMkt, int x, int y)
{
	if (m_pBtCw == owner)
	{
		if (m_nAutoKey)
		{
			m_nAutoKeyState = -1;
		}
		else
		{
			CwUp();
			return TRUE;
		}
	}
	else if (m_pBtSendFile == owner)
	{
		ChooseSendFile();
		return TRUE;
	}
	else if (this == owner)
	{
	}
	else
	{
	}

	return FALSE;
}

BOOL CLakeyMainWindow::ChooseSendFile()
{
	OPENFILENAME ofn;       // common dialog box structure
	char szFile[260] = "*.txt";       // buffer for file name
	HANDLE hf;              // file handle

	// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = GetHWnd();
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = "All\0*.*\0Text\0*.TXT\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	// Display the Open dialog box. 
	if (GetOpenFileName(&ofn))
	{
		hf = CreateFile(ofn.lpstrFile, GENERIC_READ,
			0, (LPSECURITY_ATTRIBUTES) NULL,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
			(HANDLE) NULL);

		char vBuff[256];
		int nRead = -1;
		while(ReadFile(hf, vBuff, sizeof(vBuff), (LPDWORD)&nRead, NULL) && 0 < nRead)
		{
			PushText(vBuff, nRead, 0);
		}

		CloseHandle(hf);
		return TRUE;
	}
	return FALSE;
}

void CLakeyMainWindow::PushText(const char* pBuff, int nSize, int nWordLen)
{
	for (int i = 0; i < nSize; ++i)
	{
		if (m_pSendCharQueueLabel->PushChar(pBuff[i]))
			m_oSendMorseQueue.push(GetMorseCode(pBuff[i]));

		if (0 < nWordLen && 0 == (i + 1) % (nWordLen))
		{
			if (m_pSendCharQueueLabel->PushChar('_'))
				m_oSendMorseQueue.push(GetMorseCode(' '));
		}
	}

	m_nAutoKey = 0;
	m_pBtAutoKeySwitch->SetText("Manual");

	ResumeThread(m_hMorseQueueThread);
}

void CLakeyMainWindow::ClearQueue()
{
	m_bClear = TRUE;
	ResumeThread(m_hMorseQueueThread);
}

BOOL CLakeyMainWindow::OnClick(void* owner)
{
	if (m_pBtCw == owner)
	{
	}
	else if (m_pBtSendPause == owner)
	{
		if (!(m_bPause = m_pBtSendPause->IsPressed()))
			ResumeThread(m_hMorseQueueThread);

		return TRUE;
	}
	else if (m_pBtSendFile == owner)
	{
	}
	else if (m_pBtAutoKeySwitch == owner)
	{
		switch(m_nAutoKey)
		{
		case LBUTTON:
			//m_pBtAutoKeySwitch->SetText("Auto(R/L)");
			m_nAutoKey = RBUTTON;
			break;
		case RBUTTON:
			//m_pBtAutoKeySwitch->SetText("Manual");
			m_nAutoKey = 0;
			break;
		default:
			//m_pBtAutoKeySwitch->SetText("Auto(L/R)");
			m_nAutoKey = LBUTTON;
			break;
		}

		return TRUE;
	}
	else if (m_pBtNoiseSwitch == owner)
	{
		m_pSound->SetNoise(m_pBtNoiseSwitch->IsPressed());
		return TRUE;
	}
	else if (this == owner)
	{
	}
	else
	{
		CLaButton* pMorseBt = (CLaButton *)owner;
		MORSECODE* pMorseCode = (MORSECODE *)pMorseBt->GetUserData();
		if (pMorseCode && MAX_LABEL_TEXT_LEN > m_oSendMorseQueue.size())
		{
			if (m_pSendCharQueueLabel->PushChar(pMorseCode->nAscCode))
			{
				m_oSendMorseQueue.push(pMorseCode);
				ResumeThread(m_hMorseQueueThread);
			}

			return TRUE;
		}
	}

	return FALSE;
}

void CLakeyMainWindow::CwDown()
{
	m_pSound->Play();
	if (m_pSetting->cust.m_nExtPortEnable)
		m_pExtPort->OutByte(m_pSetting->cust.m_nExtPortAddr, m_pSetting->cust.m_nOpenByte);

	LANWPKGKEYFRAME frame;
	frame.nToState = 1;
	frame.nActTick = 0;
	frame.nReserved = 0;

	m_pNetwork->AppendKeyFrame(&frame);

	m_bCwFlag = TRUE;
}

void CLakeyMainWindow::CwUp()
{
	m_pSound->Stop();
	if (m_pSetting->cust.m_nExtPortEnable)
		m_pExtPort->OutByte(m_pSetting->cust.m_nExtPortAddr, m_pSetting->cust.m_nCloseByte);

	LANWPKGKEYFRAME frame;
	frame.nToState = 0;
	frame.nActTick = 0;
	frame.nReserved = 0;

	m_pNetwork->AppendKeyFrame(&frame);

	m_bCwFlag = FALSE;
}

void CLakeyMainWindow::OnCwEvent(BOOL bCwDown)
{
	if (bCwDown)
		m_nRecvIdleCount = 0;	

	if (m_nRecvIdleCount < m_pSetting->cust.m_nRecvIdleLimit)
		m_pJpRecv->Sample(bCwDown);

	m_nRecvIdleCount += (int)RECV_SHORT_COUNT_EXPR;
}

BOOL CLakeyMainWindow::OnCommand(void* owner, int nCommId)
{
	switch(nCommId)
	{
	case IDM_SENDTRAINING:
		StartEncryptTrng();
		return TRUE;
	}
	return FALSE;
}

void CLakeyMainWindow::OnKeyFrame(const LANWPKGKEYFRAME* pFrame)
{
	if (pFrame->nToState)
	{
		m_bNwSndState = TRUE;
		m_pSound->Play();
	}
	else
	{
		m_bNwSndState = FALSE;
		m_pSound->Stop();
	}
}

void CLakeyMainWindow::OnSourceFrame(const LANWPKGSRCFRAME* pFrame, const char* szDomainName)
{
}

void CLakeyMainWindow::OnTextFrame(const LANWPKGTXTFRAME* pFrame)
{
}

void CLakeyMainWindow::OnEvent(WORD nMsg)
{
	m_pSound->Stop();
}

BOOL CLakeyMainWindow::NetworkStartup()
{
	if (m_pSetting->cust.m_bNetworkEnabled)
	{
		LANWSRCNODE node;

		for (UINT i = 0; i < m_pSetting->hosts.size(); ++i)
		{
			node.base.nAddr = inet_addr(m_pSetting->hosts[i].szHostName);
			if (INADDR_NONE != node.base.nAddr)
			{
				int nTmp;
				sscanf(m_pSetting->hosts[i].szPort, "%d", &nTmp);
				node.base.nPort = nTmp;
				m_pNetwork->AppendStbSrcNode(&node);
			}
		}

		if (!m_pNetwork->Startup(m_pSetting->cust.m_nLocalPort))
		{
			MessageBox(NULL, "无法打开网络功能", "警告", MB_OK);
			return FALSE;
		}

		return TRUE;
	}

	return FALSE;
}

BOOL CLakeyMainWindow::StartEncryptTrng()
{
	OPENFILENAME ofn;       // common dialog box structure
	char szFn[512] = "*.txt";       // buffer for file name
	HANDLE hf;              // file handle

	// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = GetHWnd();
	ofn.lpstrFile = szFn;
	ofn.nMaxFile = sizeof(szFn) - 10;	// reserved for postfix of journal file
	ofn.lpstrFilter = "All\0*.*\0Text\0*.TXT\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	// Display the Open dialog box. 
	if (GetOpenFileName(&ofn))
	{
		m_nEncryptTrngOkChars = 0;
		m_rEncryptTrngOkStdWords = 0;
		m_rEncryptTrngStdWords = 0;

		hf = CreateFile(ofn.lpstrFile, GENERIC_READ,
			0, (LPSECURITY_ATTRIBUTES) NULL,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
			(HANDLE) NULL);

		int nRead = -1;
		char szBuf[256];
		while(ReadFile(hf, szBuf, sizeof(szBuf), (LPDWORD)&nRead, NULL) && 0 < nRead)
		{
			PushEncryptTrngText(szBuf, nRead);
		}

		CloseHandle(hf);

		if (0 == m_nEncryptTrngTotalChars)
			return FALSE;

		strcat(szFn, ".jrn");
		m_hSendJrnFile = CreateFile(szFn,     // file to create
                   GENERIC_WRITE | GENERIC_READ,          // open for writing
                   0,                      // do not share
                   NULL,                   // default security
                   CREATE_ALWAYS,          // overwrite existing
                   FILE_ATTRIBUTE_NORMAL,
                   NULL);                  // no attr. template

		if (INVALID_HANDLE_VALUE == m_hSendJrnFile)
		{
		    printf("Could not create journal file (error %d)\n", GetLastError());
		}

		m_nEncryptTrngStartTC = GetTickCount();

		return TRUE;
	}
	return FALSE;

}
void CLakeyMainWindow::PushEncryptTrngText(const char* pText, int nSize)
{
	for (int i = 0; i < nSize; ++i)
	{
		switch(pText[i])
		{
		case '\n':
		case '\r':
		case '\t':
		case ' ':
			break;
		default:
			m_oEncryptTrngQueue.push(pText[i]);
			++m_nEncryptTrngTotalChars;
		}
	}
}
void CLakeyMainWindow::OnWorkOut(void* owner, const MORSECODE* pResult)
{
	if (m_nEncryptTrngTotalChars)
	{
		if (!m_oEncryptTrngQueue.empty())
		{
			char ch = m_oEncryptTrngQueue.front();
			m_oEncryptTrngQueue.pop();

			if (pResult && (ch > 'Z' ? ch - 'a' + 'A' : ch) == pResult->nAscCode)
			{
				++m_nEncryptTrngOkChars;
				m_rEncryptTrngOkStdWords += pResult->rStdWordLen;
			}

			m_rEncryptTrngStdWords += pResult->rStdWordLen;
		}

		DWORD t;
		if (pResult)
		{
			if (!WriteFile(m_hSendJrnFile, &pResult->nAscCode, 1, &t, NULL))
			{
				printf("Could not write journal file (error %d)\n", GetLastError());
			}
		}
		else
		{
			WriteFile(m_hSendJrnFile, "#", 1, &t, NULL);
		}
		
		if (m_oEncryptTrngQueue.empty())
		{
			char szBuf[128];
			float t2 = (GetTickCount() - m_nEncryptTrngStartTC) / 1000.0f;
			sprintf(szBuf
				, "\n用时:\t%2.1f秒\n应拍发字符总数:\t%d\n正确率:\t%2.1f%%\n平均标准速率:\t%2.1fwpm\n正确标准速率:\t%2.1fwpm\n(统计不含空格)"
				, t2
				, m_nEncryptTrngTotalChars
				, m_nEncryptTrngOkChars * 100.0f / m_nEncryptTrngTotalChars
				, m_rEncryptTrngStdWords / (t2 / 60.0f)
				, m_rEncryptTrngOkStdWords / (t2 / 60.0f)
			);

			WriteFile(m_hSendJrnFile, "\n====================", 16, &t, NULL);
			WriteFile(m_hSendJrnFile, szBuf, strlen(szBuf), &t, NULL);
			CloseHandle(m_hSendJrnFile);

			MessageBox(GetHWnd(), szBuf, "技术统计", MB_OK);
			m_nEncryptTrngTotalChars = 0;
		}
	}
}
void CLakeyMainWindow::StopEncryptTrng()
{
	while(m_oEncryptTrngQueue.empty())
	{
		m_oEncryptTrngQueue.pop();
	}

	m_nEncryptTrngTotalChars = 0;
	CloseHandle(m_hSendJrnFile);
}

// Threads definition
#define NOTEVALIFNEEDEXIT(expr)	if (!owner->m_bNeedExit) expr

DWORD WINAPI CLakeyMainWindow::MorseQueueThreadProc(LPVOID pOwner)
{
	CLakeyMainWindow* owner = (CLakeyMainWindow *)pOwner;

	while(!owner->m_bNeedExit)
	{
		//assert(!owner->m_oSendMorseQueue.empty());

		// need not exit and queue not empty and not pause and not in auto key mode
		// send chars mode
		while(!owner->m_bNeedExit && !owner->m_oSendMorseQueue.empty() && !owner->m_bPause && 0 == owner->m_nAutoKey)
		{
			MORSECODE* pMorse = owner->m_oSendMorseQueue.front();
			owner->m_oSendMorseQueue.pop();
			
			if (!owner->m_bClear)
			{
				if ('_' != pMorse->nAscCode)
				{
					int nWindow = pMorse->nMask + 1;
					while((!owner->m_bNeedExit) && (nWindow >>= 1))
					{
						int nHitCount =
							nWindow & pMorse->nMorseCode ? 
								owner->m_pSetting->cust.m_nShortHit : owner->m_pSetting->cust.m_nLongHit;

						owner->CwDown();
						NOTEVALIFNEEDEXIT(Sleep(nHitCount));
						owner->CwUp();
						NOTEVALIFNEEDEXIT(Sleep(owner->m_pSetting->cust.m_nHitDelay));
					}
				}
				else
				{
					NOTEVALIFNEEDEXIT(Sleep(owner->m_pSetting->cust.m_nWordDelay));
				}
			}
			
			owner->m_pSendCharQueueLabel->PopChar();

			if (!owner->m_bClear)
				NOTEVALIFNEEDEXIT(Sleep(owner->m_pSetting->cust.m_nLetterDelay));
		}

		// auto key mode
		while(!owner->m_bNeedExit && !owner->m_oAutoKeyQueue.empty() && !owner->m_bPause && 0 != owner->m_nAutoKey)
		{
			BOOL bKey = owner->m_oAutoKeyQueue.front();
			owner->m_oAutoKeyQueue.pop();
			
			int nHitCount =
				bKey ? 
					owner->m_pSetting->cust.m_nShortHit : owner->m_pSetting->cust.m_nLongHit;

			owner->CwDown();
			NOTEVALIFNEEDEXIT(Sleep(nHitCount));
			owner->CwUp();
			NOTEVALIFNEEDEXIT(Sleep(owner->m_pSetting->cust.m_nHitDelay));

			if (0 <= owner->m_nAutoKeyState)
			{
				if (owner->m_oAutoKeyQueue.empty())
					owner->m_oAutoKeyQueue.push(owner->m_nAutoKeyState);
			}
		}

		owner->m_bClear = FALSE;
		NOTEVALIFNEEDEXIT(SuspendThread(owner->m_hMorseQueueThread));
	}

	return 0;
}

DWORD WINAPI CLakeyMainWindow::MorseJournalThreadProc(LPVOID pOwner)
{
	CLakeyMainWindow* owner = (CLakeyMainWindow *)pOwner;
	int nIdleCount = 0;

	while(!owner->m_bNeedExit)
	{
		if (owner->m_bCwFlag)
			nIdleCount = 0;

		if (nIdleCount < owner->m_pSetting->cust.m_nSendIdleLimit)
			owner->m_pJpSend->Sample(owner->m_bCwFlag);

		NOTEVALIFNEEDEXIT(Sleep(owner->m_pSetting->cust.m_nSendJournalPeriod));
		nIdleCount += owner->m_pSetting->cust.m_nSendJournalPeriod;
	}

	return 0;
}

DWORD WINAPI CLakeyMainWindow::RecvMonitorThreadProc(LPVOID pOwner)
{
	CLakeyMainWindow* owner = (CLakeyMainWindow *)pOwner;
	int nIdleCount = 0;

	while(!owner->m_bNeedExit)
	{
		owner->m_pRecvDctMonitor->Refresh();

		if (owner->m_bNwSndState)
			nIdleCount = 0;

		if (nIdleCount < owner->m_pSetting->cust.m_nRecvIdleLimit)
			owner->m_pJpRecv->Sample(owner->m_bNwSndState);

		NOTEVALIFNEEDEXIT(Sleep(owner->m_pSetting->cust.m_nRecvJournalPeriod));
		nIdleCount += owner->m_pSetting->cust.m_nRecvJournalPeriod;
	}

	return 0;
}
