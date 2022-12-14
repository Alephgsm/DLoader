// DLoaderDoc.cpp : implementation of the CDLoaderDoc class
//

#include "stdafx.h"
#include "DLoader.h"

#include "DLoaderDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDLoaderDoc

IMPLEMENT_DYNCREATE(CDLoaderDoc, CDocument)

BEGIN_MESSAGE_MAP(CDLoaderDoc, CDocument)
	//{{AFX_MSG_MAP(CDLoaderDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDLoaderDoc construction/destruction

CDLoaderDoc::CDLoaderDoc()
{
	// TODO: add one-time construction code here

}

CDLoaderDoc::~CDLoaderDoc()
{
}

BOOL CDLoaderDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CDLoaderDoc serialization

void CDLoaderDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDLoaderDoc diagnostics

#ifdef _DEBUG
void CDLoaderDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CDLoaderDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDLoaderDoc commands
