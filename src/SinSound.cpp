#include "StdAfx.h"
#include "dsound.h"
#include "math.h"
#include "sinsound.h"

static float DW = 0;

CSinSound::CSinSound()
	:m_pDs(NULL),
	m_pDsb(NULL),
	m_pBgDsb(NULL),
	m_bFormatValid(FALSE),
	m_bDsbValid(FALSE),
	m_bDataValid(FALSE),
	m_nLastEnd(0),
	m_nTimeAxis(0),
	m_bNoiseOn(FALSE)
{
	memset(&m_oPlayedBufferDesc, 0, sizeof(m_oPlayedBufferDesc)); 
	memset(&m_oBgBufferDesc, 0, sizeof(m_oBgBufferDesc));
	memset(&m_oFormat, 0, sizeof(m_oFormat)); 

	m_oFormat.wFormatTag = WAVE_FORMAT_PCM;
	m_oFormat.nChannels = 1;
	m_oFormat.nSamplesPerSec = 44100;
	m_oFormat.wBitsPerSample = 16;
	m_oFormat.nAvgBytesPerSec = m_oFormat.nSamplesPerSec * (m_oFormat.wBitsPerSample / 8) * m_oFormat.nChannels;
	m_oFormat.nBlockAlign = 2;
	m_oFormat.cbSize = 0;
	DW = 2 * PI / m_oFormat.nSamplesPerSec;

	SetFrequency(1000);
	SetScale(1.0l);
}

CSinSound::~CSinSound(void)
{
	if (NULL != m_pDsb)
	{
		m_pDsb->Release();
		m_pDsb = NULL;
	}
	if (NULL != m_pBgDsb)
	{
		m_pBgDsb->Release();
		m_pBgDsb = NULL;
	}
	if (NULL != m_pDs)
	{
		m_pDs->Release();
		m_pDs = NULL;
	}
}

BOOL CSinSound::Initialize(HWND hWnd)
{
	HRESULT hr =
		DirectSoundCreate(NULL, &m_pDs, NULL);
	if (FAILED(hr))
		return FALSE;

	hr = m_pDs->SetCooperativeLevel(hWnd, DSSCL_PRIORITY);
	if (FAILED(hr))
		return FALSE;

	return InitBufferFormat();
}

BOOL CSinSound::OnTimer(void* owner, int nTimerId)
{
	FillBackground();
	return TRUE;
}

void CSinSound::FillBackground()
{
	if (m_bFormatValid)
	{
		DWORD dwOffset;

		if (SUCCEEDED(m_pBgDsb->GetCurrentPosition(&dwOffset, NULL)))
		{
			DWORD nSafeLength = m_oFormat.nAvgBytesPerSec * 1;
			DWORD nSoundBytes = 
				dwOffset <= m_nLastEnd 
				? nSafeLength - (m_nLastEnd - dwOffset + 2)
				: nSafeLength - (m_oBgBufferDesc.dwBufferBytes - dwOffset + m_nLastEnd - 2);
			if (nSoundBytes > nSafeLength)
				nSoundBytes = nSafeLength;

			LPVOID  lpvPtr1, lpvPtr2; 
			DWORD dwBytes1, dwBytes2; 

			HRESULT hr = m_pBgDsb->Lock(m_nLastEnd, nSoundBytes, &lpvPtr1, &dwBytes1, &lpvPtr2, &dwBytes2, 0); 

			// If the buffer was lost, restore and retry lock. 

			if (DSERR_BUFFERLOST == hr) 
			{ 
				m_pBgDsb->Restore(); 
				hr = m_pBgDsb->Lock(m_nLastEnd, nSoundBytes, &lpvPtr1, &dwBytes1, &lpvPtr2, &dwBytes2, 0);  
			}

			if (FAILED(hr))
				return;
			
			if (m_bNoiseOn)
			{
				FillNoises(lpvPtr1, dwBytes1);

				if (lpvPtr2)
					FillNoises(lpvPtr2, dwBytes2);
			}
			else
			{
				FillBlank(lpvPtr1, dwBytes1);

				if (lpvPtr2)
					FillBlank(lpvPtr2, dwBytes2);
			}

			m_pBgDsb->Unlock(lpvPtr1, dwBytes1, lpvPtr2, dwBytes2);

			m_nLastEnd += nSoundBytes;
			if (m_nLastEnd >= m_oBgBufferDesc.dwBufferBytes)
				m_nLastEnd -= m_oBgBufferDesc.dwBufferBytes;
		}
	}
}

inline void CSinSound::FillBlank(LPVOID lpvPtr, DWORD nBytes)
{
	DWORD dw = nBytes / 2;
	for (DWORD i = 0; i < dw; ++i)
	{
		*(((short int *)lpvPtr) + i) = 0;
	}
}

