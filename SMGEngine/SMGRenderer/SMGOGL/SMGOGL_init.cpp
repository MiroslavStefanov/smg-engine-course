#include "SMGOGL.h"
#include "SMGChildWnd.h"
#include "SMG.h"
#include "resource.h"
#include <iostream>

SMGOGL *g_SMGOGL = NULL;
bool g_bLF = false;
using namespace std;

#ifdef WIN32
BOOL WINAPI DllEntryPoint(HINSTANCE hDll,
						  DWORD     fdwReason,
						  LPVOID    lpvReserved)
{
	switch (fdwReason)
	{
		// called when we attach to the DLL
	case DLL_PROCESS_ATTACH:
		/* dll init/setup stuff */
		break;
	case DLL_PROCESS_DETACH:
		/* dll shutdown/release stuff */
		break;
	default:
		break;
	};

	return TRUE;
}
#endif

STATUS CreateRenderDevice(MODULE hDLL, SMGRenderDevice ** pInterface)
{
	if (!*pInterface)
	{
		*pInterface = new SMGOGL(hDLL);
		return SMG_OK;
	}
	return SMG_FAIL;
}

STATUS ReleaseRenderDevice(SMGRenderDevice ** pInterface)
{
	if (!*pInterface) return SMG_FAIL;
	delete *pInterface;
	*pInterface = NULL;
	return SMG_OK;
}

BOOL CALLBACK DlgProcWrap(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	return g_SMGOGL->DlgProc(hDlg, message, wParam, lParam);
}

static void __stdcall GL_error_callback(GLenum source​, GLenum type​, GLuint id​,
							  GLenum severity​, GLsizei length​, const GLchar* message​, const void* userParam);

SMGOGL::SMGOGL(MODULE hDLL)
{
	m_hDLL = hDLL;
	m_pEnum = NULL;
	m_pLog = NULL;
	m_bRunning = false;
	m_bIsSceneRunning = false;

	m_nActiveWindow = 0;

	g_SMGOGL = this;

	m_pLog = fopen("Log_SMGRenderDevice.txt", "w");
	Log("online (waiting for initialization call)");
}

SMGOGL::~SMGOGL()
{
	Release();
}

void SMGOGL::Release()
{
	if (m_pMainWindow)
	{
		delete m_pMainWindow;
		m_pMainWindow = NULL;
	}
	for(uint i = 0; i<m_nNumWindows; i++)
		if (m_Windows[i])
		{
			delete m_Windows[i];
			m_Windows[i] = NULL;
		}
	SMGGLFW::Terminate();
	if (m_pEnum)
	{
		delete m_pEnum;
		m_pEnum = NULL;
	}
	Log("offline (ok)");
	fclose(m_pLog);
}

STATUS SMGOGL::Init(SMGDisplay *pMainWindow, const SMGDisplay **pWindows, int nNumWindows, int nMinDepth, int nMinStencil, bool bSaveLog)
{
	int nResult;

	g_bLF = bSaveLog;

	if (FAILED(SMGGLFW::CreateContext()))
		return SMG_CREATECONTEXT;

	glewExperimental = GL_TRUE;
	GLenum status = glewInit();
	if (status != GLEW_OK)
	{
		std::cerr << "There was a problem initializing GLEW. Exiting..." << std::endl;
		std::cerr << glewGetErrorString(status) << std::endl;
		exit(-1);
	}

	if (nNumWindows > 0)
	{
		if (nNumWindows > MAX_3DWND) nNumWindows = MAX_3DWND;
		memcpy(&m_Windows[0], (SMGChildWnd**)pWindows, sizeof(SMGChildWnd*)*nNumWindows);
		m_nNumWindows = nNumWindows;
	}
	else
	{
		m_Windows[0] = (SMGChildWnd*)pMainWindow;
		m_nNumWindows = 0;
	}
	m_pMainWindow = (SMGGLFW*)pMainWindow;

#ifdef _DEBUG
	glDebugMessageCallback(GL_error_callback, NULL);
#endif
	
	if (nMinStencil > 0) m_bStencil = true;
	m_pEnum = new SMGOGLEnum(nMinDepth, nMinStencil);

	Log("calling dialog");

#ifdef WIN32
	nResult = DialogBox(m_hDLL, "dlgChangeDevice", (HWND)SMGGLFW::GetNativeContext(), DlgProcWrap);
#endif

	Log("dialog finished");

	SMGGLFW::DestroyContext();

	if (nResult == -1)
	{
		Log("error: selection dialog error");
		return SMG_FAIL;
	}		
	else if (nResult == 0)
	{
		Log("warning: selection dialog canceled by user");
		return SMG_CANCELED;
	}
	else
	{
		Log("firing up OpenGL");
		return Go();
	}
}


