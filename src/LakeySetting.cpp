#include "StdAfx.h"
#include "lakeysetting.h"

#include "stdio.h"

#define SetMorseCode(s, asc, mc, mm, kc)	\
	s.nAscCode = (asc); s.nMorseCode = (mc); s.nMask = (mm); s.nKeyCode = (kc); s.rStdWordLen = calcStdWordLen(mc, mm);

#define SetButtonMapping(s, nx, ny, nw, nh, kc, txt)	\
	s.x = (nx); s.y = (ny); s.w = (nw); s.h = (nh); s.nKeyCode = (kc); strncpy(s.vLabel, (txt), sizeof(s.vLabel) - 1); 

#define SetTunerMapping(s, nx, ny, nw, nh, nl, nr, nv, txt)	\
	s.x = (nx); s.y = (ny); s.w = (nw); s.h = (nh); s.vl = (nl); s.vr = (nr); s.v = (nv); strncpy(s.vLabel, (txt), sizeof(s.vLabel) - 1); 

CLakeySetting::CLakeySetting(void)
{
	cust.m_rBeepFreq = 1000.0;
	cust.m_rBeepVol = 0.25;
	cust.m_nShortHit = 80;
	cust.m_nLongHit = 240;
	cust.m_nHitDelay = 80;
	cust.m_nLetterDelay = 240;
	cust.m_nWordDelay = 240;

	SetRect(&cust.m_oWindowRect, 0, 0, 920, 436);

	cust.m_nSendJournalPeriod = 27;
	SetRect(&cust.m_oSendJournalRect, 5,235,490,267);
	cust.m_oSendLabelOffset.cx = 0;
	cust.m_oSendLabelOffset.cy = -15;
	cust.m_nSendIdleLimit = 750;
	SetRect(&cust.m_oSendCharQueueRect, 5,270,490,281);
	strcpy(cust.m_vSendLabel, "Send");

	SetRect(&cust.m_oSpectrogramRect, 498,10,908,160);
	cust.m_oSpecLabelOffset.cx = 0;
	cust.m_oSpecLabelOffset.cy = -15;
	strcpy(cust.m_vSpecLabel, "Spectrogram");

	cust.m_nRecvJournalPeriod = 27;
	SetRect(&cust.m_oRecvJournalRect, 5,180,490,212);
	cust.m_oRecvLabelOffset.cx = 0;
	cust.m_oRecvLabelOffset.cy = -15;
	cust.m_nRecvIdleLimit = 750;
	strcpy(cust.m_vRecvLabel, "Recieve");

	SetRect(&cust.m_oRecvMonitorRect, 8,10,493,160);
	cust.m_nRecvAnalyzeSamples = 512;
	cust.m_rRecvThreshold = 0.3l;
	cust.m_nRecvFreqStart = 850;
	cust.m_nRecvFreqEnd = 1250;

	SetButtonMapping(cust.m_oCwKeyButton, 500,290,297,92, VK_RETURN, "CW");
	SetButtonMapping(cust.m_oSendFileButton, 500,265,91,16, VK_F6, "File...");
	SetButtonMapping(cust.m_oSendPauseButton, 603,265,91,16, VK_F5, "Pause,Continue");
	SetButtonMapping(cust.m_oAutoKeySwitchButton, 706,265,91,16, VK_TAB, "Manual,Auto(L/R),Auto(R/L)");
	SetButtonMapping(cust.m_oNoiseSwitchButton, 809,265,91,16, VK_F7, "Disturb");

	SetTunerMapping(cust.m_oSendVolTuner, 500,168,90,90,-80.000000,0.000000, 0, "Volumn(dB)");
	SetTunerMapping(cust.m_oSendSpeedTuner, 603,168,90,90,5.000000,30.000000, 20, "Speed(wpm)");
	SetTunerMapping(cust.m_oSpecBrightnessTuner, 706,168,90,90,-60.000000,60.000000, 0.0, "Spectrogram(dB)");
	SetTunerMapping(cust.m_oNoiseVolTuner, 809,168,90,90,-80.000000,0.000000, 0, "Disturb(dB)");
	
	SetMorseCode(cust.m_vMorseCode[0], 'A', 0x0002, 0x0003, 'A');
	SetMorseCode(cust.m_vMorseCode[1], 'B', 0x0007, 0x000f, 'B');
	SetMorseCode(cust.m_vMorseCode[2], 'C', 0x0005, 0x000f, 'C');
	SetMorseCode(cust.m_vMorseCode[3], 'D', 0x0003, 0x0007, 'D');
	SetMorseCode(cust.m_vMorseCode[4], 'E', 0x0001, 0x0001, 'E');
	SetMorseCode(cust.m_vMorseCode[5], 'F', 0x000d, 0x000f, 'F');
	SetMorseCode(cust.m_vMorseCode[6], 'G', 0x0001, 0x0007, 'G');
	SetMorseCode(cust.m_vMorseCode[7], 'H', 0x000f, 0x000f, 'H');
	SetMorseCode(cust.m_vMorseCode[8], 'I', 0x0003, 0x0003, 'I');
	SetMorseCode(cust.m_vMorseCode[9], 'J', 0x0008, 0x000f, 'J');
	SetMorseCode(cust.m_vMorseCode[10], 'K', 0x0002, 0x0007, 'K');
	SetMorseCode(cust.m_vMorseCode[11], 'L', 0x000b, 0x000f, 'L');
	SetMorseCode(cust.m_vMorseCode[12], 'M', 0x0000, 0x0003, 'M');
	SetMorseCode(cust.m_vMorseCode[13], 'N', 0x0001, 0x0003, 'N');
	SetMorseCode(cust.m_vMorseCode[14], 'O', 0x0000, 0x0007, 'O');
	SetMorseCode(cust.m_vMorseCode[15], 'P', 0x0009, 0x000f, 'P');
	SetMorseCode(cust.m_vMorseCode[16], 'Q', 0x0002, 0x000f, 'Q');
	SetMorseCode(cust.m_vMorseCode[17], 'R', 0x0005, 0x0007, 'R');
	SetMorseCode(cust.m_vMorseCode[18], 'S', 0x0007, 0x0007, 'S');
	SetMorseCode(cust.m_vMorseCode[19], 'T', 0x0000, 0x0001, 'T');
	SetMorseCode(cust.m_vMorseCode[20], 'U', 0x0006, 0x0007, 'U');
	SetMorseCode(cust.m_vMorseCode[21], 'V', 0x000e, 0x000f, 'V');
	SetMorseCode(cust.m_vMorseCode[22], 'W', 0x0004, 0x0007, 'W');
	SetMorseCode(cust.m_vMorseCode[23], 'X', 0x0006, 0x000f, 'X');
	SetMorseCode(cust.m_vMorseCode[24], 'Y', 0x0004, 0x000f, 'Y');
	SetMorseCode(cust.m_vMorseCode[25], 'Z', 0x0003, 0x000f, 'Z');

	SetMorseCode(cust.m_vMorseCode[26], '1', 0x0010, 0x001f, '1');
	SetMorseCode(cust.m_vMorseCode[27], '2', 0x0018, 0x001f, '2');
	SetMorseCode(cust.m_vMorseCode[28], '3', 0x001c, 0x001f, '3');
	SetMorseCode(cust.m_vMorseCode[29], '4', 0x001e, 0x001f, '4');
	SetMorseCode(cust.m_vMorseCode[30], '5', 0x001f, 0x001f, '5');
	SetMorseCode(cust.m_vMorseCode[31], '6', 0x000f, 0x001f, '6');
	SetMorseCode(cust.m_vMorseCode[32], '7', 0x0007, 0x001f, '7');
	SetMorseCode(cust.m_vMorseCode[33], '8', 0x0003, 0x001f, '8');
	SetMorseCode(cust.m_vMorseCode[34], '9', 0x0001, 0x001f, '9');
	SetMorseCode(cust.m_vMorseCode[35], '0', 0x0000, 0x001f, '0');

	SetMorseCode(cust.m_vMorseCode[36], '?', 0x0033, 0x003f, 0);
	SetMorseCode(cust.m_vMorseCode[37], '/', 0x000d, 0x001f, VK_DIVIDE);
	SetMorseCode(cust.m_vMorseCode[38], '(', 0x0012, 0x003f, 0);
	SetMorseCode(cust.m_vMorseCode[39], '-', 0x000e, 0x003f, VK_SUBTRACT);
	SetMorseCode(cust.m_vMorseCode[40], '.', 0x002a, 0x003f, VK_DECIMAL);
	SetMorseCode(cust.m_vMorseCode[41], '_', 0x0000, 0x0000, ' ');

	strcpy(cust.m_vKochChar, "KMRSUAPTLOWI.NJEF0Y,VG5/Q9ZH38B?427C1D6X");
	cust.m_nKochWordLen = 5;

	cust.m_nExtPortEnable = 0;
	cust.m_nExtPortAddr = 0x378;
	cust.m_nOpenByte = 0xff;
	cust.m_nCloseByte = 0x00;

	cust.m_bNetworkEnabled = TRUE;
	cust.m_nLocalPort = 3010;

	cust.m_nFaBgColor = 0x000000;
	cust.m_nFaWaveColor = 0x00ff00;
	cust.m_nFaFreqAnaColor = 0xd0d0d0;
	cust.m_nFaFloatColor = 0xf0f0f0;

	Load();
}

