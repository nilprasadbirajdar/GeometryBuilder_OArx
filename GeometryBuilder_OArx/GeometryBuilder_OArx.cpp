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

void createRectangle(AcDbPolyline* pPolyline, AcGePoint2d& pVertex1, AcGePoint2d& pVertex2, AcGePoint2d& pVertex3, AcGePoint2d pVertex4)
{
    pPolyline->addVertexAt(0, pVertex1, 0, 0, 0);
    pPolyline->addVertexAt(1, pVertex2, 0, 0, 0);
    pPolyline->addVertexAt(2, pVertex3, 0, 0, 0);
    pPolyline->addVertexAt(3, pVertex4, 0, 0, 0);
    pPolyline->addVertexAt(4, pVertex1, 0, 0, 0);
    // Close the polyline (make it a closed loop)
    pPolyline->setClosed(Adesk::kTrue);
}

//Function for creating Rectangle using vertex and rectangle in autocad
static void rectangleUsingVertexAndCenter()
{
    // Get the current space block
    AcDbDatabase* pDb = acdbHostApplicationServices()->workingDatabase();
    AcDbBlockTableRecord* pBlockTableRecord;
    Acad::ErrorStatus es = acdbOpenObject(pBlockTableRecord, pDb->currentSpaceId(), AcDb::kForWrite);

    // Create 2 variables of the old point data type
    ads_point pInVertex1, pInCenter;
    // Prompt for the first point
    if (RTNORM == acedGetPoint(NULL, _T("\nSpecify the center point: "), pInCenter))
    {  
        AcGePoint2d pCenter(pInCenter[0], pInCenter[1]);
        // Prompt for the second point
        double pXDistance, pYDistance;
        if (RTNORM == acedGetPoint(pInCenter, _T("\nSpecify center point: "), pInVertex1))
        {
            AcGePoint2d pVertex1(pInVertex1[0], pInVertex1[1]);
            AcDbPolyline* pPolyline = new AcDbPolyline();
            if ((pVertex1[0] < pCenter[0]) && (pInVertex1[1] != pInCenter[1]))
            {
                if (pVertex1[1] < pCenter[1])
                {
                    pXDistance = pInCenter[0] - pInVertex1[0];
                    pYDistance = pInCenter[1] - pInVertex1[1];
                    AcGePoint2d pVertex3(pInCenter[0] + pXDistance, pInCenter[1] + pYDistance);
                    AcGePoint2d pVertex4(pVertex1[0], pVertex3[1]);
                    AcGePoint2d pVertex2(pVertex3[0], pVertex1[1]);
                    createRectangle(pPolyline, pVertex1, pVertex2, pVertex3, pVertex4);
                }
                else
                {
                    pXDistance = pInCenter[0] - pInVertex1[0];
                    pYDistance = pInVertex1[1] - pInCenter[1];
                    AcGePoint2d pVertex3(pInCenter[0] + pXDistance, pInCenter[1] - pYDistance);
                    AcGePoint2d pVertex2(pVertex1[0], pVertex3[1]);
                    AcGePoint2d pVertex4(pVertex3[0], pVertex1[1]);
                    createRectangle(pPolyline, pVertex1, pVertex2, pVertex3, pVertex4);
                }
            }
            else if ((pVertex1[0] > pCenter[0]) && (pInVertex1[1] != pInCenter[1]))
            {
                if (pVertex1[1] > pCenter[1])
                {
                    pXDistance = pInVertex1[0] - pInCenter[0];
                    pYDistance = pInVertex1[1] - pInCenter[1];
                    AcGePoint2d pVertex3(pInCenter[0] - pXDistance, pInCenter[1] - pYDistance);
                    AcGePoint2d pVertex4(pVertex1[0], pVertex3[1]);
                    AcGePoint2d pVertex2(pVertex3[0], pVertex1[1]);
                    createRectangle(pPolyline, pVertex1, pVertex2, pVertex3, pVertex4);
                }
                else
                {
                    pXDistance = pInVertex1[0] - pInCenter[0];
                    pYDistance = pInCenter[1] - pInVertex1[1];
                    AcGePoint2d pVertex3(pInCenter[0] - pXDistance, pInCenter[1] + pYDistance);
                    AcGePoint2d pVertex2(pVertex1[0], pVertex3[1]);
                    AcGePoint2d pVertex4(pVertex3[0], pVertex1[1]);
                    createRectangle(pPolyline, pVertex1, pVertex2, pVertex3, pVertex4);
                }
            }
            else
            {
                acutPrintf(_T("\nInvalid Co-ordinates"));
            }
            // Append the rectangle to modelspace
            pBlockTableRecord->appendAcDbEntity(pPolyline);
            pPolyline->close();

            // Close the block table record and the Line object
            pBlockTableRecord->close();
        }
    }
}


int acrxEntryPoint(AcRx::AppMsgCode Msg, void* pkt) {
    switch (Msg) {
    case AcRx::kInitAppMsg:
        acrxUnlockApplication(pkt);
        acrxRegisterAppMDIAware(pkt);
        acutPrintf(L"\n Command Loaded");
        //Command for making graphical object(rectangle Using Center And Vertex)
        acedRegCmds->addCommand(L"AUCommands", _T("Rectangle(center,vertex)"), _T("Rectangle(center,vertex)"), ACRX_CMD_MODAL, rectangleUsingVertexAndCenter);
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
