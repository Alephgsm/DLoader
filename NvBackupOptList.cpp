// NvBackupOptList.cpp : implementation file
//

#include "stdafx.h"
#include "dloader.h"
#include "NvBackupOptList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNvBackupOptList

CNvBackupOptList::CNvBackupOptList()
{
	m_pImageList            = NULL;
	m_pTempNvBkpItmArray    = NULL;
	m_nTempNvBkpItmCount    = 0;
	m_bBackNV               = TRUE;
	m_bTempBackNV           = TRUE;
    m_strCurID.Empty();
}


CNvBackupOptList::~CNvBackupOptList()
{
/*lint -save -e1551 */
	if(NULL != m_pImageList)
	{
		m_pImageList->DeleteImageList();
		delete m_pImageList;
		m_pImageList = NULL;
	}
    ClearMapNvBakItems(m_mapNvBkpItm);
    ClearMapNvBakItems(m_mapTempNvBkpItm);
/*lint -restore */
}

void CNvBackupOptList::ClearMapNvBakItems(MAP_NV_BAK& mapNvBkpItm)
{
    CString strID;
    PNV_BACKUP_INFO_T  pStruct = NULL;
    POSITION pos = mapNvBkpItm.GetStartPosition();
    while( pos )
    {
        mapNvBkpItm.GetNextAssoc( pos, strID, pStruct );
        if (pStruct)
        {
            pStruct->Clear();
            SAFE_DELETE(pStruct);
        }
    }
    mapNvBkpItm.RemoveAll();
}

BEGIN_MESSAGE_MAP(CNvBackupOptList, CSuperGridCtrl)
	//{{AFX_MSG_MAP(CNvBackupOptList)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	ON_NOTIFY_REFLECT(NM_CLICK, OnItemCheck)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNvBackupOptList message handlers

