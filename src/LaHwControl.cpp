#include "StdAfx.h"

#include "LaHwControl.h"
//#include "WinIo.h"

CLaHwControl::CLaHwControl(WORD nInitAddr, BYTE bInit)
{
	//m_bInitOk = InitializeWinIo();
	if (m_bInitOk)
	{
	//	m_bInitOk = SetPortVal(nInitAddr, bInit, 1);
	}
}

CLaHwControl::~CLaHwControl()
{
	//if (m_bInitOk)
	//	ShutdownWinIo();
}

BOOL CLaHwControl::OutByte(WORD nAddr, BYTE b)
{
	if (m_bInitOk && m_bState)
	{
	//	return SetPortVal(nAddr, b, 1);
	}

	return FALSE;
}

BOOL CLaHwControl::GetState()
{
	return m_bState;
}

void CLaHwControl::SetState(BOOL bState)
{
	m_bState = bState;
}

