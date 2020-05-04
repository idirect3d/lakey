#pragma once

#include "LakeySetting.h"

class IParseEventListener
{
public:
	virtual void OnWorkOut(void* owner, const MORSECODE* pResult) = 0;
};

typedef struct tagMorseCodeTreeNode
{
	const MORSECODE*				pMorseCode;
	struct tagMorseCodeTreeNode*	pDi;
	struct tagMorseCodeTreeNode*	pDa;
} MORSECODETREENODE;

class CMorseParser
{
public:
	CMorseParser(const MORSECODE* pMorseList, int nCount, int nMaxShortCount, IParseEventListener* pListener);
	virtual ~CMorseParser();

	void SetMaxShortCount(int nCount) { m_nMaxShortCount = nCount; };
	void Sample(int nState);

private:
	void AddNode(MORSECODETREENODE* pCurrNode, const MORSECODE* pMorseCode, int nMcWin);
	void ReleaseAllNodes(MORSECODETREENODE* pRoot);

	MORSECODETREENODE*		m_pMorseCodeTree;
	int						m_nMaxShortCount;
	IParseEventListener*	m_pParseEventListener;

	// for parse work
	MORSECODETREENODE*		m_pCurrNode;
	int						m_nLastState;
	int						m_nStateCount;
	int						m_nStep;
};

