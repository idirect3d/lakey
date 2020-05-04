// LakeyMain.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "Lakey.h"
#include "windows.h"
#include "Richedit.h"
#include "commctrl.h"
#include <time.h>
#include <ctype.h>

#include "LakeyMainWindow.h"
#include "LakeySetting.h"
#include "LaWaveFile.h"

#include "CommFunc.h"
#include "mytest.h"

//#include "winio.h"

#define MAX_LOADSTRING	100
#define MAX_KOCH_SAMPLE		256
#define SAFE_DELETE(x)		if (x) { delete x; x = NULL; }
#define PROMPT_BUFF_LEN		256

#define SETTING_PAGES 6
 
// 全局变量：
HINSTANCE hInst;								// 当前实例
TCHAR szTitle[MAX_LOADSTRING];					// 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING];			// 主窗口类名

CLakeyMainWindow*	g_pMainWnd = NULL;
CLakeySetting*		g_pGlobalSetting = NULL;
CUSTOMIZE			g_oTmpCust;
HostList			g_oTmpHosts;
HWND				g_hWnd = NULL;

char				g_vCopyText[MAX_KOCH_SAMPLE + 1];
char				g_vKochSample[MAX_KOCH_SAMPLE];
char g_vPromptText[PROMPT_BUFF_LEN];
char g_vPromptBuff[PROMPT_BUFF_LEN] = {'\0'};


// 此代码模块中包含的函数的前向声明：
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

LRESULT CALLBACK	Settings(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK	SettingsPage(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
BOOL				OnSettingsInit(HWND hDlg);
void				DestroyCurrPage(HWND hDlg);
void				SaveTemp(HWND hDlg);
void				AddHost(HWND hCtrl, HOSTNODE* pNode);

LRESULT CALLBACK	CopyPad(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK	TestDlg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK	PromptProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

const char* ConvertKochPathname(char* pDest, const char* pSrc, UINT nBuffLen);
void InitKochSample();
void PromptToWave(HWND hWnd);
void KochToWave(HWND hWnd);
void TextToWave(const char* pPathname, const char* pTxt, UINT nTxtLen, UINT nWordLen);
void TextToAskWave(char* pWavePathBuff, UINT nWavePathBuffSize, const char* pBuff, UINT nCount, HWND hWnd);
void CalcSpeed(HWND hDlg);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
//	Application::EnableVisualStyles();
//	Application::SetCompatibleTextRenderingDefault(false); 

	InitCommonControls();
	g_pGlobalSetting = new CLakeySetting();

	// TODO: 在此放置代码。
	MSG msg;
	HACCEL hAccelTable;

	// 初始化全局字符串
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_LAKEY, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 执行应用程序初始化：
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_LAKEY);

	// 主消息循环：
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	delete g_pGlobalSetting;

	return (int) msg.wParam;
}

//
//  函数：MyRegisterClass()
//
//  目的：注册窗口类。
//
//  注释：
//
//    仅当希望在已添加到 Windows 95 的
//    “RegisterClassEx”函数之前此代码与 Win32 系统兼容时，
//    才需要此函数及其用法。调用此函数
//    十分重要，这样应用程序就可以获得关联的
//   “格式正确的”小图标。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_LAKEY);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW);
	wcex.lpszMenuName	= (LPCTSTR)IDC_LAKEY;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

//
//   函数：InitInstance(HANDLE, int)
//
//   目的：保存实例句柄并创建主窗口
//
//   注释：
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // 将实例句柄存储在全局变量中

	g_hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME,
		g_pGlobalSetting->cust.m_oWindowRect.left, g_pGlobalSetting->cust.m_oWindowRect.top
		, g_pGlobalSetting->cust.m_oWindowRect.right - g_pGlobalSetting->cust.m_oWindowRect.left
		, g_pGlobalSetting->cust.m_oWindowRect.bottom - g_pGlobalSetting->cust.m_oWindowRect.top
		, NULL, NULL, hInstance, NULL);

	if (!g_hWnd)
	{
		return FALSE;
	}

	g_pMainWnd = new CLakeyMainWindow(g_hWnd, g_pGlobalSetting);

	::LoadLibrary("RICHED20.dll");

	if (!g_pMainWnd->Initialize())
		return FALSE;

	g_pMainWnd->NetworkStartup();

	ShowWindow(g_hWnd, nCmdShow);
	UpdateWindow(g_hWnd);

	return TRUE;
}

