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
	
	for (int y = 0; y < BOARDSIZE; y++)
	{
		for (int x = 0; x < BOARDSIZE; x++) 
		{
			_boards[y][x] = nullptr;
		}
	}

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

	Vector2Int pos(_xPos, _yPos);
	Stone* newStone = new Stone(static_cast<Stone::Color>(_nowTurn),pos);
	_stones.push_back(newStone);
	_boards[_yPos][_xPos] = newStone;

	OmokResult result = VictoryDecision(newStone);

	if (result == OmokResult::BLACK_WIN)
		MessageBox(
			NULL,
			L"흑돌 승리!",
			L"알림",
			MB_OK | MB_ICONINFORMATION
		);

	if (result == OmokResult::WHILTE_WIN)
		MessageBox(
			NULL,
			L"백돌 승리!",
			L"알림",
			MB_OK | MB_ICONINFORMATION
		);

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
		{0 + flowerShopMargin				, 0 + flowerShopMargin}, 
		{boardSize - 1 - flowerShopMargin	, 0 + flowerShopMargin},
		{0 + flowerShopMargin				,  boardSize - 1 - flowerShopMargin}, 
		{boardSize - 1 - flowerShopMargin	,  boardSize - 1 - flowerShopMargin}, 
		{boardSize - 1 - flowerShopMargin	,  boardSize / 2},
		{boardSize / 2						,  boardSize - 1 - flowerShopMargin},
		{0 + flowerShopMargin			    ,  boardSize / 2},
		{boardSize / 2					    ,  0 + flowerShopMargin},
		{boardSize / 2						,  boardSize / 2}
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

OmokWnd::OmokResult OmokWnd::VictoryDecision(Stone* stone)
{
	OmokWnd::OmokResult result = OmokWnd::OmokResult::IDONTKNOW;
	Vector2Int dirVector[4] =
	{
		{-1, 0},		// 왼쪽
		{-1,-1},		// 왼쪽 대각선
		{0, -1},		// 위쪽
		{1, -1}			// 오른쪽 대각선
	};

	Stone::Color color = stone->GetColor();
	Vector2Int pos = stone->GetPos();
	bool omok = false;
	for (auto& dir : dirVector) 
	{
		std::vector<Vector2Int> tasks;
		tasks.push_back(dir);
		tasks.push_back(dir * -1);
		int cnt = 1;
		for (const auto& task : tasks) 
		{
			Vector2Int nowPos = pos;
			while (true)
			{
				Vector2Int nextPos = { nowPos.x + task.x, nowPos.y + task.y };

				if (nextPos.x < 0 || nextPos.y < 0 || nextPos.x >= BOARDSIZE - 1 || nextPos.y >= BOARDSIZE - 1)
					break;

				if (_boards[nextPos.y][nextPos.x] == nullptr)
					break;

				if (_boards[nextPos.y][nextPos.x]->GetColor() != color)
					break;

				// 여기까지 왔다는건 ... 같은색상의 돌이 한개가 더 있다는 뜻이니깐, 
				cnt++;
				nowPos = nextPos;
			}

			// 5목 이상은 넘김
			if (cnt == 5) 
			{
				omok = true;
				break;
			}
		}

		if (omok) 
			break;
	}

	if (omok)
	{
		result = static_cast<OmokWnd::OmokResult>(color);
	}
	// 역행렬 구하기 ... 
	return result;
}

bool OmokWnd::ThreeThreeDecision()
{
	return false;
}
