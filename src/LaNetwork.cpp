#include "StdAfx.h"
//#include "winsock2.h"
//#include "Ws2tcpip.h"
#include "LaNetwork.h"

#define JOIN_SAFELY(ht)								\
{													\
	if (NULL != ht)									\
	{												\
		DWORD nExitCode_j = -1;						\
		if (GetExitCodeThread(ht, &nExitCode_j))	\
			if (STILL_ACTIVE == nExitCode_j)		\
				WaitForSingleObject(ht, INFINITE);	\
		ht = NULL;									\
	}												\
}

CLaNetwork::CLaNetwork(DWORD nFlgs)
{
	m_nFlgs = nFlgs;
	m_bWsaInit = FALSE;

	m_pEventListener = NULL;
	m_pFrameListener = NULL;

	m_pStbTabHead = NULL;
	m_pDynTabHead = NULL;

	m_hNwRecvThread = m_hNwSendThread = NULL;

	m_nSendInPos = m_nSendOutPos = 0;

	m_socket = NULL;
}

CLaNetwork::~CLaNetwork()
{
	if (m_hNwRecvThread)
	{
		Shutdown();
		ResumeThread(m_hNwRecvThread);
		JOIN_SAFELY(m_hNwRecvThread);
	}

	if (m_hNwSendThread)
	{
		Shutdown();
		ResumeThread(m_hNwSendThread);
		JOIN_SAFELY(m_hNwSendThread);
	}

	if (m_bWsaInit)
	{
		WSACleanup();
		m_bWsaInit = FALSE;
	}

	while(m_pStbTabHead)
	{
		LANWSRCNODE* p = m_pStbTabHead->next;
		delete m_pStbTabHead;
		m_pStbTabHead = p;
	}
}

BOOL CLaNetwork::AppendStbSrcNode(const LANWSRCNODE* pStbSrc)
{
	LANWSRCNODE* p = m_pStbTabHead;

	if (!m_pStbTabHead)
	{
		m_pStbTabHead = new LANWSRCNODE;
		p = m_pStbTabHead;
	}
	else
	{
		while(p->next)
		{
			if (p->base.nAddr == pStbSrc->base.nAddr && p->base.nPort == pStbSrc->base.nPort)
				return FALSE;

			p = p->next;
		}

		p->next = new LANWSRCNODE;
		p = p->next;
	}

	p->base = pStbSrc->base;
	p->attr = pStbSrc->attr;
	p->branch = NULL;
	p->next = NULL;

	return TRUE;
}

BOOL CLaNetwork::AppendNodeByAddr(const sockaddr* pAddr)
{
	if (AF_INET == pAddr->sa_family)
	{
		LANWSRCNODE *pNode = new LANWSRCNODE;
		pNode->base.nAddr = ((sockaddr_in*)pAddr)->sin_addr.S_un.S_addr;
		pNode->base.nPort = ((sockaddr_in*)pAddr)->sin_port;
		pNode->branch = pNode->next = NULL;

		return AppendStbSrcNode(pNode);
	}

	return FALSE;
}

BOOL CLaNetwork::Startup(WORD nPort)
{
	if (m_hNwRecvThread || m_hNwSendThread)
		return TRUE; // only startup once, at the same time

	if (!m_bWsaInit)
	{
		WSADATA wsaData;
		// start up as need
		if (NO_ERROR != WSAStartup(MAKEWORD(2,2), &wsaData))
			return FALSE;

		m_bWsaInit = TRUE;
	}

	m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP/*IPPROTO_TCP*/);
	if (INVALID_SOCKET == m_socket)
		return FALSE;

	if (!Bind(nPort))
		return FALSE;

	m_bNeedExit = FALSE;

	m_hNwRecvThread = CreateThread(NULL, 0, NwRecvThreadProc, this, 0, &m_nNwRecvThreadID);
	if (m_hNwRecvThread)
	{
		m_hNwSendThread = CreateThread(NULL, 0, NwSendThreadProc, this, 0, &m_nNwSendThreadID);
		if (m_hNwSendThread)
			return TRUE;

		Shutdown();
		JOIN_SAFELY(m_hNwRecvThread);
	}

	return FALSE;
}

BOOL CLaNetwork::AppendKeyFrame(const LANWPKGKEYFRAME* pFrame)
{
	int nNextPos = m_nSendInPos + 1;
	if (SEND_QUEUE_SIZE == nNextPos)
		nNextPos = 0;
	if (nNextPos == m_nSendOutPos)
		return FALSE;

	m_vSendQ[m_nSendInPos] = *pFrame;
	m_nSendInPos = nNextPos;

	ResumeThread(m_hNwSendThread);

	return TRUE;
}

