// Graphic_maybe_lab1.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "Graphic_maybe_lab1.h"
#include "MyLine.h"


#define PI 3.14159265358979323846

// макрос преобразования X градусов в радианы 
#define gradToRad(x) x * PI/180 

float movingSpeed = 4.0f;
Myline AB, CD;
Myline* choisen_line;


LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void PrintLineBresenham(POINT x0, POINT x1, HDC hdc,HWND hwnd,COLORREF color);
void PrintLineWinApi(POINT x0, POINT x1, HDC hdc, COLORREF color);

//Функция с которой начинается выполнение программ на WinApi
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	//Регистрация класса окна Window, обрабатываемого WindowProc
	const wchar_t CLASS_NAME[] = L"Window";
	WNDCLASS wc = { };
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;
	RegisterClass(&wc);

	// Создание окна класса Window

	HWND hwnd = CreateWindowEx(
		0,																
		CLASS_NAME,														
		L"Lab 1",               
		WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX,          
		CW_USEDEFAULT, CW_USEDEFAULT, 1200, 900,
		NULL,        
		NULL,       
		hInstance,  
		NULL        
	);

	//Проверка на успешность создания окна
	if (hwnd == NULL)
	{
		return 0;
	}

	//Определение состояния показа созданного окна
	ShowWindow(hwnd, nCmdShow);

	//Выбор линии, над которой проводятся преобразования, по умолчанию
	choisen_line = &AB;

	//Вывод на экран сообщения, описывающего управление
	MessageBox(hwnd,
		L"w - движение вверх\na - движение влево\ns - движение вниз\nd - движение впарво\n По умолчанию выбран отрезок AB(красный)\n1 - выбрать AB(красный)\n2 - выбрать CD(синий)\nc - обмен координатами между концом иначало отрезка\nq/e - поворот отрезка против/по часовой стрелке\nr/f - увеличение/уменьшение отрезка",
		L"Управление",
		MB_OK);

	MSG msg = { };
	
	//Цикл который отвечает за получение сообщений от системы и их обработку. Цикл оканчивется, когда приходит сообщение вызываемое функцией PostQuitMessage(0) 
	while (GetMessage(&msg, NULL, 0, 0))
	{		
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

//Функция обработки системных сообщений для окон класса Window
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		unsigned int key;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_KEYDOWN:
		key = wParam;// значение нажатой клавиши (передаётся вместе с сообщением WM_KEYDOWN)
		switch (key)
		{
		case '1'://Выбор отрезка AB для преобразования
			choisen_line = &AB;
			break;
		case '2'://Выбор отрезка CD для преобразования
			choisen_line = &CD;
			break;
		case 'c':
		case 'C'://Меняет начальную точку выбранного отрезка с конечной местами (Все дальнейшие преобразования производятся относительно начальной)
			choisen_line->swap();
			break;
		case 'e':
		case 'E'://Поворот выбраного отрезка на 30 градусов по часовой стрелке
			choisen_line->rotate(gradToRad(30));
			break;
		case 'q':
		case 'Q'://Поворот выбраного отрезка на 30 градусов против часовой стрелке
			choisen_line->rotate(gradToRad(-30));
			break;
			// движение выбраного отрезка по осям X и Y (AXIS_Y - вдоль оси Y, AXIS_X - вдоль оси X, movingSpeed перемещение вдоль выбранной оси(Знак определяет сопронавленость с осью))   
		case 'w':
		case 'W':
			choisen_line->move(AXIS_Y, -movingSpeed);
			break;
		case 's':
		case 'S':
			choisen_line->move(AXIS_Y, movingSpeed);
			
			break;
		case 'a':
		case 'A':
			choisen_line->move(AXIS_X, -movingSpeed);
			break;
		case 'd':
		case 'D':
			choisen_line->move(AXIS_X, movingSpeed);
			break;
			// увеличение выбраного отрезка в 1.1 раз
		case 'r':
		case 'R':
			choisen_line->scale(1.1f);
			break;
			// уменьшение выбраного отрезка в 0.9 раз
		case 'f':
		case 'F':
			choisen_line->scale(0.9f);
			break;
		default:
			break;
		}
		//Запрос системе на перерисовку окна после проведенных изменений
		InvalidateRect(hwnd, NULL, true);
		UpdateWindow(hwnd);
		break;
	case WM_PAINT://Отрисовка окна
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);// получение десриптора графического устройства

		// создание совместимого битмапа для создания буфера(совпадает с клиенсткой областью), 
		//в котором будет предварительная отрисовка окна для избежания мерцаний из-за постепеной отрисовки окна
		HDC bufferDC = CreateCompatibleDC(hdc);
		HBITMAP bufferBM = CreateCompatibleBitmap(hdc, ps.rcPaint.right - ps.rcPaint.left, ps.rcPaint.bottom - ps.rcPaint.top);

		SelectObject(bufferDC, bufferBM);

		//отрисовка:
		// 
		//Заполнение буфера белым цветом
		FillRect(bufferDC, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
		//Отрисовка отрезка AB с помощью встроенных функций (красный отрезок)
		PrintLineWinApi(AB.getStartPoint(), AB.getEndPoint(), bufferDC, RGB(255, 0, 0));
		//Отрисовка отрезка CD с помощью собственной реализации алгоритма Брезенхэма (синий отрезок)
		PrintLineBresenham(CD.getStartPoint(), CD.getEndPoint(), bufferDC, hwnd, RGB(0, 0, 255));
		
		//Загрузка буфера в битмап графического устройства и освобождение памяти выделенной на буфер
		BitBlt(hdc, 0, 0, ps.rcPaint.right - ps.rcPaint.left, ps.rcPaint.bottom - ps.rcPaint.top, bufferDC, 0, 0, SRCCOPY);
		DeleteDC(bufferDC);
		DeleteObject(bufferBM);

		EndPaint(hwnd, &ps);
	}
	return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

