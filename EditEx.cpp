// EditEx.cpp : implementation file
//

#include "stdafx.h"
#include "dloader.h"
#include "EditEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditEx

CEditEx::CEditEx()
{
	m_strOldText = _T("");
	m_bReturn = FALSE;
	m_nFmtMode = EX_FMT_NORMAL;
}

CEditEx::~CEditEx()
{
}


BEGIN_MESSAGE_MAP(CEditEx, CEdit)
	//{{AFX_MSG_MAP(CEditEx)
	ON_CONTROL_REFLECT(EN_UPDATE, OnUpdate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditEx message handlers
void CEditEx::OnUpdate() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CEdit::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_UPDATE flag ORed into the lParam mask.
	
    //m_bReturn is true shows this time
    //we enter OnUpdate because we called
    //SetWindowText in the OnUpdate function
    //last time,so we do not to check the content
    if(m_bReturn)
    {
        m_bReturn = false;
        return;
    }

    //Get the original caret pos
    CPoint pt = GetCaretPos();    
    int index = LOWORD(CharFromPos(pt));

    //Get the edit content after you make a change
    CString strText;
    GetWindowText(strText);

	if(m_nFmtMode == EX_FMT_HEX || m_nFmtMode == EX_FMT_DEC)
	{
		//to see whether or not is a heximal number
		if(strText.Left(2).CompareNoCase(_T("0x")))
		{
			//decimal,in this situation only digit
			//is valid
			for(int i=0;i<strText.GetLength();i++)
			{
				if(!isdigit(strText[i]))
				{
					//find invalid char,restore the content
					m_bReturn = true;
					SetWindowText(m_strOldText);
					//because SetWindowText moves the caret
					//to the begining of the text,so we must
					//move it to the right place
					for(int j=0;j<index;j++)
					{
						keybd_event(VK_RIGHT,0,0,0);
						keybd_event(VK_RIGHT,0,KEYEVENTF_KEYUP,0);
					}
					return;
				}
			}
		}
		else
		{
			//heximal,in this situation digit and char a to
			//f is valid
			int l = strText.GetLength();
			if(l > 2)
			{            
				for(int i=2;i<strText.GetLength();i++)
				{
					int c = strText[i];
					if(!isdigit(c))
					{
						c = tolower(c);
						if(c < 'a' || c > 'f')
						{
							//find invalid char,restore the content
							m_bReturn = true;
							SetWindowText(m_strOldText);
							//because SetWindowText moves the caret
							//to the begining of the text,so we must
							//move it to the right place
							for(int j=0;j<index;j++)
							{
								keybd_event(VK_RIGHT,0,0,0);
								keybd_event(VK_RIGHT,0,KEYEVENTF_KEYUP,0);
							}                        
							return;
						}
					}
				}
			}
		}
	}
    
    //the input is valid, so we save a copy
    //of the content
    m_strOldText = strText;
}

void CEditEx::SetFmtMode(UINT nMode)
{
    if(nMode >= EX_FMT_MAX)
        return;     // Invalidate parameter

	m_nFmtMode = nMode;

//     CString strText;
// 	GetWindowText(strText);
//     long n = GetDigit();
// 
//     if(nMode == EX_FMT_HEX)
//     {
//         // To hex mode
//         strText.Format(_T("0x%x"),n);
//     }
//     else // if(nMode == DEC)
//     {
//         strText.Format(_T("%d"),n);
//     }
// 
//     SetWindowText(strText);
}

long CEditEx::GetDigit()
{
    CString strText;
    GetWindowText(strText);

    long lRet;
    if(!strText.Left(2).CompareNoCase(_T("0x")))
    {
        // Hex mode
        _stscanf(strText,_T("%x"),&lRet);
    }
    else
    {
        // Decimal mode
        _stscanf(strText,_T("%d"),&lRet);
    }

    return lRet;
}