BOOL CLaNetwork::PickKeyFrame(LANWPKGKEYFRAME* pFrame)
{
	if (IsSendKeyFrameQueueEmpty())
		return FALSE; //empty Q

	*pFrame = m_vSendQ[m_nSendOutPos];
	if (++m_nSendOutPos == SEND_QUEUE_SIZE)
		m_nSendOutPos = 0;

	return TRUE;
}

void CLaNetwork::Shutdown()
{
	m_bNeedExit = TRUE;

	if (m_socket)
	{
		closesocket(m_socket);
		m_socket = NULL;
	}
}

BOOL CLaNetwork::Bind(WORD nPort)
{
	m_nLocalPort = nPort;
	sockaddr_in service;
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = INADDR_ANY; // inet_addr("127.0.0.1");
	service.sin_port = htons(m_nLocalPort);

	if (bind(m_socket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR)
	{
		if (m_pEventListener)
			m_pEventListener->OnEvent(NM_BIND_FAILED);

		return FALSE;
	}

	return TRUE;
}

#define NOTEVALIFNEEDEXIT(expr)	if (!owner->m_bNeedExit) expr

DWORD WINAPI CLaNetwork::NwRecvThreadProc(LPVOID pOwner)
{
	CLaNetwork* owner = (CLaNetwork *)pOwner;

	sockaddr	saddr;
	int nFromlen = sizeof(saddr);
	char	sPkgBuff[RECV_BUFF_BYTES];
	int	nFramePos;
	int nReadLen;
	
	LANWPKGHEAD* pLnph;
	LANWPKGKEYFRAME* pLnpkf;
//	LANWPKGSRCFRAME* pLnpsf;
//	LANWPKGTXTFRAME* pLnptf;

	while(!owner->m_bNeedExit)
	{
		nReadLen = recvfrom(owner->m_socket, sPkgBuff, sizeof(sPkgBuff), 0, &saddr, &nFromlen);
		if (SOCKET_ERROR != nReadLen && !owner->m_bNeedExit)
		{
			if (sizeof(LANWPKGHEAD) > nReadLen)
				continue;	// invalid package size

			nFramePos = 0;
			pLnph = (LANWPKGHEAD *)(sPkgBuff + nFramePos);
			if (LA_FLAG != pLnph->nLaFlg)
				continue;	// invalid package head flag

			if (LNPF_KEEPALIVE == pLnph->nPkgFlgs)
			{
				if (sizeof(LANWPKGHEAD) != nReadLen)
					continue;	// invalid keep-alive package size

				// TODO: handle keep alive package
				owner->AppendNodeByAddr(&saddr);
				continue;
			}

			if (LNPF_OFF_LINE == pLnph->nPkgFlgs)
			{
				if (sizeof(LANWPKGHEAD) != nReadLen)
					continue;	// invalid off-line package size

				// TODO: handle off-line package
				continue;
			}
		
			nFramePos += sizeof(LANWPKGHEAD);

			if (LNPF_KEY_FRAME & pLnph->nPkgFlgs)
			{
				if (LNPF_SERVER_FRAME & pLnph->nPkgFlgs)
					continue;	// server must not send key-frame

				if (nFramePos + sizeof(LANWPKGKEYFRAME) > (UINT)nReadLen)
					continue;	// package too small

				pLnpkf = (LANWPKGKEYFRAME *)(sPkgBuff + nFramePos);

				if (owner->m_pFrameListener)
					owner->m_pFrameListener->OnKeyFrame(pLnpkf);

				nFramePos += sizeof(LANWPKGKEYFRAME);
			}

			/*
			if (LNPF_OPERATOR_SOURCE_FRMAE & pLnph->nPkgFlgs)
			{
				if (nFramePos + sizeof(LANWPKGSRCFRAME) > nReadLen)
					continue;	// package too small

				//TODO: handle operator source frame here
				pLnpsf = (LANWPKGSRCFRAME *)(sPkgBuff + nFramePos);
				nFramePos += sizeof(LANWPKGSRCFRAME);

				if (LNPF_OTHER_SDN_FRAME & pLnph->nPkgFlgs)
				{
					if (nFramePos + sizeof(LANWPKGTXTFRAME) < nReadLen)
						continue;	// package too small

					pLnptf = (LANWPKGTXTFRAME *)(sPkgBuff + nFramePos);
					nFramePos += sizeof(LANWPKGTXTFRAME);
					if (owner->m_pFrameListener)
						owner->m_pFrameListener->OnSourceFrame(pLnpsf, pLnptf->vText);
				}
				else
				{
					if (owner->m_pFrameListener)
						owner->m_pFrameListener->OnSourceFrame(pLnpsf, NULL);
				}
			}

			if (LNPF_OTHER_SOURCE_FRAME & pLnph->nPkgFlgs)
			{
				if (nFramePos + sizeof(LANWPKGSRCFRAME) < nReadLen)
					continue;	// package too small

				//TODO: handle other source frame here
				pLnpsf = (LANWPKGSRCFRAME *)(sPkgBuff + nFramePos);
				nFramePos += sizeof(LANWPKGSRCFRAME);

				if (LNPF_OTHER_SDN_FRAME & pLnph->nPkgFlgs)
				{
					if (nFramePos + sizeof(LANWPKGTXTFRAME) < nReadLen)
						continue;	// package too small

					pLnptf = (LANWPKGTXTFRAME *)(sPkgBuff + nFramePos);
					nFramePos += sizeof(LANWPKGTXTFRAME);
					if (owner->m_pFrameListener)
						owner->m_pFrameListener->OnSourceFrame(pLnpsf, pLnptf->vText);
				}
				else
				{
					if (owner->m_pFrameListener)
						owner->m_pFrameListener->OnSourceFrame(pLnpsf, NULL);
				}
			}

			if (LNPF_TEXT_FRAME & pLnph->nPkgFlgs)
			{
				if (nFramePos + sizeof(LANWPKGTXTFRAME) < nReadLen)
					continue;	// package too small

				//TODO: handle text frame here
				pLnptf = (LANWPKGTXTFRAME *)(sPkgBuff + nFramePos);
				if (owner->m_pFrameListener)
					owner->m_pFrameListener->OnTextFrame(pLnptf);

				nFramePos += sizeof(LANWPKGTXTFRAME);
			}
			*/

			if (nFramePos != nReadLen)
			{
				//TODO: handle invalid package here
				continue;
			}
		} // end of if (SOCKET_ERROR != nReadLen && !owner->m_bNeedExit)
		else
		{
			int nErrCode = WSAGetLastError();
		}
	} // end of while(!owner->m_bNeedExit)

	if (owner->m_pEventListener)
		owner->m_pEventListener->OnEvent(NM_EXIT);

	return 0;
}

DWORD WINAPI CLaNetwork::NwSendThreadProc(LPVOID pOwner)
{
	CLaNetwork* owner = (CLaNetwork *)pOwner;
	char sPkgBuff[SEND_BUFF_BYTES];
	LANWPKGHEAD* pPkgHead = (LANWPKGHEAD *)sPkgBuff;
	LANWPKGKEYFRAME* pFrame = (LANWPKGKEYFRAME *)(sPkgBuff + sizeof(LANWPKGHEAD));
	sockaddr_in destAddr;
	destAddr.sin_family = AF_INET;

	while(!owner->m_bNeedExit)
	{
		if (owner->IsSendKeyFrameQueueEmpty())
		{
			SuspendThread(owner->m_hNwSendThread);
			// now send frame immediatly, but may be sleep here for keep alive frame in future
		}
		else
		{
			pPkgHead->nLaFlg = LA_FLAG;
			pPkgHead->nPkgFlgs = LNPF_KEY_FRAME;

			owner->PickKeyFrame(pFrame);

			if (owner->m_pStbTabHead)
				owner->SendKeyFrame(sPkgBuff, sizeof(LANWPKGHEAD) + sizeof(LANWPKGKEYFRAME)
					, &destAddr, owner->m_pStbTabHead);

			/* not supported dynamic node now
			if (owner->m_pDynTabHead)
				owner->SendKeyFrame(sPkgBuff, sizeof(LANWPKGHEAD) + sizeof(LANWPKGKEYFRAME)
					, &destAddr, owner->m_pDynTabHead);
			*/
		}
	}

	return 0;
}

BOOL CLaNetwork::SendKeyFrame(const char* pPkgBuff, int nLen, sockaddr_in* pAddr, const LANWSRCNODE* pHeadNode)
{
	pAddr->sin_addr.s_addr = pHeadNode->base.nAddr;
	pAddr->sin_port = htons(pHeadNode->base.nPort);

	if (SOCKET_ERROR == sendto(m_socket, pPkgBuff, nLen, 0, (SOCKADDR *)pAddr, sizeof(sockaddr_in)))
		return FALSE;

	/* only sent to every branch head
	if (NULL != pHeadNode->branch)
		if (!SendKeyFrame(pPkgBuff, nLen, pAddr, pHeadNode->branch))
			return FALSE;*/

	if (NULL != pHeadNode->next)
		return SendKeyFrame(pPkgBuff, nLen, pAddr, pHeadNode->next);

	return TRUE;
}
