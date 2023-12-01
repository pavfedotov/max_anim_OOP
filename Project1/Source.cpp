/********************************************************************/
/*                            lek_graph1.cpp                        */
/*                            ============                          */
/*    Лекции  по  машинной  графике.                                */
/*    Масштабироваие в  экранную  ситему  координат                 */
/********************************************************************/
#define _CRT_SECURE_NO_WARNINGS
#define STRICT

#include <stdio.h>
#include <malloc.h>


#include <windows.h>
#include <windowsx.h>

 // файл прототипов и определений общих элементов управления (common controls)
#include <commctrl.h>

 // файл макроопределений идентификаторов ресурсов и элементов управления
#include "Header.h"
#define  PI       3.14159265


#define ALL_STEP       200        // число  временных  интервалов
#define X_SPACE_ANIM   100.0      // Размер области вывода по X
#define Y_SPACE_ANIM    70.0      // Размер области вывода по Y
#define DX X_SPACE_ANIM/ALL_STEP  // шаг анимации по X
#define DY Y_SPACE_ANIM/ALL_STEP  // шаг анимации по Y

#define  K_SCALE  0.1           // коэффициент пропорциональности

#define  K_SC     X_SPACE_ANIM * K_SCALE  // масштабный коэфф.


#define  K_GAB    0.1           // Габаритный  коэффициент отступа
								//  выводимого изображение от края границы вывода

 // свойства приложения
HINSTANCE hiApp;                           // адрес модуля
char aczAppName[] = "Диалоговые элементы"; // имя
HICON hiconAppSmall, hiconAppBig;          // дескрипторы пиктограмм

 // --- главное окно ---
HWND hwndMain;                                                    // дескриптор
LPCSTR pczMainWndClassName = "MainWndClass";                      // имя класса
LPCSTR pczMainWndTitle = "Пример Windows-приложения с графикой";  // текст заголовка
HMENU  hmenuMain;                                                 // дескриптор меню

 // --- диалоговое окно ---
HWND hwndDlg;

// --- оконная функция и обработчики сообщений главного окна ---
LRESULT CALLBACK MainWndProc(HWND, UINT, WPARAM, LPARAM);
BOOL MainOnCreate(HWND, LPCREATESTRUCT);
BOOL MainOnCommand(HWND, int, HWND, UINT);
BOOL MainOnPaint(HWND);
BOOL polet_snar(HWND);
BOOL MainOnDestroy(HWND);

// --- функции инициализации и завершения ---
BOOL RegisterWndClasses(void);
void UnregisterWndClasses(void);
BOOL CreateWindows(void);
void InitializeApp(void);
void UninitializeApp(void);

// функция - точка входа
int APIENTRY WinMain(HINSTANCE, HINSTANCE, LPSTR, int);

int* Scale_coor_w(HWND, RECT, double*, int);
double* make_mas_coor(int*);
BOOL   anim_main(HWND hwnd);

/*----------------------------------*/
/*  данные, переменные состояния    */
/*----------------------------------*/
 // --- таймер ---
 // интервал времени (мсек)
#define TIMER_INTERVAL 10
 // идентификатор
#define TIMER_ID 1000


 // --- таймер ---
 // счётчик
int timer = -1;

#define  K_GAB 0.05  // Габаритный  коэффициент отступа
//  выводимого изображение от края границы вывода

CVect  kv[ALL_PT] =
{ {  1,   1  },
	{ -1,   -1 },

}, kv1[ALL_PT] =
{ {  -0.333,   -0.333  },
{0, -1},
	{ 0.333,   -0.333 }


}, kv2[ALL_PT] =
{ {  -0.333,   -0.333  },
	{-1, 0},
	{ -0.333,   0.333  }


}, kv3[ALL_PT] =
{ {  -0.333,   0.333  },
	{0,1},
	{ 0.333,   0.333 }


}, kv4[ALL_PT] =
{ {  0.333,   0.333  },
	{1, 0},
	{ 0.333,   -0.333 }


};
void zap(CVect* kv)
{
	double fi = 0;
	for (int i = 0; i < 36; i++)
	{
		kv[i].x = cos(fi);
		kv[i].y = sin(fi);
		fi += PI / 10;
	}
}
double z = 1;
int d = 1;
class GO_SHAPE
{

