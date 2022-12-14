// DLoaderDoc.h : interface of the CDLoaderDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_DLOADERDOC_H__F1B9566C_90B1_4F2D_BCAC_AF58C160BD32__INCLUDED_)
#define AFX_DLOADERDOC_H__F1B9566C_90B1_4F2D_BCAC_AF58C160BD32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CDLoaderDoc : public CDocument
{
protected: // create from serialization only
	CDLoaderDoc();
	DECLARE_DYNCREATE(CDLoaderDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDLoaderDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDLoaderDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CDLoaderDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLOADERDOC_H__F1B9566C_90B1_4F2D_BCAC_AF58C160BD32__INCLUDED_)
