#include "stdafx.h"
#include "PictureBrowser.h"
#include "BrowserFrame.h"
#include "DisplayParams.h"


BEGIN_MESSAGE_MAP(BrowserFrame, CFrameWnd)
	ON_WM_GETMINMAXINFO()
END_MESSAGE_MAP()

IMPLEMENT_DYNCREATE(BrowserFrame, CFrameWnd)

BrowserFrame::BrowserFrame()
{
}

BrowserFrame::~BrowserFrame()
{
}

//Sets the minimum size for the window - the size of one thumnbail
void BrowserFrame::OnGetMinMaxInfo(MINMAXINFO* mmi) {
	mmi->ptMinTrackSize.x = THUMBNAIL_WIDTH + 8 * THUMBNAIL_OUTER_PADDING;
	mmi->ptMinTrackSize.y = THUMBNAIL_HEIGHT + 8*THUMBNAIL_OUTER_PADDING+CONTROLS_HEIGHT;
}

BOOL BrowserFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	//Removes the pesky menu
	if (cs.hMenu != NULL)
	{
		DestroyMenu(cs.hMenu); 
		cs.hMenu = NULL;
	}

	cs.style = WS_OVERLAPPEDWINDOW;
	
	

	return TRUE;
}


