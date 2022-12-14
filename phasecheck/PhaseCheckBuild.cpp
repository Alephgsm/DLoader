// PhaseCheckBuild.cpp: implementation of the CPhaseCheckBuild class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PhaseCheckBuild.h"


//
#define T_SECT_VERSION          ( "VERSION" )
#define T_KEY_MAGIC             ( "MAGIC NUMBER" )
#define T_SECT_STATION          ( "STATION" )
#define T_KEY_STATION_NUM       ( "STATION NUMBER" )
#define T_KEY_STATION           ( "STATION" )
#define T_SECT_STATE_FLAG       ( "STATE FLAG" )
#define T_KEY_PASS_VALUE        ( "PASS VALUE" )

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPhaseCheckBuild::CPhaseCheckBuild()
{
    ZeroMemory(&m_phaseBuf05, sizeof(m_phaseBuf05));
    ZeroMemory(&m_phaseBuf09, sizeof(m_phaseBuf09));
    ZeroMemory(&m_phaseBuf15, sizeof(m_phaseBuf15));
    m_eMagic = SP09;
}

CPhaseCheckBuild::~CPhaseCheckBuild()
{

}

BOOL CPhaseCheckBuild::LoadConfigFile()
{
    char szIniPath[MAX_PATH] = {0};
    GetModuleFileNameA(NULL, szIniPath, MAX_PATH);
    char *pPos = strrchr(szIniPath, '\\');
    if (NULL == pPos)
    {
        return FALSE;
    }
    else
    {
        *pPos = 0;
        strcat(szIniPath, "\\PhaseCheck.ini");
    }

    // Version: Magic
    char szMagic[20] = {0};
    GetPrivateProfileStringA(T_SECT_VERSION, T_KEY_MAGIC, "SP09", szMagic, sizeof(szMagic), szIniPath);
    if (0 == stricmp(szMagic, "SP09"))
    {
        int passValue = GetPrivateProfileIntA(T_SECT_STATE_FLAG, T_KEY_PASS_VALUE, 0, szIniPath);
        
        // SP09
        m_eMagic = SP09;
    //  Xiaoping.Jing, 2010-06-11: [[[
    //  为了减少Flash擦除次数，此处将结构的测试标示位默认初始化为0xFF
        ZeroMemory(&m_phaseBuf09, sizeof(m_phaseBuf09));
        if (0 == passValue)
        {
            m_phaseBuf09.SignFlag   = PASS_0_FAIL_1;
            m_phaseBuf09.iTestSign  = 0xFFFF;
            m_phaseBuf09.iItem      = 0x7FFF;
        }
        else
        {
            m_phaseBuf09.SignFlag   = PASS_1_FAIL_0;
        }
        strcpy(m_phaseBuf09.szLastFailDescription, "PASS");
    //  ]]]

        m_phaseBuf09.Magic      = SP09_SPPH_MAGIC_NUMBER;    
        m_phaseBuf09.StationNum = (BYTE)GetPrivateProfileIntA(T_SECT_STATION, T_KEY_STATION_NUM, 5, szIniPath);
        if (   0 == m_phaseBuf09.StationNum 
            || m_phaseBuf09.StationNum > SP09_MAX_STATION_NUM ) 
        {        
            return FALSE;
        }

        for (int i=0; i<m_phaseBuf09.StationNum; i++)
        {
            char szKey[20] = {0};
            sprintf(szKey, "%s %d", T_KEY_STATION, i+1);
        
            GetPrivateProfileStringA(T_SECT_STATION, szKey, "", m_phaseBuf09.StationName[i], SP09_MAX_STATION_NAME_LEN-1, szIniPath);
        }

    }
    else if (0 == stricmp(szMagic, "SP15")) // Bug: 449541
    {
        int passValue = GetPrivateProfileIntA(T_SECT_STATE_FLAG, T_KEY_PASS_VALUE, 0, szIniPath);
        
        // SP15
        m_eMagic = SP15;
        ZeroMemory(&m_phaseBuf15, sizeof(m_phaseBuf15));
        if (0 == passValue)
        {
            m_phaseBuf15.SignFlag   = PASS_0_FAIL_1;
            m_phaseBuf15.iTestSign  = (unsigned long)-1;
            m_phaseBuf15.iItem      = (unsigned long)-1;
        }
        else
        {
            m_phaseBuf15.SignFlag   = PASS_1_FAIL_0;
        }
        strcpy(m_phaseBuf15.szLastFailDescription, "PASS");
        //  ]]]
        
        m_phaseBuf15.Magic      = SP15_SPPH_MAGIC_NUMBER;    
        m_phaseBuf15.StationNum = (BYTE)GetPrivateProfileIntA(T_SECT_STATION, T_KEY_STATION_NUM, 5, szIniPath);
        if (   0 == m_phaseBuf15.StationNum 
            || m_phaseBuf15.StationNum > SP15_MAX_STATION_NUM ) 
        {        
            return FALSE;
        }
        
        for (int i=0; i<m_phaseBuf15.StationNum; i++)
        {
            char szKey[20] = {0};
            sprintf(szKey, "%s %d", T_KEY_STATION, i+1);
            
            GetPrivateProfileStringA(T_SECT_STATION, szKey, "", m_phaseBuf15.StationName[i], SP15_MAX_STATION_NAME_LEN-1, szIniPath);
        }
        
    }
    else if (0 == stricmp(szMagic, "SP05"))
    {
        // SP05
        m_eMagic = SP05;
        ZeroMemory(&m_phaseBuf05, sizeof(m_phaseBuf05));

        m_phaseBuf05.header.Magic = SP05_SPPH_MAGIC_NUMBER;
        m_phaseBuf05.header.StationNum = (BYTE)GetPrivateProfileIntA(T_SECT_STATION, T_KEY_STATION_NUM, 5, szIniPath);
        if (   0 == m_phaseBuf05.header.StationNum 
            || m_phaseBuf05.header.StationNum > SP05_MAX_SUPPORT_STATION ) 
        {        
            return FALSE;
        }

        for (int i=0; i<m_phaseBuf05.header.StationNum; i++)
        {
            char szKey[20] = {0};
            sprintf(szKey, "%s %d", T_KEY_STATION, i+1);
        
            GetPrivateProfileStringA(T_SECT_STATION, szKey, "", m_phaseBuf05.items[i].TestStationName, SP05_TSN_LEN-1, szIniPath);
        }
    }
    else
    {
        // Invalid magic
        return FALSE;
    }

    return TRUE;
}

