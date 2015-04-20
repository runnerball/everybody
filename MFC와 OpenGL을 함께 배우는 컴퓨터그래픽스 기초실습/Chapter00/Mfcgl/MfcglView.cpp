
// MfcglView.cpp : implementation of the CMfcglView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "Mfcgl.h"
#endif

#include "MfcglDoc.h"
#include "MfcglView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMfcglView

IMPLEMENT_DYNCREATE(CMfcglView, CView)

BEGIN_MESSAGE_MAP(CMfcglView, CView)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// CMfcglView construction/destruction

CMfcglView::CMfcglView()
{
	// TODO: add construction code here

}

CMfcglView::~CMfcglView()
{
}

BOOL CMfcglView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CMfcglView drawing

void CMfcglView::OnDraw(CDC* pDC)
{
	CMfcglDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
	CString str;
	str.Format("hello world");
	pDC->TextOutA(100,50,str);
}

void CMfcglView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CMfcglView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CMfcglView diagnostics

#ifdef _DEBUG
void CMfcglView::AssertValid() const
{
	CView::AssertValid();
}

void CMfcglView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMfcglDoc* CMfcglView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMfcglDoc)));
	return (CMfcglDoc*)m_pDocument;
}
#endif //_DEBUG


// CMfcglView message handlers
