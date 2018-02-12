#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "PictureBrowser.h"
#include "BrowserFrame.h"
#include "PictureListDoc.h"
#include "BrowserView.h"

BEGIN_MESSAGE_MAP(PictureBrowserApp, CWinApp)
END_MESSAGE_MAP()

PictureBrowserApp::PictureBrowserApp()
{
	//Set bacgkround color
}

PictureBrowserApp theApp;

BOOL PictureBrowserApp::InitInstance()
{
	//Initialization
	CWinApp::InitInstance();
	
	//Document creation
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(PictureListDoc),
		RUNTIME_CLASS(BrowserFrame),       // main SDI frame window
		RUNTIME_CLASS(BrowserView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);
	
	//Cmd parsing and dispatching
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	//Window is done
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	return TRUE;
}

