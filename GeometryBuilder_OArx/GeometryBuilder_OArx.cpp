// GeometryBuilder_OArx.cpp : Defines the initialization routines for the DLL.
//

#include "pch.h"
#include "framework.h"
#include "GeometryBuilder_OArx.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include <tchar.h>
#include "rxregsvc.h"
#include "acutads.h"
#include "accmd.h"
#include "rxregsvc.h"
#include "acutads.h"
#include "aced.h"
#include "acdb.h"
#include <Windows.h>
#include <string.h>
#include <stdlib.h>
#include <aced.h>
#include <dbents.h>
#include <dbsymtb.h>
#include <adslib.h>
#include <geassign.h>
#include <dbidmap.h>
#include <dbapserv.h>
#include <acgi.h>
#include "tchar.h"
#pragma comment(linker,"/EXPORT:acrxEntryPoint")
#pragma comment(linker,"/EXPORT:acrxGetApiVersion")


int acrxEntryPoint(AcRx::AppMsgCode Msg, void* pkt) {
    switch (Msg) {
    case AcRx::kInitAppMsg:
        acrxUnlockApplication(pkt);
        acrxRegisterAppMDIAware(pkt);
        acutPrintf(L"\n Command Loaded");
        break;
    case AcRx::kUnloadAppMsg:
        acutPrintf(_T("\n Command Unloaded"));
        break;
    }
    return AcRx::kRetOK;

}
// CGeometryBuilderOArxApp

BEGIN_MESSAGE_MAP(CGeometryBuilderOArxApp, CWinApp)
END_MESSAGE_MAP()


// CGeometryBuilderOArxApp construction

CGeometryBuilderOArxApp::CGeometryBuilderOArxApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CGeometryBuilderOArxApp object

CGeometryBuilderOArxApp theApp;


// CGeometryBuilderOArxApp initialization

BOOL CGeometryBuilderOArxApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}