#define NOISE_FREQ_STEP		0.001f
inline void CSinSound::FillNoises(LPVOID lpvPtr, DWORD nBytes)
{
	DWORD dw = nBytes / 2;
	static REAL f = 0;
	static REAL df = NOISE_FREQ_STEP;
	static REAL w = 0;
	static int NC = 0;
	static BOOL bNC = TRUE;
	REAL wnl = m_rNoiseScale / 2;
	REAL nl = m_rNoiseScale * 32767;
	REAL v = 0;
	REAL t;
	int rnd;
	static REAL ft = m_rFrequencyHz / 2;

	for (DWORD i = 0; i < dw; ++i)
	{
		++m_nTimeAxis;
		rnd = rand();
		if ((f += df) >= ft && df > 0)
		{
			f = ft;
			ft = (REAL)(abs(rnd) % (int)m_rFrequencyHz * 2);
			if (ft < f)
				df = -NOISE_FREQ_STEP;
		}
		else if (f <= ft && df < 0)
		{
			f = ft;
			ft = (REAL)(abs(rnd) % (int)m_rFrequencyHz * 2);
			if (ft > f)
				df = NOISE_FREQ_STEP;
		}

		v = (0xffff & rnd) * wnl;
		w += (DW * f);
		if (w > PI2S)
			w -= PI2S;

		t = ((m_rFrequencyHz * 1.5f - abs(f - m_rFrequencyHz / 2.0f)) / (m_rFrequencyHz * 1.5f));
		v += sin(w) * nl * t * t * t;

		if (bNC)
		{
			if (0 >= --NC)
			{
				bNC = !bNC;
				NC = rnd * 25;
			}

			t = sin(DW * 50 * m_nTimeAxis) * 10000;
			v += (abs(t) > nl / 2 ? nl / 2 : t);
		}
		else
		{
			if (0 >= --NC)
			{
				bNC = !bNC;
				NC = rnd * 20;
			}
		}

		*(((short int *)lpvPtr) + i) = (short int)v; //(32767 < v ? 32767 : (-32767 > v ? -32767 : v));
	}
}

BOOL CSinSound::InitBufferFormat()
{
	m_oBgBufferDesc.dwSize = sizeof(DSBUFFERDESC);
	m_oBgBufferDesc.dwFlags = DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME 
		| DSBCAPS_CTRLFREQUENCY | DSBCAPS_GLOBALFOCUS;
	//DSBCAPS_STATIC | DSBCAPS_GLOBALFOCUS;
	m_oBgBufferDesc.dwBufferBytes = 2 * m_oFormat.nAvgBytesPerSec;
	m_oBgBufferDesc.lpwfxFormat = &m_oFormat;

	HRESULT hr = m_pDs->CreateSoundBuffer(&m_oBgBufferDesc, &m_pBgDsb, NULL);
	if (m_bFormatValid = SUCCEEDED(hr))
	{
		PlayBackground();
		return InitPlayedBuffer();
	}

	return FALSE;
}

BOOL CSinSound::InitPlayedBuffer()
{
	HRESULT hr; 

	m_oPlayedBufferDesc.dwSize = sizeof(DSBUFFERDESC); 
	m_oPlayedBufferDesc.dwFlags = DSBCAPS_STATIC | DSBCAPS_GLOBALFOCUS | DSBCAPS_CTRLVOLUME; 
	m_oPlayedBufferDesc.dwBufferBytes = (DWORD)(m_rBufferLengthMs * m_oFormat.nAvgBytesPerSec / 1000); 
	m_oPlayedBufferDesc.lpwfxFormat = &m_oFormat;
	
	hr = m_pDs->CreateSoundBuffer(&m_oPlayedBufferDesc, &m_pDsb, NULL);
	if (m_bDsbValid = SUCCEEDED(hr))
		return WriteBuffer();

	return FALSE;
}

BOOL CSinSound::WriteBuffer()
{
	LPVOID  lpvPtr1; 
	DWORD dwBytes1; 

	HRESULT hr = m_pDsb->Lock(0, m_oPlayedBufferDesc.dwBufferBytes, &lpvPtr1, &dwBytes1, NULL, NULL, 0); 

	// If the buffer was lost, restore and retry lock. 

	if (DSERR_BUFFERLOST == hr) 
	{ 
		m_pDsb->Restore(); 
		hr = m_pDsb->Lock(0, m_oPlayedBufferDesc.dwBufferBytes, &lpvPtr1, &dwBytes1, NULL, NULL, DSBLOCK_ENTIREBUFFER); 
	}

	if (FAILED(hr))
		return (m_bDataValid = FALSE);
	
	DWORD nBytesPerSample = m_oFormat.wBitsPerSample / 8;

	for (DWORD i = 0; i < dwBytes1 / nBytesPerSample; ++i)
	{
		REAL v = sin(DW * i * m_rFrequencyHz) * m_rScale;	// -1.0 ~ +1.0

		for (DWORD j = 0; j < m_oFormat.nChannels; ++j)
		{
			if (16 == m_oFormat.wBitsPerSample)
			{
				*(((short int *)lpvPtr1) + i * m_oFormat.nChannels + j) = (short int)(v * 32767);
			}
			else if (8 == m_oFormat.wBitsPerSample)
			{
				*(((char *)lpvPtr1) + i * m_oFormat.nChannels + j) = (char)(v * 255);
			}
			else
			{
				return (m_bDataValid = FALSE);
			}
		}
	}

	return (m_bDataValid = SUCCEEDED(m_pDsb->Unlock(lpvPtr1, dwBytes1, NULL, NULL)));
}