int CNvBackupOptList::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CSuperGridCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO: Add your specialized creation code here
    m_pImageList = new CImageList;

	if(!m_pImageList->Create(IDB_BMP_FOLDER,16,1,RGB(0, 255, 255)))
		return -1;
	
	SetImageList(m_pImageList, LVSIL_SMALL);
	CImageList *pImageList = GetImageList(LVSIL_SMALL);
	if(pImageList)
		ImageList_GetIconSize(pImageList->m_hImageList, &m_cxImage, &m_cyImage);
	else
		return -1;

	Init();
	
	return 0;
}
void CNvBackupOptList::Init()
{
	SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_CHECKBOXES);
	CString strClmn;
	VERIFY( strClmn.LoadString(IDS_NV_BKP_OPTION_COLUMN) );
		
	int     nLen   = strClmn.GetLength();
	LPTSTR  lpBuf  = strClmn.GetBuffer(nLen);
	LPTSTR  lpFind = _tcschr(lpBuf, _T(','));
	int     nIndex = 0;
	while(lpFind != NULL)
	{
		*lpFind = _T('\0');
		InsertColumn(nIndex++,lpBuf,LVCFMT_LEFT,308);
		lpBuf = lpFind + 1;
		lpFind = _tcschr(lpBuf, _T(','));
	}
	InsertColumn(nIndex, lpBuf, LVCFMT_LEFT, 80 );
}
void CNvBackupOptList::InitNvBackupInfo(PPRODUCT_INFO_T pCurProduct)
{
    if (NULL == pCurProduct)
    {
        return;
    }
    ClearMapNvBakItems(m_mapNvBkpItm);
    ClearMapNvBakItems(m_mapTempNvBkpItm);
    m_pTempNvBkpItmArray    = NULL;
    m_nTempNvBkpItmCount    = 0;
    m_strCurID.Empty();

    FILE_INFO_T* pFileInfoArr   = pCurProduct->pFileInfoArr; 
    int nCount                  = pCurProduct->dwFileCount;
    BOOL bIsFirstNVID           = TRUE;
    m_bBackNV                   = FALSE;
    m_bTempBackNV               = FALSE;
    for(int i = 0; i< nCount; ++i)
    {
        FILE_INFO_T * pCur = pFileInfoArr + i;
        if((pCur->isBackup == 255 || pCur->isBackup == 1) && _tcsnicmp(pCur->szID,_T("NV"),2) == 0)
        {                
            NV_BACKUP_INFO_T* pNVBakInfo = new NV_BACKUP_INFO_T;
            if (pNVBakInfo)
            {
                pNVBakInfo->pNvBkpItmArray = new NV_BACKUP_ITEM_T[pCurProduct->dwNvBackupItemCount];
                if (pNVBakInfo->pNvBkpItmArray)
                {
                    if(bIsFirstNVID)
                    {
                        m_strCurID      = pCur->szID;
                        bIsFirstNVID    = FALSE;
                    }
                    pNVBakInfo->nNvBkpItmCount = pCurProduct->dwNvBackupItemCount;
                    memcpy( pNVBakInfo->pNvBkpItmArray,pCurProduct->paNvBackupItem, sizeof(NV_BACKUP_ITEM_T)*(pCurProduct->dwNvBackupItemCount) );
                    
                    //Updated wIsBackup
                    PNV_BACKUP_ITEM_T   pNvBkpItmArray  = pNVBakInfo->pNvBkpItmArray;
                    int nCount                          = pNVBakInfo->nNvBkpItmCount;
                    int nNvBkpItmChkCount               = 0;
                    CString strNotChkItemName;
                    for( int i = 0; i< nCount; ++i)
                    {
                        CString strBakIDScene = pNvBkpItmArray[i].szScene;
                        CString strFind(pCur->szID);
                        WORD    wIsBackup = FALSE;
                        strFind += _T(",");
                        if ( 
                            pNvBkpItmArray[i].wIsBackup    &&
                            ( 
                                strBakIDScene.IsEmpty()  ||
                                ( !strBakIDScene.IsEmpty() && -1 != strBakIDScene.Find(strFind.operator LPCTSTR()) )
                            )
                            )
                        {
                            wIsBackup = TRUE;   
                        }
                        pNvBkpItmArray[i].wIsBackup = wIsBackup;

                        if (pNvBkpItmArray[i].wIsBackup)
                        {
                            ++nNvBkpItmChkCount;
                        }
                        else
                        {
                            if ( strBakIDScene.IsEmpty()  ||
                                ( !strBakIDScene.IsEmpty() && -1 != strBakIDScene.Find(strFind.operator LPCTSTR()) ))
                            {
                                strNotChkItemName += pNvBkpItmArray[i].szItemName;
                                strNotChkItemName += _T(",");
                            }  
                        }
                    }

                    
                    if(!strNotChkItemName.IsEmpty())
                    {
                        strNotChkItemName.TrimRight(_T(','));
                    }
                    //////////////////////////////////////////////////////////////////////////
                    
                    
                    pNVBakInfo->bBackNV             = nNvBkpItmChkCount ? TRUE : FALSE;
                    pNVBakInfo->nNvBkpItmChkCount   = nNvBkpItmChkCount;

                    _tcscpy(pNVBakInfo->szNotChkItemName,strNotChkItemName.operator LPCTSTR());
                    //_tcsncpy(pNVBakInfo->szNotChkItemName,strNotChkItemName.operator LPCTSTR(),sizeof(pNVBakInfo->szNotChkItemName)-sizeof(TCHAR));

                    if (nNvBkpItmChkCount)
                    {
                        m_bBackNV                   = TRUE;
                        m_bTempBackNV               = TRUE;
                    }  
                    
                    UpdatedNvBkpMap(m_mapNvBkpItm,pCur->szID,pNVBakInfo);
                    UpdatedNvBkpMap(m_mapTempNvBkpItm,pCur->szID,pNVBakInfo);
                    pNVBakInfo->Clear();
                    SAFE_DELETE(pNVBakInfo);
                }
                else
                {
                    SAFE_DELETE(pNVBakInfo);
                }
            }
        }
    }

    if (!m_strCurID.IsEmpty())
    {
        NV_BACKUP_INFO_T* pTempNvBkpInfo = NULL;
        if (m_mapTempNvBkpItm.Lookup(m_strCurID, pTempNvBkpInfo) && pTempNvBkpInfo)
        {
            m_pTempNvBkpItmArray = pTempNvBkpInfo->pNvBkpItmArray;
            m_nTempNvBkpItmCount = pTempNvBkpInfo->nNvBkpItmCount;
        }
    }
}
void CNvBackupOptList::UpdatedNvBkpMap(MAP_NV_BAK& mapNvBkpItm,LPCTSTR lpID,NV_BACKUP_INFO_T* pNVBakItemInfo)
{	
    if (NULL == pNVBakItemInfo || NULL == lpID || 0 == _tcslen(lpID))
    {
        return;
    }
    NV_BACKUP_INFO_T* pTempNVBakInfo;
    CString strID(lpID);
    if( mapNvBkpItm.Lookup(strID, pTempNVBakInfo) && pTempNVBakInfo )
    {		
        pTempNVBakInfo->Clear();
        SAFE_DELETE(pTempNVBakInfo);
        mapNvBkpItm.RemoveKey( strID );
    }

    NV_BACKUP_INFO_T* pNVBakInfo = new NV_BACKUP_INFO_T;
    if (pNVBakInfo)
    {
        memcpy(pNVBakInfo,pNVBakItemInfo,sizeof(NV_BACKUP_INFO_T));
        pNVBakInfo->pNvBkpItmArray = new NV_BACKUP_ITEM_T[pNVBakItemInfo->nNvBkpItmCount];
    }

    if (NULL == pNVBakInfo || NULL == pNVBakInfo->pNvBkpItmArray )
    {
        SAFE_DELETE(pNVBakInfo);
        return;
    }
    memcpy( pNVBakInfo->pNvBkpItmArray,pNVBakItemInfo->pNvBkpItmArray, sizeof(NV_BACKUP_ITEM_T)*(pNVBakItemInfo->nNvBkpItmCount) );
    mapNvBkpItm.SetAt(strID,pNVBakInfo);

}

