#include "stdafx.h"
#include "PictureListDoc.h"
#include "BrowserView.h"
#include "Thumbnail.h"
#include "memdc.h"

IMPLEMENT_DYNCREATE(BrowserView, CView)

BEGIN_MESSAGE_MAP(BrowserView, CView)
	ON_BN_CLICKED(OPEN_BUTTON_ID, OpenSelectedFolder)
	ON_BN_CLICKED(BACK_BUTTON_ID, BackButtonClick)
	ON_BN_CLICKED(SELECT_BUTTON_ID, SelectButtonClick)
	ON_WM_SIZE()
	ON_WM_VSCROLL()
	ON_WM_MOUSEWHEEL()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()


BrowserView::BrowserView()
{
	
}

BrowserView::~BrowserView()
{
}

PictureListDoc* BrowserView::GetDocument() const
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(PictureListDoc)));
	return (PictureListDoc*)m_pDocument;
}


//Initial update function that initializes important parts of the interface
void BrowserView::OnInitialUpdate()
{
	//Font to be used by everything
	mainFont = new CFont();
	mainFont->CreateFont(20, 0, 0, 0, FW_NORMAL, false, false,
		0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		FIXED_PITCH | FF_MODERN, _T("Tahoma"));
			
	//Creating the textbox and the buttons for it
	pathBox.Create(WS_CHILD | WS_VISIBLE| ES_AUTOHSCROLL| WS_BORDER, CRect(0, 0, 0, 0), this, PATHBOX_ID);
	pathBox.SetWindowTextW(DEFAULT_PATH);
	pathBox.SetFont(mainFont);
	
	//Creating the back and open selector buttons
	openButton.Create(_T("Open"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(0, 0, 0, 0), this, OPEN_BUTTON_ID);
	backButton.Create(_T("Back"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(0, 0, 0, 0), this, BACK_BUTTON_ID);
	selectButton.Create(_T("Select"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(0, 0, 0, 0), this, SELECT_BUTTON_ID);
	openButton.SetFont(mainFont);
	backButton.SetFont(mainFont);
	selectButton.SetFont(mainFont);
	
	//Reposition pathbox and buttons
	pathBox.MoveWindow((BACK_BUTTON_WIDTH + 2), 0, screenW - (OPEN_BUTTON_WIDTH + SELECT_BUTTON_WIDTH + BACK_BUTTON_WIDTH + 6), CONTROLS_HEIGHT);
	backButton.MoveWindow(2, 0, BACK_BUTTON_WIDTH, CONTROLS_HEIGHT);
	openButton.MoveWindow(screenW - (OPEN_BUTTON_WIDTH + SELECT_BUTTON_WIDTH + 3), 0, OPEN_BUTTON_WIDTH, CONTROLS_HEIGHT);
	selectButton.MoveWindow(screenW - (OPEN_BUTTON_WIDTH + 9), 0, SELECT_BUTTON_WIDTH, CONTROLS_HEIGHT);

	//Initializes threadpool and adds threads for it
	//N of threadpools = numbers of CPUs+1
	//This is considered a general golden rule of number of parallel threads running
	boost::asio::io_service::work work(ioService);
	SYSTEM_INFO sysinfo; 
	GetSystemInfo(&sysinfo);
	int threadsNum = 2*sysinfo.dwNumberOfProcessors;
	for (int i = 0; i < threadsNum; i++) {
		threadpool.create_thread(boost::bind(&boost::asio::io_service::run, &ioService));
	}

	
}
BOOL BrowserView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CView::PreCreateWindow(cs);
}

BOOL BrowserView::OnEraseBkgnd(CDC* pDC)
{
	return FALSE;
}

void BrowserView::OnSize(UINT nType, int cx, int cy) {
	screenW = cx;
	screenH = cy;
	//Reposition pathbox and buttons
	//Also checks if initialization has been done already
	if (::IsWindow(pathBox.GetSafeHwnd())) {
		
		pathBox.MoveWindow((BACK_BUTTON_WIDTH + 2), 0, cx - (OPEN_BUTTON_WIDTH + SELECT_BUTTON_WIDTH + BACK_BUTTON_WIDTH + 6), CONTROLS_HEIGHT);
		backButton.MoveWindow(2, 0, BACK_BUTTON_WIDTH, CONTROLS_HEIGHT);
		openButton.MoveWindow(cx - (OPEN_BUTTON_WIDTH + SELECT_BUTTON_WIDTH + 3), 0, OPEN_BUTTON_WIDTH, CONTROLS_HEIGHT);
		selectButton.MoveWindow(cx - (OPEN_BUTTON_WIDTH + 9), 0, SELECT_BUTTON_WIDTH, CONTROLS_HEIGHT);
		
		CalculateThumbnailGrid();
	}
	
}

void BrowserView::OnDraw(CDC* DC)
{
	//A library that implements double buffering, removing the nasty screen flickering
	CMemDC pDC(DC);

	//CDC settings
	pDC->SetStretchBltMode(HALFTONE);
	pDC->SelectObject(mainFont);

	PictureListDoc* pDoc = GetDocument();
		
	//Loop to draw all thumbnails(if they're present, of course)
	int pictureCounter = 0;

	//startloop
	if(	totalPicturesLoaded > 0 ){
		for (int v = 0; v <= thumbnailsV; v++) {
			for (int h = 0; h < thumbnailsH; h++) {
				pDoc->PictureList[pictureCounter+scrolledLines*thumbnailsH]->DrawThumbnail(pDC,
					THUMBNAIL_OUTER_PADDING + h*(THUMBNAIL_WIDTH + THUMBNAIL_OUTER_PADDING + (screenW % (THUMBNAIL_WIDTH+ THUMBNAIL_OUTER_PADDING)) / thumbnailsH),
					THUMBNAIL_OUTER_PADDING + CONTROLS_HEIGHT + v*(THUMBNAIL_HEIGHT + THUMBNAIL_OUTER_PADDING));
				
				if (++pictureCounter + scrolledLines*thumbnailsH >= totalPicturesLoaded) {
					return;
				}
		
			}
		}
	}
	//endloop
	
	
}

//Goes up to the root folder of the current one. 
void BrowserView::BackButtonClick()
{
	CString currentPath;
	pathBox.GetWindowTextW(currentPath);

	path p(currentPath);
	CString newPath = p.root_directory().c_str;
	
	pathBox.SetWindowTextW(newPath);
}

//Opens folder from the selected address in the path box or from a dialogue window
void BrowserView::OpenSelectedFolder() {

	CString currentPath;
	pathBox.GetWindowTextW(currentPath);
	path p(currentPath);
	
	//Checking if location selected exists
	if (!exists(p)) {
		Beep(400, 100);
		return;
	}
	
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;

	PictureListDoc* pDoc = GetDocument();

	pDoc->ClearPictureList();

	totalPicturesLoaded = 0;

	//This is the part where all files (that's not directories) are loaded into the document
	for (auto i = directory_iterator(p); i != directory_iterator(); i++)
	{
		if (is_regular_file(i->path())) {
			totalPicturesLoaded++;
			pDoc->AddPicture(i->path().c_str(), i->path().filename().c_str());
		}
	}

	scrolledLines = 0;

	CalculateThumbnailGrid();

	//Parallel preloads images into the table
	PreloadLine(0, thumbnailsH);
	
	Invalidate();
	UpdateWindow();
	
}

void BrowserView::SelectButtonClick()
{
	CFolderPickerDialog FolderSelectDialog(_T(""), NULL, this);

	if (FolderSelectDialog.DoModal() == IDOK)
	{
		CString cBuff = FolderSelectDialog.GetPathName();

		pathBox.SetWindowTextW(cBuff);

		OpenSelectedFolder();
	}

}

//Scroll function
void BrowserView::OnVScroll(UINT code, UINT pos, CScrollBar* sb)
{
	//How many lines up or down do we have to scroll
	int delta=0;

	//Variables for line preloading
	int start; int finish;

	switch (code)
	{
	case SB_LINEUP:
		delta = -1;
		start = scrolledLines + delta;
		finish = scrolledLines + delta;
		break;
	case SB_LINEDOWN:
		delta = 1;
		start = scrolledLines + delta;
		finish = scrolledLines + delta;
		break;

	case SB_PAGEUP:
		delta = -thumbnailsV;
		start = scrolledLines + delta;
		finish = scrolledLines -1;
		break;
	case SB_PAGEDOWN:
		delta = thumbnailsV;
		start = scrolledLines + 1;
		finish = scrolledLines + delta;
		break;

	case SB_TOP:
		scrolledLines = 0;
		start = 0;
		finish = thumbnailsV;
		break;

	case SB_BOTTOM:
		scrolledLines = maxScrolledLines;
		start = maxScrolledLines - thumbnailsV;
		finish = maxScrolledLines;
		break;

	case SB_THUMBTRACK:
	case SB_THUMBPOSITION:
		scrolledLines = pos;
		start = scrolledLines;
		finish = scrolledLines;
		break;

	default:
		return;
	}

	if (delta != 0) {
		scrolledLines += delta;
		if (scrolledLines < 0) {
			scrolledLines = 0;
		}
		if (scrolledLines > maxScrolledLines) {
			scrolledLines = maxScrolledLines;
		}

	}

	SetScrollPos(SB_VERT, scrolledLines);

	PreloadLine(start, finish);

	Invalidate();
	UpdateWindow();
}

//Scrolls on mouse wheel via sending a scroll message
BOOL BrowserView::OnMouseWheel(UINT flags, short zdelta, CPoint point)
{
	if (zdelta > 0) {
		SendMessage(WM_VSCROLL, SB_LINEUP, 0);
	}
	else {
		SendMessage(WM_VSCROLL, SB_LINEDOWN, 0);
	}

	return true;
}

//Detects where the button was clicked. If it was clicked on any of the previews, opens it using a default program
//Equivalent to double-clicking it in a regular windows browser
void BrowserView::OnLButtonUp(UINT nFlags, CPoint point) {
	if (totalPicturesLoaded < 1) {
		return;
	}
	//Determines which of the thumbnails was clicked at
	//Trust me it makes sense mathematically, just looks confusing because of very long names
	int tX = (point.x + (screenW % (THUMBNAIL_WIDTH + THUMBNAIL_OUTER_PADDING)) / thumbnailsH) / (THUMBNAIL_WIDTH + THUMBNAIL_OUTER_PADDING + (screenW % (THUMBNAIL_WIDTH + THUMBNAIL_OUTER_PADDING)) / thumbnailsH);
	int tY = (point.y - CONTROLS_HEIGHT) / (THUMBNAIL_HEIGHT + THUMBNAIL_OUTER_PADDING);

	int thumbnailNum = (tY + scrolledLines)*thumbnailsH + tX;
	if (thumbnailNum > totalPicturesLoaded) {
		return;
	}

	//Determines positions within individual packs of thumbnails+padding
	int dX = (point.x + (screenW % (THUMBNAIL_WIDTH + THUMBNAIL_OUTER_PADDING)) / thumbnailsH) % (THUMBNAIL_WIDTH + THUMBNAIL_OUTER_PADDING + (screenW % (THUMBNAIL_WIDTH + THUMBNAIL_OUTER_PADDING)) / thumbnailsH);
	int dY = (point.y-CONTROLS_HEIGHT) % (THUMBNAIL_HEIGHT + THUMBNAIL_OUTER_PADDING); 

		
	//Checking if the click was on actual thumbnail and not padding around it
	//If yes, calls ShellExecute at given path
	if (dX > THUMBNAIL_OUTER_PADDING + (screenW % (THUMBNAIL_WIDTH + THUMBNAIL_OUTER_PADDING)) / thumbnailsH && 
		dY>THUMBNAIL_OUTER_PADDING) {
		ShellExecute(0, 0, GetDocument()->PictureList[thumbnailNum]->getPath(), 0, 0, SW_SHOW);
	}


}

//Calculates how many verital-horizontal thumbnails should fit into the window
//Also readjusts the scrollbar - disabling it if all thumbnails fit into the window and enabling if not
//Returns true if new size changes the alignment of thumbnails
void BrowserView::CalculateThumbnailGrid()
{
	//Recounts the number of thumbnails in the grid and max scrolling distance
	thumbnailsH = screenW / (THUMBNAIL_WIDTH + THUMBNAIL_OUTER_PADDING);
	thumbnailsV = screenH / (THUMBNAIL_HEIGHT + THUMBNAIL_OUTER_PADDING);

	maxScrolledLines = (totalPicturesLoaded / thumbnailsH) - thumbnailsV + 1;

	if (totalPicturesLoaded % thumbnailsH == 0) {
		maxScrolledLines--;
	}

	if (maxScrolledLines < 0) {
		maxScrolledLines = 0;
	}

	//Turns on scroll bar if more pictures are loaded than the window fits
	if (thumbnailsV*thumbnailsH > totalPicturesLoaded) {
		ShowScrollBar(SB_VERT, false);
	}
	else {
		ShowScrollBar(SB_VERT, true);
		SetScrollRange(SB_VERT, 0, maxScrolledLines);
		SetScrollPos(SB_VERT, (scrolledLines < maxScrolledLines) ? scrolledLines : maxScrolledLines);
	}

	//Adjusts scrolled lines according to the current scroll position during resize
	if (thumbnailsV*thumbnailsH < totalPicturesLoaded) {
		scrolledLines = GetScrollPos(SB_VERT);
	}

	//Adjusts the scrollbar thumb size to match how much is actually displayed
	SCROLLINFO scrollInfo;
	GetScrollInfo(SB_VERT, &scrollInfo);
	scrollInfo.nPage = thumbnailsV/2;
	SetScrollInfo(SB_VERT, &scrollInfo);
	
}

//Preloads lines from line start to line finish inclusively
void BrowserView::PreloadLine(int start, int finish) {
	//Checks start and finish for eligibility
	if (start > maxScrolledLines||finish<0) {
		return;
	}
	if (start < 0) {
		start = 0;
	}
	if (finish > maxScrolledLines) {
		finish = maxScrolledLines;
	}

	PictureListDoc* pDoc = GetDocument();

	//startloop
	int pictureCounter = 0;
	if (totalPicturesLoaded>0) {
		for (int i = start; i <= finish; i++) {
				for (int h = 0; h < thumbnailsH; h++) {
					//Puts the loading task into a threadpool
					ioService.post(boost::bind(&Thumbnail::LoadThumbnail, pDoc->PictureList[pictureCounter + i*thumbnailsH]));

				if (++pictureCounter + i*thumbnailsH >= totalPicturesLoaded) {
					return;
				}

			}
		}
	}
	//endloop


}


 