//
//  函数：WndProc(HWND, unsigned, WORD, LONG)
//
//  目的：处理主窗口的消息。
//
//  WM_COMMAND	- 处理应用程序菜单
//  WM_PAINT	- 绘制主窗口
//  WM_DESTROY	- 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
//	PAINTSTRUCT ps;
//	HDC hdc;

	LRESULT r;
	if (g_pMainWnd)
		if (g_pMainWnd->OnMessage(message, wParam, lParam, &r))
			return TRUE;

	switch (message) 
	{
		case WM_COMMAND:
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// 分析菜单选择：
			switch (wmId)
			{
				case IDM_TEST:
				{
					//Test(hWnd, message, wParam, lParam);
					break;
				}
				case IDM_ABOUT:
					DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
					break;
				case IDM_EXIT:
					DestroyWindow(hWnd);
					break;
				case IDM_SETTINGS:
					if (IDC_ACCEPT == DialogBox(hInst, (LPCTSTR)IDD_SETTINGS, hWnd, (DLGPROC)Settings))
					{
						if (g_pGlobalSetting->cust.m_bNetworkEnabled)
							g_pMainWnd->NetworkStartup();
						else
							g_pMainWnd->NetworkShutdown();
					}

					break;
				case IDM_KOCHSTART:
					if (2 > strlen(g_pGlobalSetting->cust.m_vKochChar))
					{
						MessageBox(hWnd, "没有设置Koch训练字符\n请先在菜单的[工具->设置]中设置至少2个ASCII字符", "错误", MB_OK);
						break;
					}

					DialogBox(hInst, (LPCTSTR)IDD_COPYPAD, hWnd, (DLGPROC)CopyPad);
					break;
				case IDM_KOCHWAVE:
				{
					if (2 > strlen(g_pGlobalSetting->cust.m_vKochChar))
					{
						MessageBox(hWnd, "没有设置Koch训练字符\n请先在菜单的[工具->设置]中设置至少2个ASCII字符", "错误", MB_OK);
						break;
					}

					KochToWave(hWnd);
					break;
				}
				case IDM_PROMPTWAVE:
				{
					strcpy(g_vPromptText, "请输入不超过200个字符长度的字符串: ");
					if (0 < DialogBox(hInst, (LPCTSTR)IDD_PROMPTBOX, hWnd, (DLGPROC)PromptProc))
					{
						if (0 < strlen(g_vPromptBuff))
						{
							PromptToWave(hWnd);
						}
					}
					break;
				}
				case IDA_TEST:
					DialogBox(hInst, (LPCTSTR)IDD_TEST, hWnd, (DLGPROC)TestDlg);
					break;
				default:
					return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		case WM_DESTROY:
			SAFE_DELETE(g_pMainWnd);

			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// “关于”框的消息处理程序。
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		return TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
		{
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
	return FALSE;
}

// 1 "PARIS" = 1 word
// .__. ._ ._. .. ...
//
void CalcSpeed(HWND hDlg)
{
	if (!GetDlgItem(hDlg, IDC_SPEED))
		return;

	WPMS wpms;
	wpms.wdelay = g_oTmpCust.m_nWordDelay;

	GetDlgItemInt(hDlg, IDC_SHORTHIT, &wpms.di);
	GetDlgItemInt(hDlg, IDC_LONGHIT, &wpms.dah);
	GetDlgItemInt(hDlg, IDC_HITDELAY, &wpms.hdelay);
	GetDlgItemInt(hDlg, IDC_LETTERDELAY, &wpms.ldelay);
	
	paramsToWpm(&wpms);

	SetDlgItemInt(hDlg, IDC_SPEED, (UINT)wpms.wpm, TRUE);
}

void UncalcSpeed(HWND hDlg)
{
	WPMS wpms;
	int wpm;
	GetDlgItemInt(hDlg, IDC_SPEED, &wpm);
	if (0 == wpm)
		return;

	wpms.wpm = (float)wpm;
	wpmToParams(&wpms);

	SetDlgItemInt(hDlg, IDC_SHORTHIT, wpms.di, TRUE);
	SetDlgItemInt(hDlg, IDC_LONGHIT, wpms.dah, TRUE);
	SetDlgItemInt(hDlg, IDC_HITDELAY, wpms.hdelay, TRUE);
	SetDlgItemInt(hDlg, IDC_LETTERDELAY, wpms.ldelay, TRUE);
	//SetDlgItemInt(hDlg, IDC_WORDDELAY, nHitLen * 3, TRUE);
	g_oTmpCust.m_nWordDelay = wpms.wdelay;
}

DLGTEMPLATE * WINAPI DoLockDlgRes(LPCSTR lpszResName) 
{ 
    HRSRC hrsrc = FindResource(NULL, lpszResName, RT_DIALOG); 
    HGLOBAL hglb = LoadResource(hInst, hrsrc); 
    return (DLGTEMPLATE *) LockResource(hglb); 
} 

VOID WINAPI OnSelChanged(HWND hDlg) 
{
	HWND hTab = GetDlgItem(hDlg, IDC_SETTINGSTAB);
    int iSel = TabCtrl_GetCurSel(hTab); 
 
	DestroyCurrPage(hTab);
	DLGTEMPLATE* pDlgTemp = NULL;
	switch(iSel)
	{
		case 0:
			pDlgTemp = DoLockDlgRes(MAKEINTRESOURCE(IDD_GENERAL));
			break;
		case 1:
			pDlgTemp = DoLockDlgRes(MAKEINTRESOURCE(IDD_SEND));
			break;
		case 2:
			pDlgTemp = DoLockDlgRes(MAKEINTRESOURCE(IDD_RECV));
			break;
		case 3:
			pDlgTemp = DoLockDlgRes(MAKEINTRESOURCE(IDD_KOCH));
			break;
		case 4:
			pDlgTemp = DoLockDlgRes(MAKEINTRESOURCE(IDD_NETWORK));
			break;
		case 5:
			pDlgTemp = DoLockDlgRes(MAKEINTRESOURCE(IDD_IO));
			break;
	}

	if (pDlgTemp)
	{
		HWND hCurrPage = CreateDialogIndirect(hInst, pDlgTemp, hDlg, SettingsPage);
		if (hCurrPage)
		{
			SetWindowLongPtr(hTab, GWL_USERDATA, (LONG)hCurrPage);
		}
	}
}

BOOL OnSettingsInit(HWND hDlg)
{
	g_pGlobalSetting->GetCustomize(&g_oTmpCust);
	g_oTmpHosts = g_pGlobalSetting->hosts;
	
	//g_oTmpCust = g_pGlobalSetting->cust;

	HWND hTabCtrl = GetDlgItem(hDlg, IDC_SETTINGSTAB);
	TCITEM tie;
    tie.mask = TCIF_TEXT | TCIF_IMAGE; 
    tie.iImage = -1; 
    tie.pszText = "常规"; 
    TabCtrl_InsertItem(hTabCtrl, 0, &tie); 

    tie.mask = TCIF_TEXT | TCIF_IMAGE; 
    tie.iImage = -1; 
    tie.pszText = "发送"; 
    TabCtrl_InsertItem(hTabCtrl, 1, &tie); 

    tie.mask = TCIF_TEXT | TCIF_IMAGE; 
    tie.iImage = -1; 
    tie.pszText = "接收"; 
    TabCtrl_InsertItem(hTabCtrl, 2, &tie); 

    tie.mask = TCIF_TEXT | TCIF_IMAGE; 
    tie.iImage = -1; 
    tie.pszText = "Koch训练"; 
    TabCtrl_InsertItem(hTabCtrl, 3, &tie); 

    tie.mask = TCIF_TEXT | TCIF_IMAGE; 
    tie.iImage = -1; 
    tie.pszText = "网络"; 
    TabCtrl_InsertItem(hTabCtrl, 4, &tie); 

    tie.mask = TCIF_TEXT | TCIF_IMAGE; 
    tie.iImage = -1; 
    tie.pszText = "I/O"; 
    TabCtrl_InsertItem(hTabCtrl, 5, &tie); 

	OnSelChanged(hDlg);
	return TRUE;
}

void HostListAdd(HWND hListCtrl, char* pHost, const char* pPort, int idx)
{
	LVITEM lvi;
	lvi.mask = LVIF_TEXT | LVIF_STATE | LVIF_PARAM;	// | LVIF_IMAGE 
	lvi.state = 0;
	lvi.stateMask = 0;
	lvi.iSubItem = 0;
	lvi.iItem = idx;
	lvi.pszText = pHost;
	lvi.lParam = (LPARAM)pPort;
	ListView_InsertItem(hListCtrl, &lvi);
}

BOOL OnSettingPagesInit(HWND hDlg)
{
	SetDlgItemDec(hDlg, IDC_BEEPFREQ, g_oTmpCust.m_rBeepFreq);
	SetDlgItemDec(hDlg, IDC_BEEPVOL, g_oTmpCust.m_rBeepVol);
	SetDlgItemInt(hDlg, IDC_SENDPERIOD, g_oTmpCust.m_nSendJournalPeriod, TRUE);
	SetDlgItemInt(hDlg, IDC_SENDILDELIMIT, g_oTmpCust.m_nSendIdleLimit, TRUE);
	SetDlgItemInt(hDlg, IDC_CW, g_oTmpCust.m_oCwKeyButton.nKeyCode, TRUE);
	SetDlgItemInt(hDlg, IDC_WORDDELAY, g_oTmpCust.m_nWordDelay, TRUE);

	SetDlgItemInt(hDlg, IDC_RECVANALYZESAMPLES, g_oTmpCust.m_nRecvAnalyzeSamples, TRUE);
	SetDlgItemInt(hDlg, IDC_RECVPERIOD, g_oTmpCust.m_nRecvJournalPeriod, TRUE);
	SetDlgItemInt(hDlg, IDC_RECVILDELIMIT, g_oTmpCust.m_nRecvIdleLimit, TRUE);
	SetDlgItemDec(hDlg, IDC_RECVTHRESHOLD, g_oTmpCust.m_rRecvThreshold);
	SetDlgItemInt(hDlg, IDC_RECVFREQSTART, g_oTmpCust.m_nRecvFreqStart, TRUE);
	SetDlgItemInt(hDlg, IDC_RECVFREQEND, g_oTmpCust.m_nRecvFreqEnd, TRUE);

	SetDlgItemInt(hDlg, IDC_SHORTHIT, g_oTmpCust.m_nShortHit, TRUE);
	SetDlgItemInt(hDlg, IDC_LONGHIT, g_oTmpCust.m_nLongHit, TRUE);
	SetDlgItemInt(hDlg, IDC_HITDELAY, g_oTmpCust.m_nHitDelay, TRUE);
	SetDlgItemInt(hDlg, IDC_LETTERDELAY, g_oTmpCust.m_nLetterDelay, TRUE);

	SetDlgItemText(hDlg, IDC_KOCHCHARS, g_oTmpCust.m_vKochChar);
	SetDlgItemInt(hDlg, IDC_KOCHWORDLEN, g_oTmpCust.m_nKochWordLen, TRUE);

	HWND hCtrl = GetDlgItem(hDlg, IDC_EXTPORTENABLE);
	if (hCtrl)
		SendMessage(hCtrl, BM_SETCHECK, g_oTmpCust.m_nExtPortEnable, 0);

	SetDlgItemHex(hDlg, IDC_EXTPORTADDR, g_oTmpCust.m_nExtPortAddr);
	SetDlgItemHex(hDlg, IDC_OPENBYTE, g_oTmpCust.m_nOpenByte);
	SetDlgItemHex(hDlg, IDC_CLOSEBYTE, g_oTmpCust.m_nCloseByte);

	CalcSpeed(hDlg);

	SetDlgItemInt(hDlg, IDC_LOCALPORT, g_oTmpCust.m_nLocalPort, TRUE);
	SetDlgItemInt(hDlg, IDC_HOSTPORT, g_oTmpCust.m_nLocalPort, TRUE);

	if (NULL != (hCtrl = GetDlgItem(hDlg, IDC_HOSTLIST)))
	{
		LRESULT nStyle = SendMessage(hCtrl, LVM_GETEXTENDEDLISTVIEWSTYLE, 0, 0);
		SendMessage(hCtrl, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, nStyle | LVS_EX_FULLROWSELECT);
		//DWORD nStyle = GetWindowLong(hCtrl, GWL_EXSTYLE) | LVS_EX_FULLROWSELECT;
		//SetWindowLong(hCtrl, GWL_EXSTYLE, nStyle);

//		char szText[256];     // temporary buffer 
		LVCOLUMN lvc; 
		lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT |
		  LVCF_SUBITEM;

		lvc.iSubItem = 0;
		lvc.pszText = "主机";
		lvc.cx = 180;
		lvc.fmt = LVCFMT_LEFT;
		ListView_InsertColumn(hCtrl, 0, &lvc); 

		lvc.iSubItem = 1;
		lvc.pszText = "端口";
		lvc.cx = 80;
		lvc.fmt = LVCFMT_LEFT;
		ListView_InsertColumn(hCtrl, 1, &lvc); 

		for (DWORD i = 0; i < g_oTmpHosts.size(); ++i)
		{
			AddHost(hCtrl, &(g_oTmpHosts[i]));
		}
	}

	CheckDlgButton(hDlg, IDC_NWENABLED, g_oTmpCust.m_bNetworkEnabled ? BST_CHECKED : BST_UNCHECKED);

	return TRUE;
}

void AddHost(HWND hCtrl, HOSTNODE* pNode)
{ 
	LVITEM lvi; 

	lvi.mask = LVIF_TEXT; // | LVIF_STATE | LVIF_PARAM;	// | LVIF_IMAGE 
	lvi.state = 0;
	lvi.stateMask = 0;
	lvi.iSubItem = 0;
	lvi.iItem = ListView_GetItemCount(hCtrl);
	lvi.pszText = pNode->szHostName;
	lvi.lParam = 0; //(LPARAM)(pNode);
	ListView_InsertItem(hCtrl, &lvi);

	lvi.iSubItem = 1;
	lvi.pszText = pNode->szPort;
	ListView_SetItem(hCtrl, &lvi);
}

void DelHost(HWND hCtrl)
{
	int nIdx = ListView_GetSelectionMark(hCtrl);
	if (0 <= nIdx)
		ListView_DeleteItem(hCtrl, nIdx);
}

void SaveTemp(HWND hDlg)
{
	GetDlgItemDec(hDlg, IDC_BEEPFREQ, &g_oTmpCust.m_rBeepFreq);
	GetDlgItemDec(hDlg, IDC_BEEPVOL, &g_oTmpCust.m_rBeepVol);
	GetDlgItemInt(hDlg, IDC_SENDPERIOD, &g_oTmpCust.m_nSendJournalPeriod);
	GetDlgItemInt(hDlg, IDC_SENDILDELIMIT, &g_oTmpCust.m_nSendIdleLimit);
	GetDlgItemInt(hDlg, IDC_CW, &g_oTmpCust.m_oCwKeyButton.nKeyCode);
	GetDlgItemInt(hDlg, IDC_WORDDELAY, &g_oTmpCust.m_nWordDelay);

	GetDlgItemInt(hDlg, IDC_RECVANALYZESAMPLES, &g_oTmpCust.m_nRecvAnalyzeSamples);
	GetDlgItemInt(hDlg, IDC_RECVPERIOD, &g_oTmpCust.m_nRecvJournalPeriod);
	GetDlgItemInt(hDlg, IDC_RECVILDELIMIT, &g_oTmpCust.m_nRecvIdleLimit);
	GetDlgItemDec(hDlg, IDC_RECVTHRESHOLD, &g_oTmpCust.m_rRecvThreshold);
	GetDlgItemInt(hDlg, IDC_RECVFREQSTART, &g_oTmpCust.m_nRecvFreqStart);
	GetDlgItemInt(hDlg, IDC_RECVFREQEND, &g_oTmpCust.m_nRecvFreqEnd);

	GetDlgItemInt(hDlg, IDC_SHORTHIT, &g_oTmpCust.m_nShortHit);
	GetDlgItemInt(hDlg, IDC_LONGHIT, &g_oTmpCust.m_nLongHit);
	GetDlgItemInt(hDlg, IDC_HITDELAY, &g_oTmpCust.m_nHitDelay);
	GetDlgItemInt(hDlg, IDC_LETTERDELAY, &g_oTmpCust.m_nLetterDelay);

	GetDlgItemTxt(hDlg, IDC_KOCHCHARS, g_oTmpCust.m_vKochChar, sizeof(g_oTmpCust.m_vKochChar) / sizeof(char));
	GetDlgItemInt(hDlg, IDC_KOCHWORDLEN, &g_oTmpCust.m_nKochWordLen);

	HWND hCtrl = GetDlgItem(hDlg, IDC_EXTPORTENABLE);
	g_oTmpCust.m_nExtPortEnable =
		(int)SendMessage(hCtrl, BM_GETCHECK, 0, 0);
	GetDlgItemHex(hDlg, IDC_EXTPORTADDR, &g_oTmpCust.m_nExtPortAddr);
	GetDlgItemHex(hDlg, IDC_OPENBYTE, &g_oTmpCust.m_nOpenByte);
	GetDlgItemHex(hDlg, IDC_CLOSEBYTE, &g_oTmpCust.m_nCloseByte);

	hCtrl = GetDlgItem(hDlg, IDC_HOSTLIST);
	if (NULL != hCtrl)
	{
		int nTmp = g_oTmpCust.m_nLocalPort;
		GetDlgItemInt(hDlg, IDC_LOCALPORT, &nTmp);
		if (0 < nTmp && 65535 > nTmp)
			g_oTmpCust.m_nLocalPort = nTmp;

		g_oTmpHosts.resize(ListView_GetItemCount(hCtrl));

		for (UINT i = 0; i < g_oTmpHosts.size(); ++i)
		{
			ListView_GetItemText(hCtrl, i, 0, g_oTmpHosts[i].szHostName, sizeof(g_oTmpHosts[i].szHostName));
			ListView_GetItemText(hCtrl, i, 1, g_oTmpHosts[i].szPort, sizeof(g_oTmpHosts[i].szPort));
		}
		g_oTmpCust.m_bNetworkEnabled = (BST_CHECKED == IsDlgButtonChecked(hDlg, IDC_NWENABLED));
	}
}

void DestroyCurrPage(HWND hTab)
{
	HWND hCurrPage = (HWND)GetWindowLongPtr(hTab, GWL_USERDATA);
    // Destroy the current child dialog box, if any. 
    if (hCurrPage)
	{
		SaveTemp(hCurrPage);
        DestroyWindow(hCurrPage);
		SetWindowLong(hTab, GWL_USERDATA, (LONG)NULL);
	}
}

void OnSettingsAccept(HWND hDlg)
{
	HWND hTab = GetDlgItem(hDlg, IDC_SETTINGSTAB);
	DestroyCurrPage(hTab);
	g_pGlobalSetting->SetCustomize(&g_oTmpCust);
	g_pGlobalSetting->hosts = g_oTmpHosts;

	SAFE_DELETE(g_pMainWnd);
	g_pMainWnd = new CLakeyMainWindow(g_hWnd, g_pGlobalSetting);
	g_pMainWnd->Initialize();
}

// “设置”框的消息处理程序。
LRESULT CALLBACK Settings(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
		{
			return OnSettingsInit(hDlg);
		}
		case WM_COMMAND:
		{
			switch(LOWORD(wParam))
			{
				case IDC_ACCEPT:
				{
					OnSettingsAccept(hDlg);
				}
				case IDCANCEL:
				{
					EndDialog(hDlg, LOWORD(wParam));
					return TRUE;
				}
			}
			break;
		}
		case WM_NOTIFY:
		{
			LPNMHDR lpnmhdr = (LPNMHDR)lParam;
			switch(LOWORD(wParam))
			{
				case IDC_SETTINGSTAB:
				{
					if (TCN_SELCHANGE == lpnmhdr->code)
						OnSelChanged(hDlg);
					break;
				}
			}
			break;

		}
	}
	return FALSE;
}

INT_PTR CALLBACK SettingsPage(HWND hPage, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
		{
			SetWindowPos(hPage, HWND_TOP, 
				8, 32, 
				0, 0, SWP_NOSIZE);
			return OnSettingPagesInit(hPage);
		}
		case WM_COMMAND:
		{
			switch(LOWORD(wParam))
			{
				case IDC_CALCSPEED:
				{
					CalcSpeed(hPage);
					return TRUE;
				}
				case IDC_SPEED:
				{
					switch(HIWORD(wParam))
					{
						case EN_KILLFOCUS:
							UncalcSpeed(hPage);
							break;
					}
					break;
				}
				case IDC_HOSTADD:
				{
					HOSTNODE node;
					node.szHostName[0] = '\0';
					node.szPort[0] = '\0';

					GetDlgItemTxt(hPage, IDC_HOSTNAME, node.szHostName, sizeof(node.szHostName));

					if (0 < strlen(node.szHostName))
					{
						int nPort;
						GetDlgItemInt(hPage, IDC_HOSTPORT, &nPort);
						if (0 < nPort && 65535 > nPort)
						{
							sprintf(node.szPort, "%d", nPort);
							AddHost(GetDlgItem(hPage, IDC_HOSTLIST), &node);
						}
						else
						{
							MessageBox(NULL, "缺少主机端口", "错误", MB_OK);
						}
					}
					else
					{
						MessageBox(NULL, "缺少主机IP", "错误", MB_OK);
					}

					break;
				}
				case IDC_HOSTDEL:
				{
					DelHost(GetDlgItem(hPage, IDC_HOSTLIST));

					break;
				}
			}
			break;
		}
		case WM_NOTIFY:
		{
			LPNMHDR lpnmhdr = (LPNMHDR)lParam;
			switch(LOWORD(wParam))
			{
				case IDC_HOSTLIST:
				{
					switch (lpnmhdr->code)
					{
						case LVN_ENDLABELEDIT:
						{
							NMLVDISPINFO* plvdi = (NMLVDISPINFO*)lParam;
							plvdi->item.mask |= LVIF_TEXT;
							return TRUE;
						} // end of case LVN_ENDLABELEDIT
						case LVN_BEGINLABELEDIT:
						{
							NMLVDISPINFO* plvdi = (NMLVDISPINFO*)lParam;
							plvdi->item.mask |= LVIF_TEXT;
							return FALSE;
						} // end of case LVN_ENDLABELEDIT
					} // end of switch 
					break;
				} // end of case IDC_HOSTLIST
			}
			break;

		} // end of case WM_NOTIFY
	}
	return FALSE;
}

