//-----------------------------------------------------------------------------
//----- MyDlg.cpp : implementation file
#include "StdAfx.h"
#include "resource.h"
#include "MyDlg.h"

#include "SecrchGraph.h"

extern CMyDlg *g_pMdlsDlg;

//-----------------------------------------------------------------------------
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//-----------------------------------------------------------------------------
IMPLEMENT_DYNAMIC (CMyDlg, CAcUiDialog)

BEGIN_MESSAGE_MAP(CMyDlg, CAcUiDialog)
//{{AFX_MSG_MAP(CMyDlg)
ON_MESSAGE(WM_ACAD_KEEPFOCUS, OnAcadKeepFocus)    // Needed for modeless dialog.
ON_BN_CLICKED(IDC_BUTTON1, OnHideDlg)
ON_WM_CLOSE()
ON_BN_CLICKED(IDOK, OnCreateLine)
ON_BN_CLICKED(IDCANCEL, OnDelLine)
	ON_BN_CLICKED(IDOK2, OnSelectLine)
	ON_BN_CLICKED(IDCANCEL2, OnSeekPath)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//-----------------------------------------------------------------------------
CMyDlg::CMyDlg (CWnd* pParent /*=NULL*/, HINSTANCE hInstance /*=NULL*/) : 
		CAcUiDialog (CMyDlg::IDD, pParent, hInstance),
			m_xVec(1), m_yVec(1), m_lLineCnt(0)
{
	//{{AFX_DATA_INIT(CMyDlg)
	m_Edit1 = 0;
	m_Edit3 = 0;
	m_Edit2 = 20000;
	m_Edit4 = 20000;
	m_Edit5 = 1000;
	//}}AFX_DATA_INIT

	m_VecSize = 1;
}

void CMyDlg::DoDataExchange (CDataExchange *pDX) {
	CAcUiDialog::DoDataExchange (pDX) ;
	//{{AFX_DATA_MAP(CMyDlg)
	DDX_Control(pDX, IDC_LIST1, m_ListCtr);
	DDX_Text(pDX, IDC_EDIT1, m_Edit1);
	DDV_MinMaxInt(pDX, m_Edit1, 0, 2147483647);
	DDX_Text(pDX, IDC_EDIT3, m_Edit3);
	DDV_MinMaxInt(pDX, m_Edit3, 0, 2147483647);
	DDX_Text(pDX, IDC_EDIT2, m_Edit2);
	DDV_MinMaxInt(pDX, m_Edit2, 0, 2147483647);
	DDX_Text(pDX, IDC_EDIT4, m_Edit4);
	DDV_MinMaxInt(pDX, m_Edit4, 0, 2147483647);
	DDX_Text(pDX, IDC_EDIT5, m_Edit5);
	DDV_MinMaxInt(pDX, m_Edit5, 0, 2147483647);
	//}}AFX_DATA_MAP
}

// Needed for modeless dialogs to keep focus.
// Return FALSE to not keep the focus, return TRUE to keep the focus
LONG CMyDlg::OnAcadKeepFocus(UINT, UINT)
{
	return TRUE;
}

//-----------------------------------------------------------------------------

void CMyDlg::OnHideDlg() 
{
	// TODO: Add your control notification handler code here
	ShowWindow(SW_HIDE);
}

void CMyDlg::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	
	CAcUiDialog::OnClose();
}

