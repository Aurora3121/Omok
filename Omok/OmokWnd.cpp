#include "pch.h"
#include "OmokWnd.h"
#include "Stone.h"
// 생성자
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
	// 윈도우 클래스 등록
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

	// 등록 안됐을 경우 예외 처리
	if (!RegisterClassEx(&wcex))
	{
		// Crash
		IS_CRASH("RegisterClassEx");
	}
	// 윈도우 창 생성
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
	// 창 생성에 실패했을 경우 Crash
	if (!_hwnd)
	{
		IS_CRASH("CreateWindow Error");
	}
	
	// BOARD 2차원 배열 초기화
	for (int y = 0; y < BOARDSIZE; y++)
	{
		for (int x = 0; x < BOARDSIZE; x++) 
		{
			_boards[y][x] = nullptr;
		}
	}

	// 색상 브러쉬 생성
	_brushs.resize(BRUSH_COLOR_COUNT);
	_brushs[BRUSH_WOOD] = CreateSolidBrush(RGB(220, 179, 92));
	_brushs[BRUSH_WHITE] = CreateSolidBrush(RGB(255, 255, 255));
	_brushs[BRUSH_BLACK] = CreateSolidBrush(RGB(0, 0, 0));
}
// 소멸자
OmokWnd::~OmokWnd()
{
	// 바둑돌 메모리 할당 해제
	for (const auto& stone : _stones) 
	{
		if (stone)
			delete stone;
	}

	// 브러시 할당 해제
	for (const auto& brush : _brushs) 
	{
		if (brush)
			DeleteObject(brush);
	}

	// vector clear
	_stones.clear();
}

// 윈도우 메시지 받기 위한 처리
// Window 클래스 등록할때 lParam에 자기자신의 주소값을 넘겨주었다.
LRESULT OmokWnd::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	OmokWnd* pWnd = nullptr;
	// 만약에 맨 처음 메시지라면?
	if (message == WM_NCCREATE)
	{
		// lParam에서 주소값 가져옴
		LPCREATESTRUCT pCS = (LPCREATESTRUCT)lParam;
		SetLastError(0);
		// 강제 형변환
		pWnd = reinterpret_cast<OmokWnd*>(pCS->lpCreateParams);
		// USERDATA 영역에 저장함
		if (!SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd)))
		{
			if (GetLastError() != 0) return E_FAIL;
		}
	}
	else
	{
		// USERDATA 영역에서 꺼내옴
		pWnd = reinterpret_cast<OmokWnd*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	}

	if (message == WM_GETMINMAXINFO && pWnd == nullptr)
		return S_OK;

	// OmokWnd 클래스의 DisPatch 메소드 실행
	return pWnd->DisPatch(hWnd, message, wParam, lParam);
}