BOOL CPhaseCheckBuild::CnstPhaseInfo(const char *sn, LPBYTE lpData, int nDataLen, int *pnRetLen)
{
    if (NULL == lpData || IsBadWritePtr(lpData, nDataLen))
    {
        //
        return FALSE;
    }

    if ( !LoadConfigFile() )
    {
        //
        return FALSE;
    }

    if (SP09 == m_eMagic)
    {
        if ( nDataLen < SP09_MAX_PHASE_BUFF_SIZE )
        {
            return FALSE;
        }

        if (NULL != sn)
        {
            int nInpSnLen = lstrlenA(sn);
            ZeroMemory((void *)&m_phaseBuf09.SN1, SP09_MAX_SN_LEN); // NEWMS00178259
            memcpy(m_phaseBuf09.SN1, sn, (nInpSnLen > (SP09_MAX_SN_LEN-1)) ? (SP09_MAX_SN_LEN-1) : nInpSnLen);
        }

        if (NULL != pnRetLen)
        {
            *pnRetLen = SP09_MAX_PHASE_BUFF_SIZE;
        }

        memcpy(lpData, &m_phaseBuf09, SP09_MAX_PHASE_BUFF_SIZE);
    }
    else if (SP15 == m_eMagic) // Bug: 449541
    {
        if ( nDataLen < SP15_MAX_PHASE_BUFF_SIZE )
        {
            return FALSE;
        }
        
        if (NULL != sn)
        {
            int Len = lstrlenA(sn);
            ZeroMemory((void *)&m_phaseBuf15.SN1, SP15_MAX_SN_LEN); 
            ZeroMemory((void *)&m_phaseBuf15.SN2, SP15_MAX_SN_LEN); 
            memcpy(m_phaseBuf15.SN1, sn, (Len > (SP15_MAX_SN_LEN-1)) ? (SP15_MAX_SN_LEN-1) : Len);
        }
        
        if (NULL != pnRetLen)
        {
            *pnRetLen = SP15_MAX_PHASE_BUFF_SIZE;
        }
        
        memcpy(lpData, &m_phaseBuf15, SP15_MAX_PHASE_BUFF_SIZE);
    }
    else
    {
        if ( nDataLen < SP05_MAX_PHASE_BUFF_SIZE )
        {
            return FALSE;
        }

        if (NULL != sn)
        {
            int nInpSnLen = lstrlenA(sn);
            ZeroMemory((void *)&m_phaseBuf05.header.SN, SP05_SN_LEN); // NEWMS00178259
            memcpy(m_phaseBuf05.header.SN, sn, (nInpSnLen > (SP05_SN_LEN-1)) ? (SP05_SN_LEN-1) : nInpSnLen);
        }

        if (NULL != pnRetLen)
        {
            *pnRetLen = SP05_MAX_PHASE_BUFF_SIZE;
        }

        memcpy(lpData, &m_phaseBuf05, SP05_MAX_PHASE_BUFF_SIZE);
    }
    

    return TRUE;
}