BOOL CMyDlg::OnInitDialog() 
{
	CAcUiDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_ListCtr.InsertColumn(1, "线段ID", LVCFMT_LEFT, 80, 0);
	m_ListCtr.InsertColumn(1, "线段长度", LVCFMT_LEFT, 80, 0);
	m_ListCtr.InsertColumn(1, "线段颜色", LVCFMT_LEFT, 80, 0);
	m_ListCtr.SetExtendedStyle(m_ListCtr.GetExtendedStyle() | LVS_EX_GRIDLINES |
								LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP |
								LVS_EX_TWOCLICKACTIVATE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CMyDlg::PostNcDestroy()
{
	// 释放非模态对话框的内存
	delete this;
	if (g_pMdlsDlg != NULL)
	{
		g_pMdlsDlg = NULL;
	}
	CAcUiDialog::PostNcDestroy();
}

void CMyDlg::OnCreateLine() 
{	
	// TODO: Add your control notification handler code here
	acDocManager->lockDocument(curDoc());

	int i;
	int m, n;

	char *buf = (char*)malloc(20);
	
	AcDbObjectId LineId;
	AcDbLine *pLine = NULL;
	//块表
	AcDbBlockTable *pTb = NULL;
	//块表记录
	AcDbBlockTableRecord *pTbr = NULL;
	//层表
	AcDbLayerTable *pLyr = NULL;
	//层表记录
	AcDbLayerTableRecord* pLyrr = NULL;
	//图形数据库
	AcDbDatabase *pDb = acdbHostApplicationServices()->workingDatabase();
	
	Acad::ErrorStatus es;
	
	CListCtrl *pListCtr = (CListCtrl *)GetDlgItem( IDC_LIST1 );
	
	//设置随机数种子
	srand(time(NULL));
	
	//更新数据
	UpdateData(TRUE);

	m_VecSize += m_Edit5;
	m_xVec.resize(m_VecSize);
	m_yVec.resize(m_VecSize);

	//acutPrintf("%d\n", m_Edit1);
	
	es = pDb->getLayerTable(pLyr, AcDb::kForWrite);	//以写的方式打开层表
	es = pDb->getBlockTable(pTb, AcDb::kForRead);	//以读的方式打开块表
	es = pTb->getAt(ACDB_MODEL_SPACE, pTbr, AcDb::kForWrite);//以写的方式打开块表记录
	
	//创建图层GalLineTest
	if (!pLyr->has("GalLineTest"))
	{
		pLyrr = new AcDbLayerTableRecord;	//创建层记录
		pLyrr->setName("GalLineTest");		//设置名字
		//pLyrr->setColor(color);			//设置颜色
		//pLyrr->setLineWeight(lnWt);		//设置线宽
		
		if (Acad::eOk != pLyr->add(pLyrr))	//添加该层记录到层表
		{
			//添加失败
			delete pLyrr;		//释放内存
			pLyr->close();		//关闭层表
		}
		//关闭层表记录
		pLyrr->close();
	}
	//关闭层表
	pLyr->close();
	
	//生成点坐标
	for (i = 0; i < m_Edit5; ++i)
	{
		if ((m_Edit2 != 0) && (m_Edit4 != 0))
		{
			m_xVec[i] = rand() % m_Edit2 + m_Edit1;
			m_yVec[i] = rand() % m_Edit4 + m_Edit3;
		}
		else
		{
			m_xVec[i] = 0;
			m_yVec[i] = 0;
		}
	}
	
	//遍历点坐标
	for (m = 0; m < m_Edit5; ++m)
	{
		for (n = (m+1); n < m_Edit5; ++n)
		{
			if ((m_xVec[m] != m_xVec[n]) && (m_yVec[m] != m_yVec[n]))
			{
				AcGePoint3d ptStart(m_xVec[m], m_yVec[m], 0);
				AcGePoint3d ptEnd(m_xVec[n], m_yVec[n], 0);
				pLine = new AcDbLine(ptStart, ptEnd);
				//pLine->setColor();
				pLine->setLayer("GalLineTest");
				//创建线段
				es = pTbr->appendAcDbEntity(LineId, pLine);
				if (Acad::eOk == es)
				{
					++m_lLineCnt;
				}
				//acutPrintf("%d\n", LineId);
				sprintf(buf, "%d", LineId);			
				m_ListCtr.InsertItem(m_Row, buf);
				
				buf = itoa(pLine->colorIndex(), buf, 10);
				m_ListCtr.SetItemText(m_Row, 1, buf);
				
				buf = itoa(LineLength(m_xVec[m], m_yVec[m],
									m_xVec[n], m_yVec[n]), buf, 10);
				m_ListCtr.SetItemText(m_Row, 2, buf);
				
				++m_Row;
				pLine->close();	//关闭实体
			}
		}
	}
	pTbr->close();	//关闭块表记录
	pTb->close();	//关闭块表

	acDocManager->unlockDocument(curDoc());
	free(buf);
}

int CMyDlg::LineLength(int x1, int y1, int x2, int y2)
{
	return	sqrt(pow((x1 - x2), 2) + pow((y1 - y2), 2));
}

void CMyDlg::OnDelLine() 
{
	// TODO: Add your control notification handler code here
	int nItem;
	long lId;
	CString str;
	AcDbObjectId LineId;
	AcDbEntity *pEnt = NULL;
	AcDbLine *pLine = NULL;
	Acad::ErrorStatus es;

	acDocManager->lockDocument(curDoc());
	//在列表中删除选定行
	while(m_ListCtr.GetNextItem(-1, (LVNI_ALL | LVNI_SELECTED)) != -1)
	{
		nItem = m_ListCtr.GetNextItem(-1, (LVNI_ALL | LVNI_SELECTED));
		str = m_ListCtr.GetItemText(nItem, 0);
		//acutPrintf("%s\n", str);
		lId = atol(str);
		LineId.setFromOldId(lId);
		//获得指针
		es = acdbOpenAcDbEntity(pEnt, LineId, AcDb::kForWrite);
		//在模型空间删除相应实体
		pLine = AcDbLine::cast(pEnt);
		if (pLine)
		{
			pLine->erase();
			pLine->close();
			m_ListCtr.DeleteItem(nItem);
			--m_Row;
		}	
	}
	UpdateData(FALSE);
	
	acDocManager->unlockDocument(curDoc());	
}

void CMyDlg::UpdateListCtr()
{
	acDocManager->lockDocument(curDoc());
	int nCount = m_ListCtr.GetItemCount();
	acutPrintf("%d\n", nCount);
	long lId;
	CString str;
	AcDbObjectId LineId;
	AcDbEntity *pEnt = NULL;
	AcDbLine *pLine = NULL;
	Acad::ErrorStatus es;
	
	if (nCount > 0)
	{
			for (int i = 0; i < nCount; ++i)
			{
				str = m_ListCtr.GetItemText(i, 0);
				lId = atol(str);
				LineId.setFromOldId(lId);
				//获得指针
				es = acdbOpenAcDbEntity(pEnt, LineId, AcDb::kForWrite);

				//检查是否被删除
				pLine = AcDbLine::cast(pEnt);
				if (es == Acad::eWasErased)
				{
					m_ListCtr.DeleteItem(i);
					--m_lLineCnt;
					--i;
					--m_Row;
				} 
				//检查颜色
				else if (es == Acad::eOk)
				{
					if (pLine)
					{
						str = m_ListCtr.GetItemText(i, 1);
						Adesk::UInt16 usColor = (Adesk::UInt16)atoi(str);
						if (pLine->colorIndex() != usColor)
						{
							char *buf = (char*)malloc(20);
							itoa((int)pLine->colorIndex(), buf, 10);
							m_ListCtr.SetItemText(i, 1, buf);
							free(buf);
						}
						pLine->close();
					}
				}						
			}
	}
	
	UpdateData(FALSE);
	acDocManager->unlockDocument(curDoc());
}

void CMyDlg::OnSelectLine()
{
	// TODO: Add your control notification handler code here
	ads_name adsNameSet, adsName;
	AcDbObjectId objId;
	int ret = 0;
	long lSelCnt = 0;
	AcDbEntity *pEnt = NULL;
	int nCount = m_ListCtr.GetItemCount();
	long lId;
	CString str;
	AcDbObjectId LineId;

	acDocManager->lockDocument(curDoc());

	ShowWindow(SW_HIDE);
	ret = acedSSGet(NULL, NULL, NULL, NULL, adsNameSet);
	
	if (RTNORM == ret)
	{
		m_ListCtr.SetFocus();
		acedSSLength(adsNameSet, &lSelCnt);
		for (long i = 0; i < lSelCnt; ++i)
		{
			acedSSName(adsNameSet, i, adsName);
			acdbGetObjectId(objId, adsName);
			if (nCount > 0)
			{
				for (int i = 0; i < nCount; ++i)
				{
					str = m_ListCtr.GetItemText(i, 0);
					lId = atol(str);
					LineId.setFromOldId(lId);
					if (LineId == objId)
					{				
						m_ListCtr.SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
					}
				}
			}			
		}
		ShowWindow(SW_SHOW);
	}

	acedSSFree(adsName);
//	acutRelRb(stResBuf);
	acDocManager->unlockDocument(curDoc());
}

void CMyDlg::OnSeekPath()
{
	// TODO: Add your control notification handler code here

	acDocManager->lockDocument(curDoc());

	ads_point fromPnt, toPnt;
	CSearchGraph sGraph;

	ShowWindow(SW_HIDE);

	sGraph.Init();

	bool flag1 = false;
	bool flag2 = false;

	if (m_lLineCnt > 0)
	{
		do 
		{
			if (RTNORM == acedGetPoint(NULL, _T("选择一个点\n"), fromPnt))
			{		
				//选择的点必须是线段的起点或终点、或两线段的交点
				for (vector<stPoint>::iterator it = sGraph.m_Points.begin();
				it != sGraph.m_Points.end(); ++it)
				{
					if ((fromPnt[X] == (*it).pnt[X]) &&
						(fromPnt[Y] == (*it).pnt[Y]))
					{
						flag1 = true;
						break;
					}
				}
			}
			if (!flag1)
			{
				acutPrintf("选择的点必须是线段的起点或终点、或两线段的交点\n");
			}
		} while (!flag1);

		do 
		{
			if (RTNORM == acedGetPoint(fromPnt, _T("选择第二个点\n"), toPnt))
			{
				for (vector<stPoint>::iterator it = sGraph.m_Points.begin();
				it != sGraph.m_Points.end(); ++it)
				{
					if ((toPnt[X] == (*it).pnt[X]) &&
						(toPnt[Y] == (*it).pnt[Y]))
					{
						flag2 = true;
						break;
					}
				}
			}
			if (!flag1)
			{
				acutPrintf("选择的点必须是线段的起点或终点、或两线段的交点\n");		
			}
		} while (!flag2);
	}

//	SearchPath();

	acDocManager->unlockDocument(curDoc());

}


