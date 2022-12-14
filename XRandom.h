// XRandom.h: interface for the CXRandom class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XRANDOM_H__C1BC4EE1_E7B9_473B_8E5D_FB80CE1DDA1A__INCLUDED_)
#define AFX_XRANDOM_H__C1BC4EE1_E7B9_473B_8E5D_FB80CE1DDA1A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef HCRYPTPROV
typedef unsigned long HCRYPTPROV;
#endif

class CXRandom  
{
public:
	CXRandom();
	virtual ~CXRandom();
	BYTE GetRandomByte(void);
	int  GetRandomNumber(void);

	void GetRandomBytes(LPBYTE pBuf, DWORD dwSize);
	void GetRandomNumbers(LPBYTE pBuf, DWORD dwSize);

private:
    HCRYPTPROV m_hProv;
};

#endif // !defined(AFX_XRANDOM_H__C1BC4EE1_E7B9_473B_8E5D_FB80CE1DDA1A__INCLUDED_)
