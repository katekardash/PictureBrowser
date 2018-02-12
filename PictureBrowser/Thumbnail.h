#pragma once
#include "stdafx.h"
#include <string> 
#include "atlimage.h"
#include "DisplayParams.h"
#include <boost/asio/io_service.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>

using std::string;

#define PREVIEW_CHARS 256

//Base thumbnail class. Abstract, should not be initialized (good luck trying, though)
class Thumbnail {
public:
	virtual void DrawThumbnail(CDC * pDC, int x, int y) =0;
	virtual void LoadThumbnail() = 0;
	CString getPath();
protected:
	CString path;
	CString label;
	bool isLoaded;
};


//Thumbnail for an image
class ThumbnailImage : public Thumbnail {
public:
	ThumbnailImage(CString p, CString name);
	void DrawThumbnail(CDC * pDC, int x, int y);
	virtual void LoadThumbnail();
private:
	CImage preview;
};

//Thumbnail for a text
class ThumbnailText : public Thumbnail {
public:
	ThumbnailText(CString p, CString name);
	virtual void DrawThumbnail(CDC * pDC, int x, int y);

private:
	void virtual LoadThumbnail();
	CString previewText;
};

//Thumbnail for a misc preview; used for types not fitting into either of categories above
class ThumbnailMisc : public Thumbnail {
public:
	ThumbnailMisc(CString name, CImage* replacementImage);
	virtual void DrawThumbnail(CDC * pDC, int x, int y);
	void virtual LoadThumbnail();

private:
	CImage* noImageBitmap;
};