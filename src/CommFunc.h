#pragma once

#include "stdlib.h"

BOOL GetDlgItemTxt(HWND hDlg, int nItemId, LPSTR lpTxtBuf, int nMaxLen);
BOOL SetDlgItemDec(HWND hDlg, UINT nItemId, double val);
BOOL GetDlgItemDec(HWND hDlg, UINT nItemId, double* pVal);
BOOL SetDlgItemHex(HWND hDlg, UINT nItemId, BYTE val);
BOOL GetDlgItemHex(HWND hDlg, UINT nItemId, BYTE* pVal);
BOOL SetDlgItemHex(HWND hDlg, UINT nItemId, WORD val);
BOOL GetDlgItemHex(HWND hDlg, UINT nItemId, WORD* pVal);
BOOL GetDlgItemInt(HWND hDlg, UINT nItemId, int* pVal);

