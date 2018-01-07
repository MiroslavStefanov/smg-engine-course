#include "SMGOGL.h"
#include "SMGChildWnd.h"
#include "SMG.h"


void SMGOGL::BeginRendering(bool bClearPixel, bool bClearDepth, bool bClearStencil)
{
	Clear(bClearPixel, bClearDepth, bClearStencil);

	m_bIsSceneRunning = true;
}

void SMGOGL::Clear(bool bClearPixel, bool bClearDepth, bool bClearStencil)
{
	GLbitfield clear;
	if (bClearPixel) clear = GL_COLOR_BUFFER_BIT;
	if (bClearDepth) clear |= GL_DEPTH_BUFFER_BIT;
	if (m_bStencil && bClearStencil) clear |= GL_STENCIL_BUFFER_BIT;
	
	glClear(clear);
}

void SMGOGL::EndRendering()
{
	m_pMainWindow->Update();
	m_bIsSceneRunning = false;
}

void SMGOGL::SetClearColor(float r, float g, float b)
{
	glClearColor(r, g, b, 1.0f);
}

STATUS SMGOGL::UseWindow(uint nWindow)
{
	if (!m_bWindowed)
		return SMG_OK;
	if (nWindow >= m_nNumWindows)
		return SMG_FAIL;
	m_nActiveWindow = nWindow;
	m_Windows[m_nActiveWindow]->UseWindow();
	return SMG_OK;
}