	CMatr  ms, mr, mt;

public:

	double dx, dy, ang, k_scale_x, k_scale_y, k_scale_x1, k_scale_y1, dim_x;
	int    all_pt_shape, color;

	CVect  vt, kvadro[ALL_PT], kvadro1[ALL_PT], kvadro2[ALL_PT], kvadro3[ALL_PT], kvadro4[ALL_PT], kvadro_win[ALL_PT], kvadro_win1[ALL_PT], kvadro_win2[ALL_PT], kvadro_win3[ALL_PT], kvadro_win4[ALL_PT];

	// методы 
	GO_SHAPE(CVect[], CVect[], CVect[], CVect[], CVect[], int, double, double,
		double, double, double, double
		, double, int, double);

	void move(void);
};

//--------------------------------
// Конструктор класса GO_SHAPE
//--------------------------------

GO_SHAPE::GO_SHAPE(CVect  shape[], CVect  shape1[], CVect  shape2[], CVect  shape3[], CVect  shape4[], int all_pt,
	double dx1, double dy1,
	double ang1,
	double kx, double ky,
	double x_start,
	double y_start,
	int    color_set,
	double x_len
)
{
	int i;

	dx = dx1;
	dy = dy1;
	ang = ang1;
	k_scale_x = kx;
	k_scale_y = ky;
	dim_x = x_len;
	k_scale_x1 = kx;
	k_scale_y1 = ky;
	all_pt_shape = all_pt;
	vt.x = x_start;
	vt.y = y_start;
	color = color_set;
	zap(kvadro);

	for (i = 0; i < all_pt_shape; i++)
	{
		kvadro1[i] = shape1[i];
		kvadro2[i] = shape2[i];
		kvadro3[i] = shape3[i];
		kvadro4[i] = shape4[i];
	}


}

/****************************************************/
/*             GO_SHAPE::move                       */
/*             ==============                       */
/*  Перемещение  фигуры  на один шаг                */
/****************************************************/

void GO_SHAPE::move()
{
	CMatr  msum, msum1;

	extern HDC hdc;

	extern CMatr mpr;

	extern HPEN hpen[], hpen_prv;
	extern HBRUSH 	hbr[], hbr_prv;
	double x = k_scale_x1, y = k_scale_y1;

	
	if (d > 0)
	{
		if (vt.y <= 7 && vt.x <= 95)
		{
			vt.x += dx;
			ang -= PI / 30;
		}
		if (vt.x >= 95 && vt.y <= 63)
		{
			vt.y += dy;
			ang -= PI / 30;
		}
		if (vt.y >= 63 && vt.x > 5)
		{
			vt.x -= dx;
			ang -= PI / 30;
		}
		if (vt.x <= 5 && vt.y >= 7)
		{
			vt.y -= dy;
			ang -= PI / 30;
		}
	}
	if (d < 0)
	{
		if (vt.y >= 63 && vt.x <= 95)
		{
			vt.x += dx;
			ang += PI / 30;
		}
		if (vt.x <= 5 && vt.y <= 63)
		{
			vt.y += dy;
			ang += PI / 30;
		}
		if (vt.y <= 7 && vt.x > 5)
		{
			vt.x -= dx;
			ang += PI / 30;
		}
		if (vt.x >=95 && vt.y >= 7)
		{
			vt.y -= dy;
			ang += PI / 30;
		}
	}

	MatrScale(ms, z * k_scale_x, z * k_scale_y);
	MatrRot(mr, ang);
	MatrTransl(mt, vt);




	msum = ms * mr * mt * mpr;

	for (int i = 0; i < all_pt_shape; i++)
	{
		kvadro_win[i] = kvadro[i] * msum;
		kvadro_win1[i] = kvadro1[i] * msum;
		kvadro_win2[i] = kvadro2[i] * msum;
		kvadro_win3[i] = kvadro3[i] * msum;
		kvadro_win4[i] = kvadro4[i] * msum;

	}

	hbr_prv = (HBRUSH)SelectObject(hdc, hbr[color]);
	hpen_prv = (HPEN)SelectObject(hdc, hpen[color]);
	SelectObject(hdc, hbr[GREEN]);

	draw_polygon(hdc, kvadro_win, kvadro_win1, kvadro_win2, kvadro_win3, kvadro_win4, all_pt_shape, hbr[BLACK]);

	

	SelectObject(hdc, hbr_prv);

}


