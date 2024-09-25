#pragma once
class Stone;
class OmokWnd
{
	enum TURN 
	{
		BLACK = 0,
		WHITE = 1
	};

	enum Color 
	{
		BRUSH_WOOD,
		BRUSH_BLACK,
		BRUSH_WHITE,
		BRUSH_COLOR_COUNT
	};

private:
	HINSTANCE				_hInstance;
	HWND					_hwnd;
	int						_x;
	int						_y;
	int						_nCmdShow;
	int						_xPos = 0;
	int						_yPos = 0;
	std::vector<Stone*>		_stones;
	bool					_boards[BOARDSIZE][BOARDSIZE];
	OmokWnd::TURN			_nowTurn;
	std::vector<HBRUSH>		_brushs;

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

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT DisPatch(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	void Init();
	int  GetMsg();
	void HandleMouseClick(int xPos, int yPos);
	void Render(HDC hdc);
	void DrawCircle(HDC hdc, int centerX, int centerY, int radius);
};