BOOL CPhaseCheckBuild::CnstPhaseInfo(const char *sn1, const char *sn2, LPBYTE lpData, int nDataLen, int *pnRetLen)
{
    if (NULL == lpData || IsBadWritePtr(lpData, nDataLen))
    {
        //
        return FALSE;
    }

    if ( !LoadConfigFile() )
    {
        //
        return FALSE;
    }

    if (SP09 == m_eMagic)
    {
        if ( nDataLen < SP09_MAX_PHASE_BUFF_SIZE )
        {
            return FALSE;
        }

        if (NULL != sn1)
        {
            int nInpSnLen = lstrlenA(sn1);
            ZeroMemory((void *)&m_phaseBuf09.SN1, SP09_MAX_SN_LEN); // NEWMS00178259
            memcpy(m_phaseBuf09.SN1, sn1, (nInpSnLen > (SP09_MAX_SN_LEN-1)) ? (SP09_MAX_SN_LEN-1) : nInpSnLen);
        }

        if (NULL != sn2)
        {
            int nInpSnLen = lstrlenA(sn2);
            ZeroMemory((void *)&m_phaseBuf09.SN2, SP09_MAX_SN_LEN); // NEWMS00178259
            memcpy(m_phaseBuf09.SN2, sn2, (nInpSnLen > (SP09_MAX_SN_LEN-1)) ? (SP09_MAX_SN_LEN-1) : nInpSnLen);
        }

        if (NULL != pnRetLen)
        {
            *pnRetLen = SP09_MAX_PHASE_BUFF_SIZE;
        }


        memcpy(lpData, &m_phaseBuf09, SP09_MAX_PHASE_BUFF_SIZE);
    }
    else if (SP15 == m_eMagic) // Bug: 449541
    {
        if ( nDataLen < SP15_MAX_PHASE_BUFF_SIZE )
        {
            return FALSE;
        }
        
        if (NULL != sn1)
        {
            int Len = lstrlenA(sn1);
            ZeroMemory((void *)&m_phaseBuf15.SN1, SP15_MAX_SN_LEN); 
            memcpy(m_phaseBuf15.SN1, sn1, (Len > (SP15_MAX_SN_LEN-1)) ? (SP15_MAX_SN_LEN-1) : Len);
        }
        
        if (NULL != sn2)
        {
            int Len = lstrlenA(sn2);
            ZeroMemory((void *)&m_phaseBuf15.SN2, SP15_MAX_SN_LEN); 
            memcpy(m_phaseBuf15.SN2, sn2, (Len > (SP15_MAX_SN_LEN-1)) ? (SP15_MAX_SN_LEN-1) : Len);
        }
        
        if (NULL != pnRetLen)
        {
            *pnRetLen = SP15_MAX_PHASE_BUFF_SIZE;
        }
        
        memcpy(lpData, &m_phaseBuf15, SP15_MAX_PHASE_BUFF_SIZE);
    }
    else
    {
        if ( nDataLen < SP05_MAX_PHASE_BUFF_SIZE )
        {
            return FALSE;
        }

        if (NULL != sn1)
        {
            int nInpSnLen = lstrlenA(sn1);
            ZeroMemory((void *)&m_phaseBuf05.header.SN,  SP05_SN_LEN); // NEWMS00178259
            memcpy(m_phaseBuf05.header.SN,  sn1,  (nInpSnLen > (SP05_SN_LEN-1)) ? (SP05_SN_LEN-1) : nInpSnLen);
        }

        if (NULL != sn2)
        {
            int nInpSnLen = lstrlenA(sn2);
            ZeroMemory((void *)&m_phaseBuf05.header.SN2, SP05_SN_LEN); // NEWMS00178259
            memcpy(m_phaseBuf05.header.SN2, sn2,  (nInpSnLen > (SP05_SN_LEN-1)) ? (SP05_SN_LEN-1) : nInpSnLen);
        }

        if (NULL != pnRetLen)
        {
            *pnRetLen = SP05_MAX_PHASE_BUFF_SIZE;
        }

        
        memcpy(lpData, &m_phaseBuf05, SP05_MAX_PHASE_BUFF_SIZE);
    }
    

    return TRUE;
}

