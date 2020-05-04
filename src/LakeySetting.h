#pragma once

#include <windows.h>
#include <vector>
#include "LaTuner.h"

using namespace std;

#define MORSECODECOUNT		42
#define PROFILE				"lakey.ini"
#define HOSTS_MAX			8
#define HOSTS_NAME_LEN		64
// PARIS: .--. .- .-. .. ...
#define STD_WORD_LEN		46


typedef struct tagMorseCode
{
	unsigned int	nAscCode;
	unsigned short int	nMorseCode;
	unsigned short int	nMask;
	unsigned int	nKeyCode;
	float			rStdWordLen;
} MORSECODE;

typedef struct tagButtonMapping
{
	int		x;
	int		y;
	int		w;
	int		h;
	int		nKeyCode;
	char	vLabel[128];
} BUTTONMAPPING;

typedef struct tagTunerMapping
{
	int		x;
	int		y;
	int		w;
	int		h;
	LASCALE	vl;
	LASCALE	vr;
	LASCALE	v;
	char	vLabel[128];
} TUNERMAPPING;

typedef struct tagHostNode
{
	char	szHostName[60];
	char	szPort[8];
} HOSTNODE;

typedef vector<HOSTNODE> HostList;

typedef struct tagCustomize
{
	double		m_rBeepFreq;
	double		m_rBeepVol;
	int			m_nShortHit;
	int			m_nLongHit;
	int			m_nHitDelay;
	int			m_nLetterDelay;
	int			m_nWordDelay;

	RECT		m_oWindowRect;

	int			m_nSendJournalPeriod;
	int			m_nSendIdleLimit;
	RECT		m_oSendJournalRect;
	SIZE		m_oSendLabelOffset;
	char		m_vSendLabel[16];
	RECT		m_oSendCharQueueRect;
	int			m_nRecvJournalPeriod;
	int			m_nRecvIdleLimit;
	RECT		m_oRecvJournalRect;
	SIZE		m_oRecvLabelOffset;
	char		m_vRecvLabel[16];
	RECT		m_oRecvMonitorRect;
	int			m_nRecvAnalyzeSamples;
	double		m_rRecvThreshold;
	int			m_nRecvFreqStart;
	int			m_nRecvFreqEnd;

	RECT		m_oSpectrogramRect;
	SIZE		m_oSpecLabelOffset;
	char		m_vSpecLabel[16];

	BUTTONMAPPING	m_oCwKeyButton;
	BUTTONMAPPING	m_oSendPauseButton;
	BUTTONMAPPING	m_oSendFileButton;
	BUTTONMAPPING	m_oAutoKeySwitchButton;
	BUTTONMAPPING	m_oNoiseSwitchButton;

	TUNERMAPPING	m_oSendVolTuner;
	TUNERMAPPING	m_oNoiseVolTuner;
	TUNERMAPPING	m_oSendSpeedTuner;
	TUNERMAPPING	m_oSpecBrightnessTuner;

	char		m_vKochChar[MORSECODECOUNT + 1];
	int			m_nKochWordLen;

	MORSECODE	m_vMorseCode[MORSECODECOUNT];

	int			m_nExtPortEnable;
	WORD		m_nExtPortAddr;
	BYTE		m_nOpenByte;
	BYTE		m_nCloseByte;

	WORD		m_nLocalPort;
	BOOL		m_bNetworkEnabled;
	//char		m_vHostList[HOSTS_MAX][HOSTS_NAME_LEN];

	//Styles
	//Frequency Analyzer
	int			m_nFaBgColor;
	int			m_nFaWaveColor;
	int			m_nFaFloatColor;
	int			m_nFaFreqAnaColor;

} CUSTOMIZE;

typedef struct tagWPMS
{
	float		wpm;
	int		di;
	int		dah;
	int		hdelay;
	int		ldelay;
	int		wdelay;
} WPMS;

void wpmToParams(WPMS* pWpms);
void paramsToWpm(WPMS* pWpms);
float calcStdWordLen(short int nMorseCode, short int nMask);

class CLakeySetting
{
public:
	CLakeySetting(void);
	virtual ~CLakeySetting(void);

	virtual BOOL Load();
	virtual BOOL Save();

	CUSTOMIZE	cust;
	HostList	hosts;

	virtual void GetCustomize(CUSTOMIZE* pCust);
	virtual void SetCustomize(const CUSTOMIZE* pCust);

private:
	void StrToMorseCode(int nIdx, const char* pStr);
	char* MorseCodeToStr(char* pStr, int nIdx);
	char* ButtonMappingToStr(char* pStr, const BUTTONMAPPING* pMapping);
	void StrToButtonMapping(BUTTONMAPPING* pMapping, const char* pStr);

	char* TunerMappingToStr(char* pStr, const TUNERMAPPING* pMapping);
	void StrToTunerMapping(TUNERMAPPING* pMapping, const char* pStr);

	int GetProfileHex(const char* pSec, const char* pKey, int nDefault);
	void WriteProfileHex(const char* pSec, const char* pKey, int n);
	int GetProfileDec(const char* pSec, const char* pKey, int nDefault);
	void WriteProfileDec(const char* pSec, const char* pKey, int n);
	double GetProfileDouble(const char* pSec, const char* pKey, double rDefault);
	void WriteProfileDouble(const char* pSec, const char* pKey, double r);

	void GetButtonMapping(const char* pSec, const char* pKey, BUTTONMAPPING* pBtMap);
	void WriteButtonMapping(const char* pSec, const char* pKey, const BUTTONMAPPING* pBtMap);

	void GetTunerMapping(const char* pSec, const char* pKey, TUNERMAPPING* pTnMap);
	void WriteTunerMapping(const char* pSec, const char* pKey, const TUNERMAPPING* pTnMap);

	void GetProfileRect(const char* pSec, const char* pKey, RECT* pRect);
	void WriteProfileRect(const char* pSec, const char* pKey, const RECT* pRect);

	void GetProfileSize(const char* pSec, const char* pKey, SIZE* pSize);
	void WriteProfileSize(const char* pSec, const char* pKey, const SIZE* pSize);

};
