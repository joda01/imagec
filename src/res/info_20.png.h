#ifndef INFO_20_PNG_H
#define INFO_20_PNG_H

#include <wx/mstream.h>
#include <wx/image.h>
#include <wx/bitmap.h>

static const unsigned char info_20_png[] =
{
	0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, 0x00, 0x00, 
	0x00, 0x0D, 0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x14, 
	0x00, 0x00, 0x00, 0x14, 0x08, 0x06, 0x00, 0x00, 0x00, 0x8D, 
	0x89, 0x1D, 0x0D, 0x00, 0x00, 0x00, 0x09, 0x70, 0x48, 0x59, 
	0x73, 0x00, 0x00, 0x0B, 0x13, 0x00, 0x00, 0x0B, 0x13, 0x01, 
	0x00, 0x9A, 0x9C, 0x18, 0x00, 0x00, 0x00, 0xA7, 0x49, 0x44, 
	0x41, 0x54, 0x78, 0x9C, 0xED, 0xD3, 0x3D, 0x0E, 0x41, 0x41, 
	0x14, 0x86, 0xE1, 0x47, 0x6E, 0x45, 0xC3, 0x0E, 0x24, 0xAC, 
	0x00, 0xCB, 0xD0, 0x93, 0x68, 0x89, 0x95, 0xE8, 0x50, 0x63, 
	0x11, 0x7E, 0xD6, 0xA0, 0x44, 0x94, 0x6A, 0x4B, 0x91, 0x49, 
	0x6E, 0x71, 0xE3, 0x27, 0xB8, 0x77, 0x2A, 0xF1, 0x26, 0x5F, 
	0x32, 0x39, 0xE7, 0xE4, 0x4D, 0x66, 0x32, 0x87, 0xF7, 0x94, 
	0xD0, 0x44, 0x4D, 0x04, 0x12, 0xEC, 0xB0, 0xC6, 0x01, 0xFD, 
	0xA2, 0xC2, 0x2E, 0xA6, 0xE9, 0xB9, 0x8C, 0x73, 0x0C, 0xE1, 
	0x2C, 0xA6, 0x30, 0xC1, 0x16, 0x1B, 0x1C, 0xD1, 0x13, 0x89, 
	0x06, 0xAA, 0x45, 0x04, 0x6D, 0xAC, 0x9E, 0x64, 0x9E, 0x57, 
	0x38, 0xC6, 0x04, 0x9D, 0xBB, 0x5C, 0x62, 0x08, 0xEB, 0x99, 
	0xFA, 0x29, 0xAF, 0xB0, 0x95, 0xB9, 0xE6, 0x21, 0x86, 0xD0, 
	0x0B, 0xC9, 0x5F, 0xF8, 0x4B, 0x6F, 0x38, 0xC0, 0x35, 0x15, 
	0x2C, 0x32, 0xF5, 0x65, 0x5A, 0xBB, 0xA6, 0x33, 0x3E, 0x25, 
	0x7C, 0xEC, 0x90, 0xBC, 0xFD, 0x07, 0x46, 0xD8, 0xBF, 0xD8, 
	0xE7, 0x90, 0xD0, 0x1B, 0x7E, 0x23, 0xAC, 0x3C, 0xD9, 0xE3, 
	0xFB, 0x84, 0x99, 0x07, 0x6E, 0xC0, 0xFA, 0x2B, 0xFC, 0x65, 
	0x25, 0x6C, 0x7D, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4E, 
	0x44, 0xAE, 0x42, 0x60, 0x82, 
};

wxBitmap& info_20_png_to_wx_bitmap()
{
	static wxMemoryInputStream memIStream( info_20_png, sizeof( info_20_png ) );
	static wxImage image( memIStream, wxBITMAP_TYPE_PNG );
	static wxBitmap bmp( image );
	return bmp;
}


#endif //INFO_20_PNG_H