CLakeySetting::~CLakeySetting(void)
{
	Save();
}

void CLakeySetting::StrToMorseCode(int nIdx, const char* pStr)
{
	sscanf(pStr, "%8X,%8X,%4X"
		, &(cust.m_vMorseCode[nIdx].nMorseCode)
		, &(cust.m_vMorseCode[nIdx].nMask)
		, &(cust.m_vMorseCode[nIdx].nKeyCode));
}

char* CLakeySetting::MorseCodeToStr(char* pStr, int nIdx)
{
	sprintf(pStr, "%8.8X,%8.8X,%4.4X"
		, cust.m_vMorseCode[nIdx].nMorseCode
		, cust.m_vMorseCode[nIdx].nMask
		, cust.m_vMorseCode[nIdx].nKeyCode);

	return pStr;
}

void CLakeySetting::StrToButtonMapping(BUTTONMAPPING* pMapping, const char* pStr)
{
	sscanf(pStr, "%d,%d,%d,%d,%4X,%s"
		, &(pMapping->x)
		, &(pMapping->y)
		, &(pMapping->w)
		, &(pMapping->h)
		, &(pMapping->nKeyCode)
		, pMapping->vLabel
		);
}

char* CLakeySetting::ButtonMappingToStr(char* pStr, const BUTTONMAPPING* pMapping)
{
	sprintf(pStr, "%d,%d,%d,%d,%4.4X,%s"
		, pMapping->x
		, pMapping->y
		, pMapping->w
		, pMapping->h
		, pMapping->nKeyCode
		, pMapping->vLabel
		);

	return pStr;
}