//override called when OnLButtondown
void CNvBackupOptList::OnControlLButtonDown(UINT nFlags, CPoint point, LVHITTESTINFO& ht)
{
	UNUSED_ALWAYS(nFlags);
	UNUSED_ALWAYS(point);
	UNUSED_ALWAYS(ht);
	return;
}

BOOL CNvBackupOptList::OnItemExpanding(CTreeItem *pItem, int iItem)
{
	UNUSED_ALWAYS(iItem);
	CItemInfo* lp = GetData(pItem);
	if(lp!= NULL && lp->GetCheck())
		return TRUE;
	return FALSE;
}


BOOL CNvBackupOptList::OnItemExpanded(CTreeItem* pItem, int iItem)
{
	UNUSED_ALWAYS(pItem);
	UNUSED_ALWAYS(iItem);
	return TRUE;
}


BOOL CNvBackupOptList::OnCollapsing(CTreeItem *pItem)
{
	UNUSED_ALWAYS(pItem);
	return TRUE;
}

BOOL CNvBackupOptList::OnItemCollapsed(CTreeItem *pItem)
{
	UNUSED_ALWAYS(pItem);
	return TRUE;
}


BOOL CNvBackupOptList::OnItemLButtonDown(LVHITTESTINFO& ht)
{
	UNUSED_ALWAYS(ht);
	return TRUE;
}

BOOL CNvBackupOptList::OnVkReturn()
{
	return TRUE;
}

BOOL CNvBackupOptList::OnDeleteItem(CTreeItem* pItem, int nIndex)
{
	UNUSED_ALWAYS(pItem);
	UNUSED_ALWAYS(nIndex);
	return 0;
}

COLORREF CNvBackupOptList::GetCellRGB()
{
	return RGB(192,0,0);
}

CImageList *CNvBackupOptList::CreateDragImageEx(int nItem)
{
	UNUSED_ALWAYS(nItem);
	return NULL;
}

