#pragma once
class Stone;
class OmokWnd
{
	enum TURN // 현재 턴 누구 차례인지? 
	{
		BLACK = 0,
		WHITE = 1
	};

	enum OmokResult // 오목 경기 결과
	{
		BLACK_WIN = 0,
		WHILTE_WIN = 1,
		IDONTKNOW = 2
	};

	enum Color // 사용할 색상 정의
	{
		BRUSH_WOOD,
		BRUSH_BLACK,
		BRUSH_WHITE,
		BRUSH_COLOR_COUNT
	};

private:
	HINSTANCE				_hInstance;						// 현재 실행되고 있는 프로그램 인스턴스 (Windows API)
	HWND					_hwnd;							// 현재 생성된 창 HADNLE (Windows API)
	int						_x;								// 창이 생성될 X 위치
	int						_y;								// 창이 생성될 Y 위치
	int						_nCmdShow;						// WinMain에서 넘겨주는 값
	int						_xPos = 0;						// 마우스 클릭 좌표 X 값
	int						_yPos = 0;						// 마우스 클릭 좌표 Y 값
	std::vector<Stone*>		_stones;						// 착수된 바둑돌
	Stone*					_boards[BOARDSIZE][BOARDSIZE];	// 바둑돌을 관리하는 2차원 배열
	OmokWnd::TURN			_nowTurn;						// 현재 누구의 Trun인지 관리하는 변수
	std::vector<HBRUSH>		_brushs;						// 색상 Vector
	bool					_gameEnd;						// 게임이 종료되었는지 판단하는 Flag
public:
	OmokWnd(
		HINSTANCE hInstance
		, const WCHAR* szWindowClass
		, const WCHAR* szTitle
		, int x
		, int y
		, int width
		, int height
		, LPTSTR lpCmdLine
		, int nCmdShow);
	~OmokWnd();

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam); // WndProc
	LRESULT DisPatch(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);	// Windwo 메시지 처리
	void Init();	// 초기화 
	int  GetMsg();	// Windows 메세지 수신
	void HandleMouseClick(int xPos, int yPos);	// 마우스 클릭시 Event Bind
	void Render(HDC hdc);	// 렌더링
	void DrawCircle(HDC hdc, int centerX, int centerY, int radius);	// 원 그리기
	OmokWnd::OmokResult VictoryDecision(Stone* stone);	// 승리판정

private:
	void ReGame(); // 게임 다시하기
};

