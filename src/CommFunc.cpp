#include "stdafx.h"
#include "Lakey.h"
#include "windows.h"
#include "Richedit.h"
#include "commctrl.h"
#include <time.h>
#include <ctype.h>

#include "LakeyMainWindow.h"
#include "CommFunc.h"

BOOL GetDlgItemTxt(HWND hDlg, int nItemId, LPSTR lpTxtBuf, int nMaxLen)
{
	if (GetDlgItem(hDlg, nItemId))
	{
		GetDlgItemText(hDlg, nItemId, lpTxtBuf, nMaxLen);
		return TRUE;
	}

	return FALSE;
}

BOOL SetDlgItemDec(HWND hDlg, UINT nItemId, double val)
{
	if (GetDlgItem(hDlg, nItemId))
	{
		char vBuff[32]; vBuff[sizeof(vBuff) - 1] = '\0';
		sprintf(vBuff, "%.4lf", val);
		SetDlgItemText(hDlg, nItemId, vBuff);
		return TRUE;
	}

	return FALSE;
}

BOOL GetDlgItemDec(HWND hDlg, UINT nItemId, double* pVal)
{
	if (GetDlgItem(hDlg, nItemId))
	{
		char vBuff[32]; vBuff[sizeof(vBuff) - 1] = '\0';
		GetDlgItemText(hDlg, nItemId, vBuff, sizeof(vBuff) - 1);
		sscanf(vBuff, "%lf", pVal);
		return TRUE;
	}

	return FALSE;
}

BOOL SetDlgItemHex(HWND hDlg, UINT nItemId, BYTE val)
{
	if (GetDlgItem(hDlg, nItemId))
	{
		char vBuff[32]; vBuff[sizeof(vBuff) - 1] = '\0';
		sprintf(vBuff, "%2.2hX", val);
		SetDlgItemText(hDlg, nItemId, vBuff);
		return TRUE;
	}

	return FALSE;
}

BOOL GetDlgItemHex(HWND hDlg, UINT nItemId, BYTE* pVal)
{
	if (GetDlgItem(hDlg, nItemId))
	{
		char vBuff[32]; vBuff[sizeof(vBuff) - 1] = '\0';
		GetDlgItemText(hDlg, nItemId, vBuff, sizeof(vBuff) - 1);
		WORD w = 0;
		sscanf(vBuff, "%hX", &w);
		*pVal = (BYTE)w;
		return TRUE;
	}

	return FALSE;
}

BOOL SetDlgItemHex(HWND hDlg, UINT nItemId, WORD val)
{
	if (GetDlgItem(hDlg, nItemId))
	{
		char vBuff[32]; vBuff[sizeof(vBuff) - 1] = '\0';
		sprintf(vBuff, "%4.4hX", val);
		SetDlgItemText(hDlg, nItemId, vBuff);
		return TRUE;
	}

	return FALSE;
}

BOOL GetDlgItemHex(HWND hDlg, UINT nItemId, WORD* pVal)
{
	if (GetDlgItem(hDlg, nItemId))
	{
		char vBuff[32]; vBuff[sizeof(vBuff) - 1] = '\0';
		GetDlgItemText(hDlg, nItemId, vBuff, sizeof(vBuff) - 1);
		sscanf(vBuff, "%hX", pVal);
		return TRUE;
	}

	return FALSE;
}

BOOL GetDlgItemInt(HWND hDlg, UINT nItemId, int* pVal)
{
	if (GetDlgItem(hDlg, nItemId))
	{
		char vBuff[32]; vBuff[sizeof(vBuff) - 1] = '\0';
		GetDlgItemText(hDlg, nItemId, vBuff, sizeof(vBuff) - 1);
		sscanf(vBuff, "%d", pVal);
		return TRUE;
	}

	return FALSE;
}

