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

double findSlope(AcGePoint2d& pPoint1, AcGePoint2d& pPoint2)
{
    return (pPoint2[1] - pPoint1[1]) / (pPoint2[0] - pPoint1[0]);
}

AcGePoint2d shiftPoint(AcGePoint2d& pPoint1, AcGePoint2d& pPoint2, AcGePoint2d& pPoint3)
{
    double slope = findSlope(pPoint1, pPoint2);
    AcGePoint2d pShiftedPoint;
    pShiftedPoint[0] = ((pPoint2[1] * slope) - (pPoint3[1] * slope) + (slope * slope * pPoint3[0]) + pPoint2[0]) / (1 + (slope * slope));
    pShiftedPoint[1] = pPoint3[1] - (slope * pPoint3[0]) + (slope * pShiftedPoint[0]);
    return pShiftedPoint;
}

//Function for creating Rectangle using center, midpoint  and vertex in autocad
static void rectangleUsingCenterMidPointAndVertex()
{
    // Get the current space block
    AcDbDatabase* pDb = acdbHostApplicationServices()->workingDatabase();
    AcDbBlockTableRecord* pBlockTableRecord;
    Acad::ErrorStatus es = acdbOpenObject(pBlockTableRecord, pDb->currentSpaceId(), AcDb::kForWrite);
    // Create 2 variables of the old point data type
    ads_point pInCenter, pInMidPoint, pInVertex1;
    // Prompt for the first point
    AcDbPolyline* pPolyline = new AcDbPolyline();
    if (RTNORM == acedGetPoint(NULL, _T("\nSpecify the center point: "), pInCenter))
    {
        AcGePoint2d pCenter(pInCenter[0], pInCenter[1]);
        // Prompt for the second point
        if (RTNORM == acedGetPoint(pInCenter, _T("\nSpecify mid point: "), pInMidPoint))
        {
            AcGePoint2d pMidPoint(pInMidPoint[0], pInMidPoint[1]);
            // Prompt for the third point
            if (RTNORM == acedGetPoint(pInMidPoint, _T("\nSpecify vertex: "), pInVertex1))
            {
                if ((pInVertex1[1] == pInMidPoint[1]) && (pInMidPoint[1] == pInCenter[1]) || (pInVertex1[0] == pInMidPoint[0]) && (pInMidPoint[0] == pInCenter[0]))
                {
                    acutPrintf(_T("Invalid co-ordinates"));
                }
                AcGePoint2d pVertexTemp(pInVertex1[0], pInVertex1[1]);
                AcGePoint2d pVertex1 = shiftPoint(pCenter, pMidPoint, pVertexTemp);
                AcGePoint2d pVertex2((2 * pMidPoint[0]) - pVertex1[0], (2 * pMidPoint[1]) - pVertex1[1]);
                AcGePoint2d pVertex3((2 * pCenter[0]) - pVertex1[0], (2 * pCenter[1]) - pVertex1[1]);
                AcGePoint2d pVertex4((2 * pCenter[0]) - pVertex2[0], (2 * pCenter[1]) - pVertex2[1]);
                createRectangle(pPolyline, pVertex1, pVertex2, pVertex3, pVertex4);
                // Append the rectangle to modelspace
                pBlockTableRecord->appendAcDbEntity(pPolyline);
            }
        }
    }
    pPolyline->close();

    // Close the block table record and the Line object
    pBlockTableRecord->close();

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
        if (RTNORM == acedGetPoint(pInCenter, _T("\nSpecify vertex point: "), pInVertex1))
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

//Function for creating Rectangle using Three vertices 
static void rectangleUsingThreeVertices()
{
    // Get the current space block
    AcDbDatabase* pDb = acdbHostApplicationServices()->workingDatabase();
    AcDbBlockTableRecord* pBlockTableRecord;
    Acad::ErrorStatus es = acdbOpenObject(pBlockTableRecord, pDb->currentSpaceId(), AcDb::kForWrite);
    // Create 2 variables of the old point data type
    ads_point pInVertex1, pInVertex2, pInVertex3;
    // Prompt for the first point
    AcDbPolyline* pPolyline = new AcDbPolyline();
    if (RTNORM == acedGetPoint(NULL, _T("\nSpecify the first vertex: "), pInVertex1))
    {
        AcGePoint2d pVertex1(pInVertex1[0], pInVertex1[1]);
        // Prompt for the second point
        if (RTNORM == acedGetPoint(pInVertex1, _T("\nSpecify second vertex: "), pInVertex2))
        {
            AcGePoint2d pVertex2(pInVertex2[0], pInVertex2[1]);
            if (RTNORM == acedGetPoint(pInVertex2, _T("\nSpecify third vertex: "), pInVertex3))
            {
                if ((pInVertex1[1] == pInVertex2[1]) && (pInVertex2[1] == pInVertex3[1]) || (pInVertex1[0] == pInVertex2[0]) && (pInVertex2[0] == pInVertex3[0]))
                {
                    acutPrintf(_T("Invalid co-ordinates"));
                }  
                AcGePoint2d pVertexTemp(pInVertex3[0], pInVertex3[1]);
                AcGePoint2d pVertex3 = shiftPoint(pVertex1, pVertex2, pVertexTemp);
                AcGePoint2d pCenter((pInVertex1[0] + pVertex3[0]) / 2.0, (pInVertex1[1] + pVertex3[1]) / 2.0);
                AcGePoint2d pVertex4((2 * pCenter[0]) - pInVertex2[0], (2 * pCenter[1]) - pInVertex2[1]);
                createRectangle(pPolyline, pVertex1, pVertex2, pVertex3, pVertex4);
                // Append the rectangle to modelspace
                pBlockTableRecord->appendAcDbEntity(pPolyline);
            }
        }
    }
    pPolyline->close();

    // Close the block table record and the Line object
    pBlockTableRecord->close();
}

static void twoCenterSlot()
{
    // Get the current space block
    AcDbDatabase* pDb = acdbHostApplicationServices()->workingDatabase();
    AcDbBlockTableRecord* pBlockTableRecord;
    Acad::ErrorStatus es = acdbOpenObject(pBlockTableRecord, pDb->currentSpaceId(), AcDb::kForWrite);
    // Create 2 variables of the old point data type
    ads_point pInCenter1, pInCenter2, pInVertex1;
    // Prompt for the first point
    AcDbPolyline* pPolyline = new AcDbPolyline();
    if (RTNORM == acedGetPoint(NULL, _T("\nSpecify the first vertex: "), pInCenter1))
    {
        AcGePoint2d pCenter1(pInCenter1[0], pInCenter1[1]);
        // Prompt for the second point
        if (RTNORM == acedGetPoint(pInCenter1, _T("\nSpecify second vertex: "), pInCenter2))
        {
            AcGePoint2d pCenter2(pInCenter2[0], pInCenter2[1]);
            if (RTNORM == acedGetPoint(pInCenter2, _T("\nSpecify third vertex: "), pInVertex1))
            {

                //AcGePoint2d pVertex1(pInVertex1[0], pInVertex1[1]);
                //AcGePoint2d pCenter((pInVertex1[0] + pInVertex3[0]) / 2.0, (pInVertex1[1] + pInVertex3[1]) / 2.0);
                //AcGePoint2d pVertex4((2 * pCenter[0]) - pInVertex2[0], (2 * pCenter[1]) - pInVertex2[1]);
                //createRectangle(pPolyline, pVertex1, pVertex2, pVertex3, pVertex4);
                // Append the rectangle to modelspace
                //pBlockTableRecord->appendAcDbEntity(pPolyline);
            }
        }
    }
    pPolyline->close();

    // Close the block table record and the Line object
    pBlockTableRecord->close();
}

int acrxEntryPoint(AcRx::AppMsgCode Msg, void* pkt) {
    switch (Msg) {
    case AcRx::kInitAppMsg:
        acrxUnlockApplication(pkt);
        acrxRegisterAppMDIAware(pkt);
        acutPrintf(L"\n Command Loaded");
        //Command for making graphical object(rectangle Using Center And Vertex)
        acedRegCmds->addCommand(L"AUCommands", _T("Rectangle(center,vertex)"), _T("Rectangle(center,vertex)"), ACRX_CMD_MODAL, rectangleUsingVertexAndCenter);
        //Command for making graphical object(rectangle Using Three Vertices)
        acedRegCmds->addCommand(L"AUCommands", _T("Rectangle(vertex1,vertex2,vertex3)"), _T("Rectangle(vertex1,vertex2,vertex3)"), ACRX_CMD_MODAL, rectangleUsingThreeVertices);
        //Command for making graphical object(rectangle UsingCenter, Midpoint And Vertex)
        acedRegCmds->addCommand(L"AUCommands", _T("Rectangle(center,midpoint,vertex)"), _T("Rectangle1(center,midpoint,vertex)"), ACRX_CMD_MODAL, rectangleUsingCenterMidPointAndVertex);
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

