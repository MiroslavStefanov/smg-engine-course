#pragma once
#include "../SMGRenderer/SMGRenderDevice.h"
#include "SMGGL.h"

extern "C"				__declspec(dllexport) SMGDisplay* DisplayFactory(int, int, int, int, char*, bool, SMGDisplay*);

class SMGGLFW : public SMGDisplay
{
public:
	SMGGLFW(int, int, int, int, char*);
	~SMGGLFW();

	STATUS OpenWindow(int r, int g, int b, int a, int depth, int stencil, int w, int h, int major, int minor, bool bWindowed);
	void Update();
	void Close();
	void UseWindow();

	bool IsDone();
	void* GetNativeWindow();
	void GetWindowRect(int &x, int &y, int &w, int &h);
	void GetPixelScreenRatio(double &ratioX, double &ratioY);

	static STATUS CreateContext();
	static void DestroyContext();
	static void Terminate();
	static void* GetNativeContext();

	friend void WindowSizeCallback(GLFWwindow*, int, int);
	friend void FramebufferSizeCallback(GLFWwindow*, int, int);

private:
	GLFWwindow* m_pWindow;
	GLFWmonitor* m_pMonitor;
	const GLFWvidmode* m_pMode;
	int m_nWidth, m_nHeight;
	int m_nFramebufferWidth, m_nFramebufferHeight;
	int m_nXpos, m_nYpos;
	char m_chTitle[256];

	static GLFWwindow* ms_context;
	static bool ms_bInitialized;
};