void CSinSound::Play()
{
	m_pDsb->SetCurrentPosition(0);
	HRESULT hr = m_pDsb->Play(0, 0, DSBPLAY_LOOPING);
	if (DSERR_BUFFERLOST == hr)
	{
		m_pDsb->Restore();
		WriteBuffer();
		m_pDsb->Play(0, 0, DSBPLAY_LOOPING);
	}
}

void CSinSound::Stop()
{
	m_pDsb->Stop();
}

void CSinSound::PlayBackground()
{
	HRESULT hr = m_pBgDsb->Play(0, 0, DSBPLAY_LOOPING);
	if (DSERR_BUFFERLOST == hr)
	{
		m_pBgDsb->Restore();
		m_pBgDsb->Play(0, 0, DSBPLAY_LOOPING);
	}
}

void CSinSound::StopBackground()
{
	m_pBgDsb->Stop();
}

void CSinSound::SetSampleRate(DWORD nSamplesPerSec)
{
	if (m_oFormat.nSamplesPerSec != nSamplesPerSec)
	{
		m_oFormat.nSamplesPerSec = nSamplesPerSec;
		m_oFormat.nAvgBytesPerSec = m_oFormat.nSamplesPerSec * (m_oFormat.wBitsPerSample / 8) * m_oFormat.nChannels;
		DW = 2 * PI / nSamplesPerSec;
		m_bFormatValid = FALSE;
	}
}

void CSinSound::SetBitsPerSample(WORD nBits)
{
	if (m_oFormat.wBitsPerSample != nBits)
	{
		m_oFormat.wBitsPerSample = nBits;
		m_oFormat.nAvgBytesPerSec = m_oFormat.nSamplesPerSec * (m_oFormat.wBitsPerSample / 8) * m_oFormat.nChannels;
		m_bFormatValid = FALSE;
	}
}

void CSinSound::SetBufferLength(REAL rMSec)
{
	if (m_rBufferLengthMs != rMSec)
	{
		m_rBufferLengthMs = rMSec;
		m_bDsbValid = FALSE;
	}
}

void CSinSound::SetFrequency(REAL rFrequency)
{
	if (m_rFrequencyHz != rFrequency)
	{
		REAL w = 1.0f / (REAL)rFrequency;
		REAL d = 1.0f / (REAL)m_oFormat.nSamplesPerSec;
		int nTimes;
		for (nTimes = 100; nTimes <= 1000; ++nTimes)
		{
			REAL rRemScale = fmod(w * nTimes, d) / w;

			if (rRemScale < 0.001l || rRemScale > 0.998l)
				break;
		}

		m_rFrequencyHz = rFrequency;
		m_rBufferLengthMs = nTimes * w * 1000.0f;
		m_bDsbValid = FALSE;
	}
}

void CSinSound::SetScale(REAL rScale)
{
	if (m_rScale != rScale)
	{
		m_rScale = rScale;
		m_rNoiseScale = rScale * 0.25f;
		m_bDataValid = FALSE;
	}
}

BOOL CSinSound::Validate()
{
	if (!m_bFormatValid)
	{
		m_pDsb->Release(); m_pDsb = NULL;
		m_pBgDsb->Release(); m_pBgDsb = NULL;
		return InitBufferFormat();
	}
	else if (!m_bDsbValid)
	{
		m_pDsb->Release(); m_pDsb = NULL;
		return InitPlayedBuffer();
	}
	else if (!m_bDataValid)
	{
		return WriteBuffer();
	}

	return true;
}

void CSinSound::SetVolume(LONG nAtte)
{
	HRESULT h = m_pDsb->SetVolume(nAtte);
	//h = m_pBgDsb->SetVolume(nAtte);
}

LONG CSinSound::GetVolume()
{
	LONG r = -1;
	m_pDsb->GetVolume(&r);
	return r;
}

void CSinSound::SetNoiseVolume(LONG nAtte)
{
	m_pBgDsb->SetVolume(nAtte);
}

LONG CSinSound::GetNoiseVolume()
{
	LONG r = -1;
	m_pBgDsb->GetVolume(&r);
	return r;
}

void CSinSound::SetNoise(BOOL bOn)
{
	m_bNoiseOn = bOn;
}
