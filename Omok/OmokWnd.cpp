#include "pch.h"
#include "OmokWnd.h"
#include "Stone.h"
OmokWnd::OmokWnd(
	HINSTANCE hInstance
	, const WCHAR* szWindowClass
	, const WCHAR* szTitle
	, int x
	, int y
	, int width
	, int height
	, LPTSTR lpCmdLine
	, int nCmdShow
)
	: _hInstance(hInstance), _nCmdShow(nCmdShow), _nowTurn(OmokWnd::TURN::BLACK)
{
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = _hInstance;
	wcex.hIcon = LoadIcon(_hInstance, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

	if (!RegisterClassEx(&wcex))
	{
		IS_CRASH("RegisterClassEx");
	}
	
	_hwnd = CreateWindow(
		szWindowClass,
		szTitle,
		WS_OVERLAPPEDWINDOW,
		x, y,
		width, height,
		NULL,
		NULL,
		hInstance,
		this);

	if (!_hwnd)
	{
		IS_CRASH("CreateWindow Error");
	}
	::memset(_boards, 0, sizeof(_boards));
	_brushs.resize(BRUSH_COLOR_COUNT);
	_brushs[BRUSH_WOOD] = CreateSolidBrush(RGB(220, 179, 92));
	_brushs[BRUSH_WHITE] = CreateSolidBrush(RGB(255, 255, 255));
	_brushs[BRUSH_BLACK] = CreateSolidBrush(RGB(0, 0, 0));
}

OmokWnd::~OmokWnd()
{
	for (const auto& stone : _stones) 
	{
		if (stone)
			delete stone;
	}

	for (const auto& brush : _brushs) 
	{
		if (brush)
			DeleteObject(brush);
	}

	_stones.clear();
}

LRESULT OmokWnd::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	OmokWnd* pWnd = nullptr;
	if (message == WM_NCCREATE)
	{
		LPCREATESTRUCT pCS = (LPCREATESTRUCT)lParam;
		SetLastError(0);
		pWnd = reinterpret_cast<OmokWnd*>(pCS->lpCreateParams);
		if (!SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd)))
		{
			if (GetLastError() != 0) return E_FAIL;
		}
	}
	else
	{
		pWnd = reinterpret_cast<OmokWnd*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	}

	if (message == WM_GETMINMAXINFO && pWnd == nullptr)
		return S_OK;

	return pWnd->DisPatch(hWnd, message, wParam, lParam);
}

LRESULT OmokWnd::DisPatch(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	switch (message)
	{
	case WM_CREATE:
		break;

	case WM_KEYUP:
		break;

	case WM_KEYDOWN:
		break;

	case WM_TIMER:
		break;

	case WM_SIZE:
		break;

	case WM_COMMAND:
		break;

	case WM_LBUTTONDOWN:
	{
		int xPos = LOWORD(lParam);
		int yPos = HIWORD(lParam);
		HandleMouseClick(xPos, yPos);
		break;
	}

	case WM_MOUSEMOVE:
		break;

	case WM_LBUTTONUP:
		break;

	case WM_MOUSEWHEEL:
		break;

	case WM_VSCROLL:
		break;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		Render(hdc);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}
}

void OmokWnd::Init()
{
	ShowWindow(_hwnd, _nCmdShow);
	UpdateWindow(_hwnd);
}

int OmokWnd::GetMsg()
{
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}

