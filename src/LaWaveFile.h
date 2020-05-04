
typedef struct
{
	DWORD	nRiffFlag;		//'RIFF'
	DWORD	nCbSize;
	struct
	{
		DWORD nWaveFlag;	//'WAVE'
		DWORD nFmtFlag;		//'fmt '
		DWORD nCbSize;
		struct
		{
			WORD nBlank;
			WORD nChannels;
			DWORD nSamplesPerSec; 
			DWORD nAvgBytesPerSec; 
			WORD  nBlockAlign; 
			WORD  wBitsPerSample;
		} oWaveInfo;
		DWORD nDataFlag;	//'data'
		DWORD nDataLength;
	} oWaveHeader;
} LAAUDIOFILEHEADER;

class CWaveFile
{
public:
	CWaveFile(const char* pPathname, WORD nChannels, DWORD nSamplesPerSec, WORD  wBitsPerSample, float rSampleBuffSec);
	~CWaveFile();
	int AppendRaw(const char* pBuff, DWORD nLen, int nCount);
	void SetFreq(float rFreq);
	void SetVolumn(float rVol);
	int Append(float rSec);
	int AppendBlank(float rSec);

private:
	void InitWavBuff();
	FILE* m_pDestFile;
	LAAUDIOFILEHEADER m_oWaveFileInfo;
	char*	m_pWavBuff;
	DWORD	m_nWavBuffLen;
	BOOL	m_bModified;

	float	m_rSampleBuffSec;
	float	m_rFreq;
	float	m_rVol;
};
