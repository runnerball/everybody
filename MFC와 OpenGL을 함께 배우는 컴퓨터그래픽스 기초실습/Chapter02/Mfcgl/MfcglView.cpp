// mfcglView.cpp : implementation of the CMfcglView class
//

#include "stdafx.h"
#include "mfcgl.h"

#include "mfcglDoc.h"
#include "mfcglView.h"


#define glRGB(x, y, z)	glColor3ub((GLubyte)x, (GLubyte)y, (GLubyte)z)


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// CMfcglView

IMPLEMENT_DYNCREATE(CMfcglView, CView)

BEGIN_MESSAGE_MAP(CMfcglView, CView)
	//{{AFX_MSG_MAP(CMfcglView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_WM_QUERYNEWPALETTE()
	ON_WM_PALETTECHANGED()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_DRAW_POINT, &CMfcglView::OnDrawPoint)
	ON_COMMAND(ID_DRAW_LINE, &CMfcglView::OnDrawLine)
	ON_COMMAND(ID_DRAW_POLYGON, &CMfcglView::OnDrawPolygon)
	ON_COMMAND(ID_DRAW_CLEAR, &CMfcglView::OnDrawClear)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMfcglView construction/destruction

CMfcglView::CMfcglView()
{
	m_DrawType=1;
	m_NumOfDots=0;
}

CMfcglView::~CMfcglView()
{
}

BOOL CMfcglView::PreCreateWindow(CREATESTRUCT& cs)
{
	// Add Window styles required for OpenGL before window is created
	cs.style |= (WS_CLIPCHILDREN | WS_CLIPSIBLINGS | CS_OWNDC);

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CMfcglView drawing

// Called when window receives WM_PAINT, render our scene
void CMfcglView::OnDraw(CDC* pDC)
{
	// Make the rendering context current
	wglMakeCurrent(m_hDC,m_hRC);

	// Call our external OpenGL code
	GLRenderScene(NULL);

	// Swap our scene to the front
	SwapBuffers(m_hDC);

	// Allow other rendering contexts to co-exist
	wglMakeCurrent(m_hDC,NULL);
}

	/////////////////////////////////////////////////////////////////////////////
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

CMfcglDoc* CMfcglView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMfcglDoc)));
	return (CMfcglDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMfcglView message handlers

int CMfcglView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	int nPixelFormat;					// Pixel format index
	m_hDC = ::GetDC(m_hWnd);			// Get the Device context

	static PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),	// Size of this structure
		1,								// Version of this structure	
		PFD_DRAW_TO_WINDOW |			// Draw to Window (not to bitmap)
		PFD_SUPPORT_OPENGL |			// Support OpenGL calls in window
		PFD_DOUBLEBUFFER,				// Double buffered mode
		PFD_TYPE_RGBA,					// RGBA Color mode
		24,								// Want 24bit color 
		0,0,0,0,0,0,					// Not used to select mode
		0,0,							// Not used to select mode
		0,0,0,0,0,						// Not used to select mode
		32,								// Size of depth buffer
		0,								// Not used to select mode
		0,								// Not used to select mode
		PFD_MAIN_PLANE,					// Draw in main plane
		0,								// Not used to select mode
		0,0,0 };						// Not used to select mode

		// Choose a pixel format that best matches that described in pfd
		nPixelFormat = ChoosePixelFormat(m_hDC, &pfd);

		// Set the pixel format for the device context
		VERIFY(SetPixelFormat(m_hDC, nPixelFormat, &pfd));

		// Create the rendering context
		m_hRC = wglCreateContext(m_hDC);

		// Make the rendering context current, perform initialization, then
		// deselect it
		VERIFY(wglMakeCurrent(m_hDC,m_hRC));
		GLSetupRC(m_hDC);
		wglMakeCurrent(NULL,NULL);

		// Create the palette if needed
		InitializePalette();

		return 0;
}