void OmokWnd::HandleMouseClick(int xPos, int yPos)
{
	int margin = MARGIN;    // 여백
	int cellSize = CELLSIZE;  // 각 셀의 크기
	int boardSize = BOARDSIZE; // 19x19 바둑판

	xPos -= margin;
	yPos -= margin;

	int xIndex = xPos / cellSize;
	int yIndex = yPos / cellSize;
	
	int midX = (xIndex * cellSize) + (cellSize / 2);
	int midY = (yIndex * cellSize) + (cellSize / 2);

	int left = 0;
	int bottom = 1;
	int top = 0;
	int right = 1;
	
	int xDir = 0;
	int yDir = 0;

	if (xPos < midX)
		xDir = left;
	else
		xDir = right;
	
	if (yPos > midY)
		yDir = bottom;
	else
		yDir = top;

	xIndex += xDir;
	yIndex += yDir;

	if (xIndex < 0)
		xIndex = 0;
	if (xIndex >= boardSize - 1)
		xIndex = boardSize - 1;
	if (yIndex < 0)
		yIndex = 0;
	if (yIndex >= boardSize - 1)
		yIndex = boardSize - 1;

	_xPos = xIndex;
	_yPos = yIndex;

	if (_boards[_yPos][_xPos]) 
		return;

	_boards[_yPos][_xPos] = true;

	Vector2Int pos(_xPos, _yPos);
	Stone* newStone = new Stone(static_cast<Stone::Color>(_nowTurn),pos);
	_stones.push_back(newStone);

	_nowTurn = static_cast<OmokWnd::TURN>((static_cast<int>(_nowTurn) + 1) % 2);

	InvalidateRect(_hwnd, NULL, TRUE);
}

void OmokWnd::Render(HDC hdc)
{
	// 바둑판 그리기
	int boardSize = BOARDSIZE; // 19x19 바둑판
	int cellSize = CELLSIZE;  // 각 셀의 크기
	int margin = MARGIN;    // 여백

	RECT bgRect = { margin, margin, margin + ((boardSize - 1) * cellSize), margin + ((boardSize - 1) * cellSize)};
	FillRect(hdc, &bgRect, _brushs[BRUSH_WOOD]);

	for (int i = 0; i < boardSize; i++)
	{
		MoveToEx(hdc, margin, margin + i * cellSize, NULL);
		LineTo(hdc, margin + (boardSize - 1) * cellSize, margin + i * cellSize);
	}

	for (int i = 0; i < boardSize; i++)
	{
		MoveToEx(hdc, margin + i * cellSize, margin, NULL);
		LineTo(hdc, margin + i * cellSize, margin + (boardSize - 1) * cellSize);
	}

	// 화점 그리기 
	SelectObject(hdc, _brushs[BRUSH_BLACK]);
	const int flowerShopMargin = 3;
	Vector2Int flowerShops[9] = 
	{ 
		{0 + flowerShopMargin,0 + flowerShopMargin}, 
		{boardSize - 1 - flowerShopMargin, 0 + flowerShopMargin},
		{0 + flowerShopMargin, boardSize - 1 - flowerShopMargin}, 
		{boardSize - 1 - flowerShopMargin,boardSize - 1 - flowerShopMargin}, 
		{boardSize - 1 - flowerShopMargin,boardSize / 2},
		{boardSize / 2,boardSize - 1 - flowerShopMargin},
		{0 + flowerShopMargin,boardSize / 2},
		{boardSize / 2, 0 + flowerShopMargin},
		{boardSize / 2, boardSize / 2}
	};
	
	for (const auto& flowerShop : flowerShops) 
	{
		DrawCircle(hdc, (flowerShop.x * cellSize) + MARGIN, (flowerShop.y * cellSize) + MARGIN, R/2);
	}

	SelectObject(hdc, _brushs[WHITE_BRUSH]);

	// 돌 그리기
	for (const auto& stone : _stones) 
	{
		Vector2Int pos = stone->GetPos();
		Stone::Color color = stone->GetColor();

		if (color == Stone::Color::BLACK) 
		{
			SelectObject(hdc, _brushs[BRUSH_BLACK]);
		}
		DrawCircle(hdc, (pos.x * cellSize) + MARGIN, (pos.y * cellSize) + MARGIN, R);
		SelectObject(hdc, _brushs[BRUSH_WHITE]);
	}

	WCHAR buffer[100] = L"";
	swprintf_s(buffer, 100, L"x:%d y:%d", _xPos, _yPos);
	TextOutW(hdc, 0, 0, buffer, wcslen(buffer));
}

void OmokWnd::DrawCircle(HDC hdc, int centerX, int centerY, int radius)
{
	Ellipse(hdc, centerX - radius, centerY - radius, centerX + radius, centerY + radius);
}