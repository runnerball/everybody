// mfcglView.h : interface of the CMfcglView class
//
/////////////////////////////////////////////////////////////////////////////


class CMfcglView : public CView
{
protected: // create from serialization only
	CMfcglView();
	DECLARE_DYNCREATE(CMfcglView)

	// Attributes
public:
	CMfcglDoc* GetDocument();


	// Operations
public:
	void GLResize(GLsizei w, GLsizei h);
	void GLRenderScene(void* pData);
	void GLSetupRC(void *pData);

	HGLRC m_hRC;	// Rendering Context
	HDC m_hDC;		// Device Context

	CPalette m_GLPalette;	// Logical Palette

	GLfloat gl_left, gl_right, gl_top, gl_bottom;
	GLfloat width, height;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMfcglView)
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

	// Implementation
public:
	virtual ~CMfcglView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	void InitializePalette(void);

	// Generated message map functions
protected:
	//{{AFX_MSG(CMfcglView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg BOOL OnQueryNewPalette();
	afx_msg void OnPaletteChanged(CWnd* pFocusWnd);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


#ifndef _DEBUG  // debug version in mfcglView.cpp
inline CMfcglDoc* CMfcglView::GetDocument()
{ return (CMfcglDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
