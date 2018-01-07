#pragma once
#include "../SMGRenderer/SMGRenderDevice.h"
#include <cstdio>
#include <string>
#include <sstream>
#include <vector>
#include "SMGGL.h"

struct PixelFormat
{
	int r, g, b, a;
	char name[256];
	PixelFormat(int _r = 0, int _g = 0, int _b = 0, int _a = 0) : r(_r), g(_g), b(_b), a(_a)
	{
		std::stringstream ss;
		ss << "A" << a << "R" << r << "G" << g << "B" << b;
		std::string s;
		ss >> s;
		strcpy_s(name, s.c_str());
	}		
};

struct Resolution
{
	uint w, h;
	char name[256];
	Resolution(uint _w = 0, uint _h = 0) : w(_w), h(_h)
	{
		std::stringstream ss;
		ss << w << "x" << h;
		std::string s;
		ss >> s;
		strcpy_s(name, s.c_str());
	}
};

struct Version
{
	int major, minor;
	char name[256];
	Version(int m = 1, int n = 0) : major(m), minor(n)
	{
		std::stringstream ss;
		ss << major << "." << minor;
		std::string s;
		ss >> s;
		strcpy_s(name, s.c_str());
	}
};

class SMGOGLEnum
{
public:
	SMGOGLEnum(int nMinDepth, int nMinStencil)
	{
		m_nMinDepth = nMinDepth; m_nMinStencil = nMinStencil;
	}
	~SMGOGLEnum() {};

	STATUS Enum(HWND, HWND, HWND, HWND, HWND, HWND, FILE*);

	void ChangedWindowMode();
	void ChangedFormat();

	void GetSelections(Resolution *res, PixelFormat *fmt, Version *version, bool *bWindowed);

	uint m_nMinDepth;
	uint m_nMinStencil;

private:
	PixelFormat m_pixelFormat;
	const Version m_Versions[19] = { { 1, 0 },{ 1, 1 },{ 1, 2 },{ 1, 3 },{ 1, 4 },{ 1, 5 },{ 2, 0 },{ 2, 1 },{ 3, 0 },
	{ 3, 1 },{ 3, 2 },{ 3, 3 },{ 4, 0 },{ 4, 1 },{ 4, 2 },{ 4, 3 },{ 4, 4 },{ 4, 5 },{ 4, 6 } };
	uint m_nLastVersion;
	std::vector<Resolution> m_vResolutions;
	uint m_nMinWidth;
	uint m_nMinHeight;
	FILE *m_pLog;
	char m_chAdapter[256];

	HWND m_hADAPTER, m_hMODE, m_hFORMAT, m_hVERSION, m_hWND, m_hFULL;

	void EnumAdapter();
	void EnumVersion();
};