#include "SMGChildWnd.h"


SMGChildWnd::SMGChildWnd(int x, int y, int w, int h, SMGDisplay * p)
{
	m_nWidth = w;
	m_nHeight = h;
	m_nXpos = x;
	m_nYpos = y;
	m_pParent = p;
}

SMGChildWnd::~SMGChildWnd()
{
}

void SMGChildWnd::GetWindowRect(int & x, int & y, int & w, int & h)
{
	int x1, y1, w1, h1;
	m_pParent->GetWindowRect(x1, y1, w1, h1);
	x = x1 + m_nXpos; y = y1 + m_nYpos;
	w = m_nWidth; h = m_nHeight;
}

void SMGChildWnd::GetPixelScreenRatio(double & ratioX, double & ratioY)
{
	m_pParent->GetPixelScreenRatio(ratioX, ratioY);
}

void SMGChildWnd::UseWindow()
{
	double ratioX, ratioY;
	int x, y, w, h;
	m_pParent->GetWindowRect(x, y, w, h);
	m_pParent->GetPixelScreenRatio(ratioX, ratioY);
	m_nFramebufferWidth = ratioX*(double)m_nWidth;
	m_nFramebufferHeight = ratioY*(double)m_nHeight;
	int offsetX = ratioX*(double)m_nXpos;
	int offsetY = ratioY*((double)h - (double)m_nYpos) - m_nFramebufferHeight;
	glViewport(offsetX, offsetY, m_nFramebufferWidth, m_nFramebufferHeight);
	glScissor(offsetX, offsetY, m_nFramebufferWidth, m_nFramebufferHeight);
	glEnable(GL_SCISSOR_TEST);
}

SMGDisplay * DisplayFactory(int x, int y, int w, int h, char * title, bool bChild, SMGDisplay * parent)
{
	if (!bChild)
		return new SMGGLFW(x, y, w, h, title);
	else
		return new SMGChildWnd(x, y, w, h, parent);
}