PixelFormat g_PixelFormat;
Resolution g_Resolution;
Version g_Version;


STATUS SMGOGL::Go()
{
	STATUS st = m_pMainWindow->OpenWindow(g_PixelFormat.r, g_PixelFormat.g, g_PixelFormat.b, g_PixelFormat.a,
										  m_pEnum->m_nMinDepth, m_pEnum->m_nMinStencil, g_Resolution.w, g_Resolution.h,
										  g_Version.major, g_Version.minor, m_bWindowed);

	delete m_pEnum;
	m_pEnum = NULL;

	if (st != SMG_OK) return st;

	m_bRunning = true;
	m_bIsSceneRunning = false;
	Log("initialized (online and ready)");
	return SMG_OK;
}

void SMGOGL::Log(char *chString, ...)
{
	char ch[256];
	char *pArgs;

	pArgs = (char*)&chString + sizeof(chString);
	vsprintf(ch, chString, pArgs);
	fprintf(m_pLog, "[SMGOGLDevice]: ");
	fprintf(m_pLog, ch);
	fprintf(m_pLog, "\n");

	if (g_bLF)
		fflush(m_pLog);
}

#ifdef WIN32
BOOL SMGOGL::DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	BOOL bWnd = FALSE;

	//get handles
	HWND hFULL = GetDlgItem(hDlg, IDC_FULL);
	HWND hWND = GetDlgItem(hDlg, IDC_WND);
	HWND hADAPTER = GetDlgItem(hDlg, IDC_ADAPTER);
	HWND hMODE = GetDlgItem(hDlg, IDC_MODE);
	HWND hFORMAT = GetDlgItem(hDlg, IDC_FORMAT);
	HWND hVERSION = GetDlgItem(hDlg, IDC_VERSION);

	switch (message)
	{
	case WM_INITDIALOG:
	{
		SendMessage(hWND, BM_SETCHECK, BST_CHECKED, 0);
		m_pEnum->Enum(hADAPTER, hMODE, hFORMAT, hVERSION, hWND, hFULL, m_pLog);
		return TRUE;
	}
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case IDOK:
		{
			m_bWindowed = SendMessage(hFULL, BM_GETCHECK, 0, 0) != BST_CHECKED;
			m_pEnum->GetSelections(&g_Resolution, &g_PixelFormat, &g_Version, &m_bWindowed);
			GetWindowText(hADAPTER, m_chAdapter, 256);
			EndDialog(hDlg, 1);
			return TRUE;
		} break;
		case IDCANCEL:
		{
			EndDialog(hDlg, 0);
			return TRUE;
		} break;
		case IDC_FORMAT:
		{
			if (HIWORD(wParam) == CBN_SELCHANGE)
				m_pEnum->ChangedFormat();
		} break;
		case IDC_FULL: case IDC_WND:
		{
			m_pEnum->ChangedWindowMode();
		} break;
		}
	} break;
	}
	return FALSE;
}
#endif

void __stdcall GL_error_callback(GLenum source​, GLenum type​, GLuint id​, GLenum severity​, GLsizei length​, const GLchar * message​, const void * userParam)
{
	cout << "---------------------opengl-callback-start------------" << endl;
	cout << "message: " << message​ << endl;
	cout << "source: ";
	switch (source​)
	{
	case GL_DEBUG_SOURCE_API:
		cout << "API";
		break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
		cout << "WINDOW_SYSTEM";
		break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER:
		cout << "SHADER_COMPILER";
		break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:
		cout << "THIRD_PARTY";
		break;
	case GL_DEBUG_SOURCE_APPLICATION:
		cout << "APPLICATION";
		break;
	case GL_DEBUG_SOURCE_OTHER:
		cout << "OHTER";
		break;
	}
	cout << endl;
	cout << "type: ";
	switch (type​)
	{
	case GL_DEBUG_TYPE_ERROR:
		cout << "ERROR";
		break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		cout << "DEPRECATED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		cout << "UNDEFINED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_PORTABILITY:
		cout << "PORTABILITY";
		break;
	case GL_DEBUG_TYPE_PERFORMANCE:
		cout << "PERFORMANCE";
		break;
	case GL_DEBUG_TYPE_OTHER:
		cout << "OTHER";
		break;
	}
	cout << endl;

	cout << "id: " << id​ << endl;
	cout << "severity: ";
	switch (severity​)
	{
	case GL_DEBUG_SEVERITY_LOW:
		cout << "LOW";
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		cout << "MEDIUM";
		break;
	case GL_DEBUG_SEVERITY_HIGH:
		cout << "HIGH";
		break;
	}
	cout << endl;
	cout << "---------------------opengl-callback-end--------------" << endl;
	if (severity​ == GL_DEBUG_SEVERITY_HIGH)
		throw(exception("GL ERROR WITH HIGH SEVERITY"));
}