void CLakeySetting::StrToTunerMapping(TUNERMAPPING* pMapping, const char* pStr)
{
	sscanf(pStr, "%d,%d,%d,%d,%f,%f,%f,%s"
		, &(pMapping->x)
		, &(pMapping->y)
		, &(pMapping->w)
		, &(pMapping->h)
		, &(pMapping->vl)
		, &(pMapping->vr)
		, &(pMapping->v)
		, pMapping->vLabel
		);
}

char* CLakeySetting::TunerMappingToStr(char* pStr, const TUNERMAPPING* pMapping)
{
	sprintf(pStr, "%d,%d,%d,%d,%f,%f,%f,%s"
		, pMapping->x
		, pMapping->y
		, pMapping->w
		, pMapping->h
		, pMapping->vl
		, pMapping->vr
		, pMapping->v
		, pMapping->vLabel
		);

	return pStr;
}

#define PROFILE_PATHNAME	"./lakey.ini"
#define SECTION_CW			"CW"
#define CW_BEEPFREQ			"beepfreq"
#define CW_BEEPVOL			"beepvol"
#define CW_SHORTHIT			"shorthit"
#define CW_LONGHIT			"longhit"
#define CW_HITDELAY			"hitdelay"
#define CW_LETTERDELAY		"letterdelay"
#define CW_WORDDELAY		"worddelay"

#define SECTION_GLOBAL			"GLOBAL"
#define GLOBAL_WINDOWRECT		"windowrect"

#define SECTION_JOURNAL			"JOURNAL"
#define JOURNAL_SENDPERIOD		"sendperiod"
#define JOURNAL_SENDIDLELIMIT	"sendidlelimit"
#define JOURNAL_SENDRECT		"sendrect"
#define JOURNAL_SENDLABELOFFSET	"sendlabeloffset"
#define JOURNAL_SENDLABEL		"sendlabel"
#define JOURNAL_SENDQUEUERECT	"sendqueuerect"

#define JOURNAL_SPECRECT		"specrect"
#define JOURNAL_SPECLABELOFFSET	"speclabeloffset"
#define JOURNAL_SPECLABEL		"speclabel"

#define JOURNAL_RECVPERIOD		"recvperiod"
#define JOURNAL_RECVIDLELIMIT	"recvidlelimit"
#define JOURNAL_RECVRECT		"recvrect"
#define JOURNAL_RECVLABELOFFSET	"recvlabeloffset"
#define JOURNAL_RECVLABEL		"recvlabel"
#define JOURNAL_RECVMRECT		"recvmrect"
#define JOURNAL_RECVANALYZESAMPLE	"recvanalyzesamples"
#define JOURNAL_RECVTHRESHOLD	"recvthreshold"
#define JOURNAL_RECVFREQSTART	"recvfreqstart"
#define JOURNAL_RECVFREQEND		"recvfreqend"

#define SECTION_MORSE		"MORSE"

