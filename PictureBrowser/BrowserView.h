#pragma once
#include "atlimage.h"
#include <boost/filesystem.hpp>
#include "DisplayParams.h"
using namespace std;
using namespace boost::filesystem;

//Ids for control elements
#define BACK_BUTTON_ID 991199
#define OPEN_BUTTON_ID 991188
#define SELECT_BUTTON_ID 991177
#define PATHBOX_ID 991166

//String defines
#define DEFAULT_PATH _T("C:\\Test")

//Heights and widths for elements
#define OPEN_BUTTON_WIDTH 50
#define BACK_BUTTON_WIDTH 45
#define SELECT_BUTTON_WIDTH 57

class BrowserView : public CView
{
	//Control elements (and a font) (and a "No Image" image)
	CButton backButton;
	CButton openButton;
	CButton selectButton;
	CEdit pathBox;
	CFont* mainFont;
	CImage* NoImage;
	
	//Dimensions for window, thumbnail grid and other display\positioning shenanigans
	int screenW;
	int screenH;
	int thumbnailsH;
	int thumbnailsV;
	int totalPicturesLoaded=0;
	int scrolledLines = 0; //How many lines have we scrolled down, used for display purposes
	int maxScrolledLines=1;

	//Threadpool for all our thread needs
	boost::asio::io_service ioService;
	boost::thread_group threadpool;

	BrowserView();
	DECLARE_DYNCREATE(BrowserView)
	DECLARE_MESSAGE_MAP()
	void BackButtonClick();
	void OpenSelectedFolder();
	void SelectButtonClick();
	void virtual OnVScroll(UINT code, UINT pos, CScrollBar * sb);
	BOOL virtual OnMouseWheel(UINT flags, short zdelta, CPoint point);
	void virtual OnLButtonUp(UINT nFlags, CPoint point);
	void CalculateThumbnailGrid();
	void PreloadLine(int a, int b);


public:
	PictureListDoc* GetDocument() const;
	virtual void OnDraw(CDC* pDC);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	BOOL virtual OnEraseBkgnd(CDC * pDC);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual ~BrowserView();
	virtual void OnInitialUpdate();

	
};

