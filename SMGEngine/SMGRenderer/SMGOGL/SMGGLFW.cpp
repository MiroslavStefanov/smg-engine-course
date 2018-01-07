#include "SMGGLFW.h"
#include "SMG.h"
#include <unordered_map>
#include <iostream>

GLFWwindow* SMGGLFW::ms_context = NULL;
bool SMGGLFW::ms_bInitialized = false;

static void WindowSizeCallback(GLFWwindow*, int, int);
static void FramebufferSizeCallback(GLFWwindow*, int, int);
static SMGGLFW *g_Window = NULL;

static void Error_callback(int error, const char* description)
{
	std::cerr << description << std::endl;
}

STATUS SMGGLFW::CreateContext()
{
	if (ms_context)
		return SMG_OK;

	if (!ms_bInitialized)
	{
		glfwInit();
		ms_bInitialized = true;
	}
	//glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
	glfwWindowHint(GLFW_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_VERSION_MINOR, 3);
#ifdef _DEBUG
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);
#endif
	ms_context = glfwCreateWindow(5, 5, "", NULL, NULL);
	//glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
	if (ms_context == NULL)
		return SMG_CREATECONTEXT;
	glfwMakeContextCurrent(ms_context);
	
	return SMG_OK;
}

void SMGGLFW::DestroyContext()
{
	if(ms_context)
		glfwDestroyWindow(ms_context);
}

void SMGGLFW::Terminate()
{
	if (!ms_bInitialized)
		return;
	glfwTerminate();
	ms_bInitialized = false;
}

void * SMGGLFW::GetNativeContext()
{
	if (!ms_context)
		return NULL;
	return glfwGetWin32Window(ms_context);
}

SMGGLFW::SMGGLFW(int x, int y, int w, int h, char *t)
{
	m_nXpos = x;
	m_nYpos = y;
	m_nWidth = w;
	m_nHeight = h;
	strcpy_s(m_chTitle, t);
	m_pWindow = NULL;
	m_pMonitor = NULL;
	m_pMode = NULL;
}

SMGGLFW::~SMGGLFW()
{
	if(m_pWindow)
		glfwDestroyWindow(m_pWindow);
	m_pWindow = NULL;
}

STATUS SMGGLFW::OpenWindow(int r, int g, int b, int a, int depth, int stencil, int w, int h, int major, int minor, bool bWindowed)
{
	if (!ms_bInitialized)
	{
		glfwInit();
		ms_bInitialized = true;
	}

	glfwSetErrorCallback(Error_callback);

	glfwWindowHint(GLFW_RED_BITS, r);
	glfwWindowHint(GLFW_GREEN_BITS, g);
	glfwWindowHint(GLFW_BLUE_BITS, b);
	glfwWindowHint(GLFW_ALPHA_BITS, a);
	glfwWindowHint(GLFW_DEPTH_BITS, depth);
	glfwWindowHint(GLFW_STENCIL_BITS, stencil);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef _DEBUG
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);
#endif

	if (!bWindowed)
		m_pMonitor = glfwGetPrimaryMonitor();

	m_pWindow = glfwCreateWindow(m_nWidth, m_nHeight, m_chTitle, m_pMonitor, NULL);
	if (m_pWindow == NULL)
		return SMG_CREATECONTEXT;

	if (!bWindowed)
		glfwSetWindowSize(m_pWindow, w, h);

	glfwGetFramebufferSize(m_pWindow, &m_nFramebufferWidth, &m_nFramebufferHeight);

	UseWindow();
	glfwSwapInterval(1);

	g_Window = this;
	glfwSetWindowSizeCallback(m_pWindow, WindowSizeCallback);
	glfwSetFramebufferSizeCallback(m_pWindow, FramebufferSizeCallback);

	return SMG_OK;
}

void SMGGLFW::Update()
{
	glfwSwapBuffers(m_pWindow);
	glfwPollEvents();
}

void SMGGLFW::Close()
{
	glfwSetWindowShouldClose(m_pWindow, GLFW_TRUE);
}

void SMGGLFW::UseWindow()
{
	glfwMakeContextCurrent(m_pWindow);
	glViewport(0, 0, m_nFramebufferWidth, m_nFramebufferHeight);
}

bool SMGGLFW::IsDone()
{
	return glfwWindowShouldClose(m_pWindow);
}

void* SMGGLFW::GetNativeWindow()
{
#ifdef WIN32
	return glfwGetWin32Window(m_pWindow);
#endif
}

void SMGGLFW::GetWindowRect(int & x, int & y, int & w, int & h)
{
	x = m_nXpos;
	y = m_nYpos;
	w = m_nWidth;
	h = m_nHeight;
}

void SMGGLFW::GetPixelScreenRatio(double & ratioX, double & ratioY)
{
	ratioX = m_nFramebufferWidth / m_nWidth;
	ratioY = m_nFramebufferHeight / m_nHeight;
}

void WindowSizeCallback(GLFWwindow *pWindow, int nWidth, int nHeight)
{
	g_Window->m_nWidth = nWidth;
	g_Window->m_nHeight = nHeight;
}

void FramebufferSizeCallback(GLFWwindow *pWindow, int nWidth, int nHeight)
{
	g_Window->m_nFramebufferHeight = nHeight;
	g_Window->m_nFramebufferWidth = nWidth;
}

