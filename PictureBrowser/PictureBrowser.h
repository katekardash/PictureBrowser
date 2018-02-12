#pragma once
#include "resource.h"       // main symbols

class PictureBrowserApp : public CWinApp
{
public:
	PictureBrowserApp();
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

extern PictureBrowserApp theApp;
