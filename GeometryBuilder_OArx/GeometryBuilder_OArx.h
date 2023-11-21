// GeometryBuilder_OArx.h : main header file for the GeometryBuilder_OArx DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CGeometryBuilderOArxApp
// See GeometryBuilder_OArx.cpp for the implementation of this class
//

class CGeometryBuilderOArxApp : public CWinApp
{
public:
	CGeometryBuilderOArxApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
