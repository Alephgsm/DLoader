// XRandom.cpp: implementation of the CXRandom class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "XRandom.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#ifndef _lint
#include "WINCRYPT.H" 
#pragma comment(lib, "Advapi32.lib")
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CXRandom::CXRandom()
{
	m_hProv = NULL;
#ifndef _lint 
	VERIFY(CryptAcquireContext(&m_hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT));
#endif
}

CXRandom::~CXRandom()
{
    if (NULL != m_hProv)
    {
#ifndef _lint 
        CryptReleaseContext(m_hProv, 0);
#endif
        m_hProv = NULL;
    }
}

BYTE CXRandom::GetRandomByte()
{
    BYTE buff = 0; 
#ifndef _lint 
	VERIFY(CryptGenRandom(m_hProv, 1, &buff));
#endif
    return buff;
}

int CXRandom::GetRandomNumber()
{
    BYTE buff = 0; 
#ifndef _lint 
    VERIFY(CryptGenRandom(m_hProv, 1, &buff));
#endif
    return buff%10;
}

void CXRandom::GetRandomBytes(LPBYTE pBuf, DWORD dwSize)
{
	if(pBuf == NULL)
		return;
#ifndef _lint 
	VERIFY(CryptGenRandom(m_hProv, dwSize, pBuf));
#endif
}

void CXRandom::GetRandomNumbers(LPBYTE pBuf, DWORD dwSize)
{
	if(pBuf == NULL)
		return;

	for(UINT i= 0; i< dwSize; i++)
	{
		pBuf[i] = (BYTE)GetRandomNumber();
	}
}