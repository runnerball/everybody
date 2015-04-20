
// MfcglView.h : interface of the CMfcglView class
//

#pragma once


class CMfcglView : public CView
{
protected: // create from serialization only
	CMfcglView();
	DECLARE_DYNCREATE(CMfcglView)

// Attributes
public:
	CMfcglDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// Implementation
public:
	virtual ~CMfcglView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in MfcglView.cpp
inline CMfcglDoc* CMfcglView::GetDocument() const
   { return reinterpret_cast<CMfcglDoc*>(m_pDocument); }
#endif