#define SECTION_MAINPANEL	"MAINPANEL"
#define MAINPANEL_CW		"cw"
#define MAINPANEL_SENDPAUSE	"sendpause"
#define MAINPANEL_SENDFILE	"sendfile"
#define MAINPANEL_AUTOKEYSWITCH	"autokeyswitch"
#define MAINPANEL_NOISESWITCH	"noiseswitch"

#define MAINPANEL_SENDVOL	"sendvol"
#define MAINPANEL_NOISEVOL	"noisevol"
#define MAINPANEL_SENDSPEED	"sendspeed"
#define MAINPANEL_SPECBRIGHTNESS	"specbrightness"

#define SECTION_KOCH		"KOCH"
#define KOCH_CHARS			"charlist"
#define KOCH_WORDLEN		"koch_wordlen"

#define SECTION_HWCTRL		"HWCTRL"
#define HWCTRL_ENABLEEXTPORT	"enableextport"
#define HWCTRL_EXTPORTADDR	"extportaddr"
#define HWCTRL_OPENBYTE		"openbyte"
#define HWCTRL_CLOSEBYTE	"closebyte"

#define SECTION_NETWORK		"NETWORK"
#define NETWORK_NWENABLED	"nwenabled"
#define NETWORK_LOCALPORT	"localport"
#define NETWORK_LOCALPORT_DEF	3010
//主机数
#define NETWORK_HOSTS		"hosts"
//主机信息属性名前缀(host1, host2, host3, ...)
#define NETWORK_HOSTENTRYEXPR	"host%d"