int CNvBackupOptList::GetIcon(const CTreeItem* pItem)
{
	if(pItem!=NULL)
	{
		int n = GetData(pItem)->GetImage();
		if(n!=-1)
			return n;
		
		int iImage = 0;
		if(ItemHasChildren(pItem))
		{
			IsCollapsed(pItem) ? iImage = 1/*close icon*/:iImage = 0;/*open icon*/
		}
		else
			iImage = 2;//doc icon
		return iImage;
	}
	return 0;
}
void CNvBackupOptList::AddNvBackupItemInfo(PNV_BACKUP_ITEM_T pNvBkpItmInfo)
{
	ASSERT(pNvBkpItmInfo != NULL);
	if(pNvBkpItmInfo == NULL)
	{
		return;
	}

	CItemInfo* lp = new CItemInfo();
	lp->SetCheck(pNvBkpItmInfo->wIsBackup);
	lp->SetItemText(pNvBkpItmInfo->szItemName);	
	//add subitem text
	CString strValue;
	if(pNvBkpItmInfo->dwID != 0xFFFFFFFF)
		strValue.Format(_T("0x%X"),pNvBkpItmInfo->dwID);
	else
		strValue = _T("");
	lp->AddSubItemText(strValue);      // 0 zero based subitems...
    //Create root item
	CTreeItem * pRoot = InsertRootItem(lp);//previous on N.Y.P.D we call it CreateTreeCtrl(lp)
	if( pRoot == NULL )
	{
		/*lint -save -e429*/
		return;
		/*lint -restore*/
	}
	//insert items	
	int nSubCount = pNvBkpItmInfo->dwFlagCount;
	if(pNvBkpItmInfo->wIsUseFlag)
	{	
		CString strText;
		for(int i=0; i < nSubCount; i++)
		{
			CItemInfo* lpItemInfo = new CItemInfo();
			//add items text
			//lpItemInfo->SetItemText(pNvBkpItmInfo->nbftArray[i].szFlagName);
			if(_tcscmp(pNvBkpItmInfo->nbftArray[i].szFlagName,_T("Replace"))==0)
			{
				//strText.LoadString(IDS_NV_BKP_FLAG_REPLACE);
				strText.Format(IDS_NV_BKP_FLAG_REPLACE,pNvBkpItmInfo->szItemName);
			}
			else
			{
				//strText.LoadString(IDS_NV_BKP_FLAG_CONTINUE);
				strText.Format(IDS_NV_BKP_FLAG_CONTINUE,pNvBkpItmInfo->szItemName);
			}
		
			
			lpItemInfo->SetItemText(strText);
			lpItemInfo->SetCheck(pNvBkpItmInfo->nbftArray[i].dwCheck);
			//lpItemInfo->SetCheck(1);
			//insert the iteminfo with ParentPtr
			InsertItem(pRoot, lpItemInfo);
		/*lint -save -e429*/
		}
		/*lint -restore*/
	}
//	Expand(pRoot, 0 /*listview index 0*/); 
//	UINT uflag = LVIS_SELECTED | LVIS_FOCUSED;
//	SetItemState(0, uflag, uflag);

/*lint -save -e429*/
}/*lint -restore*/

BOOL CNvBackupOptList::SaveSettings(LPCTSTR lpszIniFile)
{
	UNUSED_ALWAYS(lpszIniFile);
	return TRUE;
}
BOOL CNvBackupOptList::IsBackupNV(LPCTSTR lpszNVID)
{
    BOOL bBackUp = FALSE;
    NV_BACKUP_INFO_T* pNvBkpInfo    = NULL;
    if (m_mapNvBkpItm.Lookup(lpszNVID, pNvBkpInfo) && pNvBkpInfo)
    {
        bBackUp = pNvBkpInfo->bBackNV;
    }
    return bBackUp;
}
void CNvBackupOptList::Update()
{
    ClearMapNvBakItems(m_mapNvBkpItm); 
    CString strID;
    PNV_BACKUP_INFO_T  pStruct  = NULL;
    m_bBackNV                   = FALSE;
    POSITION pos = m_mapTempNvBkpItm.GetStartPosition();
    while( pos )
    {
        m_mapTempNvBkpItm.GetNextAssoc( pos, strID, pStruct );
        if (pStruct)
        {
            NV_BACKUP_INFO_T* pNVBakInfo = new NV_BACKUP_INFO_T;
            if (pNVBakInfo)
            {
                memcpy(pNVBakInfo,pStruct,sizeof(NV_BACKUP_INFO_T));
                pNVBakInfo->pNvBkpItmArray = new NV_BACKUP_ITEM_T[pStruct->nNvBkpItmCount];
            }

            if (NULL == pNVBakInfo || NULL == pNVBakInfo->pNvBkpItmArray )
            {
                SAFE_DELETE(pNVBakInfo);
            }
            else
            {
                memcpy( pNVBakInfo->pNvBkpItmArray,pStruct->pNvBkpItmArray, sizeof(NV_BACKUP_ITEM_T)*(pStruct->nNvBkpItmCount) );
                m_mapNvBkpItm.SetAt(strID,pNVBakInfo);
                //set m_bBackNV
                if (pNVBakInfo->bBackNV && !m_bBackNV)
                {
                    m_bBackNV = TRUE;
                }        
            }
        }
    }

}

