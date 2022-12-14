// CmdDloader.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>

#include "CmdDloader.h"
#include "PipeClient.h"
#include "CfgInfoOpr.h"
#include "DloaderProxy.h"
#include "FindValidDut.h"

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

HANDLE  g_hThread = NULL;
DWORD   g_dwThreadID = 0; 
DWORD	g_dwRetCode	 = DOWNLOAD_OK;
CDloaderProxy   g_cDloaderProxy;
CCfgInfoOpr     g_cCfgOpr;
CFindValidDut   g_cDutMoniter;
DWORD WINAPI WaitExit(LPVOID pParam);
// The one and only application object

CWinApp theApp;

using namespace std;
void ShowUsage()
{
    _tprintf(_T("CmdDloader Usage\n"));
    _tprintf(_T("CmdDloader.exe <-pac file> [-port portnumber] [-ReadNV nvfile]\n"));
    _tprintf(_T("e.g.CmdDloader.exe -pac d:\\test.pac -port 5\n"));
}
DWORD WINAPI WaitExit(LPVOID pParam)
{
    CDloaderProxy* pDloaderProxy = (CDloaderProxy*)pParam;
    if ( NULL == pParam )
    {
        return 1;
    }
    char szBuf[MAX_PATH] = {0};
    while(TRUE)
    {
        std::cin.getline( szBuf, MAX_PATH );
        if (
            stricmp( szBuf, "q" ) == 0     ||
            stricmp( szBuf, "quit" ) == 0  ||
            stricmp( szBuf, "exit" ) == 0 
            )

        {
            pDloaderProxy->ExitDloader();
            _tprintf(_T("User Exit DLoader.\n"));
            break;
        }
        else if( ( GetKeyState( VK_LCONTROL ) & 0x8000 ) && ( GetKeyState( 'C' ) & 0x8000 ) ) 
        {
            printf( "Exit CMD.\n" );
        }
    }
    return 0;
}


