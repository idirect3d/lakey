#pragma once

#include <math.h>

#ifdef PI
#undef PI
#endif

#define PI		3.1415926f

/**
 * class T: unit of external stored
 * class V: unit of internal handled
 */

template <class T, class V> class IFilterDFT
{
public:
	virtual T OnEncodeFilter(V val, int i) = 0;
	virtual T OnDecodeFilter(V val, int i) = 0;
};

template <class T, class V> class TCosDFT
{
public:
	TCosDFT(int nAnalyzeSamples, IFilterDFT<T, V>* pFilter = NULL);
	~TCosDFT(void);
	void Encode(T* pDest, const T* pSrc);
	void Decode(T* pDest, const T* pSrc);

private:
	int			m_nAnalyzeSamples;
	V**	m_ppCosTab;
	IFilterDFT<T, V>* m_pFilter;
};

template <class T, class V> class TSinDFT
{
public:
	TSinDFT(int nAnalyzeSamples, IFilterDFT<T, V>* pFilter = NULL);
	~TSinDFT(void);
	void Encode(T* pDest, const T* pSrc);
	void Decode(T* pDest, const T* pSrc);

private:
	int			m_nAnalyzeSamples;
	V**	m_ppSinTab;
	IFilterDFT<T, V>* m_pFilter;
};

template <class T, class V> class TFastFT
{
public:
	TFastFT(int nAnalyzeSamples, IFilterDFT<T, V>* pFilter = NULL);
	~TFastFT(void);
	void Encode(T* pDest, const T* pSrc);
	void Decode(T* pDest, const T* pSrc);

private:
	void		fftCore();
	int			m_nAnalyzeSamples;
	int			m_nLog2;
	V*			m_vBuf[2];

	IFilterDFT<T, V>* m_pFilter;
};

template <class T, class V> 
TFastFT<T, V>::TFastFT(int nAnalyzeSamples, IFilterDFT<T, V>* pFilter/* = NULL*/)
{
	m_nAnalyzeSamples = nAnalyzeSamples;
	m_pFilter = pFilter;
	m_vBuf[0] = new V[m_nAnalyzeSamples];	// real
	m_vBuf[1] = new V[m_nAnalyzeSamples];	// imod

	for (m_nLog2 = 0; nAnalyzeSamples > 1; ++m_nLog2)
	{
		nAnalyzeSamples >>= 1;
	}
}

template <class T, class V> 
TFastFT<T, V>::~TFastFT(void)
{
	delete[] m_vBuf[0];
	delete[] m_vBuf[1];
}

template <class T, class V> 
TCosDFT<T, V>::TCosDFT(int nAnalyzeSamples, IFilterDFT<T, V>* pFilter/* = NULL*/)
{
	m_nAnalyzeSamples = nAnalyzeSamples;
	m_ppCosTab = new V*[m_nAnalyzeSamples];
	m_pFilter = pFilter;

	for (int i = 0; i < m_nAnalyzeSamples; ++i)
	{
		m_ppCosTab[i] = new V[m_nAnalyzeSamples];

		for (int x = 0; x < m_nAnalyzeSamples; ++x)
			m_ppCosTab[i][x] = cos(((x * 2) + 1) * i * PI / (2 * m_nAnalyzeSamples));
	}
}

template <class T, class V> 
TCosDFT<T, V>::~TCosDFT(void)
{
	for (int i = 0; i < m_nAnalyzeSamples; ++i)
		delete[] m_ppCosTab[i];

	delete[] m_ppCosTab;
}

template <class T, class V> 
TSinDFT<T, V>::TSinDFT(int nAnalyzeSamples, IFilterDFT<T, V>* pFilter/* = NULL*/)
{
	m_nAnalyzeSamples = nAnalyzeSamples;
	m_ppSinTab = new V*[m_nAnalyzeSamples];
	m_pFilter = pFilter;

	for (int i = 0; i < m_nAnalyzeSamples; ++i)
	{
		m_ppSinTab[i] = new V[m_nAnalyzeSamples];

		for (int x = 0; x < m_nAnalyzeSamples; ++x)
			m_ppSinTab[i][x] = sin(((x * 2) + 1) * i * PI / (2 * m_nAnalyzeSamples));
	}
}

template <class T, class V> 
TSinDFT<T, V>::~TSinDFT(void)
{
	for (int i = 0; i < m_nAnalyzeSamples; ++i)
		delete[] m_ppSinTab[i];

	delete[] m_ppSinTab;
}

template <class T, class V> 
void TSinDFT<T, V>::Encode(T* pDest, const T* pSrc)
{
	for (int i = 0; i < m_nAnalyzeSamples; ++i)
	{
		V c = 0;
		for (int x = 0; x < m_nAnalyzeSamples; ++x)
		{
			c += pSrc[x] * m_ppSinTab[i][x];
		}
		pDest[i] = m_pFilter ? m_pFilter->OnEncodeFilter(2 * c / m_nAnalyzeSamples, i) 
			: (T)(2 * c / m_nAnalyzeSamples);
	}
}

template <class T, class V> 
void TCosDFT<T, V>::Encode(T* pDest, const T* pSrc)
{
	V s = 0;
	static int ddd = 100;
	for (int i = 0; i < m_nAnalyzeSamples; ++i)
	{
		V c = 0;
		for (int x = 0; x < m_nAnalyzeSamples; ++x)
		{
			c += pSrc[x] * m_ppCosTab[i][x];
		}
		pDest[i] = m_pFilter ? m_pFilter->OnEncodeFilter(2 * c / m_nAnalyzeSamples, i) 
			: (T)(2 * c / m_nAnalyzeSamples);

		s += abs(pDest[i]);
	}

	if (!(0 != ddd || 1.0 < s))
	{
		ddd = 0;
	}

	if (0 < ddd)
		--ddd;

}