void InitKochSample()
{
	int nSampleRange = (int)strlen(g_pGlobalSetting->cust.m_vKochChar);
	srand((unsigned)time(NULL ));

	for (int i = 0; i < MAX_KOCH_SAMPLE; ++i)
	{
		if (0 < (i + 1) % (g_pGlobalSetting->cust.m_nKochWordLen + 1))
		{
			g_vKochSample[i] = g_pGlobalSetting->cust.m_vKochChar[rand() % nSampleRange];
		}
		else
		{
			g_vKochSample[i] = ' ';
		}
	}
}

#define TIMER_ELAPSETIME	101

// “抄报栏”框的消息处理程序。
LRESULT CALLBACK CopyPad(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static BOOL bFinished = FALSE;
	static DWORD nTickCountStart = 0;

	switch (message)
	{
		case WM_INITDIALOG:
		{
			bFinished = FALSE;
			InitKochSample();

			MessageBox(hDlg, "点击[确定]或按[回车]键开始...", "准备", MB_OK);
			g_pMainWnd->PushText(g_vKochSample, MAX_KOCH_SAMPLE, 0);
			
			SetTimer(hDlg, TIMER_ELAPSETIME, 220, NULL);
			nTickCountStart = GetTickCount();

			SendMessage(GetDlgItem(hDlg, IDC_COPYTEXT), EM_SETEVENTMASK, 0, ENM_CHANGE);

			return TRUE;
		}
		case WM_COMMAND:
		{
			switch(LOWORD(wParam))
			{
				case IDC_ACCEPT:
				{
					if (!bFinished)
					{
						KillTimer(hDlg, TIMER_ELAPSETIME);
						g_pMainWnd->ClearQueue();
/*
						char vBuff[MAX_KOCH_SAMPLE * 2 + 256];
						memset(vBuff, 0, sizeof(vBuff));
						GetDlgItemText(hDlg, IDC_COPYTEXT, g_vCopyText, MAX_KOCH_SAMPLE);
						g_vCopyText[MAX_KOCH_SAMPLE] = NULL;
						strcpy(vBuff, g_vCopyText);
						strcat(vBuff, "\r\n----------------------------------------------------------------------------------\r\n");
						strncat(vBuff, g_vKochSample, strlen(g_vCopyText));
						SetDlgItemText(hDlg, IDC_COPYTEXT, vBuff);
*/
						SetDlgItemText(hDlg, IDC_ACCEPT, "关闭");

						bFinished = TRUE;
/*
						CHARFORMAT fmt;
						memset(&fmt, 0, sizeof(fmt));
						fmt.cbSize = sizeof(fmt);
						fmt.dwMask = CFM_COLOR;
						fmt.crTextColor = (toupper(g_vCopyText[nCopied - 1]) == g_vKochSample[nCopied - 1] ? RGB(0, 0, 0) : RGB(255, 0, 0));

						SendMessage((HWND)lParam, EM_SETCHARFORMAT, SCF_SELECTION, ((LPARAM)&fmt));
*/
						break;
					}
				}
				case IDCANCEL:
				{
					KillTimer(hDlg, TIMER_ELAPSETIME);
					g_pMainWnd->ClearQueue();
					EndDialog(hDlg, LOWORD(wParam));
					return TRUE;
				}
				case IDC_COPYTEXT:
				{
					if (EN_CHANGE == HIWORD(wParam))
					{
						g_vCopyText[MAX_KOCH_SAMPLE] = NULL;
						GetDlgItemTxt(hDlg, IDC_COPYTEXT, g_vCopyText, MAX_KOCH_SAMPLE);
						char vBuff[20];
						int nCopied = (int)strlen(g_vCopyText);
						sprintf(vBuff, "%d", nCopied);
						SetDlgItemText(hDlg, IDC_COPIED, vBuff);
						int nCorrect = 0;

						for (int i = 0; i < nCopied; ++i)
						{
							if (toupper(g_vCopyText[i]) == g_vKochSample[i])
								++nCorrect;
						}
						sprintf(vBuff, "%d", nCopied - nCorrect);
						SetDlgItemText(hDlg, IDC_INCORRECT, vBuff);
						
						if (0 < nCopied)
							sprintf(vBuff, "%2.1f%%", 100.0f * nCorrect / nCopied);
						else
							sprintf(vBuff, "%%");

						SetDlgItemText(hDlg, IDC_CORRECTPERC, vBuff);

						HWND hCopyText = (HWND)lParam;
						CHARRANGE oRange;
						oRange.cpMin = nCopied - 1;
						oRange.cpMax = nCopied;
						SendMessage(hCopyText, EM_EXSETSEL, 0, ((LPARAM)&oRange));

						CHARFORMAT fmt;
						memset(&fmt, 0, sizeof(fmt));
						fmt.cbSize = sizeof(fmt);
						fmt.dwMask = CFM_COLOR;
						fmt.crTextColor = (toupper(g_vCopyText[nCopied - 1]) == g_vKochSample[nCopied - 1] ? RGB(0, 0, 0) : RGB(255, 0, 0));
						SendMessage(hCopyText, EM_SETCHARFORMAT, SCF_SELECTION, ((LPARAM)&fmt));

						oRange.cpMin = oRange.cpMax;
						SendMessage(hCopyText, EM_EXSETSEL, 0, ((LPARAM)&oRange));

						return 0;
					}
					break;
				}
			}
			break;
		}
/*		case WM_NOTIFY:
		{
			if (IDC_COPYTEXT == wParam)
			{
				LPNMHDR pNMHDR = (LPNMHDR)lParam;
				switch(pNMHDR->code)
				{
					case EN_SELCHANGE:
					{
						g_vCopyText[MAX_KOCH_SAMPLE] = NULL;
						GetDlgItemText(hDlg, IDC_COPYTEXT, g_vCopyText, MAX_KOCH_SAMPLE);

						char vBuff[20];
						int nCopied = (int)strlen(g_vCopyText);
						sprintf(vBuff, "%d", nCopied);
						SetDlgItemText(hDlg, IDC_COPIED, vBuff);
						int nCorrect = 0;

						for (int i = 0; i < nCopied; ++i)
						{
							if (toupper(g_vCopyText[i]) == g_vKochSample[i])
								++nCorrect;
						}
						sprintf(vBuff, "%d", nCopied - nCorrect);
						SetDlgItemText(hDlg, IDC_INCORRECT, vBuff);
						
						if (0 < nCopied)
							sprintf(vBuff, "%2.1f%%", 100.0f * nCorrect / nCopied);
						else
							sprintf(vBuff, "%%");

						SetDlgItemText(hDlg, IDC_CORRECTPERC, vBuff);

						CHARFORMAT fmt;
						memset(&fmt, 0, sizeof(fmt));
						fmt.cbSize = sizeof(fmt);
						fmt.dwMask = CFM_COLOR;
						fmt.crTextColor = (toupper(g_vCopyText[nCopied - 1]) == g_vKochSample[nCopied - 1] ? RGB(0, 0, 0) : RGB(255, 0, 0));

						SendMessage((HWND)lParam, EM_SETCHARFORMAT, SCF_SELECTION, ((LPARAM)&fmt));
						break;
					}
				}
			}
			break;
		}
		*/
		case WM_TIMER:
		{
			if (TIMER_ELAPSETIME == wParam)
			{
				DWORD nTickCount = (GetTickCount() - nTickCountStart) / 1000;
				DWORD nSec = nTickCount % 60;
				DWORD nMin = (nTickCount /= 60) % 60;
				DWORD nHour = nTickCount / 60;
				char vBuff[20];
				sprintf(vBuff, "%2.2d:%2.2d:%2.2d", nHour, nMin, nSec);
				SetDlgItemText(hDlg, IDC_ELAPSETIME, vBuff);
				return TRUE;
			}
			break;
		}
	}
	return FALSE;
}

