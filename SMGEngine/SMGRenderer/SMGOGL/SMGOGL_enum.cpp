#include "SMGOGLEnum.h"
#include "SMG.h"
#include "resource.h"
#include "SMGGLFW.h"

extern bool g_bLF;

const uint g_nNumFormats = 1;
const PixelFormat g_fmtMonitor[] = { PixelFormat(8, 8, 8, 8) };

void AddItem(HWND hWnd, const char *ch, void *pData);

STATUS SMGOGLEnum::Enum(HWND hADAPTER, HWND hMODE, HWND hFORMAT, HWND hVERSION, HWND hWND, HWND hFULL, FILE *pLog)
{
	if (pLog) m_pLog = pLog;

	m_hADAPTER = hADAPTER;
	m_hMODE = hMODE;
	m_hFORMAT = hFORMAT;
	m_hVERSION = hVERSION;
	m_hWND = hWND;
	m_hFULL = hFULL;

	m_nMinWidth = 800;
	m_nMinHeight = 600;

	int nNumModes;
	const GLFWvidmode *Modes = glfwGetVideoModes(glfwGetPrimaryMonitor(), &nNumModes);
	for (int i = 0; i < nNumModes; i++)
		m_vResolutions.emplace_back(Modes[i].width, Modes[i].height);
	m_pixelFormat = PixelFormat(Modes[0].redBits, Modes[0].greenBits, Modes[0].blueBits, 8);
	EnumAdapter();
	EnumVersion();

	SendMessage(m_hADAPTER, CB_RESETCONTENT, 0, 0);
	AddItem(m_hADAPTER, m_chAdapter, NULL);	

	SendMessage(m_hVERSION, CB_RESETCONTENT, 0, 0);
	for (uint i = 0; i <= m_nLastVersion; i++)
	{
		AddItem(m_hVERSION, m_Versions[i].name, (void*)(&m_Versions[i]));
	}

	SendMessage(m_hVERSION, CB_SETCURSEL, (WPARAM)m_nLastVersion, 0);
	SendMessage(m_hADAPTER, CB_SETCURSEL, (WPARAM)0, 0);

	ChangedWindowMode();

	SendMessage(m_hMODE, CB_SETCURSEL, (WPARAM)0, 0);
	SendMessage(m_hFORMAT, CB_SETCURSEL, (WPARAM)0, 0);

	return SMG_OK;
}
/*----------------------------------------------------------------*/

/**
* Three helper function for combobox-convenience.
*/
void* GetSelectedItem(HWND hWnd)
{
	WPARAM nI = (WPARAM)((int)(DWORD)SendMessage(hWnd, CB_GETCURSEL, 0, 0));
	return ((void*)SendMessage(hWnd, CB_GETITEMDATA, nI, 0));
}
/*----------------------------------------------------------------*/

void AddItem(HWND hWnd, const char *ch, void *pData)
{
	WPARAM nI = (WPARAM)((int)(DWORD)SendMessage(hWnd, CB_ADDSTRING, 0, (LPARAM)ch));
	SendMessage(hWnd, CB_SETITEMDATA, nI, (LPARAM)pData);
}
/*----------------------------------------------------------------*/

bool ContainsString(HWND hWnd, const char *ch)
{
	int n = (int)SendMessage(hWnd, CB_GETCOUNT, 0, 0);
	char buffer[200];

	for (int nIndex = 0; nIndex<n; nIndex++)
	{
		SendMessage(hWnd, CB_GETLBTEXT, (WPARAM)nIndex, (LPARAM)buffer);
		if (lstrcmp(buffer, ch) == 0)
			return true;
	}
	return false;
}
/*----------------------------------------------------------------*/

void SMGOGLEnum::GetSelections(Resolution * res, PixelFormat * fmt, Version * version, bool *bWindowed)
{
	if(res)
		memcpy(res, GetSelectedItem(m_hMODE), sizeof(Resolution));
	if (fmt)
		memcpy(fmt, GetSelectedItem(m_hFORMAT), sizeof(PixelFormat));
	if(version)
		memcpy(version, GetSelectedItem(m_hVERSION), sizeof(Version));
	*bWindowed = SendMessage(m_hWND, BM_GETCHECK, 0, 0) == BST_CHECKED;
}

void SMGOGLEnum::ChangedWindowMode()
{
	SendMessage(m_hFORMAT, CB_RESETCONTENT, 0, 0);
	SendMessage(m_hMODE, CB_RESETCONTENT, 0, 0);

	if (SendMessage(m_hWND, BM_GETCHECK, 0, 0) == BST_CHECKED)
	{
		// use current desktop format
		AddItem(m_hFORMAT, m_pixelFormat.name,
				&m_pixelFormat);

		EnableWindow(m_hFORMAT, false);
		EnableWindow(m_hMODE, false);
	}
	else
	{
		EnableWindow(m_hFORMAT, true);
		EnableWindow(m_hMODE, true);

		// list all combos for this device
		for (UINT f = 0; f<g_nNumFormats; f++)
		{
			if (!ContainsString(m_hFORMAT, g_fmtMonitor[f].name))
				AddItem(m_hFORMAT, g_fmtMonitor[f].name, (void*)(&g_fmtMonitor[f]));
		}
	}

	SendMessage(m_hFORMAT, CB_SETCURSEL, (WPARAM)0, 0);
	SendMessage(m_hMODE, CB_SETCURSEL, (WPARAM)0, 0);

	ChangedFormat();
}

void SMGOGLEnum::ChangedFormat()
{
	PixelFormat *fmt;
	bool bWindowed;

	fmt = (PixelFormat*)GetSelectedItem(m_hFORMAT);

	bWindowed = (SendMessage(m_hWND, BM_GETCHECK, 0, 0) == BST_CHECKED);
	if(!bWindowed)
	{
		SendMessage(m_hMODE, CB_RESETCONTENT, 0, 0);

		for (uint i = 0; i < m_vResolutions.size(); i++)
		{
			if (!ContainsString(m_hMODE, m_vResolutions[i].name))
				AddItem(m_hMODE, m_vResolutions[i].name, &m_vResolutions[i]);
		}
		SendMessage(m_hMODE, CB_SETCURSEL, (WPARAM)0, 0);
	}
	else
		AddItem(m_hMODE, m_vResolutions[0].name, &m_vResolutions[0]);
}

void SMGOGLEnum::EnumAdapter()
{
	std::string vendor((char*)glGetString(GL_VENDOR));
	std::string renderer((char*)glGetString(GL_RENDERER));
	renderer = renderer.substr(0, renderer.find_first_of('/'));
	std::string adapter = vendor + ' ' + renderer;
	strcpy_s(m_chAdapter, adapter.c_str());
}

void SMGOGLEnum::EnumVersion()
{
	int minor, major;
	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);
	uint i;
	for (i = 0; i < 19; i++)
	{
		if (m_Versions[i].major == major && m_Versions[i].minor == minor)
			break;
	}
	m_nLastVersion = i;
}
