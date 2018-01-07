#include "SMGRenderer.h"
#include <cstdio>


SMGRenderer::SMGRenderer(char * chAPI)
{
	m_pDevice = NULL;
	m_hDLL = NULL;
	m_DisplayFactory = 0;

	char buffer[300];

	if (strcmp(chAPI, "OpenGL") == 0)
	{
#ifdef WIN32
		m_hDLL = LoadLibrary("SMGOGL.dll");
		if (!m_hDLL)
		{
			DWORD dw = GetLastError();
			MessageBox(NULL, "Loading SMGOGL.dll failed.", "SMGEngine - error", MB_OK | MB_ICONERROR);
		}
	}
	else
	{
		snprintf(buffer, 300, "API %s not supported.", chAPI);
		MessageBox(NULL, buffer, "SMGEngine - error", MB_OK | MB_ICONERROR);
	}
#endif
}


SMGRenderer::~SMGRenderer()
{
	Release();
}

STATUS SMGRenderer::CreateDevice()
{
		CREATERENDERDEVICE _CreateRenderDevice = 0;
		STATUS st;

		//Pointer to DLL function 'CreateRenderDevice'
#ifdef WIN32
		_CreateRenderDevice = (CREATERENDERDEVICE)GetProcAddress(m_hDLL, "CreateRenderDevice");
		if (!_CreateRenderDevice) return E_FAIL;
#endif
		st = _CreateRenderDevice(m_hDLL, &m_pDevice);
		if (FAILED(st))
		{
#ifdef WIN32
			MessageBox(NULL, "CreateRenderDevice() from lib failed.", "SMGEngine - error", MB_OK | MB_ICONERROR);
			m_pDevice = NULL;
			return E_FAIL;
#endif
		}
	return S_OK;
}

void SMGRenderer::Release()
{
	RELEASERENDERDEVICE _ReleaseRenderDevice = 0;
	STATUS st;

	if (m_hDLL)
	{
		//Pointer to DLL function 'ReleaseRenderDevice'
#ifdef WIN32
		_ReleaseRenderDevice = (RELEASERENDERDEVICE)GetProcAddress(m_hDLL, "ReleaseRenderDevice");
#endif
	}
	//call dll`s release function
	if (m_pDevice)
	{
		st = _ReleaseRenderDevice(&m_pDevice);
		if (FAILED(st))
		{
			m_pDevice = NULL;
		}
	}
}

SMGDisplay * SMGRenderer::OpenWindow(int x, int y, int w, int h, char* title, bool isChild, SMGDisplay* parent)
{
	if(m_DisplayFactory == 0)
		m_DisplayFactory = (DISPLAYFACTORY)GetProcAddress(m_hDLL, "DisplayFactory");

	if (m_DisplayFactory == 0)
		return NULL;

	return m_DisplayFactory(x, y, w, h, title, isChild, parent);
}
