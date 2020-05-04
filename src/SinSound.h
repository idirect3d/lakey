#pragma once

#include "EventManagerWin32.h"

#define PI				3.1415926f
#define PI2S			2 * PI
typedef float			REAL;

class CSinSound : public ITimerEventControl
{
public:
	CSinSound();
	~CSinSound();

	BOOL Initialize(HWND hWnd);

	void SetSampleRate(DWORD nSamplesPerSec);
	DWORD GetSampleRate() { return m_oFormat.nSamplesPerSec; };

	void SetBitsPerSample(WORD nBits);
	WORD GetBitsPerSample() { return m_oFormat.wBitsPerSample; };

	void SetBufferLength(REAL rMSec);
	REAL GetBufferLength() { return m_rBufferLengthMs; };

	void SetFrequency(REAL rFrequency);
	REAL GetFrequency() { return m_rFrequencyHz; };

	void SetScale(REAL rScale);
	REAL GetScale() { return m_rScale; };

	void SetVolume(LONG rVol);
	LONG GetVolume();

	void SetNoiseVolume(LONG rVol);
	LONG GetNoiseVolume();

	BOOL Validate();
	void Play();
	void Stop();

	void PlayBackground();
	void StopBackground();

	void SetNoise(BOOL bOn);
	BOOL GetNoise() { return m_bNoiseOn; };

	virtual BOOL OnTimer(void* owner, int nTimerId);

private:

	BOOL InitBufferFormat();
	BOOL InitPlayedBuffer();
	BOOL WriteBuffer();
	BOOL InitNoiseBuffer();
	void FillBackground();
	void FillNoises(LPVOID lpvPtr, DWORD nBytes);
	void FillBlank(LPVOID lpvPtr, DWORD nBytes);

	LPDIRECTSOUND		m_pDs;
	LPDIRECTSOUNDBUFFER m_pBgDsb;
	LPDIRECTSOUNDBUFFER	m_pDsb;

	WAVEFORMATEX		m_oFormat;
	DSBUFFERDESC		m_oPlayedBufferDesc;
	DSBUFFERDESC		m_oBgBufferDesc;

	DWORD				m_nLastEnd;
	DWORD				m_nTimeAxis;

	REAL				m_rBufferLengthMs;
	REAL				m_rFrequencyHz;
	REAL				m_rScale;
	REAL				m_rNoiseScale;

	BOOL				m_bFormatValid;
	BOOL				m_bDsbValid;
	BOOL				m_bDataValid;

	BOOL				m_bNoiseOn;
};
