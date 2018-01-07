// SMGDemo.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "SMGRenderDevice.h"
#include "SMGRenderer.h"
#include "SMG.h"
#pragma comment(lib, "SMGRenderer.lib")

bool ProgramStartup();
void ProgramCleanup();

SMGRenderer *g_pRenderer = NULL;
SMGRenderDevice *g_pDevice = NULL;
SMGDisplay *g_pDisplay = NULL;

int main()
{
	g_pRenderer = new SMGRenderer("OpenGL");
	if (!g_pRenderer) return -1;

	if (g_pRenderer->CreateDevice() == SMG_FAIL)
		return -1;

	g_pDisplay = g_pRenderer->OpenWindow(1000, 700, 380, 280, "Cranking up SMGEngine...");

	if (!ProgramStartup())
		return -1;

	int wnd = 0;
	float cc[][3] = { { 0.1f, 0.3f, 0.1f },{ 0.000f, 0.749f, 1.000f },{ 0.855f, 0.439f, 0.839f },{ 0.957f, 0.643f, 0.376f } };

	while (!g_pDisplay->IsDone())
	{
		if (g_pDevice->IsRunning())
		{			
			g_pDevice->BeginRendering(true, true, true);
			for (wnd = 0; wnd < 4; wnd++)
			{
				g_pDevice->UseWindow(wnd);
				g_pDevice->SetClearColor(cc[wnd][0], cc[wnd][1], cc[wnd][2]);
				g_pDevice->Clear(true, true, true);
			}
			g_pDevice->EndRendering();
		}
	}

	ProgramCleanup();

	return 0;
}

bool ProgramStartup()
{
	const SMGDisplay *hWnd3D[4];

	g_pDevice = g_pRenderer->GetDevice();
	if (!g_pDevice) return false;

	int x1, y1, w, h;
	g_pDisplay->GetWindowRect(x1, y1, w, h);

	int x, y;

	for (int i = 0; i<4; i++)
	{
		if ((i == 0) || (i == 2)) x = 10;
		else x = w / 2 + 10;

		if ((i == 0) || (i == 1)) y = 10;
		else y = h / 2 + 10;

		hWnd3D[i] = g_pRenderer->OpenWindow(x, y, w / 2 - 20, h / 2 - 20, "", true, g_pDisplay);
	}

	if (FAILED(g_pDevice->Init(g_pDisplay, hWnd3D, 4, 16, 0, false))) return false;
	return true;
}

void ProgramCleanup()
{
	if (g_pRenderer)
		delete g_pRenderer;
}