//Отрисовка отрезка с начальной точкой x0 и конечной x1 в графическое устройство с дескриптором hdc 
//в цвет color реализованная с помощью алгоритма Брезенхэма
void PrintLineBresenham(POINT x0, POINT x1,HDC hdc,HWND hwnd,COLORREF color)
{

	int deltaX = abs(x0.x - x1.x);
	int deltaY = abs(x0.y - x1.y);

	//signX и signY определяют напрвление движения по соответсвующим осям 
	int signX = x0.x < x1.x ?1 :-1 ;
	int signY = x0.y < x1.y ? 1: -1;

	//Присвоение начального значения ошибки исходя из коофицента наклона отрезка (Определяет как координаты будут меняться) 
	int error = (deltaX > deltaY ? deltaX : -deltaY) / 2;

	RECT clientRect;
	//Получение клиентской области
	GetClientRect(hwnd,&clientRect);

	for (;;)
	{
		//Проверка на преднадлежность координат клиентской области и отрисовка при удовлетворительном ответе
		if (x0.x >= 0 && x0.x < clientRect.right && x0.y >= 0 && x0.y < clientRect.bottom)
			//Помещение пикселя цветом color по координатам x0.x, x0.y
			SetPixelV(hdc,x0.x, x0.y, color);

		//Выход из цикла при достижении конечной точки
		if (x0.x == x1.x && x0.y == x1.y) break;
		
		//если ошибка по оси X превышает пороговое значение, умешьшаем ошибку на deltaY меняем х координату
		if (error > -deltaX) 
		{ 
			error -= deltaY;
			//х координата текщей точки меняется на 1 согласно знаку разности координат
			x0.x += signX; 
		}

		//если ошибка по оси Y превышает пороговое значение, умешьшаем ошибку на deltaX меняем y координату
		if (error < deltaY)
		{ 
			error += deltaX;
			//y координата текщей точки меняется на 1 согласно знаку разности координат
			x0.y += signY; 
		}
	}

}

//Отрисовка отрезка с начальной точкой x0 и конечной x1 в графическое устройство с дескриптором hdc 
//в цвет color реализованная с помощью функций WinApi (GDI)
void PrintLineWinApi(POINT x0, POINT x1, HDC hdc, COLORREF color) 
{
	//создание нового сплошного пера цвета color
	HPEN newPen = CreatePen(PS_SOLID, NULL, color);
	//выбор нового пера с сохранением старого
	HGDIOBJ prevPen = SelectObject(hdc, newPen);

	//устанока кисти в координаты в x0.x, x0.y
	MoveToEx(hdc, x0.x, x0.y, NULL);
	//отрисовка линии от установленных координат кисти до x1.x, x1.y
	LineTo(hdc,x1.x, x1.y);

	//выбор старой кисти
	SelectObject(hdc, prevPen);
	//Освобождение памяти от новой кисте
	DeleteObject(newPen);
}