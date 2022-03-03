// Graphic_maybe_lab1.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "Graphic_maybe_lab1.h"

#define AXIS_Y 0
#define AXIS_X 1
#define PI 3.14159265358979323846
#define gradToRad(x) x * PI/180


POINT a, b, c, d;
LPPOINT first, last;
int movingSpeed = 4;


LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void PrintLineBresenham(POINT x0, POINT x1, HDC hdc,HWND hwnd,COLORREF color);
void PrintLineWinApi(POINT x0, POINT x1, HDC hdc, COLORREF color);
void moveLines(LPPOINT ptr_x0, LPPOINT ptr_x1, unsigned int direction, int moving);
void scaleLine(LPPOINT ptr_x0, LPPOINT ptr_x1, float scale);
void rotateLine(LPPOINT ptr_x0, LPPOINT ptr_x1, double angle);
void swapPoint(LPPOINT ptr_x0, LPPOINT ptr_x1);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	// Register the window class.
	const wchar_t CLASS_NAME[] = L"Sample Window Class";
	WNDCLASS wc = { };
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;
	RegisterClass(&wc);

	// Create the window.

	HWND hwnd = CreateWindowEx(
		0,																  // Optional window styles.
		CLASS_NAME,														// Window class
		L"что пялишься?",               // Window text
		WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX,           // Window style
		CW_USEDEFAULT, CW_USEDEFAULT, 700, 329,
		NULL,       // Parent window    
		NULL,       // Menu
		hInstance,  // Instance handle
		NULL        // Additional application data
	);

	if (hwnd == NULL)
	{
		return 0;
	}
	ShowWindow(hwnd, nCmdShow);

	a.x = 0;
	a.y = 0;
	b.x = 90;
	b.y = 90;

	c.x = 40;
	c.y = 80;
	d.x = 80;
	d.y = 40;

	first = &a;
	last = &b;

	MSG msg = { };
	
	while (GetMessage(&msg, NULL, 0, 0))
	{		
		
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		
	}

	return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		unsigned int key;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_KEYDOWN:
		key = wParam;
		switch (key)
		{
		case '1':
			first =&a ;
			last = &b;
			break;
		case '2':
			first = &c;
			last = &d;
			break;
		case 'c':
		case 'C':
			swapPoint(first,last);
			break;
		case 'e':
		case 'E':
			rotateLine(first, last, gradToRad(30));
			break;
		case 'q':
		case 'Q':
			rotateLine(first, last, gradToRad(-30));
			break;
			// движение по осям X и Y
		case 'w':
		case 'W':
			moveLines(first,last,AXIS_Y,-movingSpeed);
			break;
		case 's':
		case 'S':
			moveLines(first, last, AXIS_Y, movingSpeed);
			
			break;
		case 'a':
		case 'A':
			moveLines(first, last, AXIS_X, -movingSpeed);
			break;
		case 'd':
		case 'D':
			moveLines(first, last, AXIS_X, movingSpeed);
			break;
			// масштабирование		
			// увеличение
		case 'r':
		case 'R':
			scaleLine(first, last, 1.1f);
			break;
			// уменьшение
		case 'f':
		case 'F':
			scaleLine(first, last, 0.9f);
			break;
		default:
			break;
		}
		InvalidateRect(hwnd, NULL, true);
		UpdateWindow(hwnd);
		break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);

		HDC bufferDC = CreateCompatibleDC(hdc);
		HBITMAP bufferBM = CreateCompatibleBitmap(hdc, ps.rcPaint.right - ps.rcPaint.left, ps.rcPaint.bottom - ps.rcPaint.top);
		SelectObject(bufferDC, bufferBM);
		//////
		FillRect(bufferDC, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
		PrintLineWinApi(a, b, bufferDC, RGB(255, 0, 0));
		PrintLineBresenham(c, d, bufferDC, hwnd, RGB(0, 0, 255));
		//////
		BitBlt(hdc, 0, 0, ps.rcPaint.right - ps.rcPaint.left, ps.rcPaint.bottom - ps.rcPaint.top, bufferDC, 0, 0, SRCCOPY);
		DeleteDC(bufferDC);
		DeleteObject(bufferBM);

		EndPaint(hwnd, &ps);
	}
	return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
void PrintLineBresenham(POINT x0, POINT x1,HDC hdc,HWND hwnd,COLORREF color)
{
	int deltaX = abs(x0.x - x1.x);
	int deltaY = abs(x0.y - x1.y);
	int signX = x0.x < x1.x ?1 :-1 ;
	int signY = x0.y < x1.y ? 1: -1;
	int error = (deltaX > deltaY ? deltaX : -deltaY) / 2;
	int e2 = error;
	RECT clientRect;
	GetClientRect(hwnd,&clientRect);
	for (;;)
	{
		if (x0.x >= 0 && x0.x < clientRect.right && x0.y >= 0 && x0.y < clientRect.bottom)
			SetPixelV(hdc,x0.x, x0.y, color);
		if (x0.x == x1.x && x0.y == x1.y) break;
		e2 = error;
		if (e2 > -deltaX) { error -= deltaY; x0.x += signX; }
		if (e2 < deltaY) { error += deltaX; x0.y += signY; }
	}

}
void PrintLineWinApi(POINT x0, POINT x1, HDC hdc, COLORREF color) 
{
	HPEN newPen = CreatePen(PS_SOLID, NULL, color);
	HGDIOBJ prevPen = SelectObject(hdc, newPen);

	MoveToEx(hdc, x0.x, x0.y, NULL);
	LineTo(hdc,x1.x, x1.y);

	SelectObject(hdc, prevPen);
	DeleteObject(newPen);
}
void moveLines(LPPOINT ptr_x0, LPPOINT ptr_x1, unsigned int direction, int moving)
{
	switch (direction)
	{
	case AXIS_X:
		ptr_x0->x += moving;
		ptr_x1->x += moving;
		break;
	case AXIS_Y:
		ptr_x0->y += moving;
		ptr_x1->y += moving;
		break;
	default:
		break;
	}
}
void scaleLine(LPPOINT ptr_x0, LPPOINT ptr_x1,float scale)
{
	if (scale < 1) {
		if ((abs(ptr_x1->x - ptr_x0->x)  - ptr_x0->x >= 1) || (abs(ptr_x1->y - ptr_x0->y)  - ptr_x0->y >= 1))
		{
			ptr_x1->x = (ptr_x1->x - ptr_x0->x) * scale + ptr_x0->x;
			ptr_x1->y = (ptr_x1->y - ptr_x0->y) * scale + ptr_x0->y;
		}
	}
	else
	{
		ptr_x1->x = (ptr_x1->x - ptr_x0->x) * scale + ptr_x0->x;
		ptr_x1->y = (ptr_x1->y - ptr_x0->y) * scale + ptr_x0->y;
	}
}
void rotateLine(LPPOINT ptr_x0, LPPOINT ptr_x1, double angle)
{
	int tmp = ptr_x1->x;
	ptr_x1->x = (ptr_x1->x - ptr_x0->x) * cos(angle) - (ptr_x1->y - ptr_x0->y) * sin(angle) + ptr_x0->x;
	ptr_x1->y = (tmp - ptr_x0->x) * sin(angle) + (ptr_x1->y - ptr_x0->y) * cos(angle) + ptr_x0->y;
}
void swapPoint(LPPOINT ptr_x0, LPPOINT ptr_x1)
{
	POINT tmp = *ptr_x0;
	*ptr_x0 = *ptr_x1;
	*ptr_x1 = tmp;
}