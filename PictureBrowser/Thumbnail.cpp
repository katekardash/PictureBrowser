#include "stdafx.h"
#include "Thumbnail.h"
#include <fstream>
using std::ifstream;

//IMAGE
ThumbnailImage::ThumbnailImage(CString p, CString name)
{
	isLoaded = false;
	path = p;
	label = name;
}

void ThumbnailImage::DrawThumbnail(CDC * pDC, int x, int y)
{
	//If image is not loaded yet, loads a RESIZED copy
	//Otherwise it will be resized every time it is drawn, slowing down performance considerably
	if (!isLoaded) {
		LoadThumbnail();
	}

	RECT labelRect = { x + PREVIEW_PADDING, y + PREVIEW_PADDING * 2 + PREVIEW_SIZE,
		x + PREVIEW_PADDING + PREVIEW_SIZE, y + PREVIEW_PADDING * 2 + PREVIEW_SIZE + LABEL_HEIGHT };

	pDC->Rectangle(x, y, x + THUMBNAIL_WIDTH, y + THUMBNAIL_HEIGHT);
	pDC->Rectangle(x + PREVIEW_PADDING - 1, y + PREVIEW_PADDING - 1, x + PREVIEW_SIZE + PREVIEW_PADDING + 1, y + PREVIEW_SIZE + PREVIEW_PADDING + 1);

	preview.Draw(pDC->GetSafeHdc(), x + PREVIEW_PADDING, y + PREVIEW_PADDING, PREVIEW_SIZE, PREVIEW_SIZE);
	
	DrawTextW(*pDC, label, -1, &labelRect, DT_CENTER | DT_TOP | DT_END_ELLIPSIS);
}

//Loads a middle square part of an image to fit into preview
void ThumbnailImage::LoadThumbnail() {
	if (isLoaded) {
		return;
	}
	CImage placeholder;
	placeholder.Load(path);
	preview.Create(PREVIEW_SIZE, PREVIEW_SIZE, 32);

	SetStretchBltMode(placeholder.GetDC(), HALFTONE);
	placeholder.ReleaseDC();

	//Here there resizing
	int h = placeholder.GetHeight();
	int w = placeholder.GetWidth();
	int z = (h > w) ? w: h; //Selects the smaller one
	
	//The offset is half the difference between height and weight becaus of "rule of thirds" composition found in nearly everything
	//This way the cropped square is more likely to contain the picture's most important elements
	placeholder.Draw(preview.GetDC(), 0, 0, PREVIEW_SIZE, PREVIEW_SIZE, (w - z) / 2, (h - z) / 2, z, z);
	preview.ReleaseDC();
	
	isLoaded = true;
}

//TEXT
ThumbnailText::ThumbnailText(CString p, CString name)
{
	isLoaded = false;
	label = name;
	path = p;
}


//Grabs first n symbols from given text file for preview purposes
void ThumbnailText::LoadThumbnail() {
	if (isLoaded) {
		return;
	}
	char* buffer = new char[PREVIEW_CHARS+1];

	std::fstream inStream(path, std::fstream::in);
	int counter = 0;
	while (inStream >> std::noskipws >> buffer[counter] && counter<PREVIEW_CHARS) {
		counter++;
	}

	buffer[counter] = '\0';
	
	previewText = buffer;

	inStream.close();
	
}

void ThumbnailText::DrawThumbnail(CDC * pDC, int x, int y)
{
	if (!isLoaded) {
		LoadThumbnail();
	}

	RECT textRect = { x + PREVIEW_PADDING + 1, y + PREVIEW_PADDING + 1, x + PREVIEW_SIZE + PREVIEW_PADDING - 1, y + PREVIEW_SIZE + 1 };
	RECT labelRect = { x + PREVIEW_PADDING, y + PREVIEW_PADDING * 2 + PREVIEW_SIZE,
		x + PREVIEW_PADDING + PREVIEW_SIZE, y + PREVIEW_PADDING * 2 + PREVIEW_SIZE + LABEL_HEIGHT };

	pDC->Rectangle(x, y, x + THUMBNAIL_WIDTH, y + THUMBNAIL_HEIGHT);
	pDC->Rectangle(x + PREVIEW_PADDING - 1, y + PREVIEW_PADDING - 1, x + PREVIEW_SIZE + PREVIEW_PADDING + 1, y + PREVIEW_SIZE + PREVIEW_PADDING + 1);

	DrawTextW(*pDC, previewText, -1, &textRect, DT_LEFT | DT_WORDBREAK | DT_END_ELLIPSIS);
	DrawTextW(*pDC, label, -1, &labelRect, DT_CENTER | DT_END_ELLIPSIS);

}

//MISC
ThumbnailMisc::ThumbnailMisc(CString name, CImage* replacementImage)
{
	noImageBitmap = replacementImage;
	label = name;
	bool isLoaded = true; //Misc images are always loaded
}

void ThumbnailMisc::DrawThumbnail(CDC * pDC, int x, int y)
{
	RECT labelRect = { x + PREVIEW_PADDING, y + PREVIEW_PADDING * 2 + PREVIEW_SIZE,
		x + PREVIEW_PADDING + PREVIEW_SIZE, y + PREVIEW_PADDING * 2 + PREVIEW_SIZE + LABEL_HEIGHT };

	pDC->Rectangle(x, y, x + THUMBNAIL_WIDTH, y + THUMBNAIL_HEIGHT);
	pDC->Rectangle(x + PREVIEW_PADDING - 1, y + PREVIEW_PADDING - 1, x + PREVIEW_SIZE + PREVIEW_PADDING + 1, y + PREVIEW_SIZE + PREVIEW_PADDING + 1);

	noImageBitmap->Draw(pDC->GetSafeHdc(), x + PREVIEW_PADDING, y + PREVIEW_PADDING, PREVIEW_SIZE, PREVIEW_SIZE);

	DrawTextW(*pDC, label, -1, &labelRect, DT_CENTER | DT_TOP | DT_END_ELLIPSIS);
}


//No loading for misc class is required, so this is left empty
void ThumbnailMisc::LoadThumbnail()
{}