// 윈도우 메시지 처리 (이벤트 처리)
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

		// 마우스 클릭했을때 (바둑돌 착수 처리)
	case WM_LBUTTONDOWN:
	{
		int xPos = LOWORD(lParam);
		int yPos = HIWORD(lParam);
		HandleMouseClick(xPos, yPos); // x, y 좌표 넘겨줌
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
		Render(hdc); // 렌더링
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

// Window Msg 수신
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

// 착수!
void OmokWnd::HandleMouseClick(int xPos, int yPos)
{
	// 게임이 종료되었다면, 이벤트를 받지 않음
	if (_gameEnd)
		return;


	int margin = MARGIN;    // 여백
	int cellSize = CELLSIZE;  // 각 셀의 크기
	int boardSize = BOARDSIZE; // 19x19 바둑판

	// 여기서부터............
	// 바둑돌 격자 지점에 올려놓는 로직임!!!!!!!!!!!!!!

	// margin을 빼주어서 실제 좌표 투영함
	xPos -= margin;
	yPos -= margin;

	// 현재 클릭한 지점이 BOARD_SIZE의 몇번째 행과 열에 속한지 파악
	int xIndex = xPos / cellSize;
	int yIndex = yPos / cellSize;
	
	// 해당 행과 열을 구했다면 중심 좌표를 구한다!!!
	int midX = (xIndex * cellSize) + (cellSize / 2);
	int midY = (yIndex * cellSize) + (cellSize / 2);

	// 만약에 중심좌표보다 왼쪽 OR 위라면 아무것도 가산하지 않는다, 
	// 만약에 중심좌표보다 오른쪽 OR 아래라면 +1씩 조정해준다.
	// 그래야만, 격자지점에 올릴수 있음, 그림을 그려보면 쉽다!
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


	// 하지만 만약에... 이미 착수되어있다면?
	// return 처리
	if (_boards[_yPos][_xPos])
		return;

	// 여기까지 왔다는 것은, 이제 정말 착수해야 한다.
	// 현재 좌표 객체 만들고
	Vector2Int pos(_xPos, _yPos);
	// 바둑돌 생성
	Stone* newStone = new Stone(static_cast<Stone::Color>(_nowTurn),pos);
	// 바둑돌 벡터에 넣는다.
	_stones.push_back(newStone);
	_boards[_yPos][_xPos] = newStone;

	// 중요!! 
	// 게임 결과 판정, 누가 이겼는가? 
	OmokResult result = VictoryDecision(newStone);

	int msgboxID = INT_MIN;
	// 흑돌이 이겼다.
	if (result == OmokResult::BLACK_WIN)
	{
		InvalidateRect(_hwnd, NULL, TRUE);
		msgboxID = MessageBox(
			NULL,
			L"흑돌 승리!\n게임을 다시 하시겠습니까?",
			L"알림",
			MB_ICONQUESTION | MB_YESNO
		);
	}
	// 백돌이 이겼다.
	if (result == OmokResult::WHILTE_WIN)
	{
		InvalidateRect(_hwnd, NULL, TRUE);
		msgboxID = MessageBox(
			NULL,
			L"백돌 승리!\n게임을 다시 하시겠습니까?",
			L"알림",
			MB_ICONQUESTION | MB_YESNO
		);
	}
	// 다음 턴을 계산한다, 흑돌이였으면, 다음턴은 백돌
	_nowTurn = static_cast<OmokWnd::TURN>((static_cast<int>(_nowTurn) + 1) % 2);

	// 만약에 게임이 결과가 났다면...
	if (msgboxID != INT_MIN) 
	{
		switch (msgboxID)
		{
			// 게임 다시하기 눌렀을 경우
		case IDYES:
			ReGame();
			break;
		case IDNO:
			// 게임 끝내기 눌렀을 경우
			_gameEnd = true;
			break;
		}
	}

	InvalidateRect(_hwnd, NULL, TRUE);
}

void OmokWnd::Render(HDC hdc)
{
	// 바둑판 그리기
	int boardSize = BOARDSIZE; // 19x19 바둑판
	int cellSize = CELLSIZE;  // 각 셀의 크기
	int margin = MARGIN;    // 여백

	// 바둑판 배경색을 그린다 WOOD 색상
	RECT bgRect = { margin, margin, margin + ((boardSize - 1) * cellSize), margin + ((boardSize - 1) * cellSize)};
	FillRect(hdc, &bgRect, _brushs[BRUSH_WOOD]);

	// GRID 그리기
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

	// 디버그용, 현재 클릭한 지점 X,Y 좌표 출력
	WCHAR buffer[100] = L"";
	swprintf_s(buffer, 100, L"x:%d y:%d", _xPos, _yPos);
	TextOutW(hdc, 0, 0, buffer, wcslen(buffer));
}

// 원 그리기 (중점과 반지름 넘겨주면 그리도록)
void OmokWnd::DrawCircle(HDC hdc, int centerX, int centerY, int radius)
{
	Ellipse(hdc, centerX - radius, centerY - radius, centerX + radius, centerY + radius);
}

// 승리판정
OmokWnd::OmokResult OmokWnd::VictoryDecision(Stone* stone)
{
	/*
			원래는 DFS, BFS 알고리즘을 사용하려고 했지만,
			매번 같은 방식으로 하는건 재미가 없으니...
			이번에는 구현에 초점을 맞추어 만들었다. 
	*/
	// 반환할 게임 결과, 처음은 IDONTKNOW (알수없음)
	OmokWnd::OmokResult result = OmokWnd::OmokResult::IDONTKNOW;
	
	/*
		 오목 5개를 카운트 할건데... 로직은 이러하다

		 왼쪽으로 5개를 카운트 할건데, 만약에 더이상 왼쪽에 돌이 없거나 막혔을경우, 뒤로 돌아서 다시 카운트를 센다.
		 XOOㅁOOX << 이런 그림이라고 해보자 , 현재 백돌인 상황이라고 가정하겠다
		 ㅁ은 시작점 백돌, O는 백돌이라고 가정한다, 그리고 왼쪽 방향으로 카운트를 세면
		 XOOㅁ O 2개 ㅁ 1개 해서 총 3개다, 이건 오목이 아니다, 이때 왼쪽 방향으로 가던걸 처음 시작점에서 뒤로 돌아서 오른쪽 방향으로 간다 
		 OOX << O 2개 오른쪽으로 2개 왼쪽으로 3개 총 5개가 된다, 이런식으로 판단한다... 제가 설명을 이상하게 해서... 모르면 댓글 주세요!

	*/
	Vector2Int dirVector[4] =
	{
		{-1, 0},		// 왼쪽
		{-1,-1},		// 왼쪽 대각선
		{0, -1},		// 위쪽
		{1, -1}			// 오른쪽 대각선
	};
	// 현재 착수한 돌의 색상 좌표 가져오기
	Stone::Color color = stone->GetColor();
	Vector2Int pos = stone->GetPos();
	// 오목인지 판단
	bool omok = false;
	// 방향 브루트포스 알고리즘 ㄱㄱ ... 
	for (auto& dir : dirVector) 
	{
		std::vector<Vector2Int> tasks; // 처리할 task
		tasks.push_back(dir);
		tasks.push_back(dir * -1); // 역방향을 게산하기 위하여 -1 을 구한다
		int cnt = 1; // 이미 착수한돌은 1개 카운트 세고 시작
		for (const auto& task : tasks) 
		{
			Vector2Int nowPos = pos;
			while (true)
			{
				// 다음 좌표 계산
				Vector2Int nextPos = { nowPos.x + task.x, nowPos.y + task.y };
				// 갈수있는 지점인가?
				if (nextPos.x < 0 || nextPos.y < 0 || nextPos.x >= BOARDSIZE - 1 || nextPos.y >= BOARDSIZE - 1)
					break;
				// 돌이 없는가?
				if (_boards[nextPos.y][nextPos.x] == nullptr)
					break;
				// 색상이 일치하는가?
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
	// 만약에 오목이라면
	if (omok)
	{
		// 승자 설정
		result = static_cast<OmokWnd::OmokResult>(color);
	}
	return result;
}

// 게임 재시작
void OmokWnd::ReGame()
{
	// 턴은 무조건 흑돌 먼저
	_nowTurn = OmokWnd::TURN::BLACK;

	// 모든 돌 메모리 할당 해제
	for (const auto& stone : _stones)
	{
		if (stone)
			delete stone;
	}

	_stones.clear();

	// _boards 2차원 배열 초기화
	for (int y = 0; y < BOARDSIZE; y++)
	{
		for (int x = 0; x < BOARDSIZE; x++)
		{
			_boards[y][x] = nullptr;
		}
	}
}
