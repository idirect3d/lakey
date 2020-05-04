#include "StdAfx.h"
#include "morseparser.h"

CMorseParser::CMorseParser(const MORSECODE* pMorseList, int nCount, int nMaxShortCount, IParseEventListener* pListener)
{
	m_pMorseCodeTree = new MORSECODETREENODE;
	m_pMorseCodeTree->pMorseCode = NULL;
	m_pMorseCodeTree->pDi = m_pMorseCodeTree->pDa = NULL;

	for (int i = 0; i < nCount; ++i)
	{
		AddNode(m_pMorseCodeTree, pMorseList + i, ((pMorseList + i)->nMask + 1) >> 1);
	}

	m_pCurrNode = m_pMorseCodeTree;
	m_nLastState = 0;
	m_nStateCount = 0;

	m_nMaxShortCount = nMaxShortCount;
	m_pParseEventListener = pListener;
	m_nStep = 1;
}

CMorseParser::~CMorseParser(void)
{
	ReleaseAllNodes(m_pMorseCodeTree);
}

void CMorseParser::AddNode(MORSECODETREENODE* pCurrNode, const MORSECODE* pMorseCode, int nMcWin)
{
	if (!nMcWin)
	{
		// position it!
		pCurrNode->pMorseCode = pMorseCode;
	}
	else
	{
		if (pMorseCode->nMorseCode & nMcWin)
		{
			if (!pCurrNode->pDi)
			{
				pCurrNode->pDi = new MORSECODETREENODE;
				pCurrNode->pDi->pMorseCode = NULL;
				pCurrNode->pDi->pDi = pCurrNode->pDi->pDa = NULL;
			}

			AddNode(pCurrNode->pDi, pMorseCode, nMcWin >> 1);
		}
		else
		{
			if (!pCurrNode->pDa)
			{
				pCurrNode->pDa = new MORSECODETREENODE;
				pCurrNode->pDa->pMorseCode = NULL;
				pCurrNode->pDa->pDi = pCurrNode->pDa->pDa = NULL;
			}

			AddNode(pCurrNode->pDa, pMorseCode, nMcWin >> 1);
		}
	}
}

void CMorseParser::ReleaseAllNodes(MORSECODETREENODE* pRoot)
{
	if (!pRoot)
	{
		ReleaseAllNodes(pRoot->pDi);
		ReleaseAllNodes(pRoot->pDa);
		delete pRoot;
	}
}

void CMorseParser::Sample(int nState)
{
	switch(m_nStep)
	{
		case 1: // test start new letter
			if (1 == nState)
			{
				m_nStateCount = 1;
				m_pCurrNode = m_pMorseCodeTree;
				m_nStep = 2;
			}
			break;
		case 2:
			if (0 == nState)
			{
				m_nStep = 3;
				if (m_nStateCount > m_nMaxShortCount)
				{
					if (m_pCurrNode->pDa)
						m_pCurrNode = m_pCurrNode->pDa;
					else
					{
						m_pParseEventListener->OnWorkOut(this, m_pCurrNode->pMorseCode);
						m_nStep = 1;
					}
				}
				else
				{
					if (m_pCurrNode->pDi)
						m_pCurrNode = m_pCurrNode->pDi;
					else
					{
						m_pParseEventListener->OnWorkOut(this, m_pCurrNode->pMorseCode);
						m_nStep = 1;
					}
				}

				m_nStateCount = 0;
			}
			else
				++m_nStateCount;
			break;
		case 3:
			if (1 == nState)
			{
				if (m_nStateCount > m_nMaxShortCount)
				{
					if (m_pCurrNode->pMorseCode)
					{
						m_pParseEventListener->OnWorkOut(this, m_pCurrNode->pMorseCode);
						m_pCurrNode = m_pMorseCodeTree;
					}
				}
				m_nStateCount = 1;
				m_nStep = 2;
			}
			else
			{
				if (++m_nStateCount > m_nMaxShortCount)
				{
					if (m_pCurrNode->pMorseCode)
					{
						m_pParseEventListener->OnWorkOut(this, m_pCurrNode->pMorseCode);
						m_nStep = 1;
					}
					else if (!m_pCurrNode->pDi && !m_pCurrNode->pDa || m_nStateCount > 1.5 * m_nMaxShortCount)
					{
						m_pParseEventListener->OnWorkOut(this, NULL);
						m_nStep = 1;
					}
				}
			}
			break;
	}
}