BOOL CLakeySetting::Load()
{
	char vBuff[64];

	cust.m_rBeepFreq = GetProfileDouble(SECTION_CW, CW_BEEPFREQ, cust.m_rBeepFreq);
	cust.m_rBeepVol = GetProfileDouble(SECTION_CW, CW_BEEPVOL, cust.m_rBeepVol);
	cust.m_nShortHit = GetProfileDec(SECTION_CW, CW_SHORTHIT, cust.m_nShortHit);
	cust.m_nLongHit = GetProfileDec(SECTION_CW, CW_LONGHIT, cust.m_nLongHit);
	cust.m_nHitDelay = GetProfileDec(SECTION_CW, CW_HITDELAY, cust.m_nHitDelay);
	cust.m_nLetterDelay = GetProfileDec(SECTION_CW, CW_LETTERDELAY, cust.m_nLetterDelay);
	cust.m_nWordDelay = GetProfileDec(SECTION_CW, CW_WORDDELAY, cust.m_nWordDelay);

	GetProfileRect(SECTION_GLOBAL, GLOBAL_WINDOWRECT, &cust.m_oWindowRect);

	cust.m_nSendJournalPeriod = GetProfileDec(SECTION_JOURNAL, JOURNAL_SENDPERIOD, cust.m_nSendJournalPeriod);
	cust.m_nSendIdleLimit = GetProfileDec(SECTION_JOURNAL, JOURNAL_SENDIDLELIMIT, cust.m_nSendIdleLimit);
	GetProfileRect(SECTION_JOURNAL, JOURNAL_SENDRECT, &cust.m_oSendJournalRect);
	GetProfileRect(SECTION_JOURNAL, JOURNAL_SENDQUEUERECT, &cust.m_oSendCharQueueRect);
	GetProfileSize(SECTION_JOURNAL, JOURNAL_SENDLABELOFFSET, &cust.m_oSendLabelOffset);
	GetPrivateProfileString(SECTION_JOURNAL, JOURNAL_SENDLABEL, cust.m_vSendLabel, cust.m_vSendLabel, sizeof(cust.m_vSendLabel), PROFILE_PATHNAME);

	GetProfileRect(SECTION_JOURNAL, JOURNAL_SPECRECT, &cust.m_oSpectrogramRect);
	GetProfileSize(SECTION_JOURNAL, JOURNAL_SPECLABELOFFSET, &cust.m_oSpecLabelOffset);
	GetPrivateProfileString(SECTION_JOURNAL, JOURNAL_SPECLABEL, cust.m_vSpecLabel, cust.m_vSpecLabel, sizeof(cust.m_vSpecLabel), PROFILE_PATHNAME);

	cust.m_nRecvJournalPeriod = GetProfileDec(SECTION_JOURNAL, JOURNAL_RECVPERIOD, cust.m_nRecvJournalPeriod);
	cust.m_nRecvIdleLimit = GetProfileDec(SECTION_JOURNAL, JOURNAL_RECVIDLELIMIT, cust.m_nRecvIdleLimit);
	GetProfileRect(SECTION_JOURNAL, JOURNAL_RECVRECT, &cust.m_oRecvJournalRect);
	GetProfileSize(SECTION_JOURNAL, JOURNAL_RECVLABELOFFSET, &cust.m_oRecvLabelOffset);
	GetPrivateProfileString(SECTION_JOURNAL, JOURNAL_RECVLABEL, cust.m_vRecvLabel, cust.m_vRecvLabel, sizeof(cust.m_vRecvLabel), PROFILE_PATHNAME);
	GetProfileRect(SECTION_JOURNAL, JOURNAL_RECVMRECT, &cust.m_oRecvMonitorRect);
	cust.m_nRecvAnalyzeSamples = GetProfileDec(SECTION_JOURNAL, JOURNAL_RECVANALYZESAMPLE, cust.m_nRecvAnalyzeSamples);
	cust.m_rRecvThreshold = GetProfileDouble(SECTION_JOURNAL, JOURNAL_RECVTHRESHOLD, cust.m_rRecvThreshold);
	cust.m_nRecvFreqStart = GetProfileDec(SECTION_JOURNAL, JOURNAL_RECVFREQSTART, cust.m_nRecvFreqStart);
	cust.m_nRecvFreqEnd = GetProfileDec(SECTION_JOURNAL, JOURNAL_RECVFREQEND, cust.m_nRecvFreqEnd);

	GetButtonMapping(SECTION_MAINPANEL, MAINPANEL_CW, &cust.m_oCwKeyButton);
	GetButtonMapping(SECTION_MAINPANEL, MAINPANEL_SENDPAUSE, &cust.m_oSendPauseButton);
	GetButtonMapping(SECTION_MAINPANEL, MAINPANEL_SENDFILE, &cust.m_oSendFileButton);
	GetButtonMapping(SECTION_MAINPANEL, MAINPANEL_AUTOKEYSWITCH, &cust.m_oAutoKeySwitchButton);
	GetButtonMapping(SECTION_MAINPANEL, MAINPANEL_NOISESWITCH, &cust.m_oNoiseSwitchButton);

	GetTunerMapping(SECTION_MAINPANEL, MAINPANEL_SENDVOL, &cust.m_oSendVolTuner);
	GetTunerMapping(SECTION_MAINPANEL, MAINPANEL_NOISEVOL, &cust.m_oNoiseVolTuner);
	GetTunerMapping(SECTION_MAINPANEL, MAINPANEL_SENDSPEED, &cust.m_oSendSpeedTuner);
	GetTunerMapping(SECTION_MAINPANEL, MAINPANEL_SPECBRIGHTNESS, &cust.m_oSpecBrightnessTuner);

	GetPrivateProfileString(SECTION_KOCH, KOCH_CHARS, "", cust.m_vKochChar, sizeof(cust.m_vKochChar), PROFILE_PATHNAME);
	cust.m_nKochWordLen = GetProfileDec(SECTION_KOCH, KOCH_WORDLEN, cust.m_nKochWordLen);

	for (int i = 0; i < MORSECODECOUNT; ++i)
	{
		char vMorseKey[2] = {' ', '\0'};
		vMorseKey[0] = (char)cust.m_vMorseCode[i].nAscCode;
		GetPrivateProfileString(SECTION_MORSE, vMorseKey, "", vBuff, sizeof(vBuff), PROFILE_PATHNAME);
		if (strlen(vBuff))
			StrToMorseCode(i, vBuff);
	}

	cust.m_nExtPortEnable = GetProfileHex(SECTION_HWCTRL, HWCTRL_ENABLEEXTPORT, cust.m_nExtPortEnable);
	cust.m_nExtPortAddr = GetProfileHex(SECTION_HWCTRL, HWCTRL_EXTPORTADDR, cust.m_nExtPortAddr);
	cust.m_nOpenByte = GetProfileHex(SECTION_HWCTRL, HWCTRL_OPENBYTE, cust.m_nOpenByte);
	cust.m_nCloseByte = GetProfileHex(SECTION_HWCTRL, HWCTRL_CLOSEBYTE, cust.m_nCloseByte);

	cust.m_bNetworkEnabled = (0 == GetProfileDec(SECTION_NETWORK, NETWORK_NWENABLED, cust.m_bNetworkEnabled) ? FALSE : TRUE);
	cust.m_nLocalPort = GetProfileDec(SECTION_NETWORK, NETWORK_LOCALPORT, cust.m_nLocalPort);
	int nHosts = GetProfileDec(SECTION_NETWORK, NETWORK_HOSTS, 0);
//	if (HOSTS_MAX < cust.m_nHosts)
//		cust.m_nHosts = HOSTS_MAX;
	hosts.resize(nHosts);

	for (int i = 0; i < nHosts; ++i)
	{
		sprintf(vBuff, NETWORK_HOSTENTRYEXPR, i + 1);
		GetPrivateProfileString(SECTION_NETWORK, vBuff, "", vBuff, sizeof(vBuff), PROFILE_PATHNAME);
		char* pSpliter = strstr(vBuff, ":");
		*pSpliter = '\n';
		sscanf(vBuff, "%s%s"
			, hosts[i].szHostName
			, hosts[i].szPort);
	}

	return TRUE;
}