template <class T, class V> 
void TSinDFT<T, V>::Decode(T* pDest, const T* pSrc)
{
	for (int i = 0; i < m_nAnalyzeSamples; ++i)
	{
		V c = 0;
		for (int x = 0; x < m_nAnalyzeSamples; ++x)
		{
			c += pSrc[x] * m_ppSinTab[i][x];
		}
		pDest[i] = m_pFilter ? m_pFilter->OnDecodeFilter(c, i) 
			: (T)c;
	}
}

template <class T, class V> 
void TCosDFT<T, V>::Decode(T* pDest, const T* pSrc)
{
	for (int i = 0; i < m_nAnalyzeSamples; ++i)
	{
		V c = 0;
		for (int x = 0; x < m_nAnalyzeSamples; ++x)
		{
			c += pSrc[x] * m_ppCosTab[i][x];
		}
		pDest[i] = m_pFilter ? m_pFilter->OnDecodeFilter(c, i) 
			: (T)c;
	}
}

template <class T, class V> 
void TFastFT<T, V>::Encode(T* pDest, const T* pSrc)
{
	for (int i = 0; i < m_nAnalyzeSamples; ++i)
	{
		m_vBuf[0][i] = (V)pSrc[i];
		m_vBuf[1][i] = (V)0;
	}

	fftCore();

	int n = m_nAnalyzeSamples / 2;
	V x, y;

	// 不知道为啥，FFT变换后的序列分辨率降低一半，数据以中心为对称，整齐分布
	// 因此原来的频谱(LaWaveCapture)显示，只要取一半即可
	if (m_pFilter)
	{
		for (int i = 0; i < n; ++i)
		{
			x = m_vBuf[0][i];
			y = m_vBuf[1][i];
			//pDest[i] = (T)m_vBuf[0][i];
			pDest[i * 2] = m_pFilter->OnEncodeFilter(sqrt(x * x + y * y), i * 2);

			x = m_vBuf[0][m_nAnalyzeSamples - i - 1];
			y = m_vBuf[1][m_nAnalyzeSamples - i - 1];
			pDest[i * 2 + 1] = m_pFilter->OnEncodeFilter(sqrt(x * x + y * y), i * 2 + 1);
		}
	}
	else
	{
		for (int i = 0; i < m_nAnalyzeSamples; ++i)
		{
			x = m_vBuf[0][i];
			y = m_vBuf[1][i];
			//pDest[i] = (T)m_vBuf[0][i];
			pDest[i * 2] = sqrt(x * x + y * y);

			x = m_vBuf[0][m_nAnalyzeSamples - i - 1];
			y = m_vBuf[1][m_nAnalyzeSamples - i - 1];
			pDest[i * 2 + 1] = sqrt(x * x + y * y);
		}
	}
}

template <class T, class V> 
void TFastFT<T, V>::Decode(T* pDest, const T* pSrc)
{
	for (int i = 0; i < m_nAnalyzeSamples; ++i)
	{
		m_vBuf[0][i] = (V)pSrc[i];
		m_vBuf[1][i] = (V)0;
	}

	fftCore();

	for (int i = 0; i < m_nAnalyzeSamples; ++i)
	{
		//pDest[i] = (T)(m_vBuf[0][i] / m_nAnalyzeSamples);
		pDest[i] = m_pFilter ? m_pFilter->OnDecodeFilter((T)m_vBuf[0][i], i) 
			: (T)m_vBuf[0][i];
	}
}

template <class T, class V> 
void TFastFT<T, V>::fftCore()
{
	int np, lmx, lix, nv2, npm1, lo, lm, li, j1, j2, i, j, k, l;
	V scl, arg, s, c, t1, t2;

	np = m_nAnalyzeSamples;
	l = m_nLog2;
	
	lmx = np;
	scl = 2 * PI / np;
	for (lo = 0; lo < l; ++lo)
	{
		lix = lmx;
		lmx = lmx / 2;
		arg = 0;
		for (lm = 0; lm < lmx; ++lm)
		{
			c = cos(arg);
			s = sin(arg);
			arg = arg + scl;
			for (li = lix; li <= np; li += lix)
			{
				j1 = li - lix + lm;
				j2 = j1 + lmx;
				t1 = m_vBuf[0][j1] - m_vBuf[0][j2];
				t2 = m_vBuf[1][j1] - m_vBuf[1][j2];
				m_vBuf[0][j1] += m_vBuf[0][j2];
				m_vBuf[1][j1] += m_vBuf[1][j2];
				m_vBuf[0][j2] = c * t1 + s * t2;
				m_vBuf[1][j2] = c * t2 - s * t1;
			} // for li
		} // for lm

		scl *= 2;
	} // for lo

	j = 0;
	nv2 = np / 2;
	npm1 = np - 1;
	for (i = 0; i < npm1; ++i)
	{
		if (i < j)
		{
			t1 = m_vBuf[0][j];
			t2 = m_vBuf[1][j];
			m_vBuf[0][j] = m_vBuf[0][i];
			m_vBuf[1][j] = m_vBuf[1][i];
			m_vBuf[0][i] = t1;
			m_vBuf[1][i] = t2;
		}

		k = nv2;
		while(k - 1 < j)
		{
			j -= k;
			k /= 2;
		}
		j += k;
	}
}