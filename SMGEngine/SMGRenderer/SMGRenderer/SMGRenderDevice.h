#pragma once

#ifdef WIN32
#include <windows.h>
typedef HRESULT STATUS;
typedef HMODULE MODULE;
#endif

typedef unsigned int uint;

class SMGDisplay
{
public:
	SMGDisplay() {};
	virtual ~SMGDisplay() {};

	virtual void Close() = 0;
	virtual bool IsDone() = 0;
	virtual void* GetNativeWindow() = 0;
	virtual void GetWindowRect(int &x, int &y, int &w, int &h) = 0;
	virtual void GetPixelScreenRatio(double &ratioX, double &ratioY) = 0;
};

class SMGRenderDevice
{
public:
	SMGRenderDevice() {};
	virtual ~SMGRenderDevice() {};

	//INIT/RELEASE STUFF:
	//===================
	virtual STATUS Init(SMGDisplay*, const SMGDisplay**,
						int, int, int, bool) = 0;
	virtual void	Release()				= 0;
	virtual bool	IsRunning()				= 0;

	//RENDERING STUFF:
	//================
	virtual STATUS UseWindow(uint nWindow) = 0;
	virtual void BeginRendering(bool bClearPixel,
								  bool bClearDepth,
								  bool bClearStencil) = 0;
	virtual void	EndRendering() = 0;
	virtual void	Clear(bool bClearPixel,
						  bool bClearDepth,
						  bool bClearStencil) = 0;
	virtual void	SetClearColor(float fRed,
								  float fGreen,
								  float fBlue) = 0;
};
typedef class SMGRenderDevice *LPSMGRENDERDEVICE;
/*----------------------------------------------------------------*/


extern "C"
{
	typedef STATUS(*CREATERENDERDEVICE)(MODULE hDLL, SMGRenderDevice **pInterface);

	typedef STATUS(*RELEASERENDERDEVICE)(SMGRenderDevice **pInterface);

	typedef SMGDisplay*(*DISPLAYFACTORY)(int, int, int, int, char*, bool, SMGDisplay*);
}
/*----------------------------------------------------------------*/