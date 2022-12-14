// NumericEdit.cpp : implementation file
//

#include "stdafx.h"
#include "NumericEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNumericEdit

CNumericEdit::CNumericEdit():m_strOldText((TCHAR)0,0)
{
    m_bReturn = FALSE;
	m_nMode = DEC;
}

CNumericEdit::~CNumericEdit()
{
}


BEGIN_MESSAGE_MAP(CNumericEdit, CEdit)
	//{{AFX_MSG_MAP(CNumericEdit)
	ON_CONTROL_REFLECT(EN_UPDATE, OnUpdate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNumericEdit message handlers

void CNumericEdit::OnUpdate() 
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

    //to see whether or not is a heximal number
    //if(strText.Left(2).CompareNoCase(_T("0x")))
	if(m_nMode==DEC)
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
        if(l > 0)
        {            
            for(int i=0;i<strText.GetLength();i++)
            {
                int c = strText[i];
                if(!isdigit(c) || i==8)
                {
                    c = tolower(c);
                    if((c < 'a' || c > 'f') || i==8)
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
    
    //the input is valid, so we save a copy
    //of the content
    m_strOldText = strText;
}

void CNumericEdit::SetDispMode(int nMode)
{
    if(nMode != HEX && nMode != DEC)
        return;     // Invalidate parameter

    CString strText;
    long n = GetDigit();
	m_nMode = nMode;

    if(nMode == HEX)
    {
        // To hex mode
        strText.Format(_T("%x"),n);
    }
    else // if(nMode == DEC)
    {
        strText.Format(_T("%d"),n);
    }

    SetWindowText(strText);
}

long CNumericEdit::GetDigit()
{
    CString strText;
    GetWindowText(strText);

    long lRet;
    //if(!strText.Left(2).CompareNoCase(_T("0x")))
	if(m_nMode==HEX)
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
