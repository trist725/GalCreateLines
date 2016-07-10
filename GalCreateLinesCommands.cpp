//
// ObjectARX defined commands, created by  [2012/1/5], , 

#include "StdAfx.h"
#include "StdArx.h"
#include "resource.h"

#include "MyDlg.h"

extern CMyDlg *g_pMdlsDlg;

//-----------------------------------------------------------------------------
// This is command 'GALCREATELINES, by  [2012/1/5], , 
void GalCreateLinesBASICGalCreateLines()
{
#ifdef OARXWIZDEBUG
	acutPrintf ("\nOARXWIZDEBUG - GalCreateLinesBASICGalCreateLines() called.");
#endif // OARXWIZDEBUG

	// TODO: Implement the command
	CAcModuleResourceOverride resOverride;

	// 以非模态方式启动对话框
	if (g_pMdlsDlg == NULL)
	{
		g_pMdlsDlg = new CMyDlg(acedGetAcadFrame());
		g_pMdlsDlg->Create(IDD_DIALOG1);
		g_pMdlsDlg->ShowWindow(SW_SHOW);
	}
	else
	{
		g_pMdlsDlg->UpdateListCtr();	//同步listcontrol与模型空间数据
		g_pMdlsDlg->ShowWindow(SW_SHOW);
	}
	
}