/********************************************************************/
/*                            MainWndProc                           */
/*                            ============                          */
/*                      оконная функция главного окна               */
/********************************************************************/

GO_SHAPE kv_white(kv, kv1, kv2, kv3, kv4, 100, DX, DY, 0, K_SC, K_SC,
	0, Y_SPACE_ANIM * 0.1, GREEN, X_SPACE_ANIM);
LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_COMMAND:
		return MainOnCommand(hwnd, LOWORD(wParam), HWND(lParam), HIWORD(wParam));

	case WM_PAINT:
		return anim_main(hwnd);

	case WM_TIMER:
		timer++;
		InvalidateRect(hwnd, NULL, TRUE);
		break;

	case WM_DESTROY:
		return MainOnDestroy(hwnd);
	case WM_KEYDOWN:

		switch (wParam)
		{
		
		case VK_ADD:        z =z*1.3; 
			return 
			InvalidateRect(hwnd, NULL, FALSE);
		case VK_SUBTRACT:  if (z > 1) z=z/1.3; 
			return InvalidateRect(hwnd, NULL, FALSE);

		case VK_SPACE:if (kv_white.color < 3) kv_white.color++; else kv_white.color = 0;
			InvalidateRect(hwnd, NULL, FALSE);
			break;
		case 'D':d = -d;
		//	return 0L;
		}
		break;

	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0L;
}  //func MainWndProc


/********************************************************************/
/*                            MainOnCommand                         */
/*                            ============                          */
/*            обработчик  падающего  меню  WM_COMMAND               */
/********************************************************************/
BOOL MainOnCommand(HWND hwnd, int cmdId, HWND hwndCtrlItem, UINT ntfCode)
{
	switch (cmdId)
	{
	case IDC_PLI:
		InvalidateRect(hwnd, NULL, TRUE);

		if (-1 == timer)
			SetTimer(hwnd, TIMER_ID, TIMER_INTERVAL, NULL);
		else
		{
			KillTimer(hwnd, TIMER_ID);
			timer = -1;
		}

		break;

	case IDC_FILE_EXIT:
		SendMessage(hwnd, WM_CLOSE, 0, 0L);
		break;
	}
	return FALSE;
}  //func MainOnCommand


/***********************************************/
/*               MainOnDestroy                 */
/*               =============                 */
/*          обработчик WM_DESTROY              */
/***********************************************/

BOOL MainOnDestroy(HWND hwnd)
{
	PostQuitMessage(0);
	return TRUE;
}  //func MainOnDestroy


/***********************************************/
/*               RegisterWndClasses            */
/*               ==================            */
/*     регистрация классов окон в БД Windows   */
/***********************************************/

BOOL RegisterWndClasses()
{
	// инициализация структуры описания класса окон
	WNDCLASSEX wce_main;
	memset(&wce_main, 0, sizeof(WNDCLASSEX));
	wce_main.cbSize = sizeof(WNDCLASSEX);
	wce_main.hInstance = hiApp;
	wce_main.style = CS_VREDRAW | CS_HREDRAW;
	// wce_main.hIcon = LoadIcon(hiApp, MAKEINTRESOURCE(IDI_ICON));
	wce_main.lpfnWndProc = (WNDPROC)MainWndProc;
	wce_main.hCursor = LoadCursor(NULL, IDC_ARROW);
	wce_main.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wce_main.lpszMenuName = MAKEINTRESOURCE(IDR_MENU);
	wce_main.lpszClassName = pczMainWndClassName;
	return RegisterClassEx(&wce_main);
}  //func RegisterWndClasses


/***************************************************/
/*               UnregisterWndClasses              */
/*               ====================              */
/*   удаление описания классов окон из БД Windows  */
/***************************************************/

