#pragma once

class CLaHwControl
{
public:
	CLaHwControl(WORD nInitAddr, BYTE bInit);
	~CLaHwControl();

	BOOL OutByte(WORD nAddr, BYTE b);
	BOOL GetState();
	void SetState(BOOL bState);

private:
	BOOL m_bInitOk;
	BOOL m_bState;
};