BOOL CLakeySetting::Save()
{
	WriteProfileDouble(SECTION_CW, CW_BEEPFREQ, cust.m_rBeepFreq);
	WriteProfileDouble(SECTION_CW, CW_BEEPVOL, cust.m_rBeepVol);
	WriteProfileDec(SECTION_CW, CW_SHORTHIT, cust.m_nShortHit);
	WriteProfileDec(SECTION_CW, CW_LONGHIT, cust.m_nLongHit);
	WriteProfileDec(SECTION_CW, CW_HITDELAY, cust.m_nHitDelay);
	WriteProfileDec(SECTION_CW, CW_LETTERDELAY, cust.m_nLetterDelay);
	WriteProfileDec(SECTION_CW, CW_WORDDELAY, cust.m_nWordDelay);

	WriteProfileRect(SECTION_GLOBAL, GLOBAL_WINDOWRECT, &cust.m_oWindowRect);

	WriteProfileDec(SECTION_JOURNAL, JOURNAL_SENDPERIOD, cust.m_nSendJournalPeriod);
	WriteProfileDec(SECTION_JOURNAL, JOURNAL_SENDIDLELIMIT, cust.m_nSendIdleLimit);
	WriteProfileRect(SECTION_JOURNAL, JOURNAL_SENDRECT, &cust.m_oSendJournalRect);
	WriteProfileSize(SECTION_JOURNAL, JOURNAL_SENDLABELOFFSET, &cust.m_oSendLabelOffset);
	WritePrivateProfileString(SECTION_JOURNAL, JOURNAL_SENDLABEL, cust.m_vSendLabel, PROFILE_PATHNAME);
	WriteProfileRect(SECTION_JOURNAL, JOURNAL_SENDQUEUERECT, &cust.m_oSendCharQueueRect);

	WriteProfileRect(SECTION_JOURNAL, JOURNAL_SPECRECT, &cust.m_oSpectrogramRect);
	WriteProfileSize(SECTION_JOURNAL, JOURNAL_SPECLABELOFFSET, &cust.m_oSpecLabelOffset);
	WritePrivateProfileString(SECTION_JOURNAL, JOURNAL_SPECLABEL, cust.m_vSpecLabel, PROFILE_PATHNAME);

	WriteProfileDec(SECTION_JOURNAL, JOURNAL_RECVPERIOD, cust.m_nRecvJournalPeriod);
	WriteProfileDec(SECTION_JOURNAL, JOURNAL_RECVIDLELIMIT, cust.m_nRecvIdleLimit);
	WriteProfileRect(SECTION_JOURNAL, JOURNAL_RECVRECT, &cust.m_oRecvJournalRect);
	WriteProfileSize(SECTION_JOURNAL, JOURNAL_RECVLABELOFFSET, &cust.m_oRecvLabelOffset);
	WritePrivateProfileString(SECTION_JOURNAL, JOURNAL_RECVLABEL, cust.m_vRecvLabel, PROFILE_PATHNAME);
	WriteProfileRect(SECTION_JOURNAL, JOURNAL_RECVMRECT, &cust.m_oRecvMonitorRect);
	WriteProfileDec(SECTION_JOURNAL, JOURNAL_RECVANALYZESAMPLE, cust.m_nRecvAnalyzeSamples);
	WriteProfileDouble(SECTION_JOURNAL, JOURNAL_RECVTHRESHOLD, cust.m_rRecvThreshold);
	WriteProfileDec(SECTION_JOURNAL, JOURNAL_RECVFREQSTART, cust.m_nRecvFreqStart);
	WriteProfileDec(SECTION_JOURNAL, JOURNAL_RECVFREQEND, cust.m_nRecvFreqEnd);

	WriteButtonMapping(SECTION_MAINPANEL, MAINPANEL_CW, &cust.m_oCwKeyButton);
	WriteButtonMapping(SECTION_MAINPANEL, MAINPANEL_SENDPAUSE, &cust.m_oSendPauseButton);
	WriteButtonMapping(SECTION_MAINPANEL, MAINPANEL_SENDFILE, &cust.m_oSendFileButton);
	WriteButtonMapping(SECTION_MAINPANEL, MAINPANEL_AUTOKEYSWITCH, &cust.m_oAutoKeySwitchButton);
	WriteButtonMapping(SECTION_MAINPANEL, MAINPANEL_NOISESWITCH, &cust.m_oNoiseSwitchButton);

	WriteTunerMapping(SECTION_MAINPANEL, MAINPANEL_SENDVOL, &cust.m_oSendVolTuner);
	WriteTunerMapping(SECTION_MAINPANEL, MAINPANEL_NOISEVOL, &cust.m_oNoiseVolTuner);
	WriteTunerMapping(SECTION_MAINPANEL, MAINPANEL_SENDSPEED, &cust.m_oSendSpeedTuner);
	WriteTunerMapping(SECTION_MAINPANEL, MAINPANEL_SPECBRIGHTNESS, &cust.m_oSpecBrightnessTuner);

	WritePrivateProfileString(SECTION_KOCH, KOCH_CHARS, cust.m_vKochChar, PROFILE_PATHNAME);
	WriteProfileDec(SECTION_KOCH, KOCH_WORDLEN, cust.m_nKochWordLen);

	for (int i = 0; i < MORSECODECOUNT; ++i)
	{
		char vMorseKey[2] = {' ', '\0'};
		char vBuff[64];
		vMorseKey[0] = (char)cust.m_vMorseCode[i].nAscCode;
		WritePrivateProfileString(SECTION_MORSE, vMorseKey, MorseCodeToStr(vBuff, i), PROFILE_PATHNAME);
	}

	
	WriteProfileHex(SECTION_HWCTRL, HWCTRL_ENABLEEXTPORT, cust.m_nExtPortEnable);
	WriteProfileHex(SECTION_HWCTRL, HWCTRL_EXTPORTADDR, cust.m_nExtPortAddr);
	WriteProfileHex(SECTION_HWCTRL, HWCTRL_OPENBYTE, cust.m_nOpenByte);
	WriteProfileHex(SECTION_HWCTRL, HWCTRL_CLOSEBYTE, cust.m_nCloseByte);

	WriteProfileDec(SECTION_NETWORK, NETWORK_NWENABLED, cust.m_bNetworkEnabled);
	WriteProfileDec(SECTION_NETWORK, NETWORK_LOCALPORT, cust.m_nLocalPort);
	WriteProfileDec(SECTION_NETWORK, NETWORK_HOSTS, (int)hosts.size());

	char vPropBuff[8];
	char vHostBuff[64];

	for (UINT i = 0; i < hosts.size(); ++i)
	{
		sprintf(vPropBuff, NETWORK_HOSTENTRYEXPR, i + 1);
		sprintf(vHostBuff, "%s:%s", hosts[i].szHostName, hosts[i].szPort);
		WritePrivateProfileString(SECTION_NETWORK, vPropBuff, vHostBuff, PROFILE_PATHNAME);
	}

	return TRUE;
}