LRESULT CALLBACK TestDlg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		return TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
		{
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
	return FALSE;
}

const char* ConvertKochPathname(char* pDest, const char* pSrc, UINT nBuffLen)
{
	UINT nLen = (UINT)strlen(pSrc);
	UINT i;
	for (i = 0; i < nBuffLen - 20 && i < nLen; ++i)
	{
		pDest[i] = (isalpha(pSrc[i]) ? pSrc[i] : '_');
	}

	strcpy(pDest + i, ".wav");
	return pDest;
}

void PromptToWave(HWND hWnd)
{
	char vDestPathname[260];
	ConvertKochPathname(vDestPathname, g_vPromptBuff, 256);

	TextToAskWave(vDestPathname, (UINT)sizeof(vDestPathname), g_vPromptBuff, (UINT)strlen(g_vPromptBuff), hWnd);
}

void KochToWave(HWND hWnd)
{
	InitKochSample();

	char vDestPathname[260];
	ConvertKochPathname(vDestPathname, g_pGlobalSetting->cust.m_vKochChar, 256);

	TextToAskWave(vDestPathname, sizeof(vDestPathname), g_vKochSample, MAX_KOCH_SAMPLE, hWnd);
}

void TextToAskWave(char* pWavePathBuff, UINT nWavePathBuffSize, const char* pBuff, UINT nCount, HWND hWnd)
{
	OPENFILENAME ofn;       // common dialog box structure
//	HANDLE hf;              // file handle

	// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = pWavePathBuff;
	ofn.nMaxFile = nWavePathBuffSize;
	ofn.lpstrFilter = "All\0*.*\0Wave\0*.wav\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST;

	// Display the Open dialog box. 
	if (GetOpenFileName(&ofn))
	{
		TextToWave(ofn.lpstrFile, pBuff, nCount, 0);
	}
}

