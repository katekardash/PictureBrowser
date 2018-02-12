#pragma once
#include "Thumbnail.h"
#include <string> 
#include <vector>
#include <boost/filesystem.hpp>
using std::string;
using std::vector;


class PictureListDoc : public CDocument
{
protected: 
	PictureListDoc();
	DECLARE_DYNCREATE(PictureListDoc);

public:
	CImage* noImage;
	vector<Thumbnail*> PictureList;
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	void AddPicture(CString path, CString filename);
	void ClearPictureList();
	bool checkExtension(CString path, string ext);
	virtual ~PictureListDoc();
};