BOOL CPhaseCheckBuild::Cnst8KBuffer(const char *sn/*IN*/, LPBYTE lpData/*OUT*/, int nDataLen)
{
    if (NULL == lpData || nDataLen < MAX_PRODUCTIONINFO_SIZE || IsBadWritePtr(lpData, nDataLen))
    {
        //
        return FALSE;
    }
  
    memset(lpData, 0xFF, nDataLen);

    if ( !CnstPhaseInfo(sn, lpData, nDataLen, NULL) )
    {
        //
        return FALSE;
    }
    
    //  Xiaoping: 严格判断DOWNLOAD站位名称，如果有DOWNLOAD站，则设置PASS[[[ 
    if (SP09 == m_eMagic)
    {
        for (int nIndex=0; nIndex<m_phaseBuf09.StationNum; nIndex++)
        {
            if (0 == stricmp(m_phaseBuf09.StationName[nIndex], "DOWNLOAD"))
            {
                if (PASS_0_FAIL_1 == m_phaseBuf09.SignFlag)
                {
                    // 0: pass, 0: tested
                    m_phaseBuf09.iItem     &= (USHORT)(0x7FFF & (~(USHORT)(1<<nIndex)));
                    m_phaseBuf09.iTestSign &= (USHORT)(0xFFFF & (~(USHORT)(1<<nIndex)));  //标志已经测试
                }
                else
                {
                    // 1: pass, 1: tested
                    m_phaseBuf09.iItem     |= (1 << nIndex);
                //  m_phaseBuf09.iTestSign |= (1 << nIndex);  //标志已经测试
                }


            //  Xiaoping.Jing, 2010-07-05, 修正Download下载完成之后没有将Download标志设置为PASS的BUG [[[
                memcpy(lpData, &m_phaseBuf09, SP09_MAX_PHASE_BUFF_SIZE);
            //  ]]]

                break;
            }
        }
    }
    else if (SP15 == m_eMagic)
    {
        //
        for (int nIndex=0; nIndex<m_phaseBuf15.StationNum; nIndex++)
        {
            if (0 == stricmp(m_phaseBuf15.StationName[nIndex], "DOWNLOAD"))
            {
                if (PASS_0_FAIL_1 == m_phaseBuf15.SignFlag)
                {
                    // 0: pass, 0: tested
                    m_phaseBuf15.iItem     &= (ULONG)(0xFFFFFFFF & (~(ULONG)(1<<nIndex)));
                    m_phaseBuf15.iTestSign &= (ULONG)(0xFFFFFFFF & (~(ULONG)(1<<nIndex)));  //标志已经测试
                }
                else
                {
                    // 1: pass, 1: tested
                    m_phaseBuf15.iItem     |= (1 << nIndex);
                }
               
                memcpy(lpData, &m_phaseBuf15, SP15_MAX_PHASE_BUFF_SIZE);
                break;
            }
        }
    }
    else
    {
        for (int nIndex=0; nIndex<m_phaseBuf05.header.StationNum; nIndex++)
        {
            if (0 == stricmp(m_phaseBuf05.items[nIndex].TestStationName, "DOWNLOAD"))
            {
                memcpy(m_phaseBuf05.items[nIndex].TestState, "PASS", 4);

            //  Xiaoping.Jing, 2010-07-05, 修正Download下载完成之后没有将Download标志设置为PASS的BUG [[[
                memcpy(lpData, &m_phaseBuf05, SP05_MAX_PHASE_BUFF_SIZE);
            //  ]]]

                break;
            }
        }
    }
    //  ]]]


    return TRUE;
}