int CLakeySetting::GetProfileHex(const char* pSec, const char* pKey, int nDefault)
{
	char vBuff[32];
	GetPrivateProfileString(pSec, pKey, "", vBuff, sizeof(vBuff), PROFILE_PATHNAME);
	if (strlen(vBuff))
		sscanf(vBuff, "%8X", &nDefault);

	return nDefault;
}

void CLakeySetting::WriteProfileHex(const char* pSec, const char* pKey, int n)
{
	char vBuff[32];
	sprintf(vBuff, "%8.8X", n);

	WritePrivateProfileString(pSec, pKey, vBuff, PROFILE_PATHNAME);
}

int CLakeySetting::GetProfileDec(const char* pSec, const char* pKey, int nDefault)
{
	char vBuff[32];
	GetPrivateProfileString(pSec, pKey, "", vBuff, sizeof(vBuff), PROFILE_PATHNAME);
	if (strlen(vBuff))
		sscanf(vBuff, "%d", &nDefault);

	return nDefault;
}

void CLakeySetting::WriteProfileDec(const char* pSec, const char* pKey, int n)
{
	char vBuff[32];
	sprintf(vBuff, "%d", n);

	WritePrivateProfileString(pSec, pKey, vBuff, PROFILE_PATHNAME);
}

double CLakeySetting::GetProfileDouble(const char* pSec, const char* pKey, double rDefault)
{
	char vBuff[32];
	GetPrivateProfileString(pSec, pKey, "", vBuff, sizeof(vBuff), PROFILE_PATHNAME);
	if (strlen(vBuff))
		sscanf(vBuff, "%lf", &rDefault);

	return rDefault;
}

void CLakeySetting::WriteProfileDouble(const char* pSec, const char* pKey, double r)
{
	char vBuff[32];
	sprintf(vBuff, "%lf", r);

	WritePrivateProfileString(pSec, pKey, vBuff, PROFILE_PATHNAME);
}