void UnregisterWndClasses()
{
	UnregisterClass(pczMainWndClassName, hiApp);
}  //func UnregisterWndClasses


/***************************************************/
/*               CreateWindows                     */
/*               =============                     */
/*     функция создания основных окон              */
/***************************************************/
BOOL CreateWindows()
{
	// определение размеров экрана в пикселах
	int scrw2 = GetSystemMetrics(SM_CXSCREEN) / 2,
		scrh2 = GetSystemMetrics(SM_CYSCREEN) / 2;
	// создание главного окна
	hwndMain = CreateWindow(pczMainWndClassName, pczMainWndTitle,
		WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX |
		WS_MAXIMIZEBOX | WS_THICKFRAME | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
		scrw2 - 400, scrh2 - 300, 800, 600, NULL, NULL, hiApp, NULL);
	if (!hwndMain)
		return FALSE;
	return TRUE;
}  //func CreateWindows

/***************************************************/
/*               InitializeApp                     */
/*               =============                     */
/*     предварительные действия                    */
/***************************************************/
void InitializeApp()
{
	RegisterWndClasses();
	CreateWindows();
	ShowWindow(hwndMain, SW_SHOW);
	UpdateWindow(hwndMain);
}  //func InitializeApp


/***************************************************/
/*               UninitializeApp                   */
/*               ===============                   */
/*     заключительные  действия                    */
/***************************************************/
void UninitializeApp()
{

	extern HBRUSH hbr[];
	extern HPEN hpen[];


	extern int all_br;

	for (int i = 0; i < all_br; i++)
		DeleteObject(hbr[i]);

	for (int i = 0; i < all_br; i++)
		DeleteObject(hpen[i]);

	UnregisterWndClasses();
}  //func UninitializeApp

/***************************************************/
/*                 WinMain                         */
/*                 =======                         */
/*        Главная  функция                         */
/***************************************************/

int APIENTRY WinMain(HINSTANCE hiApp__, HINSTANCE, LPSTR, int)
{
	MSG msg;
	hiApp = hiApp__;

	InitializeApp();
	while (GetMessage(&msg, NULL, 0, 0))
		DispatchMessage(&msg);
	UninitializeApp();

	return msg.wParam;
}  //func WINMAIN


/*--------------------------------*/
/*  Описание класса GO_SHAPE      */
/*--------------------------------*/


// обнуление матрицы
void MatrZero(CMatr& m_)
{
	ZeroMemory(&m_, sizeof(m_));
}  //func MatrZero

 // установка единичной матрицы
void MatrIdentity(CMatr& m_)
{
	MatrZero(m_);
	m_.m[0][0] = m_.m[1][1] = m_.m[2][2] = 1;
}  //func MatrIdentity

// установка матрицы масштабирования
void MatrScale(CMatr& m_, double kx, double ky)
{
	MatrZero(m_);
	m_.m[0][0] = kx;
	m_.m[1][1] = ky;
	m_.m[2][2] = 1;
}  //func MatrScale

 // установка матрицы перемещения
void MatrTransl(CMatr& m_,
	const CVect& v_)
{
	MatrIdentity(m_);
	m_.m[2][0] = v_.x;
	m_.m[2][1] = v_.y;
}  //func MatrTransl

 // установка матрицы поворота
void MatrRot(CMatr& m_,
	const double& dAng_)
{
	MatrIdentity(m_);
	m_.m[0][0] = m_.m[1][1] = cos(dAng_);
	m_.m[1][0] = -sin(dAng_);
	m_.m[0][1] = sin(dAng_);
}  //func MatrRot


 // умножение матриц
CMatr operator * (const CMatr& m1_,
	const CMatr& m2_)
{
	int i, j, k;
	CMatr mRes;
	MatrZero(mRes);
	for (i = 0; i < S_DIMS; i++)
		for (j = 0; j < S_DIMS; j++)
			for (k = 0; k < S_DIMS; k++)
				mRes.m[i][j] += m1_.m[i][k] * m2_.m[k][j];
	return mRes;
}  //operator * (CMatr, CMatr)
CVect operator + (const CVect& v1, const double& dz)
{
	CVect vRes;
	vRes.x = v1.x + dz;
	vRes.y = v1.y + dz;
	return vRes;
}

 // умножение вектора на матрицу
