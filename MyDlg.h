//-----------------------------------------------------------------------------
#if !defined(ARX__MYDLG_H__20120105_145616)
#define ARX__MYDLG_H__20120105_145616

//-----------------------------------------------------------------------------
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//-----------------------------------------------------------------------------

#include <cmath>
#include <vector>
#include <dbents.h>
using namespace std;

class CMyDlg : public CAcUiDialog {
	DECLARE_DYNAMIC (CMyDlg)

public:
	CMyDlg (CWnd* pParent =NULL, HINSTANCE hInstance =NULL) ;

	//{{AFX_DATA(CMyDlg)
	enum { IDD = IDD_DIALOG1 };
	CListCtrl	m_ListCtr;
	int		m_Edit1;
	int		m_Edit3;
	int		m_Edit2;
	int		m_Edit4;
	int		m_Edit5;
	int		m_Row;
	//}}AFX_DATA

	void PostNcDestroy();
	void UpdateListCtr();

	long m_VecSize;
	vector<int> m_xVec;
	vector<int> m_yVec;
	long m_lLineCnt;

	//{{AFX_VIRTUAL(CMyDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CMyDlg)
	afx_msg LONG OnAcadKeepFocus(UINT, UINT);
	afx_msg void OnHideDlg();
	afx_msg void OnClose();
	virtual BOOL OnInitDialog();
	afx_msg void OnCreateLine();
	afx_msg void OnDelLine();
	afx_msg void OnSelectLine();
	afx_msg void OnSeekPath();
	//}}AFX_MSG

	int LineLength(int x1, int y1, int x2, int y2);

	DECLARE_MESSAGE_MAP()
} ;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //----- !defined(ARX__MYDLG_H__20120105_145616)
