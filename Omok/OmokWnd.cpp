#include "pch.h"
#include "OmokWnd.h"
#include "Stone.h"
// ������
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
	: _hInstance(hInstance), _nCmdShow(nCmdShow), _nowTurn(OmokWnd::TURN::BLACK), _gameEnd(false)
{
	// ������ Ŭ���� ���
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

	// ��� �ȵ��� ��� ���� ó��
	if (!RegisterClassEx(&wcex))
	{
		// Crash
		IS_CRASH("RegisterClassEx");
	}
	// ������ â ����
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
	// â ������ �������� ��� Crash
	if (!_hwnd)
	{
		IS_CRASH("CreateWindow Error");
	}
	
	// BOARD 2���� �迭 �ʱ�ȭ
	for (int y = 0; y < BOARDSIZE; y++)
	{
		for (int x = 0; x < BOARDSIZE; x++) 
		{
			_boards[y][x] = nullptr;
		}
	}

	// ���� �귯�� ����
	_brushs.resize(BRUSH_COLOR_COUNT);
	_brushs[BRUSH_WOOD] = CreateSolidBrush(RGB(220, 179, 92));
	_brushs[BRUSH_WHITE] = CreateSolidBrush(RGB(255, 255, 255));
	_brushs[BRUSH_BLACK] = CreateSolidBrush(RGB(0, 0, 0));
}
// �Ҹ���
OmokWnd::~OmokWnd()
{
	// �ٵϵ� �޸� �Ҵ� ����
	for (const auto& stone : _stones) 
	{
		if (stone)
			delete stone;
	}

	// �귯�� �Ҵ� ����
	for (const auto& brush : _brushs) 
	{
		if (brush)
			DeleteObject(brush);
	}

	// vector clear
	_stones.clear();
}

// ������ �޽��� �ޱ� ���� ó��
// Window Ŭ���� ����Ҷ� lParam�� �ڱ��ڽ��� �ּҰ��� �Ѱ��־���.
LRESULT OmokWnd::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	OmokWnd* pWnd = nullptr;
	// ���࿡ �� ó�� �޽������?
	if (message == WM_NCCREATE)
	{
		// lParam���� �ּҰ� ������
		LPCREATESTRUCT pCS = (LPCREATESTRUCT)lParam;
		SetLastError(0);
		// ���� ����ȯ
		pWnd = reinterpret_cast<OmokWnd*>(pCS->lpCreateParams);
		// USERDATA ������ ������
		if (!SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd)))
		{
			if (GetLastError() != 0) return E_FAIL;
		}
	}
	else
	{
		// USERDATA �������� ������
		pWnd = reinterpret_cast<OmokWnd*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	}

	if (message == WM_GETMINMAXINFO && pWnd == nullptr)
		return S_OK;

	// OmokWnd Ŭ������ DisPatch �޼ҵ� ����
	return pWnd->DisPatch(hWnd, message, wParam, lParam);
}

