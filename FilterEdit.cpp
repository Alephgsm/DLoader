// FilterEdit.cpp : implementation file
//

#include "stdafx.h"
#include "FilterEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFilterEdit

CFilterEdit::CFilterEdit()
{
}

CFilterEdit::~CFilterEdit()
{
}


BEGIN_MESSAGE_MAP(CFilterEdit, CEdit)
	//{{AFX_MSG_MAP(CFilterEdit)
	ON_WM_CHAR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFilterEdit message handlers

void CFilterEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	if (   isdigit(nChar) 
        || (nChar >= _T('a') && nChar <= _T('z')) 
	    || (nChar >= _T('A') && nChar <= _T('Z')) 
		|| (nChar == 8)	)
    {
	    CEdit::OnChar(nChar, nRepCnt, nFlags);
    }
    else
    {
        ::MessageBeep(MB_ICONEXCLAMATION);
    }
}
