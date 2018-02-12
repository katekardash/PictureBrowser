#include "stdafx.h"
#include "PictureListDoc.h"
#include <propkey.h>

IMPLEMENT_DYNCREATE(PictureListDoc, CDocument)

PictureListDoc::PictureListDoc(){
	noImage = new CImage();
	noImage->LoadFromResource(AfxGetInstanceHandle(), 106);
}

PictureListDoc::~PictureListDoc()
{
}

BOOL PictureListDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

void PictureListDoc::Serialize(CArchive& ar)
{
}

//Converts a path to picture into a thumbnail object
void PictureListDoc::AddPicture(CString path, CString filename)
{
	if (checkExtension(path, ".jpg") || checkExtension(path, ".jpeg") || checkExtension(path, ".png") || checkExtension(path, ".bmp") || checkExtension(path, ".gif")) {
		PictureList.push_back(new ThumbnailImage(path, filename));
	}

	else if (checkExtension(path, ".txt") || checkExtension(path, ".cpp") || checkExtension(path, ".c") || checkExtension(path, ".h") || checkExtension(path, ".html")) {
		PictureList.push_back(new ThumbnailText(path, filename));
	}

	else {
		PictureList.push_back(new ThumbnailMisc(filename, noImage));
	}

}

//Clears pictures from "cache" before loading new page
//Also deletes all selected images and frees their memory n order to prevent memory leaks
void PictureListDoc::ClearPictureList()
{
	for (Thumbnail* t : PictureList) {
		delete t;
	}

	PictureList.clear();
}

//Parses two strings from the end, comparing them.
//This was written before I decided to use boost and also forgoes some problems with comparing\converting between CString and string
bool PictureListDoc::checkExtension(CString path, string ext)
{
	int pathEnd = path.GetLength() - 1;
	int extEnd = ext.length() - 1;

	if (pathEnd < extEnd) {
		return false;
	}

	while (extEnd > 0) {
		if (ext[extEnd] != path[pathEnd]) {
			return false;
		}
		extEnd--;
		pathEnd--;
	}

	return true;
};