CVect operator * (const CVect& v_,
	const CMatr& m_)
{
	int i, j, k;
	CVect vRes;
	vRes.x = v_.x * m_.m[0][0] + v_.y * m_.m[1][0] + m_.m[2][0];
	vRes.y = v_.x * m_.m[0][1] + v_.y * m_.m[1][1] + m_.m[2][1];
	return vRes;
}  //operator * (CVect, CMatr)

 // печать в строку
void MatrPrint(const CMatr& m1_, LPSTR pcBuff)
{
	int i, j, pos = 0;
	for (i = 0; i < S_DIMS; i++) {
		for (j = 0; j < S_DIMS; j++)
			pos += sprintf(pcBuff + pos, "%f ", m1_.m[i][j]);
		pos += sprintf(pcBuff + pos, "\n");
	}
}  //matr MatrPrint


/********************************************************************/
/*                           anim_proc                              */
/*                           =========                              */
/*    Функции для анимации  произвольного набора плоских объектов   */
/*                                                                  */
/********************************************************************/




HDC hdc;

HBRUSH 	hbr[] = { CreateSolidBrush(RGB(255, 0, 0)),
				  CreateSolidBrush(RGB(0, 255, 0)),
				  CreateSolidBrush(RGB(0, 0, 255)),
				  CreateSolidBrush(RGB(255, 255, 255)),
				  CreateSolidBrush(RGB(0, 0, 0))
},
hbr_prv;
HPEN 	hpen[] = { CreatePen(PS_SOLID, 1, RGB(255, 0, 0)),
				  CreatePen(PS_SOLID, 1,RGB(0, 255, 0)),
				  CreatePen(PS_SOLID, 1,RGB(0, 0, 255)),
				  CreatePen(PS_SOLID, 1,RGB(255, 255, 255)),
				 CreatePen(PS_SOLID, 1,RGB(0, 0, 0))
},
hpen_prv;


int pr_start = 0, all_br = sizeof(hbr) / sizeof(hbr[0]);

CMatr  mpr;

/********************************************************************/
/*                        anim_main                                 */
/*                        ==========                                */
/*      Управляющая  процедура  отрисовки  полёта  квадрата         */
/********************************************************************/







BOOL  anim_main(HWND hwnd)
{
	int    i;
	double dx, dy, ang;

	extern int timer;

	CVect vt,zn_win[5];


	PAINTSTRUCT ps;
	hdc = BeginPaint(hwnd, &ps);

	if (pr_start == 0)
	{
		pr_start = 1;
		EndPaint(hwnd, &ps);
		return TRUE;
	}

	// определить размеры клиентской области окна
	if (timer == 0)
	{
		RECT rc;
		GetClientRect(hwnd, &rc);

		mpr = make_matr_pr(rc);
	}

	/*---------------------------------------*/
	/*      Текущая перерисовка              */
	/*---------------------------------------*/
	
	
	//kv_white.vt.y = Y_SPACE_ANIM / 2 + sin(kv_white.vt.x * 0.1) * 15;
	//kv_white.ang = cos(kv_white.vt.x * 0.1);
	kv_white.move();

	/*---------------------------------------*/


	EndPaint(hwnd, &ps);

	return TRUE;
}

/********************************************************************/
/*                        draw_polygon                              */
/*                        ============                              */
/*      Рисование многоугольника  на экране по  вещественным коорд  */
/********************************************************************/