void CMfcglView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	RECT rect;
	GetClientRect(&rect);

	width = (float)(rect.right - rect.left);
	height = (float)(rect.bottom - rect.top);

	VERIFY(wglMakeCurrent(m_hDC,m_hRC));
	GLResize(cx, cy);
	VERIFY(wglMakeCurrent(NULL,NULL));
}

// The window is being destroyed, delete the rendering context,
// and release the device context
void CMfcglView::OnDestroy() 
{
	// Kill the timer
	KillTimer(101);

	// Clean up rendering context stuff
	wglDeleteContext(m_hRC);
	::ReleaseDC(m_hWnd,m_hDC);

	CView::OnDestroy();
}


// Override to keep the background from being erased everytime
// the window is repainted
BOOL CMfcglView::OnEraseBkgnd(CDC* pDC) 
{
	return FALSE;
}


// Initializes the CPalette object
void CMfcglView::InitializePalette(void)
{
	PIXELFORMATDESCRIPTOR pfd;	// Pixel Format Descriptor
	LOGPALETTE *pPal;			// Pointer to memory for logical palette
	int nPixelFormat;			// Pixel format index
	int nColors;				// Number of entries in palette
	int i;						// Counting variable
	BYTE RedRange,GreenRange,BlueRange;
	// Range for each color entry (7,7,and 3)

	// Get the pixel format index and retrieve the pixel format description
	nPixelFormat = GetPixelFormat(m_hDC);
	DescribePixelFormat(m_hDC, nPixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

	// Does this pixel format require a palette?  If not, do not create a
	// palette and just return NULL
	if(!(pfd.dwFlags & PFD_NEED_PALETTE))
		return;

	// Number of entries in palette.  8 bits yeilds 256 entries
	nColors = 1 << pfd.cColorBits;	

	// Allocate space for a logical palette structure plus all the palette entries
	pPal = (LOGPALETTE*)malloc(sizeof(LOGPALETTE) +nColors*sizeof(PALETTEENTRY));

	// Fill in palette header 
	pPal->palVersion = 0x300;		// Windows 3.0
	pPal->palNumEntries = nColors; // table size

	// Build mask of all 1's.  This creates a number represented by having
	// the low order x bits set, where x = pfd.cRedBits, pfd.cGreenBits, and
	// pfd.cBlueBits.  
	RedRange = (1 << pfd.cRedBits) -1;
	GreenRange = (1 << pfd.cGreenBits) - 1;
	BlueRange = (1 << pfd.cBlueBits) -1;

	// Loop through all the palette entries
	for(i = 0; i < nColors; i++)
	{
		// Fill in the 8-bit equivalents for each component
		pPal->palPalEntry[i].peRed = (i >> pfd.cRedShift) & RedRange;
		pPal->palPalEntry[i].peRed = (unsigned char)(
			(double) pPal->palPalEntry[i].peRed * 255.0 / RedRange);

		pPal->palPalEntry[i].peGreen = (i >> pfd.cGreenShift) & GreenRange;
		pPal->palPalEntry[i].peGreen = (unsigned char)(
			(double)pPal->palPalEntry[i].peGreen * 255.0 / GreenRange);

		pPal->palPalEntry[i].peBlue = (i >> pfd.cBlueShift) & BlueRange;
		pPal->palPalEntry[i].peBlue = (unsigned char)(
			(double)pPal->palPalEntry[i].peBlue * 255.0 / BlueRange);

		pPal->palPalEntry[i].peFlags = (unsigned char) NULL;
	}

	// Create the palette
	m_GLPalette.CreatePalette(pPal);

	// Go ahead and select and realize the palette for this device context
	SelectPalette(m_hDC,(HPALETTE)m_GLPalette,FALSE);
	RealizePalette(m_hDC);

	// Free the memory used for the logical palette structure
	free(pPal);
}

BOOL CMfcglView::OnQueryNewPalette() 
{
	// If the palette was created.
	if((HPALETTE)m_GLPalette)
	{
		int nRet;

		// Selects the palette into the current device context
		SelectPalette(m_hDC, (HPALETTE)m_GLPalette, FALSE);

		// Map entries from the currently selected palette to
		// the system palette.  The return value is the number 
		// of palette entries modified.
		nRet = RealizePalette(m_hDC);

		// Repaint, forces remap of palette in current window
		InvalidateRect(NULL,FALSE);

		return nRet;
	}

	return CView::OnQueryNewPalette();
}


void CMfcglView::OnPaletteChanged(CWnd* pFocusWnd) 
{
	if(((HPALETTE)m_GLPalette != NULL) && (pFocusWnd != this))
	{
		// Select the palette into the device context
		SelectPalette(m_hDC,(HPALETTE)m_GLPalette,FALSE);

		// Map entries to system palette
		RealizePalette(m_hDC);

		// Remap the current colors to the newly realized palette
		UpdateColors(m_hDC);
		return;
	}

	CView::OnPaletteChanged(pFocusWnd);
}


void CMfcglView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CMfcglDoc* pDoc = GetDocument();
	GLfloat x, y;

	x = point.x/width*(gl_right-gl_left)+gl_left;
	y = (height-point.y)/height*(gl_top-gl_bottom)+gl_bottom;

	if(m_NumOfDots<Pmax && m_DrawType==1)
	{
		m_LaPoint[m_NumOfDots][0]=x;
		m_LaPoint[m_NumOfDots][1]=y;
		++m_NumOfDots;
		Invalidate();
	}

	CView::OnLButtonDown(nFlags, point);
}

