#pragma once

#define SEND_BUFF_BYTES		128
#define RECV_BUFF_BYTES		128

//上线/维持帧/空应答帧
#define LNPF_KEEPALIVE				0x0000
//键击帧
#define LNPF_KEY_FRAME				0x0001
//源信息帧
#define LNPF_OPERATOR_SOURCE_FRMAE	0x0002
//源信息帧域名附加帧
#define LNPF_OPERATOR_SDN_FRMAE		0x0004
//附带其他源信息帧
#define LNPF_OTHER_SOURCE_FRAME		0x0020
//附带其它源信息帧域名附加帧
#define LNPF_OTHER_SDN_FRMAE		0x0040
//可传递文字或域名等信息(保留)
#define LNPF_TEXT_FRAME				0x0100
//服务器帧，不接受/转发KEY_FRAME
#define LNPF_SERVER_FRAME			0x8000
//离线
#define LNPF_OFF_LINE				0xffff

#define LNPSF_OFF_LINE				0x0000
#define LNPSF_ON_LINE				0x0001
#define LNPSF_REQUEST				0x0002
//强调持久性
#define LNPSF_SERVER				0x0004
//是否转发包，除SERVER外，其他的主机都要转发收到的包
#define LNPSF_BROKER				0x0008

#define NM_BIND_OK					0x0010
#define NM_BIND_FAILED				0x8010
#define NM_EXIT						0x0030

#define LRC_WSASTARTUP_FAILED		0x8001

#define SEND_QUEUE_SIZE				100

#define LA_FLAG						((((WORD)'A') << 8) | ((WORD)'L'))
typedef struct
{
	WORD	nLaFlg;		//'LA'	LA_FLAG
	WORD	nPkgFlgs;	//LNPF_...
} LANWPKGHEAD;

typedef struct
{
	DWORD	nToState : 1;	// 0: off; 1: on
	DWORD	nActTick : 31;	// action tickcount
	DWORD	nReserved;		// 30,000,000
} LANWPKGKEYFRAME;

typedef struct
{
	WORD	nSrcFlgs;	//LNPSF_
	WORD	nPort;
	DWORD	nAddr;
} LANWPKGSRCFRAME;

typedef struct
{
	BYTE	vText[32];
} LANWPKGTXTFRAME;

typedef struct
{
	long	nLastSendto;		// 最后发送至该节点的时间
	long	nLastRecvFrom;		// 最后接收自该节点的时间
	long	nSendPkgs;			// 发送总包数
	long	nRecvPkgs;			// 接收总包数
} LANWSRCNODEATTR;

typedef struct tagLANWSRCNODE
{
	LANWPKGSRCFRAME			base;
	LANWSRCNODEATTR			attr;
	struct tagLANWSRCNODE*	branch;	// 同一来源，除主干上的节点外，分支节点暂只有branch
	struct tagLANWSRCNODE*	next;	// 不同来源
} LANWSRCNODE;

class INetworkEventListener
{
public:
	virtual void OnEvent(WORD nMsg) = 0;
};

class INetworkFrameListener
{
public:
	virtual void OnKeyFrame(const LANWPKGKEYFRAME* pFrame) = 0;
	virtual void OnSourceFrame(const LANWPKGSRCFRAME* pFrame, const char* szDomainName) = 0;
	virtual void OnTextFrame(const LANWPKGTXTFRAME* pFrame) = 0;
};

/**
[STABLESOURCE]
node1=[www.layala.org:2009,x,x,x],[...],...
**/
class CLaNetwork
{
public:
	CLaNetwork(DWORD nFlgs);
	~CLaNetwork();

	BOOL Startup(WORD nPort);
	void Shutdown();

	void BindEventListener(INetworkEventListener* pEventListener) { m_pEventListener = pEventListener; };
	void BindFrameListener(INetworkFrameListener* pFrameListener) { m_pFrameListener = pFrameListener; };

	BOOL AppendKeyFrame(const LANWPKGKEYFRAME* pFrame);
	BOOL PickKeyFrame(LANWPKGKEYFRAME* pFrame);
	BOOL IsSendKeyFrameQueueEmpty() { return m_nSendInPos == m_nSendOutPos; };
	//void AppendTextFrameQ(const LANWPKGKEYFRAME* pFrame);

	BOOL AppendStbSrcNode(const LANWSRCNODE* pStbSrc);
	const LANWSRCNODE* GetStbSrcList() { return m_pStbTabHead; };

private:

	// 本机属性
	DWORD	m_nFlgs;
	WORD	m_nLocalPort;
	BOOL	m_bNeedExit;

	// CW键发送队列
	LANWPKGKEYFRAME		m_vSendQ[SEND_QUEUE_SIZE];
	int					m_nSendInPos;	// 先进队, 再移位置
	int					m_nSendOutPos;	// 先出队, 再移位置, 当等于InPos时队列为空

	// 静态主机节点表
	LANWSRCNODE*		m_pStbTabHead;

	// 动态主机节点表
	LANWSRCNODE*		m_pDynTabHead;

	INetworkEventListener*	m_pEventListener;
	INetworkFrameListener*	m_pFrameListener;

	BOOL		m_bWsaInit;
	HANDLE		m_hNwRecvThread;
	HANDLE		m_hNwSendThread;
	DWORD		m_nNwRecvThreadID;
	DWORD		m_nNwSendThreadID;
	SOCKET		m_socket;

	BOOL Bind(WORD nPort);
	static DWORD WINAPI NwRecvThreadProc(LPVOID pOwner);
	static DWORD WINAPI NwSendThreadProc(LPVOID pOwner);
	BOOL SendKeyFrame(const char* pPkgBuff, int nLen, sockaddr_in* pAddr, const LANWSRCNODE* pHeadNode);
	BOOL AppendNodeByAddr(const sockaddr* pAddr);
};
