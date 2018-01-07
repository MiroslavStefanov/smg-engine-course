#pragma once

#include "SMGOGLEnum.h"

#define MAX_3DWND 8

extern "C"				__declspec(dllexport) STATUS CreateRenderDevice(MODULE hDLL, SMGRenderDevice **pInterface);
extern "C"				__declspec(dllexport) STATUS ReleaseRenderDevice(SMGRenderDevice **pInterface);

class SMGGLFW;
class SMGChildWnd;

class SMGOGL : public SMGRenderDevice
{
public:
	SMGOGL(MODULE instance);
	~SMGOGL();

	//INITIALIZATION
	STATUS Init(SMGDisplay*, const SMGDisplay**, int, int, int, bool);

#ifdef WIN32
	BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
#endif

	//Interface functions
	void	Release();
	bool	IsRunning() { return m_bRunning; }
	void	BeginRendering(bool, bool, bool);
	void	Clear(bool, bool, bool);
	void	EndRendering();
	void	SetClearColor(float, float, float);
	STATUS	UseWindow(uint nWindow);

private:
	SMGGLFW *m_pMainWindow;
	SMGChildWnd *m_Windows[MAX_3DWND];
	uint m_nNumWindows;
	uint m_nActiveWindow;
	MODULE m_hDLL;
	bool m_bWindowed;
	char m_chAdapter[256];
	FILE *m_pLog;
	bool m_bRunning;
	bool m_bIsSceneRunning;
	bool m_bStencil;
	SMGOGLEnum *m_pEnum;

	//start the API
	STATUS Go();

	void Log(char *, ...);
};