CString CNvBackupOptList::GetTmpNotChkItemName()
{
    CString strTmpNotChkItemName;
    CString  strID;
    PNV_BACKUP_INFO_T  pStruct  = NULL;
    POSITION pos                = m_mapTempNvBkpItm.GetStartPosition();
    while( pos )
    {
        m_mapTempNvBkpItm.GetNextAssoc( pos, strID, pStruct );
        if (pStruct)
        {
            if (_tcslen(pStruct->szNotChkItemName))
            {
                strTmpNotChkItemName = pStruct->szNotChkItemName;
                break;
            }
            
        }
    }
    return strTmpNotChkItemName;
}
void CNvBackupOptList::UpdateTemp()
{
    CString  strID;
    PNV_BACKUP_INFO_T  pStruct  = NULL;
    m_bTempBackNV               = FALSE;
    POSITION pos                = m_mapTempNvBkpItm.GetStartPosition();
    while( pos )
    {
        m_mapTempNvBkpItm.GetNextAssoc( pos, strID, pStruct );
        if (pStruct)
        {
            int nNvBkpItmChkCount = 0;
            CString strNotChkItemName;
            for (int i = 0; i<pStruct->nNvBkpItmCount; ++i)
            {
                PNV_BACKUP_ITEM_T   pNvBkpItm = pStruct->pNvBkpItmArray + i;
                if (pNvBkpItm->wIsBackup)
                {
                    ++nNvBkpItmChkCount;
                    m_bTempBackNV = TRUE;
                }
                else
                {
                    strNotChkItemName += pNvBkpItm->szItemName;
                    strNotChkItemName += _T(",");
                }
  
            }
            if(!strNotChkItemName.IsEmpty())
            {
                strNotChkItemName.TrimRight(_T(','));
            }
            pStruct->bBackNV           = nNvBkpItmChkCount ? TRUE : FALSE;
            pStruct->nNvBkpItmChkCount = nNvBkpItmChkCount;
            //_tcsncpy(pStruct->szNotChkItemName,strNotChkItemName.operator LPCTSTR(),sizeof(pStruct->szNotChkItemName)-sizeof(TCHAR));
            _tcscpy(pStruct->szNotChkItemName,strNotChkItemName.operator LPCTSTR());
            //set m_bTempBackNV
            if (pStruct->bBackNV && !m_bTempBackNV)
            {
                m_bTempBackNV = TRUE;
            }  
        }
    }
}
void CNvBackupOptList::OnItemCheck(NMHDR *pNMHDR, LRESULT *pResult)
{

    LPNMITEMACTIVATE   lpnmitem   =   (LPNMITEMACTIVATE)pNMHDR;
    CTreeItem * pTreeItem = GetTreeItem(lpnmitem->iItem);
    ASSERT(pTreeItem != NULL);
    CItemInfo* lp = GetData(pTreeItem);
    if(lp!=NULL && !lp->GetCheck() && ItemHasChildren(pTreeItem))
    {
        Collapse(pTreeItem);
    }
 
    BOOL bParentItem = TRUE;
    CTreeItem *pParent = pTreeItem;
    int nIndex =  GetRootIndex(pTreeItem);
    if (-1 == nIndex)
    {
        bParentItem = FALSE;
        pParent = GetParentItem(pTreeItem);
        nIndex = GetRootIndex(pParent);
    }

    if (
            -1 == nIndex                    ||
            NULL == m_pTempNvBkpItmArray    ||
            nIndex >= m_nTempNvBkpItmCount
        )
    {
        return;
    }
    
    if (bParentItem || ItemHasChildren(pTreeItem))
    {  
        if(lp->GetCheck())
        {
            m_pTempNvBkpItmArray[nIndex].wIsBackup = TRUE;
        }
        else
        {
            m_pTempNvBkpItmArray[nIndex].wIsBackup = FALSE;
        }
    }
    else
    {
        int nParentIndex = GetCurIndex(pParent);
        int nChildIndex = GetCurIndex(pTreeItem);
        int nChild = nChildIndex - nParentIndex -1;
        lp = GetData(pTreeItem);
        if(lp->GetCheck())
        {
            m_pTempNvBkpItmArray[nIndex].nbftArray[nChild].dwCheck = TRUE;
        }
        else
        {
            m_pTempNvBkpItmArray[nIndex].nbftArray[nChild].dwCheck = FALSE;
        }
    }
    *pResult = 0;

}

