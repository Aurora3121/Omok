#pragma once
class Stone;
class OmokWnd
{
	enum TURN // ���� �� ���� ��������? 
	{
		BLACK = 0,
		WHITE = 1
	};

	enum OmokResult // ���� ��� ���
	{
		BLACK_WIN = 0,
		WHILTE_WIN = 1,
		IDONTKNOW = 2
	};

	enum Color // ����� ���� ����
	{
		BRUSH_WOOD,
		BRUSH_BLACK,
		BRUSH_WHITE,
		BRUSH_COLOR_COUNT
	};

private:
	HINSTANCE				_hInstance;						// ���� ����ǰ� �ִ� ���α׷� �ν��Ͻ� (Windows API)
	HWND					_hwnd;							// ���� ������ â HADNLE (Windows API)
	int						_x;								// â�� ������ X ��ġ
	int						_y;								// â�� ������ Y ��ġ
	int						_nCmdShow;						// WinMain���� �Ѱ��ִ� ��
	int						_xPos = 0;						// ���콺 Ŭ�� ��ǥ X ��
	int						_yPos = 0;						// ���콺 Ŭ�� ��ǥ Y ��
	std::vector<Stone*>		_stones;						// ������ �ٵϵ�
	Stone*					_boards[BOARDSIZE][BOARDSIZE];	// �ٵϵ��� �����ϴ� 2���� �迭
	OmokWnd::TURN			_nowTurn;						// ���� ������ Trun���� �����ϴ� ����
	std::vector<HBRUSH>		_brushs;						// ���� Vector
	bool					_gameEnd;						// ������ ����Ǿ����� �Ǵ��ϴ� Flag
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
	LRESULT DisPatch(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);	// Windwo �޽��� ó��
	void Init();	// �ʱ�ȭ 
	int  GetMsg();	// Windows �޼��� ����
	void HandleMouseClick(int xPos, int yPos);	// ���콺 Ŭ���� Event Bind
	void Render(HDC hdc);	// ������
	void DrawCircle(HDC hdc, int centerX, int centerY, int radius);	// �� �׸���
	OmokWnd::OmokResult VictoryDecision(Stone* stone);	// �¸�����

private:
	void ReGame(); // ���� �ٽ��ϱ�
};

