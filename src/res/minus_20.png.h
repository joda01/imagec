#ifndef MINUS_20_PNG_H
#define MINUS_20_PNG_H

#include <wx/mstream.h>
#include <wx/image.h>
#include <wx/bitmap.h>

static const unsigned char minus_20_png[] =
{
	0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, 0x00, 0x00, 
	0x00, 0x0D, 0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x14, 
	0x00, 0x00, 0x00, 0x14, 0x08, 0x06, 0x00, 0x00, 0x00, 0x8D, 
	0x89, 0x1D, 0x0D, 0x00, 0x00, 0x00, 0x09, 0x70, 0x48, 0x59, 
	0x73, 0x00, 0x00, 0x0B, 0x13, 0x00, 0x00, 0x0B, 0x13, 0x01, 
	0x00, 0x9A, 0x9C, 0x18, 0x00, 0x00, 0x00, 0xF8, 0x49, 0x44, 
	0x41, 0x54, 0x78, 0x9C, 0xAD, 0x93, 0x59, 0x0A, 0xC2, 0x40, 
	0x10, 0x44, 0x1F, 0x7E, 0xAA, 0xD1, 0x5B, 0xA8, 0xE7, 0x71, 
	0xC3, 0x7F, 0xB7, 0x0B, 0x48, 0x50, 0xE3, 0x75, 0xD4, 0x1F, 
	0x41, 0x3C, 0x82, 0x1B, 0x2E, 0x1F, 0xEA, 0x91, 0xA4, 0xA1, 
	0x02, 0x21, 0x24, 0x26, 0x8E, 0x16, 0x34, 0x4C, 0x6A, 0x7A, 
	0x2A, 0xDD, 0x3D, 0x53, 0x90, 0x8D, 0x32, 0xD0, 0x50, 0x94, 
	0x70, 0x44, 0x15, 0x58, 0x00, 0x57, 0xE0, 0x00, 0x2C, 0x15, 
	0x47, 0x71, 0x81, 0x72, 0x72, 0xA1, 0x07, 0x3C, 0x81, 0xBE, 
	0xAA, 0x8B, 0xC3, 0xB8, 0x81, 0x72, 0xBA, 0x59, 0x62, 0x53, 
	0x60, 0x93, 0xB3, 0x35, 0x13, 0xDE, 0x02, 0xFE, 0xA7, 0xCA, 
	0x4C, 0xAC, 0x40, 0x7E, 0x14, 0x24, 0xDA, 0x89, 0x6F, 0xD8, 
	0x3C, 0x1E, 0x8E, 0x43, 0xF7, 0xD4, 0x7E, 0x25, 0x4A, 0x2E, 
	0x34, 0x33, 0x57, 0x0C, 0x81, 0x79, 0x94, 0xB8, 0xEB, 0x4F, 
	0xAE, 0x28, 0x03, 0x97, 0xF0, 0xC3, 0x84, 0xF6, 0xFC, 0x8E, 
	0x63, 0x38, 0xB2, 0x1A, 0xB0, 0x8A, 0x6D, 0x36, 0x81, 0x51, 
	0x46, 0x58, 0x4E, 0x14, 0x6B, 0x69, 0xFD, 0x5F, 0xD0, 0x93, 
	0x1B, 0xFE, 0xD6, 0xB2, 0xE1, 0xF6, 0xE3, 0xA5, 0x78, 0xC0, 
	0x39, 0x4A, 0x04, 0xB2, 0x93, 0x2B, 0x46, 0xC0, 0x2C, 0xFE, 
	0xB0, 0x9F, 0x8E, 0x55, 0x56, 0x80, 0x57, 0xD2, 0xD9, 0xAE, 
	0x6C, 0xF4, 0xAD, 0xF5, 0x76, 0x40, 0x2B, 0x2D, 0xC1, 0x97, 
	0xA8, 0x97, 0xB3, 0xB2, 0x1D, 0x30, 0xC9, 0x4A, 0x6C, 0xAB, 
	0x7D, 0xB3, 0x53, 0x92, 0xB0, 0x09, 0x8D, 0xD5, 0x66, 0x6A, 
	0x65, 0x49, 0x87, 0xCC, 0x9B, 0x66, 0xA7, 0x93, 0xDE, 0xA9, 
	0x85, 0xAD, 0x8D, 0xB3, 0x0B, 0x70, 0x7E, 0x15, 0x45, 0xA0, 
	0xAE, 0xB0, 0xF5, 0x47, 0xBC, 0x01, 0x7B, 0xEE, 0x33, 0x6D, 
	0xEB, 0x3F, 0x8E, 0xAF, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 
	0x4E, 0x44, 0xAE, 0x42, 0x60, 0x82, 
};

wxBitmap& minus_20_png_to_wx_bitmap()
{
	static wxMemoryInputStream memIStream( minus_20_png, sizeof( minus_20_png ) );
	static wxImage image( memIStream, wxBITMAP_TYPE_PNG );
	static wxBitmap bmp( image );
	return bmp;
}


#endif //MINUS_20_PNG_H