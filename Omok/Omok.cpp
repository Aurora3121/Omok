#include "pch.h"
#include "OmokWnd.h"

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstace, LPTSTR lpCmdLine, int nCmdShow)
{
	int width = 800;	// width
	int height = 800;	// height
	OmokWnd app(hInstance,L"Omok", L"¿À¸ñ", CW_USEDEFAULT, CW_USEDEFAULT, width, height,lpCmdLine, nCmdShow);
	app.Init();
	return app.GetMsg();
}