// ������ �޽��� ó�� (�̺�Ʈ ó��)
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

		// ���콺 Ŭ�������� (�ٵϵ� ���� ó��)
	case WM_LBUTTONDOWN:
	{
		int xPos = LOWORD(lParam);
		int yPos = HIWORD(lParam);
		HandleMouseClick(xPos, yPos); // x, y ��ǥ �Ѱ���
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
		Render(hdc); // ������
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

// Window Show, Update
void OmokWnd::Init()
{
	ShowWindow(_hwnd, _nCmdShow);
	UpdateWindow(_hwnd);
}

// Window Msg ����
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

// ����!
void OmokWnd::HandleMouseClick(int xPos, int yPos)
{
	// ������ ����Ǿ��ٸ�, �̺�Ʈ�� ���� ����
	if (_gameEnd)
		return;


	int margin = MARGIN;    // ����
	int cellSize = CELLSIZE;  // �� ���� ũ��
	int boardSize = BOARDSIZE; // 19x19 �ٵ���

	// ���⼭����............
	// �ٵϵ� ���� ������ �÷����� ������!!!!!!!!!!!!!!

	// margin�� ���־ ���� ��ǥ ������
	xPos -= margin;
	yPos -= margin;

	// ���� Ŭ���� ������ BOARD_SIZE�� ���° ��� ���� ������ �ľ�
	int xIndex = xPos / cellSize;
	int yIndex = yPos / cellSize;
	
	// �ش� ��� ���� ���ߴٸ� �߽� ��ǥ�� ���Ѵ�!!!
	int midX = (xIndex * cellSize) + (cellSize / 2);
	int midY = (yIndex * cellSize) + (cellSize / 2);

	// ���࿡ �߽���ǥ���� ���� OR ����� �ƹ��͵� �������� �ʴ´�, 
	// ���࿡ �߽���ǥ���� ������ OR �Ʒ���� +1�� �������ش�.
	// �׷��߸�, ���������� �ø��� ����, �׸��� �׷����� ����!
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


	// ������ ���࿡... �̹� �����Ǿ��ִٸ�?
	// return ó��
	if (_boards[_yPos][_xPos])
		return;

	// ������� �Դٴ� ����, ���� ���� �����ؾ� �Ѵ�.
	// ���� ��ǥ ��ü �����
	Vector2Int pos(_xPos, _yPos);
	// �ٵϵ� ����
	Stone* newStone = new Stone(static_cast<Stone::Color>(_nowTurn),pos);
	// �ٵϵ� ���Ϳ� �ִ´�.
	_stones.push_back(newStone);
	_boards[_yPos][_xPos] = newStone;

	// �߿�!! 
	// ���� ��� ����, ���� �̰�°�? 
	OmokResult result = VictoryDecision(newStone);

	int msgboxID = INT_MIN;
	// �浹�� �̰��.
	if (result == OmokResult::BLACK_WIN)
	{
		InvalidateRect(_hwnd, NULL, TRUE);
		msgboxID = MessageBox(
			NULL,
			L"�浹 �¸�!\n������ �ٽ� �Ͻðڽ��ϱ�?",
			L"�˸�",
			MB_ICONQUESTION | MB_YESNO
		);
	}
	// �鵹�� �̰��.
	if (result == OmokResult::WHILTE_WIN)
	{
		InvalidateRect(_hwnd, NULL, TRUE);
		msgboxID = MessageBox(
			NULL,
			L"�鵹 �¸�!\n������ �ٽ� �Ͻðڽ��ϱ�?",
			L"�˸�",
			MB_ICONQUESTION | MB_YESNO
		);
	}
	// ���� ���� ����Ѵ�, �浹�̿�����, �������� �鵹
	_nowTurn = static_cast<OmokWnd::TURN>((static_cast<int>(_nowTurn) + 1) % 2);

	// ���࿡ ������ ����� ���ٸ�...
	if (msgboxID != INT_MIN) 
	{
		switch (msgboxID)
		{
			// ���� �ٽ��ϱ� ������ ���
		case IDYES:
			ReGame();
			break;
		case IDNO:
			// ���� ������ ������ ���
			_gameEnd = true;
			break;
		}
	}

	InvalidateRect(_hwnd, NULL, TRUE);
}

void OmokWnd::Render(HDC hdc)
{
	// �ٵ��� �׸���
	int boardSize = BOARDSIZE; // 19x19 �ٵ���
	int cellSize = CELLSIZE;  // �� ���� ũ��
	int margin = MARGIN;    // ����

	// �ٵ��� ������ �׸��� WOOD ����
	RECT bgRect = { margin, margin, margin + ((boardSize - 1) * cellSize), margin + ((boardSize - 1) * cellSize)};
	FillRect(hdc, &bgRect, _brushs[BRUSH_WOOD]);

	// GRID �׸���
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

	// ȭ�� �׸��� 
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

	// �� �׸���
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

	// ����׿�, ���� Ŭ���� ���� X,Y ��ǥ ���
	WCHAR buffer[100] = L"";
	swprintf_s(buffer, 100, L"x:%d y:%d", _xPos, _yPos);
	TextOutW(hdc, 0, 0, buffer, wcslen(buffer));
}

// �� �׸��� (������ ������ �Ѱ��ָ� �׸�����)
void OmokWnd::DrawCircle(HDC hdc, int centerX, int centerY, int radius)
{
	Ellipse(hdc, centerX - radius, centerY - radius, centerX + radius, centerY + radius);
}

// �¸�����
OmokWnd::OmokResult OmokWnd::VictoryDecision(Stone* stone)
{
	/*
			������ DFS, BFS �˰����� ����Ϸ��� ������,
			�Ź� ���� ������� �ϴ°� ��̰� ������...
			�̹����� ������ ������ ���߾� �������. 
	*/
	// ��ȯ�� ���� ���, ó���� IDONTKNOW (�˼�����)
	OmokWnd::OmokResult result = OmokWnd::OmokResult::IDONTKNOW;
	
	/*
		 ���� 5���� ī��Ʈ �Ұǵ�... ������ �̷��ϴ�

		 �������� 5���� ī��Ʈ �Ұǵ�, ���࿡ ���̻� ���ʿ� ���� ���ų� ���������, �ڷ� ���Ƽ� �ٽ� ī��Ʈ�� ����.
		 XOO��OOX << �̷� �׸��̶�� �غ��� , ���� �鵹�� ��Ȳ�̶�� �����ϰڴ�
		 ���� ������ �鵹, O�� �鵹�̶�� �����Ѵ�, �׸��� ���� �������� ī��Ʈ�� ����
		 XOO�� O 2�� �� 1�� �ؼ� �� 3����, �̰� ������ �ƴϴ�, �̶� ���� �������� ������ ó�� ���������� �ڷ� ���Ƽ� ������ �������� ���� 
		 OOX << O 2�� ���������� 2�� �������� 3�� �� 5���� �ȴ�, �̷������� �Ǵ��Ѵ�... ���� ������ �̻��ϰ� �ؼ�... �𸣸� ��� �ּ���!

	*/
	Vector2Int dirVector[4] =
	{
		{-1, 0},		// ����
		{-1,-1},		// ���� �밢��
		{0, -1},		// ����
		{1, -1}			// ������ �밢��
	};
	// ���� ������ ���� ���� ��ǥ ��������
	Stone::Color color = stone->GetColor();
	Vector2Int pos = stone->GetPos();
	// �������� �Ǵ�
	bool omok = false;
	// ���� ���Ʈ���� �˰��� ���� ... 
	for (auto& dir : dirVector) 
	{
		std::vector<Vector2Int> tasks; // ó���� task
		tasks.push_back(dir);
		tasks.push_back(dir * -1); // �������� �Ի��ϱ� ���Ͽ� -1 �� ���Ѵ�
		int cnt = 1; // �̹� �����ѵ��� 1�� ī��Ʈ ���� ����
		for (const auto& task : tasks) 
		{
			Vector2Int nowPos = pos;
			while (true)
			{
				// ���� ��ǥ ���
				Vector2Int nextPos = { nowPos.x + task.x, nowPos.y + task.y };
				// �����ִ� �����ΰ�?
				if (nextPos.x < 0 || nextPos.y < 0 || nextPos.x >= BOARDSIZE - 1 || nextPos.y >= BOARDSIZE - 1)
					break;
				// ���� ���°�?
				if (_boards[nextPos.y][nextPos.x] == nullptr)
					break;
				// ������ ��ġ�ϴ°�?
				if (_boards[nextPos.y][nextPos.x]->GetColor() != color)
					break;

				// ������� �Դٴ°� ... ���������� ���� �Ѱ��� �� �ִٴ� ���̴ϱ�, 
				cnt++;
				nowPos = nextPos;
			}

			// 5�� �̻��� �ѱ�
			if (cnt == 5) 
			{
				omok = true;
				break;
			}
		}

		if (omok) 
			break;
	}
	// ���࿡ �����̶��
	if (omok)
	{
		// ���� ����
		result = static_cast<OmokWnd::OmokResult>(color);
	}
	return result;
}

// ���� �����
void OmokWnd::ReGame()
{
	// ���� ������ �浹 ����
	_nowTurn = OmokWnd::TURN::BLACK;

	// ��� �� �޸� �Ҵ� ����
	for (const auto& stone : _stones)
	{
		if (stone)
			delete stone;
	}

	_stones.clear();

	// _boards 2���� �迭 �ʱ�ȭ
	for (int y = 0; y < BOARDSIZE; y++)
	{
		for (int x = 0; x < BOARDSIZE; x++)
		{
			_boards[y][x] = nullptr;
		}
	}
}
