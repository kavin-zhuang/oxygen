﻿// BoxObject.c  | Oxygen © 2015 by Thomas Führinger
#include "Oxygen.h"

static OxClass* pOxClass;
static LPWSTR szBoxClass = L"OxBoxClass";

OxBoxObject*
OxBox_New(OxWidgetObject* oxParent, OxRect* rc)
{
	OxBoxObject* ox = (OxBoxObject*)OxObject_Allocate(pOxClass);
	if (ox == NULL)
		return NULL;

	return OxBox_Init((OxBoxObject*)ox, oxParent, rc) ? ox : NULL;
}

BOOL
OxBox_Init(OxBoxObject* ox, OxWidgetObject* oxParent, OxRect* rc)
{
	if (!OxWidget_Init((OxWidgetObject*)ox, oxParent, rc))
		return FALSE;

	OxRect rect;
	rc = &rect;
	OxWidget_CalculateRect((OxWidgetObject*)ox, rc);
	ox->hWin = CreateWindowExW(0, szBoxClass, L"", // WS_EX_CLIENTEDGE
		WS_CHILD | WS_CLIPSIBLINGS | WS_TABSTOP | WS_VISIBLE,
		rc->iLeft, rc->iTop, rc->iWidth, rc->iHeight,
		oxParent->hWin, (HMENU)IDC_OXBOX, OxApp->hInstance, NULL);

	if (ox->hWin == NULL) {
		OxErr_SetFromWindows();
		return FALSE;
	}

	SetWindowLongPtr(ox->hWin, GWLP_USERDATA, (LONG_PTR)ox);
	return TRUE;
}

static BOOL
OxBox_Delete(OxBoxObject* ox)
{
	DestroyWindow(ox->hWin);
	return pOxClass->pBase->fnDelete(ox);
}

OxClass OxBoxClass = {
	"Box",                     /* sName */
	&OxWidgetClass,            /* pBase */
	sizeof(OxBoxObject),       /* nSize */
	OxBox_Delete,              /* fnDelete */
	NULL,                      /* pUserData */
	0                          /* aMethods */
};

static LRESULT CALLBACK OxBoxProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

BOOL
OxBoxClass_Init()
{
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = OxBoxProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = OxApp->hInstance;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL; //(HBRUSH)(OxWINDOWBKGCOLOR + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = szBoxClass;
	wc.hIconSm = NULL;

	if (!RegisterClassEx(&wc))
	{
		OxErr_SetFromWindows();
		return FALSE;
	}

	pOxClass = &OxBoxClass;
	Ox_INHERIT_METHODS(pOxClass);
	return TRUE;
}

static LRESULT CALLBACK
OxBoxProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	OxBoxObject* ox = (OxBoxObject*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
	switch (uMsg)
	{
	case WM_ERASEBKGND:
		if (ox == NULL)
			break;
		RECT rc;
		GetClientRect(hwnd, &rc);
		FillRect((HDC)wParam, &rc, ox->hBkgBrush);
		return 1L;

	case WM_DESTROY:
		OxCALL(OxWidget_ReleaseChildren(ox));
		break;
	}

	return DefParentProc(hwnd, uMsg, wParam, lParam, FALSE);
}