BOOL CPhaseCheckBuild::FindSnFrom8K(const BYTE *lpData, int nDataLen, BYTE *sn, int nSnLen)
{
#define  CONVERT_INT(Src,Dst)   {\
                                 (Dst)  = MAKELONG(MAKEWORD(HIBYTE(HIWORD(Src)),LOBYTE(HIWORD(Src))),\
                                                   MAKEWORD(HIBYTE(LOWORD(Src)),LOBYTE(LOWORD(Src))));\
                                }


    if (   NULL == lpData 
        || nDataLen < MAX_PRODUCTIONINFO_SIZE
        || IsBadReadPtr(lpData, nDataLen)
       )
    {
        //
        return FALSE;
    }

    if (SP05_SPPH_MAGIC_NUMBER == *((unsigned int *)lpData))
	{			
		LPSP05_PHASE_CHECK_T pPhaseCheckData = (LPSP05_PHASE_CHECK_T)lpData;
		memcpy(sn, pPhaseCheckData->header.SN, nSnLen>SP05_SN_LEN ?SP05_SN_LEN:nSnLen);

        return TRUE;
	}
	else if (SP09_SPPH_MAGIC_NUMBER == *((unsigned int *)lpData))
	{
    /*********************************************************************** 
        Lookup the latest data block
        block size must be equal or multiple than SP09_MAX_PHASE_BUFF_SIZE
      
    **********************************************************************/
    
        const int BLOCK_SIZE  = SP09_MAX_PHASE_BUFF_SIZE;
        int nTotalBlockNum = MAX_PRODUCTIONINFO_SIZE/BLOCK_SIZE;
        LPBYTE lpBuff = (LPBYTE)(lpData+MAX_PRODUCTIONINFO_SIZE-BLOCK_SIZE);
        for (int nIndex=0; nIndex<nTotalBlockNum-1; nIndex++)
        {
            
            if (SP09_SPPH_MAGIC_NUMBER == *((unsigned int *)lpBuff))
            {
                // Find the last data block
                break;
            }
            
            lpBuff -= BLOCK_SIZE;
        }
        
        LPSP09_PHASE_CHECK_T lpPhaseInfo = (LPSP09_PHASE_CHECK_T)(lpBuff);
        memcpy(sn, lpPhaseInfo->SN1, nSnLen>SP09_MAX_SN_LEN?SP09_MAX_SN_LEN:nSnLen);
        
        return TRUE;
    }
    else if (SP15_SPPH_MAGIC_NUMBER == *((unsigned int *)lpData)) // Bug: 449541
	{
        const int BLOCK_SIZE  = SP15_MAX_PHASE_BUFF_SIZE;
        int nTotalBlockNum = MAX_PRODUCTIONINFO_SIZE/BLOCK_SIZE;
        LPBYTE lpBuff = (LPBYTE)(lpData+MAX_PRODUCTIONINFO_SIZE-BLOCK_SIZE);
        for (int nIndex=0; nIndex<nTotalBlockNum-1; nIndex++)
        {       
            if (SP15_SPPH_MAGIC_NUMBER == *((unsigned int *)lpBuff))
            {
                // Find the last data block
                break;
            }
            
            lpBuff -= BLOCK_SIZE;
        }
        
        LPSP15_PHASE_CHECK_T lpPhaseInfo = (LPSP15_PHASE_CHECK_T)(lpBuff);
        memcpy(sn, lpPhaseInfo->SN1, nSnLen>SP15_MAX_SN_LEN?SP15_MAX_SN_LEN:nSnLen);
        
        return TRUE;
    }
    


#if 0
		DWORD dwIndex = *((DWORD*)(lpData+(MAX_PRODUCTIONINFO_SIZE-4)));		
		CONVERT_INT(dwIndex, dwIndex);

		int nIndex = 0;

		/************************************************************************/
		//  if dwIndex == 0xFFFFFFFF, nIndex = 0
		//  if dwIndex == 0xFFFFFFFE, nIndex = 1                                
		/************************************************************************/
	
		if (dwIndex != 0xFFFFFFFF)
		{
			for(int i = 0; i< 32; i++)
			{
				nIndex = ((dwIndex & (1<<(31-i)) )>> (31-i));
				if(nIndex == 0)
				{
					nIndex = (32-i);
					break;
				}
			}
		}
		// the range of  nIndex is [0,30], the last one 31 is not used.
		if (nIndex >= 0 && nIndex <= 30)
		{
			LPSP09_PHASE_CHECK_T pPhaseCheckData = (LPSP09_PHASE_CHECK_T)lpData;
			pPhaseCheckData += nIndex;
			memcpy(sn, pPhaseCheckData->SN1, nSnLen);

            return TRUE;
		}

	}
    else
    {
        return FALSE;
    }
#endif

    return FALSE;
}