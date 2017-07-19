// Gobang.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "Gobang.h"

#define MAX_LOADSTRING 100
#define WND_WIGHT	   800
#define WND_HIGHT      680
#define BOARD_WIGHT    10
#define BOARD_HIGHT    10

// ȫ�ֱ���: 
HINSTANCE hInst;                                // ��ǰʵ��
WCHAR     szTitle[MAX_LOADSTRING];                  // �������ı�
WCHAR     szWindowClass[MAX_LOADSTRING];            // ����������
HDC       hdc, mdc, bufdc;
HBITMAP   board, black, white, fullBoard, bg;
BOOL      turn, over;
BOOL	  b[BOARD_WIGHT][BOARD_HIGHT][192] = { 0 }, w[BOARD_WIGHT][BOARD_HIGHT][192] = { 0 };	//��ʤ��ϱ�
INT       map[BOARD_WIGHT][BOARD_HIGHT] = { 0 }, win[2][192] = { 0 };	//��ͼ����ʤλ���������� 5 ʱ�� ��ʤ��
DWORD     tb;	//�������ʱ��

// �˴���ģ���а����ĺ�����ǰ������: 
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
VOID				Print(HDC);
VOID				InitGame();
VOID				Computer();

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: �ڴ˷��ô��롣

    // ��ʼ��ȫ���ַ���
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_GOBANG, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // ִ��Ӧ�ó����ʼ��: 
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GOBANG));

    MSG msg;

    // ����Ϣѭ��: 
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  ����: MyRegisterClass()
//
//  Ŀ��: ע�ᴰ���ࡣ
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GOBANG));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_GOBANG);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   ����: InitInstance(HINSTANCE, int)
//
//   Ŀ��: ����ʵ�����������������
//
//   ע��: 
//
//        �ڴ˺����У�������ȫ�ֱ����б���ʵ�������
//        ��������ʾ�����򴰿ڡ�
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // ��ʵ������洢��ȫ�ֱ�����

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   MoveWindow(hWnd, 200, 100, WND_WIGHT, WND_HIGHT, true);
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   hdc = GetDC(hWnd);
   mdc = CreateCompatibleDC(hdc);
   bufdc = CreateCompatibleDC(hdc);
   fullBoard = CreateCompatibleBitmap(hdc, WND_WIGHT, WND_HIGHT);

   board = (HBITMAP)LoadImage(NULL, _T("bmp\\board.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
   black = (HBITMAP)LoadImage(NULL, _T("bmp\\black.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
   white = (HBITMAP)LoadImage(NULL, _T("bmp\\white.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
   bg    = (HBITMAP)LoadImage(NULL, _T("bmp\\bg.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

   InitGame();

   SelectObject(mdc, fullBoard);
   Print(hdc);
//   ReleaseDC(hWnd, hdc);

   return TRUE;
}

//
//  ����: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  Ŀ��:    ���������ڵ���Ϣ��
//
//  WM_COMMAND  - ����Ӧ�ó���˵�
//  WM_PAINT    - ����������
//  WM_DESTROY  - �����˳���Ϣ������
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // �����˵�ѡ��: 
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
			Print(hdc);
			DWORD t = GetTickCount();
			if (t - tb > 100)
			{
				if (!over && !turn)
					Computer();
			}
        }
        break;
	case WM_KEYDOWN:
		{
			switch (wParam)
			{
			case VK_F1:
				InitGame();
				break;
			case VK_ESCAPE:
				PostQuitMessage(0);
				break;
			default:
				break;
			}
		}
		break;
	case WM_LBUTTONDOWN:
		{
			//��һغ�
			if (!over && turn)
			{
				int x = LOWORD(lParam);
				int y = HIWORD(lParam);
				if ((x > 10 && x < 610) && (y > 10 && y < 610))
				{
					int m = (x - 10) / 60;
					int n = (y - 10) / 60;
					if (!map[m][n])
					{
						map[m][n] = 1;
						tb = GetTickCount();
						for (int i = 0; i < 192; ++i)
						{
							if (b[m][n][i])
							{
								++win[0][i];
								win[1][i] = 7;
							}
							if (win[0][i] == 5)
								over = 1;
						}
						turn = FALSE;
					}
				}
			}
		}
		break;
    case WM_DESTROY:
        PostQuitMessage(0);
		DeleteDC(mdc);
		DeleteDC(bufdc);
		DeleteObject(fullBoard);
		DeleteObject(board);
		DeleteObject(white);
		DeleteObject(black);
		ReleaseDC(hWnd, hdc);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// �����ڡ������Ϣ�������
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

VOID Print(HDC hdc)
{
	//��ͼ����
	SelectObject(bufdc, bg);
	BitBlt(mdc, 0, 0, WND_WIGHT, WND_HIGHT, bufdc, 0, 0, SRCCOPY);

	//��ͼ���̡�����
	for (int i = 0; i < BOARD_WIGHT; ++i)
	{
		for (int j = 0; j < BOARD_HIGHT; ++j)
		{
			SelectObject(bufdc, board);
			BitBlt(mdc, i * 60 + 10, j * 60 + 10, 60, 60, bufdc, 0, 0, SRCCOPY);
			if (map[i][j] == 1)
			{
				SelectObject(bufdc, black);
				BitBlt(mdc, i * 60 + 15, j * 60 + 15, 50, 50, bufdc, 0, 50, SRCAND);
				BitBlt(mdc, i * 60 + 15, j * 60 + 15, 50, 50, bufdc, 0, 0, SRCPAINT);
			}
			if (map[i][j] == 2)
			{
				SelectObject(bufdc, white);
				BitBlt(mdc, i * 60 + 15, j * 60 + 15, 50, 50, bufdc, 0, 50, SRCAND);
				BitBlt(mdc, i * 60 + 15, j * 60 + 15, 50, 50, bufdc, 0, 0, SRCPAINT);
			}
			
		}
	}

	//���ڴ�DC���Ƶ�Ӳ��DC
	BitBlt(hdc, 0, 0, WND_WIGHT, WND_HIGHT, mdc, 0, 0, SRCCOPY);

	//���������չʾʤ����
	//���ߣ���ʾ˭�Ļغ�
	TCHAR *str;
	if (over)
		str = (over == 2) ? _T("����ʤ����") : _T("���ʤ����");	
	else if (turn)
		str = _T("��һغ�");
	else
		str = _T("���Իغ�");
	SetBkMode(hdc, TRANSPARENT);
	TextOut(hdc, 650, 520, str, _tcsclen(str));

}

VOID InitGame()
{
	//���ѡ�񿪾�ִ�巽
	DWORD t = GetTickCount();
	srand(t);
	turn = rand() % 2;

	
	//��ͼ��ʼ��
	for (int i = 0; i < BOARD_WIGHT; ++i)
	{
		for (int j = 0; j < BOARD_HIGHT; ++j)
		{
			map[i][j] = 0;
		}
	}

	//�����־
	over = 0;
	for (int i = 0; i < 192; ++i)
	{
		win[0][i] = 0;
		win[1][i] = 0;
	}

	//�Ի�ʤ��ϱ��ʼ��
	//���ĸ�����

	int count = 0;

	//����
	for (int i = 0; i <= BOARD_WIGHT - 5; ++i)
	{
		for (int j = 0; j <= BOARD_HIGHT - 1; ++j)
		{
			for (int k = 0; k < 5; ++k)
			{
				b[i + k][j][count] = TRUE;
				w[i + k][j][count] = TRUE;
			}
			++count;
		}
	}

	//����
	for (int i = 0; i <= BOARD_WIGHT - 1; ++i)
	{
		for (int j = 0; j <= BOARD_HIGHT - 5; ++j)
		{
			for (int k = 0; k < 5; ++k)
			{
				b[i][j + k][count] = TRUE;
				w[i][j + k][count] = TRUE;
			}
			++count;
		}
	}

	//б��
	for (int i = 0; i <= BOARD_WIGHT - 5; ++i)
	{
		for (int j = 0; j <= BOARD_HIGHT - 5; ++j)
		{
			for (int k = 0; k < 5; ++k)
			{
				b[i + k][j + k][count] = TRUE;
				w[i + k][j + k][count] = TRUE;
			}
			++count;
		}
	}

	//��б��
	for (int i = BOARD_WIGHT - 1; i >= 4; --i)
	{
		for (int j = 0; j <= BOARD_HIGHT - 5; ++j)
		{
			for (int k = 0; k < 5; ++k)
			{
				b[i - k][j + k][count] = TRUE;
				w[i - k][j + k][count] = TRUE;
			}
			++count;
		}
	}
}

VOID Computer()
{
	int score[2][BOARD_WIGHT][BOARD_HIGHT];
	//��������ѡ��
	int max = 0, m = 0, n = 0;
	for (int i = 0; i < BOARD_WIGHT; ++i)
	{
		for (int j = 0; j < BOARD_HIGHT; ++j)
		{
			score[0][i][j] = 0;
			if (!map[i][j])
			{
				for (int k = 0; k < 192; ++k)
				{
					if (b[i][j][k] && win[0][k] < 5)
					{
						switch (win[0][k])
						{
						case 0:
							score[0][i][j] += 1;
							break;
						case 1:
							score[0][i][j] += 200;
							break;
						case 2:
							score[0][i][j] += 400;
							break;
						case 3:
							score[0][i][j] += 2000;
							break;
						case 4:
							score[0][i][j] += 10000;
							break;
						default:
							break;
						}
					}
				}
			}

			score[1][i][j] = 0;
			if (!map[i][j])
			{
				for (int k = 0; k < 192; ++k)
				{
					if (w[i][j][k] && win[1][k] < 5)
					{
						switch (win[1][k])
						{
						case 0:
							score[1][i][j] += 1;
							break;
						case 1:
							score[1][i][j] += 230;
							break;
						case 2:
							score[1][i][j] += 430;
							break;
						case 3:
							score[1][i][j] += 2030;
							break;
						case 4:
							score[1][i][j] += 10030;
							break;
						default:
							break;
						}
					}
				}
			}

			if (max == 0)
			{
				m = i;
				n = j;
			}
			if (max < score[0][i][j])
			{
				max = score[0][i][j];
				m = i;
				n = j;
			}
			else if (max == score[0][i][j])
			{
				if (score[1][i][j] > score[1][m][n])
				{
					m = i;
					n = j;
				}
			}
			if (max < score[1][i][j])
			{
				max = score[1][i][j];
				m = i;
				n = j;
			}
			else if (max == score[1][i][j])
			{
				if (score[0][i][j] > score[0][m][n])
				{
					m = i;
					n = j;
				}
			}
		}
	}
	//ѡ������ѡ��
	map[m][n] = 2;
	for (int i = 0; i < 192; ++i)
	{
		if (w[m][n][i])
		{
			++win[1][i];
			win[0][i] = 7;
		}
		if (win[1][i] == 5)
			over = 2;
	}
	turn = TRUE;
}