void TextToWave(const char* pPathname, const char* pTxt, UINT nTxtLen, UINT nWordLen)
{
	CWaveFile oWaveFile(pPathname, 1, 8000, 16, g_pGlobalSetting->cust.m_nLongHit / 999.0f);
	oWaveFile.SetFreq((float)g_pGlobalSetting->cust.m_rBeepFreq);
	oWaveFile.SetVolumn((float)g_pGlobalSetting->cust.m_rBeepVol);

	for (UINT i = 0; i < nTxtLen; ++i)
	{
		char ch = pTxt[i];
		MORSECODE* pMorse = &(g_pGlobalSetting->cust.m_vMorseCode[MORSECODECOUNT - 1]);
		for (int j = 0; j < MORSECODECOUNT; ++j)
		{
			MORSECODE* p = &(g_pGlobalSetting->cust.m_vMorseCode[j]);
			if (ch == p->nAscCode || ch + ('A' - 'a') == p->nAscCode)
			{
				pMorse = p;
				break;
			}
		}

		if ('_' != pMorse->nAscCode)
		{
			int nWindow = pMorse->nMask + 1;
			while(nWindow >>= 1)
			{
				int nHitCount =
					nWindow & pMorse->nMorseCode ? 
						g_pGlobalSetting->cust.m_nShortHit : g_pGlobalSetting->cust.m_nLongHit;

				oWaveFile.Append(nHitCount / 1000.0f);
				oWaveFile.AppendBlank(g_pGlobalSetting->cust.m_nHitDelay / 1000.0f);
			}

			oWaveFile.AppendBlank(g_pGlobalSetting->cust.m_nLetterDelay / 1000.0f);
		}
		else
		{
			oWaveFile.AppendBlank(g_pGlobalSetting->cust.m_nWordDelay / 1000.0f);
		}

		if (0 < nWordLen && 0 == (i + 1) % nWordLen)
			oWaveFile.AppendBlank(g_pGlobalSetting->cust.m_nWordDelay / 1000.0f);
	}
}

LRESULT CALLBACK PromptProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
		{
			SetDlgItemText(hDlg, IDC_PROMPTTEXT, g_vPromptText);
			SetDlgItemText(hDlg, IDC_INPUTSTR, g_vPromptBuff);
			return TRUE;
		}
		case WM_COMMAND:
		{
			switch(LOWORD(wParam))
			{
				case IDOK:
				{
					GetDlgItemTxt(hDlg, IDC_INPUTSTR, g_vPromptBuff, sizeof(g_vPromptBuff));
					EndDialog(hDlg, 1);
					return TRUE;
				}
				case IDCANCEL:
				{
					EndDialog(hDlg, 0);
					return TRUE;
				}
			}
			break;
		}
	}
	return FALSE;
}
