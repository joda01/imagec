#ifndef LAYERS_20_PNG_H
#define LAYERS_20_PNG_H

#include <wx/mstream.h>
#include <wx/image.h>
#include <wx/bitmap.h>

static const unsigned char layers_20_png[] =
{
	0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, 0x00, 0x00, 
	0x00, 0x0D, 0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x14, 
	0x00, 0x00, 0x00, 0x14, 0x08, 0x06, 0x00, 0x00, 0x00, 0x8D, 
	0x89, 0x1D, 0x0D, 0x00, 0x00, 0x00, 0x09, 0x70, 0x48, 0x59, 
	0x73, 0x00, 0x00, 0x0B, 0x13, 0x00, 0x00, 0x0B, 0x13, 0x01, 
	0x00, 0x9A, 0x9C, 0x18, 0x00, 0x00, 0x01, 0xFC, 0x49, 0x44, 
	0x41, 0x54, 0x78, 0x9C, 0x95, 0xD4, 0xCB, 0x8B, 0x4F, 0x61, 
	0x1C, 0x06, 0xF0, 0x8F, 0xFB, 0x25, 0xBF, 0x84, 0xF9, 0x4D, 
	0x09, 0x4D, 0x4A, 0x2C, 0xC4, 0x28, 0xC2, 0x8C, 0x2C, 0xC4, 
	0x42, 0x1A, 0x92, 0xDC, 0x4A, 0x61, 0x31, 0x14, 0x45, 0xB9, 
	0x25, 0x29, 0xE5, 0x16, 0x53, 0x16, 0x9A, 0xD9, 0x20, 0xB7, 
	0x28, 0x59, 0x48, 0x72, 0x17, 0xCA, 0x35, 0xF7, 0xEB, 0x42, 
	0xD9, 0xFA, 0x17, 0x2C, 0x6D, 0xF4, 0x4E, 0xDF, 0xF3, 0xEB, 
	0x75, 0xFA, 0xCD, 0x98, 0x79, 0x36, 0xE7, 0xBC, 0xEF, 0x79, 
	0xCF, 0xF3, 0xFD, 0x7E, 0x9F, 0xE7, 0x39, 0x87, 0xDE, 0x31, 
	0x10, 0x87, 0xF0, 0x13, 0x6F, 0x71, 0x15, 0xFB, 0xD1, 0x86, 
	0xC9, 0x18, 0xA0, 0x1F, 0x68, 0xC0, 0x7D, 0x5C, 0xC6, 0x38, 
	0x54, 0xD1, 0x8E, 0xB5, 0x38, 0x81, 0x1B, 0xF8, 0x8C, 0x37, 
	0xB8, 0x80, 0x3D, 0x58, 0xD8, 0x13, 0xD9, 0xBC, 0x38, 0x7C, 
	0x31, 0xD6, 0xAD, 0x68, 0x89, 0xFB, 0xBD, 0x71, 0xDD, 0x14, 
	0x45, 0x26, 0xE2, 0x28, 0xEE, 0xE2, 0x71, 0x3D, 0xB2, 0x1D, 
	0x78, 0x8E, 0xAE, 0xD2, 0x8B, 0xD5, 0xB8, 0xCF, 0x49, 0x5B, 
	0xA3, 0xD0, 0x2E, 0xDC, 0xC1, 0x88, 0x9C, 0xA8, 0x82, 0xEB, 
	0xA1, 0xD3, 0xAA, 0x8C, 0x2C, 0x61, 0x16, 0x9A, 0x4B, 0x7B, 
	0xCB, 0x43, 0x8A, 0xC3, 0xB8, 0x86, 0x21, 0x39, 0xD9, 0x74, 
	0xBC, 0xC3, 0x6E, 0x34, 0x61, 0x6C, 0x54, 0xAF, 0x06, 0x59, 
	0xD1, 0x61, 0x73, 0x5C, 0x5B, 0x43, 0xE3, 0x2E, 0x9C, 0x0B, 
	0xF3, 0x6A, 0xD8, 0x10, 0xC2, 0xB6, 0x04, 0x59, 0x53, 0x54, 
	0x4E, 0xEB, 0xC6, 0x8C, 0xA4, 0x20, 0x6C, 0x0C, 0xF1, 0x93, 
	0xBE, 0x4F, 0xF1, 0x01, 0xEF, 0xA3, 0xCB, 0x9D, 0x89, 0xF0, 
	0x0C, 0xBE, 0xE3, 0x17, 0x0E, 0x84, 0x21, 0x29, 0x16, 0x09, 
	0x1B, 0xE3, 0xDA, 0x9C, 0x8D, 0xBC, 0x25, 0x1C, 0x4E, 0xF1, 
	0x29, 0x30, 0x14, 0xB7, 0xD1, 0x59, 0x1E, 0x3B, 0x1D, 0xFA, 
	0x83, 0x1F, 0xF8, 0x14, 0x84, 0x07, 0xB3, 0x0E, 0xB7, 0xE3, 
	0x21, 0xB6, 0x66, 0xEF, 0x0D, 0xC6, 0x15, 0x1C, 0x57, 0xC2, 
	0xB4, 0xD0, 0x71, 0x33, 0x06, 0x61, 0x0E, 0xF6, 0xE1, 0x01, 
	0xBE, 0xE2, 0x2C, 0x5E, 0x62, 0x5D, 0xF6, 0xCE, 0x30, 0xDC, 
	0xCC, 0x5C, 0xAF, 0x61, 0x75, 0x68, 0x31, 0x43, 0x7D, 0x24, 
	0x07, 0x17, 0x60, 0x6E, 0xB6, 0x37, 0x2A, 0x72, 0xD7, 0x19, 
	0xC4, 0x35, 0x74, 0xE0, 0x49, 0xC4, 0xA6, 0xAF, 0x18, 0x13, 
	0x59, 0xED, 0xC0, 0xA5, 0x30, 0x35, 0x49, 0x74, 0x3E, 0x3D, 
	0xDC, 0x86, 0x7B, 0x31, 0x56, 0x8A, 0xC0, 0xFA, 0x70, 0xB2, 
	0x27, 0x8C, 0x0F, 0x82, 0xA5, 0xD9, 0x5E, 0x25, 0xBA, 0x6D, 
	0x2F, 0x8F, 0x95, 0x22, 0xF4, 0x1B, 0x2F, 0x42, 0x82, 0xD3, 
	0x58, 0x81, 0xD1, 0x71, 0x26, 0xFD, 0x10, 0x3E, 0xC6, 0xF8, 
	0x05, 0x1A, 0xF0, 0x0A, 0x6B, 0xCA, 0x95, 0xDB, 0xE2, 0xF0, 
	0xEC, 0x58, 0xA7, 0xCF, 0x68, 0x31, 0x8E, 0x85, 0x19, 0xAF, 
	0x63, 0x8A, 0x14, 0xF4, 0x02, 0x13, 0xA2, 0x70, 0xDE, 0x6D, 
	0xB7, 0xED, 0xE9, 0xEF, 0x71, 0x2B, 0xB4, 0xE9, 0x09, 0x95, 
	0x08, 0x7C, 0x81, 0x29, 0xF8, 0x52, 0xEA, 0xB6, 0x1B, 0x8F, 
	0x22, 0xF1, 0xF9, 0xE1, 0xFF, 0x61, 0x26, 0xBE, 0x95, 0xBA, 
	0x55, 0x60, 0x3E, 0x8E, 0x84, 0x6B, 0xE9, 0x27, 0x7A, 0x0A, 
	0xCB, 0x22, 0x16, 0xF5, 0x90, 0x3E, 0xC9, 0x34, 0xFA, 0xD4, 
	0xBE, 0x54, 0x1E, 0x89, 0x25, 0x38, 0x89, 0x67, 0xE1, 0x66, 
	0xBA, 0x4F, 0x7B, 0xC3, 0xB1, 0x28, 0x34, 0x9B, 0xD4, 0x8F, 
	0x69, 0xFE, 0x41, 0xD2, 0x74, 0x65, 0x84, 0x37, 0x19, 0x96, 
	0xCC, 0xE9, 0x2D, 0x52, 0xFE, 0x02, 0x60, 0xAA, 0x61, 0xA0, 
	0xCA, 0xF7, 0x6B, 0x88, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 
	0x4E, 0x44, 0xAE, 0x42, 0x60, 0x82, 
};

wxBitmap& layers_20_png_to_wx_bitmap()
{
	static wxMemoryInputStream memIStream( layers_20_png, sizeof( layers_20_png ) );
	static wxImage image( memIStream, wxBITMAP_TYPE_PNG );
	static wxBitmap bmp( image );
	return bmp;
}


#endif //LAYERS_20_PNG_H