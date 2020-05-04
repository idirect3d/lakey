#include "StdAfx.h"
#include "stdio.h"
#include "math.h"
#include "assert.h"
#include "LaWaveFile.h"

#define RESOLVEDWORD(a, b, c, d) (((DWORD)(a)) | (((DWORD)(b)) << 8) | (((DWORD)(c)) << 16) | (((DWORD)(d)) << 24))
#define PI				3.1415926f

CWaveFile::CWaveFile(const char* pPathname, WORD nChannels, DWORD nSamplesPerSec, WORD  wBitsPerSample, float rSampleBuffSec)
{
	m_pDestFile = fopen(pPathname, "wb");
	memset(&m_oWaveFileInfo, 0, sizeof(m_oWaveFileInfo));
	m_oWaveFileInfo.nRiffFlag = RESOLVEDWORD('R', 'I', 'F', 'F');
	m_oWaveFileInfo.nCbSize = 
		sizeof(m_oWaveFileInfo) - sizeof(m_oWaveFileInfo.nRiffFlag) - sizeof(m_oWaveFileInfo.nCbSize);
	m_oWaveFileInfo.oWaveHeader.nWaveFlag = RESOLVEDWORD('W', 'A', 'V', 'E');
	m_oWaveFileInfo.oWaveHeader.nFmtFlag = RESOLVEDWORD('f', 'm', 't', ' ');
	m_oWaveFileInfo.oWaveHeader.nCbSize = sizeof(m_oWaveFileInfo.oWaveHeader.oWaveInfo);
	m_oWaveFileInfo.oWaveHeader.oWaveInfo.nBlank = 1;
	m_oWaveFileInfo.oWaveHeader.oWaveInfo.nChannels = nChannels;
	m_oWaveFileInfo.oWaveHeader.oWaveInfo.nSamplesPerSec = nSamplesPerSec;
	m_oWaveFileInfo.oWaveHeader.oWaveInfo.nAvgBytesPerSec = 
		nSamplesPerSec * nChannels * wBitsPerSample / 8;
	m_oWaveFileInfo.oWaveHeader.oWaveInfo.nBlockAlign = nChannels * wBitsPerSample / 8;
	m_oWaveFileInfo.oWaveHeader.oWaveInfo.wBitsPerSample = wBitsPerSample;
	m_oWaveFileInfo.oWaveHeader.nDataFlag = RESOLVEDWORD('d', 'a', 't', 'a');
	m_oWaveFileInfo.oWaveHeader.nDataLength = 0;

	fwrite(&m_oWaveFileInfo, sizeof(m_oWaveFileInfo), 1, m_pDestFile);

	m_rSampleBuffSec = rSampleBuffSec;

	m_nWavBuffLen = (int)(rSampleBuffSec * m_oWaveFileInfo.oWaveHeader.oWaveInfo.nAvgBytesPerSec);
	m_pWavBuff = new char[m_nWavBuffLen];
	m_bModified = TRUE;
}

CWaveFile::~CWaveFile()
{
	delete[] m_pWavBuff;
	fclose(m_pDestFile);
}

void CWaveFile::SetFreq(float rFreq)
{
	m_rFreq = rFreq;
	m_bModified = TRUE;
}

void CWaveFile::SetVolumn(float rVol)
{
	m_rVol = rVol;
	m_bModified = TRUE;
}

int CWaveFile::AppendRaw(const char* pBuff, DWORD nLen, int nCount)
{
	assert(0 == (nLen * nCount) % 2);
	DWORD nBlockSize = nLen * nCount;

	m_oWaveFileInfo.nCbSize += nBlockSize;
	m_oWaveFileInfo.oWaveHeader.nDataLength += nBlockSize;

	fseek(m_pDestFile, 0, SEEK_END);
	for (int i = 0; i < nCount; ++i)
		fwrite(pBuff, 1, nLen, m_pDestFile);

	fseek(m_pDestFile, 0, SEEK_SET);
	fwrite(&m_oWaveFileInfo, sizeof(m_oWaveFileInfo), 1, m_pDestFile);
	return nBlockSize;
}

int CWaveFile::AppendBlank(float rSec)
{
	DWORD nBytes = (DWORD)(rSec * m_oWaveFileInfo.oWaveHeader.oWaveInfo.nAvgBytesPerSec);
	if (0 != nBytes % 2)
		++nBytes;
	char c = '\0';
	return AppendRaw(&c, 1, nBytes);
}

int CWaveFile::Append(float rSec)
{
	if (m_bModified)
	{
		InitWavBuff();
	}
	
	DWORD nBytes = (DWORD)(rSec * m_oWaveFileInfo.oWaveHeader.oWaveInfo.nAvgBytesPerSec);
	if (0 != nBytes % 2)
		++nBytes;

	assert(nBytes <= m_nWavBuffLen);
	return AppendRaw(m_pWavBuff, nBytes, 1);
}

void CWaveFile::InitWavBuff()
{
	float dt = 1.0f / m_oWaveFileInfo.oWaveHeader.oWaveInfo.nSamplesPerSec;
	DWORD nBytesPerSample = m_oWaveFileInfo.oWaveHeader.oWaveInfo.wBitsPerSample / 8;

	for (DWORD i = 0; i < m_nWavBuffLen / nBytesPerSample; ++i)
	{
		float v = sin(2 * PI * dt * i * m_rFreq) * m_rVol;	// -1.0 ~ +1.0

		for (DWORD j = 0; j < m_oWaveFileInfo.oWaveHeader.oWaveInfo.nChannels; ++j)
		{
			if (16 == m_oWaveFileInfo.oWaveHeader.oWaveInfo.wBitsPerSample)
			{
				*(((short int *)m_pWavBuff) + i * m_oWaveFileInfo.oWaveHeader.oWaveInfo.nChannels + j) = (short int)(v * 32767);
			}
			else if (8 == m_oWaveFileInfo.oWaveHeader.oWaveInfo.wBitsPerSample)
			{
				*(((char *)m_pWavBuff) + i * m_oWaveFileInfo.oWaveHeader.oWaveInfo.nChannels + j) = (char)(v * 255);
			}
			else
			{
				assert(0);
			}
		}
	}

	m_bModified = FALSE;
}