void draw_polygon(HDC hDc, CVect* pVect, CVect* pVect1, CVect* pVect2, CVect* pVect3, CVect* pVect4, int nPoints, HBRUSH hbr)
{
	POINT* pPnt = new POINT[nPoints];


	
	for (int i = 0; i < nPoints; i++) {
		pPnt[i].x = LONG(pVect[i].x);
		pPnt[i].y = LONG(pVect[i].y);
	}
	MoveToEx(hDc, pPnt[0].x, pPnt[0].y, NULL);
	for (int i = 0; i < 35; i++)
	{
		LineTo(hDc, pPnt[i + 1].x, pPnt[i+1].y);
	}
	for (int i = 0; i < nPoints; i++) {
		pPnt[i].x = LONG(pVect1[i].x);
		pPnt[i].y = LONG(pVect1[i].y);
	}
	MoveToEx(hDc, pPnt[0].x, pPnt[0].y, NULL);
	for (int i = 0; i < 2; i++)
	{
		LineTo(hDc, pPnt[i + 1].x, pPnt[i + 1].y);
	}
	for (int i = 0; i < nPoints; i++) {
		pPnt[i].x = LONG(pVect2[i].x);
		pPnt[i].y = LONG(pVect2[i].y);
	}
	MoveToEx(hDc, pPnt[0].x, pPnt[0].y, NULL);
	for (int i = 0; i < 2; i++)
	{
		LineTo(hDc, pPnt[i + 1].x, pPnt[i + 1].y);
	}
	for (int i = 0; i < nPoints; i++) {
		pPnt[i].x = LONG(pVect3[i].x);
		pPnt[i].y = LONG(pVect3[i].y);
	}
	MoveToEx(hDc, pPnt[0].x, pPnt[0].y, NULL);
	for (int i = 0; i < 2; i++)
	{
		LineTo(hDc, pPnt[i + 1].x, pPnt[i + 1].y);
	}
	for (int i = 0; i < nPoints; i++) {
		pPnt[i].x = LONG(pVect4[i].x);
		pPnt[i].y = LONG(pVect4[i].y);
	}
	MoveToEx(hDc, pPnt[0].x, pPnt[0].y, NULL);
	for (int i = 0; i < 2; i++)
	{
		LineTo(hDc, pPnt[i + 1].x, pPnt[i + 1].y);
	}

	delete[] pPnt;

}


/********************************************************************/
/*                        make_matr_pr                              */
/*                        ============                              */
/*              Формирование  матрицы  проекции                     */
/********************************************************************/
CMatr  make_matr_pr(RECT& cp)
{
	int a, b, gab_a, gab_b, xc_w, yc_w;
	double x_max, y_max, x_min, y_min, k_scale, k1, k2,
		dx, dy, xc, yc, l_snar, l_pushka;

	CMatr  mt1, ms, mr, mt2;

	CVect  vt;


	/*--------------------------------*/
	/*  Параметры  окна  вывода       */
	/*--------------------------------*/
	a = abs(cp.right - cp.left);
	b = abs(cp.top - cp.bottom);

	gab_a = (int)((double)a * K_GAB);  // отступ по ширине
	gab_b = (int)((double)b * K_GAB);  // отступ по высоте

	a -= gab_a;
	b -= gab_b;

	xc_w = (cp.right + cp.left) / 2;
	yc_w = (cp.top + cp.bottom) / 2;

	/*-----------------------------------------*/
	/*  Определение  масштабного коэффициента  */
	/*-----------------------------------------*/
	x_min = 0;
	y_min = 0;

	x_max = X_SPACE_ANIM;
	y_max = Y_SPACE_ANIM;

	dx = fabs(x_max - x_min);
	dy = fabs(y_max - y_min);

	xc = (x_max + x_min) / 2;
	yc = (y_max + y_min) / 2;

	k1 = a / dx;
	k2 = b / dy;

	k_scale = (k1 < k2) ? k1 : k2;


	vt.x = -xc;
	vt.y = -yc;
	MatrTransl(mt1, vt);
	MatrScale(ms, k_scale, k_scale);
	// Зеркальное отображение
	MatrScale(mr, 1, -1);

	vt.x = xc_w;
	vt.y = yc_w;
	MatrTransl(mt2, vt);

	return (mt1 * ms * mr * mt2);
}

/********************************************************************/
/*                           trans_coor                             */
/*                           ==========                             */
/*  Изменение массива координат  по  матрице  преобразований        */
/********************************************************************/

void trans_coor(CVect* pvOld, CVect* pvNew, int all_pt, CMatr& mpr)
{
	for (int i = 0; i < all_pt; i++)
		pvNew[i] = pvOld[i] * mpr;
}






