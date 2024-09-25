#include "pch.h"
#include "OmokWnd.h"

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstace, LPTSTR lpCmdLine, int nCmdShow)
{
	int width = 800;
	int height = 800;
	OmokWnd app(hInstance,L"Omok", L"¿À¸ñ", CW_USEDEFAULT, CW_USEDEFAULT, width, height,lpCmdLine, nCmdShow);
	app.Init();
	return app.GetMsg();
}