void WINAPI RecvDataCB(LPBYTE lpRecvData,DWORD dwDataLen)
{
    if(lpRecvData && dwDataLen >0 )
    {
        CString strInfo(lpRecvData);
		if ( -1 != strInfo.Find(_T("[ERROR]")) )
		{
			g_dwRetCode = DOWNLOAD_ERRDOWNLOAD;
		}
		if (g_cCfgOpr.m_bEZMode)
		{
			printf("%s \n",lpRecvData); 
			fflush(stdout);
			return;
		}
        if (g_cCfgOpr.m_bShowProcess)
        {
            if ( -1 != strInfo.Find(_T("%")) )
            {
                if (dwDataLen == 4)
                {
                    printf("\b\b\b\b\b\b(%s)",lpRecvData);
                } 
            }
            else
            {
                printf("\n%-56s ",lpRecvData);            
            }
        }
        else
        {
            if (-1 == strInfo.Find(_T("%")))
            {
                printf("\n%-56s ",lpRecvData); 
            }
            
        }
		fflush(stdout);
        
    }
}

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
    int nRetCode = DOWNLOAD_ERRPARAM;
    DWORD dwProcID = GetCurrentProcessId();
    // initialize MFC and print and error on failure
    if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
    {
        // TODO: change error code to suit your needs
        _tprintf(_T("Fatal Error: MFC initialization failed\n"));
        return nRetCode;
    }
	// TODO: code your application's behavior here.
	BOOL	bReadFixNV = FALSE;
    DWORD   dwComPort   = 0;
	DWORD   dwMultCount = 1;
    CString strPacFile;
	CString strSaveFixNV;
    BOOL    bKClearDloader = FALSE;
    strPacFile.Empty();

    DWORD dwBaudRate = 115200;
    for(int i= 1; i<  argc; ++i)
    {
        if( 0 == _tcsicmp(argv[i],_T("-pac")) )   
        {
            if((i+1)<argc)
            {
                strPacFile = argv[++i];
            }
        }
		else if( 0 == _tcsicmp(argv[i],_T("-port")) )   
		{
			if((i+1)<argc)
			{
				dwComPort = _ttoi(argv[++i]);
			}
		}
        else if( 0 == _tcsicmp(argv[i],_T("-baudrate")) )
        {
            if((i+1)<argc)
            {
                dwBaudRate = _ttoi(argv[++i]);
            }
        }
		else if( 0 == _tcsicmp(argv[i],_T("-count")) )   
		{
			if((i+1)<argc)
			{
				dwMultCount = _ttoi(argv[++i]);
			}
		}
		else if (0 == _tcsicmp(argv[i],_T("-ReadNV")))
		{
			if((i+1)<argc)
			{
				bReadFixNV = TRUE;
				strSaveFixNV = argv[++i];
				::DeleteFile(strSaveFixNV.operator LPCTSTR());
			}
		}
        else if( 0 == _tcsicmp(argv[i],_T("-c")) )   
        {
            bKClearDloader = TRUE;
        }
    }
    if ( strPacFile.IsEmpty())
    {
        _tprintf(_T("Invalid arguments\n"));

        ShowUsage();
        return DOWNLOAD_ERRPARAM;
    }
    if (!strPacFile.IsEmpty())
    {
        CFileFind finder;

        if (!finder.FindFile(strPacFile.operator LPCTSTR()))
        {
            _tprintf(_T("Invalid arguments,Pac file [%s] don't exist.\n"), strPacFile.operator LPCTSTR());
            ShowUsage();
            return nRetCode;
        }
        finder.Close();
    }

	if ( 0 == dwComPort) // need detect device
	{
		if (!g_cCfgOpr.m_bEZMode)
		{
			printf("Detecting download device ... \n");
			fflush(stdout);
		}
		dwComPort = g_cDutMoniter.FindOneValidDut();
	}

	if ( 0 == dwComPort)
	{
		_tprintf(_T("[Status] fail:Not find valid download devices.\n"));
		return DOWNLOAD_ERR_DETECT_DUT;
	}
	if (g_cCfgOpr.m_bEZMode)
	{
		//_tprintf(_T("[Status] Begin\n"));
	}
	else
	{
		_tprintf(_T("||*****************************************************************||\n"));
		_tprintf(_T("  Pac  : %s \n"),strPacFile.operator LPCTSTR());
		if(DO_PACKET_PORT != dwComPort)
		{
			_tprintf(_T("  Port : %d \n"),dwComPort);
		}
		_tprintf(_T("||*****************************************************************||\n"));

	}
    
    fflush(stdout);
    CPipeClient cPipeClient;

    ///Hide cursor
    CONSOLE_CURSOR_INFO cci;
    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE),&cci);
    cci.bVisible = FALSE;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cci);
    
    if ( !g_cCfgOpr.m_strFlashAppName.IsEmpty())
    {
        CString strBaudrate,strFlashAppCfgFile;
        strBaudrate.Format(_T("%d"),dwBaudRate);
		strFlashAppCfgFile.Format(_T("%s\\%s.ini"),GetModuleFilePath(FALSE),GetStrFileTitle(g_cCfgOpr.m_strFlashAppName));
        ::WritePrivateProfileString( _T("Serial port"),_T("Baud rate"),strBaudrate.operator LPCTSTR(),strFlashAppCfgFile.operator LPCTSTR());

        CString strDloaderApp;
        strDloaderApp.Format(_T("%s\\%s"),GetModuleFilePath(FALSE),g_cCfgOpr.m_strFlashAppName);
        g_cDloaderProxy.SetFlashApp(strDloaderApp);
    }

    if (bKClearDloader)
    {
        g_cDloaderProxy.KillDloader();
    }

    cPipeClient.SetPipeServerName(g_cCfgOpr.m_strPipeServer);
    cPipeClient.SetPipeName(g_cCfgOpr.m_strPipeName);
    cPipeClient.SetRecvCallback(RecvDataCB);
    cPipeClient.Init(dwComPort);
    g_cDloaderProxy.SetFlashArgv(strPacFile,dwComPort,dwMultCount,g_cCfgOpr.m_bEZMode);
	g_cDloaderProxy.EnableReadFixnv(bReadFixNV,strSaveFixNV);
	g_dwRetCode = g_cDloaderProxy.StartDloader() ? DOWNLOAD_OK : DOWNLOAD_ERRLSTARTDLOADER;
  //  g_hThread = CreateThread(NULL,0,WaitExit,(LPVOID)(&g_cDloaderProxy),0,&g_dwThreadID); 

    while ( !g_cDloaderProxy.IsEnd() )
    {
        Sleep(1000);      
    }

    cPipeClient.UnInit();
    ///Show cursor
    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE),&cci);
    cci.bVisible = TRUE;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cci);

  /*  if (g_hThread)
    {

        if( WAIT_OBJECT_0 != WaitForSingleObject(g_hThread,1000) )
        {
            TerminateThread(g_hThread,1);
            //ProcessMgr cProcMgr;
            //cProcMgr.KillXProcess_ByPid(dwProcID);
            _exit(5);

        }
        CloseHandle(g_hThread);
        g_hThread = NULL;
    }*/
	return g_dwRetCode;
}
