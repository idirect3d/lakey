#pragma once

#include <queue>

#include "EventManagerWin32.h"
#include "LakeySetting.h"
#include "LaWaveCapture.h"
#include "LaNetwork.h"
#include "LaButton.h"
#include "LaTuner.h"
#include "MorseParser.h"

class CSinSound;
class CLaButton;
class CLaJournalPanel;
class CLaWaveCapture;
class CLaSpectrogram;
class CLaLabel;
class CLaLine;
class CLaHwControl;
class CLaNetwork;
class CLaTuner;

class CLakeyMainWindow
	: public CEventDispatcherWin32
	, public IMouseMoveEventListener
	, public IKeyboardEventControl
	, public IMouseKeyEventListener
	, public IPaintEventControl
	, public ICwEventListener
	, public ICommandEventControl
	, public INetworkFrameListener
	, public INetworkEventListener
	, public ITunerEventListener
	, public IParseEventListener
{
public:
	CLakeyMainWindow(HWND hWnd, CLakeySetting* pSetting);
	virtual ~CLakeyMainWindow(void);

	virtual BOOL Initialize();
	virtual BOOL NetworkStartup();
	virtual void NetworkShutdown() { return m_pNetwork->Shutdown(); };

	virtual BOOL IsRelated(int x, int y) { return TRUE; };
	virtual void GetRect(RECT* r);

	virtual BOOL OnMouseMove(void* owner, int x, int y);
	virtual BOOL OnKeyDown(void* owner, int nKeyCode);
	virtual BOOL OnKeyUp(void* owner, int nKeyCode);
	virtual BOOL OnMouseKeyDown(void* owner, MouseKeyType nMkt, int x, int y);
	virtual BOOL OnMouseKeyUp(void* owner, MouseKeyType nMkt, int x, int y);
	virtual BOOL OnClick(void* owner);
	virtual BOOL OnPaint(void* owner, CGraphics* g, const RECT* pRect);

	virtual BOOL OnTune(void* owner, LASCALE nScale);

	virtual void OnCwEvent(BOOL bCwDown);
	virtual BOOL OnCommand(void* owner, int nCommId);

	virtual void OnKeyFrame(const LANWPKGKEYFRAME* pFrame);
	virtual void OnSourceFrame(const LANWPKGSRCFRAME* pFrame, const char* szDomainName);
	virtual void OnTextFrame(const LANWPKGTXTFRAME* pFrame);
	virtual void OnEvent(WORD nMsg);
	virtual void OnWorkOut(void* owner, const MORSECODE* pResult);

	void PushText(const char* pText, int nSize, int nWordLen);
	void ClearQueue();

	BOOL StartEncryptTrng();
	void PushEncryptTrngText(const char* pText, int nSize);
	void StopEncryptTrng();

private:
	CLaButton* CreateButton(const char* pText, int x, int y, int w, int h, int nWantKeyCode, CLaButton::ButtonStyle nStyle = CLaButton::BT_NORMAL);
	CLaButton* CreateButton(const BUTTONMAPPING* pMapping, CLaButton::ButtonStyle nStyle = CLaButton::BT_NORMAL);
	CLaJournalPanel* CreateJournalPanel(const RECT* pRect, int nMaxShortCount, STYLE* pStyle, IParseEventListener* pParseListener);
	CLaWaveCapture* CreateWaveCapture(const RECT* pRect, IFilterDFT<short int, float>* pNextFilter);
	CLaSpectrogram* CreateSpectrogram(const RECT* pRect, IFilterDFT<short int, float>* pNextFilter);
	CLaLabel* CreateLabel(const RECT* pRect, const char* pText, STYLE* pStyle, const char* pMutexName = NULL);
	CLaLine* CreateLine(int x1, int y1, int x2, int y2);
	CLaTuner* CreateTuner(const char* pText, int x, int y, int w, int h, LASCALE nScaleL, LASCALE nScaleR, LASCALE nScale);
	CLaTuner* CreateTuner(const TUNERMAPPING* pMapping);

	MORSECODE* GetMorseCode(char ch);
	BOOL ChooseSendFile();

	void BuildMorseButtons();
	void CwDown();
	void CwUp();

private:
	CLakeySetting*	m_pSetting;
	CSinSound*	m_pSound;

	// Resources
	CFont*		m_pFont;
	CFont*		m_pLogoFont;
	STYLE		m_oBtStyle;
	STYLE		m_oLabelStyle;
	STYLE		m_oScqlStyle;
	STYLE		m_oLogoStyle;
	STYLE		m_oVersionStyle;

	// Controls
	CLaButton*	m_pBtCw;
	CLaButton*	m_pBtSendPause;
	CLaButton*	m_pBtSendFile;
	CLaButton*	m_pBtAutoKeySwitch;
	CLaButton*	m_pBtNoiseSwitch;

	CLaTuner*	m_pTnSendVol;
	CLaTuner*	m_pTnBgVol;
	CLaTuner*	m_pTnSendSpeed;
	CLaTuner*	m_pTnSpecBrightness;

	BOOL		m_bPause;
	BOOL		m_bClear;
	BOOL		m_bNwSndState;
	int			m_nAutoKey;
	int			m_nAutoKeyState;

	// Morse buttons
	CLaButton*	m_vBtMorse[MORSECODECOUNT];

	// CW journal panel
	CLaJournalPanel*	m_pJpSend;
	CLaLabel*			m_pSendCharQueueLabel;
	CLaLine*			m_pSendJpBorder;
	CLaLine*			m_pSendTunerBorder;

	CLaWaveCapture*		m_pRecvDctMonitor;
	CLaSpectrogram*		m_pRecvSpectrogram;
	CLaJournalPanel*	m_pJpRecv;
	CLaLine*			m_pRecvJpBorder;
	CLaLine*			m_pRecvTunerBorder;

	CLaLabel*			m_pSendLabel;
	CLaLabel*			m_pRecvLabel;

	// CLaLabel*			m_pLogo;
	// CLaLabel*			m_pVersion;

	// CW Flag
	BOOL		m_bCwFlag;

	// HW Control
	CLaHwControl*		m_pExtPort;

	// Network
	CLaNetwork*			m_pNetwork;

	// Send morse queue
	queue<MORSECODE *>	m_oSendMorseQueue;
	queue<BOOL>			m_oAutoKeyQueue;
	BOOL		m_bNeedExit;
	HANDLE		m_hMorseQueueThread;
	DWORD		m_nMorseQueueThreadID;
	HANDLE		m_hMorseJournalThread;
	DWORD		m_nMorseJournalThreadID;
	HANDLE		m_hRecvMonitorThread;
	DWORD		m_nRecvMonitorThreadID;

	int			m_nRecvIdleCount;

	// encrypt training
	queue<char>	m_oEncryptTrngQueue;
	DWORD		m_nEncryptTrngTotalChars;
	DWORD		m_nEncryptTrngOkChars;
	float		m_rEncryptTrngStdWords;
	float		m_rEncryptTrngOkStdWords;
	DWORD		m_nEncryptTrngStartTC;
	HANDLE		m_hSendJrnFile;

	static DWORD WINAPI MorseQueueThreadProc(LPVOID pOwner);
	static DWORD WINAPI MorseJournalThreadProc(LPVOID pOwner);
	static DWORD WINAPI RecvMonitorThreadProc(LPVOID pOwner);
};
