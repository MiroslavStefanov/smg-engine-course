#pragma once

#include "SMGRenderDevice.h"

class SMGRenderer
{
public:
	SMGRenderer(char *chAPI);
	~SMGRenderer();

	STATUS				CreateDevice();
	void				Release();
	LPSMGRENDERDEVICE	GetDevice() { return m_pDevice; }
	MODULE				GetModule() { return m_hDLL; }

	SMGDisplay* OpenWindow(int x, int y, int w, int h, char* title, bool isChild = false, SMGDisplay* parent = NULL);
	
private:
	SMGRenderDevice *m_pDevice;
	MODULE			m_hDLL;
	DISPLAYFACTORY m_DisplayFactory;
};
typedef class SMGRenderer *LPSMGRENDERER;

