#pragma once

class BrowserFrame : public CFrameWnd
{	
protected:
	DECLARE_MESSAGE_MAP()
	BrowserFrame();
	DECLARE_DYNCREATE(BrowserFrame)

public:
	void virtual OnGetMinMaxInfo(MINMAXINFO * mmi);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual ~BrowserFrame();
};