void CNvBackupOptList::FillList(LPCTSTR lpNVID /*= NULL*/,BOOL bChangeItem /*= FALSE*/)
{
	this->DeleteAll();
    CString strNVID(lpNVID);
    if (!strNVID.IsEmpty())
    {
        m_strCurID = lpNVID;
    }
    
    int i = 0;
    NV_BACKUP_INFO_T* pTempNvBkpInfo    = NULL;
    if (bChangeItem)
    {
        if (m_mapTempNvBkpItm.Lookup(m_strCurID, pTempNvBkpInfo) && pTempNvBkpInfo)
        {
            m_pTempNvBkpItmArray = pTempNvBkpInfo->pNvBkpItmArray;
            m_nTempNvBkpItmCount = pTempNvBkpInfo->nNvBkpItmCount;
            if(m_pTempNvBkpItmArray != NULL)
            {
                for( i = 0; i<m_nTempNvBkpItmCount; ++i)
                {
                    AddNvBackupItemInfo(m_pTempNvBkpItmArray+i);
                }
            }
        }
    }
    else
    {  
        NV_BACKUP_INFO_T* pNvBkpInfo        = NULL;
        if (m_mapNvBkpItm.Lookup(m_strCurID, pNvBkpInfo) && pNvBkpInfo)
        {
            PNV_BACKUP_ITEM_T pNvBkpItmArray = pNvBkpInfo->pNvBkpItmArray;
            if(pNvBkpItmArray != NULL)
            {
                for( i = 0; i<pNvBkpInfo->nNvBkpItmCount; ++i)
                {
                    AddNvBackupItemInfo(pNvBkpItmArray+i);
                }
            }

            //Updated mapTempNvBkp
            UpdatedNvBkpMap(m_mapTempNvBkpItm,m_strCurID.operator LPCTSTR(),pNvBkpInfo);
        }

        if (m_mapTempNvBkpItm.Lookup(m_strCurID, pTempNvBkpInfo) && pTempNvBkpInfo)
        {
            m_pTempNvBkpItmArray = pTempNvBkpInfo->pNvBkpItmArray;
            m_nTempNvBkpItmCount = pTempNvBkpInfo->nNvBkpItmCount;
        }
    }
    

}

int  CNvBackupOptList::GetNvBkpItemCount(LPCTSTR lpNvID)
{
    int nCount                      = 0;
    NV_BACKUP_INFO_T* pNvBkpInfo    = NULL;
    if (m_mapNvBkpItm.Lookup(lpNvID, pNvBkpInfo) && pNvBkpInfo)
    {
        nCount = pNvBkpInfo->nNvBkpItmCount;
    }

    return nCount;
}
PNV_BACKUP_ITEM_T CNvBackupOptList::GetNvBkpItemInfo(int nIndex,LPCTSTR lpNvID)
{
    PNV_BACKUP_ITEM_T pNvBkpItm     = NULL;
    NV_BACKUP_INFO_T* pNvBkpInfo    = NULL;
    if (m_mapNvBkpItm.Lookup(lpNvID, pNvBkpInfo) && pNvBkpInfo)
    {
        pNvBkpItm = pNvBkpInfo->pNvBkpItmArray+nIndex;
    }
    return pNvBkpItm;
}