#if !defined(AFX_NVBACKUPOPTLIST_H__1E401A66_75FF_404F_8D83_EEE15CA974A3__INCLUDED_)
#define AFX_NVBACKUPOPTLIST_H__1E401A66_75FF_404F_8D83_EEE15CA974A3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NvBackupOptList.h : header file
//
#include "SuperGridCtrl.h"
#include "BMAGlobal.h"
/////////////////////////////////////////////////////////////////////////////
// CNvBackupOptList window

typedef struct _NV_BACKUP_INFO_T
{
    PNV_BACKUP_ITEM_T   pNvBkpItmArray;
    int                 nNvBkpItmCount;
    BOOL                bBackNV;
    int                 nNvBkpItmChkCount;
    TCHAR               szNotChkItemName[MAX_PATH*4];
    _NV_BACKUP_INFO_T()
    {
        memset(this,0,sizeof(_NV_BACKUP_INFO_T));
    }
    void Clear()
    {
        if( NULL != pNvBkpItmArray)
        {
            delete []pNvBkpItmArray;
            pNvBkpItmArray = NULL;
        }
        memset(this,0,sizeof(_NV_BACKUP_INFO_T));
    }
}NV_BACKUP_INFO_T,*PNV_BACKUP_INFO_T;

typedef CMap<CString, LPCTSTR, PNV_BACKUP_INFO_T, PNV_BACKUP_INFO_T>   MAP_NV_BAK; 
class CNvBackupOptList : public CSuperGridCtrl
{
// Construction
public:
	CNvBackupOptList();
	void Init();
    void InitNvBackupInfo(PPRODUCT_INFO_T pCurProduct);
    int  GetNvBkpItemCount(LPCTSTR lpNvID);
    PNV_BACKUP_ITEM_T GetNvBkpItemInfo(int nIndex,LPCTSTR lpNvID);
    BOOL IsBackupNV(LPCTSTR lpszNVID);
    CString GetTmpNotChkItemName();

// Attributes
public:
	PNV_BACKUP_ITEM_T   m_pTempNvBkpItmArray;
	int                 m_nTempNvBkpItmCount;
	BOOL                m_bBackNV;
	BOOL                m_bTempBackNV;

    MAP_NV_BAK          m_mapNvBkpItm;
    MAP_NV_BAK          m_mapTempNvBkpItm;
    CString             m_strCurID;

     
// Operations
public:
    void AddNvBackupItemInfo(PNV_BACKUP_ITEM_T pNvBkpItmInfo);
	BOOL SaveSettings(LPCTSTR lpszIniFile);
	void Update();
	void UpdateTemp();
    void FillList(LPCTSTR lpNVID = NULL,BOOL bChangeItem = FALSE);
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNvBackupOptList)
	//}}AFX_VIRTUAL
	int GetIcon(const CTreeItem* pItem);
	COLORREF GetCellRGB(void);
	CImageList * CreateDragImageEx(int nItem);
    void OnControlLButtonDown(UINT nFlags, CPoint point, LVHITTESTINFO& ht);
	BOOL OnItemExpanding(CTreeItem *pItem, int iItem);
	BOOL OnItemExpanded(CTreeItem* pItem, int iItem);
	BOOL OnCollapsing(CTreeItem *pItem);
	BOOL OnItemCollapsed(CTreeItem *pItem);
	BOOL OnItemLButtonDown(LVHITTESTINFO& ht);
	BOOL OnVkReturn(void);
	BOOL OnDeleteItem(CTreeItem* pItem, int nIndex);
// Implementation
public:
	virtual ~CNvBackupOptList();
    
	// Generated message map functions
protected:
	//{{AFX_MSG(CNvBackupOptList)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
    afx_msg void OnItemCheck(NMHDR *pNMHDR, LRESULT *pResult);
	DECLARE_MESSAGE_MAP()
    void UpdatedNvBkpMap(MAP_NV_BAK& mapNvBkpItm, LPCTSTR lpID,NV_BACKUP_INFO_T* pNVBakInfo);
    void ClearMapNvBakItems(MAP_NV_BAK& mapNvBkpItm);

private:
	CImageList *m_pImageList;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NVBACKUPOPTLIST_H__1E401A66_75FF_404F_8D83_EEE15CA974A3__INCLUDED_)