void CLakeySetting::GetButtonMapping(const char* pSec, const char* pKey, BUTTONMAPPING* pBtMap)
{
	char vBuff[64];
	GetPrivateProfileString(pSec, pKey, "", vBuff, sizeof(vBuff), PROFILE_PATHNAME);
	if (strlen(vBuff))
		StrToButtonMapping(pBtMap, vBuff);
}

void CLakeySetting::WriteButtonMapping(const char* pSec, const char* pKey, const BUTTONMAPPING* pBtMap)
{
	char vBuff[64];
	WritePrivateProfileString(pSec, pKey, ButtonMappingToStr(vBuff, pBtMap), PROFILE_PATHNAME);
}

void CLakeySetting::GetTunerMapping(const char* pSec, const char* pKey, TUNERMAPPING* pTnMap)
{
	char vBuff[64];
	GetPrivateProfileString(pSec, pKey, "", vBuff, sizeof(vBuff), PROFILE_PATHNAME);
	if (strlen(vBuff))
		StrToTunerMapping(pTnMap, vBuff);
}

void CLakeySetting::WriteTunerMapping(const char* pSec, const char* pKey, const TUNERMAPPING* pTnMap)
{
	char vBuff[64];
	WritePrivateProfileString(pSec, pKey, TunerMappingToStr(vBuff, pTnMap), PROFILE_PATHNAME);
}

void CLakeySetting::GetProfileRect(const char* pSec, const char* pKey, RECT* pRect)
{
	char vBuff[64];
	GetPrivateProfileString(pSec, pKey, "", vBuff, sizeof(vBuff), PROFILE_PATHNAME);
	if (strlen(vBuff))
		sscanf(vBuff, "%d,%d,%d,%d", &pRect->left, &pRect->top, &pRect->right, &pRect->bottom);
}

void CLakeySetting::WriteProfileRect(const char* pSec, const char* pKey, const RECT* pRect)
{
	char vBuff[64];
	sprintf(vBuff, "%d,%d,%d,%d", pRect->left, pRect->top, pRect->right, pRect->bottom);
	WritePrivateProfileString(pSec, pKey, vBuff, PROFILE_PATHNAME);
}

void CLakeySetting::GetProfileSize(const char* pSec, const char* pKey, SIZE* pSize)
{
	char vBuff[64];
	GetPrivateProfileString(pSec, pKey, "", vBuff, sizeof(vBuff), PROFILE_PATHNAME);
	if (strlen(vBuff))
		sscanf(vBuff, "%d,%d", &pSize->cx, &pSize->cy);
}

void CLakeySetting::WriteProfileSize(const char* pSec, const char* pKey, const SIZE* pSize)
{
	char vBuff[64];
	sprintf(vBuff, "%d,%d", pSize->cx, pSize->cy);
	WritePrivateProfileString(pSec, pKey, vBuff, PROFILE_PATHNAME);
}

void CLakeySetting::GetCustomize(CUSTOMIZE* pCust)
{
	memcpy(pCust, &cust, sizeof(cust));
	memcpy(pCust->m_vKochChar, cust.m_vKochChar, sizeof(cust.m_vKochChar));
}

void CLakeySetting::SetCustomize(const CUSTOMIZE* pCust)
{
	memcpy(&cust, pCust, sizeof(cust));
	memcpy(cust.m_vKochChar, pCust->m_vKochChar, sizeof(cust.m_vKochChar));
}

float calcStdWordLen(short int nMorseCode, short int nMask)
{
	int s = 2; // word space - hit space
	do
	{
		if (0x01 & nMorseCode)
		{
			// di
			s += 2;
		}
		else
		{
			// dah
			s += 4;
		}
	}
	while(0x01 & (nMask >>= 1));

	return (float)s / (float)STD_WORD_LEN;
}

void wpmToParams(WPMS* pWpms)
{
	// .--. .- .-. .. ...
	pWpms->di = (int)(60 * 1000 / (pWpms->wpm * STD_WORD_LEN));
	pWpms->dah = pWpms->di * 3;
	pWpms->hdelay = pWpms->di;
	pWpms->ldelay = pWpms->di * 3;
	pWpms->wdelay = pWpms->di * 3;
}

void paramsToWpm(WPMS* pWpms)
{
	short nCalc = 10 * pWpms->di
		+ 4 * pWpms->dah
		+ 9 * pWpms->hdelay
		+ 4 * pWpms->ldelay
		+ 1 * pWpms->wdelay;

	if (0 == nCalc)
		nCalc = 1;

	nCalc = (60 * 1000 + nCalc / 2) / nCalc;

	if (99 < nCalc)
		nCalc = 99;

	pWpms->wpm = nCalc;
}