////////////////////////////////////////////////////////////////

void CMfcglView::GLSetupRC(void *pData)
{
	// Black background
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f );

	// Set drawing color to yellow
	glColor3f(0.94f, 0.77f, 0.80f);

}



//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
// Called by Windows when it receives the WM_SIZE message.
// Put any code needed here to recalc the viewing volume and
// viewport info
void CMfcglView::GLResize(GLsizei w, GLsizei h)
{
	GLfloat nRange = 100.0f;

	// Prevent a divide by zero
	if(h == 0)
		h = 1;

	// Set Viewport to window dimensions
	glViewport(0, 0, w, h);

	// Reset coordinate system
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Establish clipping volume (left, right, bottom, top, near, far)
	if (w <= h) 
	{	
		gl_left = -nRange;gl_right = nRange;gl_bottom = -nRange*h/w;gl_top = nRange*h/w;
		glOrtho (gl_left, gl_right, gl_bottom, gl_top, nRange, -nRange);		    
	}
	else
	{
		gl_left = -nRange*w/h;gl_right = nRange*w/h;gl_bottom = -nRange;gl_top = nRange;
		glOrtho (gl_left, gl_right, gl_bottom, gl_top, nRange, -nRange);		
	}
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
// Render your OpenGL Scene here.
void CMfcglView::GLRenderScene(void *pData)
{
	CMfcglDoc* pDoc = GetDocument();
	// Clear the window with current clearing color
	glClear(GL_COLOR_BUFFER_BIT); 

	if (m_NumOfDots>0)
	{
		glPointSize(2.0f);
		glBegin(GL_POINTS);
		for (UINT i=0 ; i<m_NumOfDots ; i++)
		{
			glVertex3f(m_LaPoint[i][0],m_LaPoint[i][1],0.0f);
		}
		glEnd(); 
	}
}

void CMfcglView::OnDrawPoint()
{
	// TODO: Add your command handler code here
	m_DrawType = 1;
}


void CMfcglView::OnDrawLine()
{
	// TODO: Add your command handler code here
}


void CMfcglView::OnDrawPolygon()
{
	// TODO: Add your command handler code here
}


void CMfcglView::OnDrawClear()
{
	// TODO: Add your command handler code here
	m_DrawType=0;
	m_NumOfDots=0;
	Invalidate();
}
