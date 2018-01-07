#pragma once
#include "SMGGLFW.h"

class SMGChildWnd : public SMGDisplay
{
public:
	SMGChildWnd(int x, int y, int w, int h, SMGDisplay* p);
	~SMGChildWnd();


	void Close() {};
	bool IsDone() { return false; }
	void* GetNativeWindow() { return NULL; }
	void GetWindowRect(int &x, int &y, int &w, int &h);
	void GetPixelScreenRatio(double &ratioX, double &ratioY);

	void UseWindow();

private:
	int m_nWidth, m_nHeight;
	int m_nFramebufferWidth, m_nFramebufferHeight;
	int m_nXpos, m_nYpos;
	SMGDisplay *m_pParent;
};

