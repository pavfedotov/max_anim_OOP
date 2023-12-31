#pragma once
#include <math.h>
#include <stdio.h>
#include <windows.h>
//{{NO_DEPENDENCIES}}
// Microsoft Developer Studio generated include file.
// Used by lek_graph1.RC
//
#define IDR_APPLAUSE                    103
#define IDR_EXPLODE                     104
#define IDR_GLASS                       105
#define IDR_START                       106
#define IDR_DRUMROLL                    107
#define IDR_GUNSHOT                     109
#define IDR_MENU                        500
#define IDI_ICON                        600
#define IDD_DIALOG                      800
#define IDC_LABEL                       1000
#define IDC_EDIT_WIDTH                  1001
#define IDC_RADIO_SHOWMODE_CONTOUR      1002
#define IDC_NAME1                       1002
#define IDC_RADIO_SHOWMODE_FILL         1003
#define IDC_NAME2                       1003
#define IDC_EDIT_HEIGHT                 1004
#define IDC_NAME3                       1004
#define IDC_BUTTON                      1005
#define IDC_CHECK                       1006
#define IDC_CHECK_SHOWSIZE              1006
#define IDC_CHECK_FILL                  1006
#define IDC_COMBO_COLOR                 1007
#define IDC_COMBO_GROUP                 1007
#define IDC_LIST_FIGURE                 1008
#define IDC_LIST_STUD                   1008
#define IDC_FILE_DIALOG                 5000
#define IDC_K1                          5000
#define IDC_FILE_ABOUT                  5002
#define IDC_K2                          5002
#define IDC_FILE_EXIT                   5004
#define IDC_CLOSE                       10000
#define IDC_UPDATEPICTURE               10001
#define IDC_PRN                         10002
#define IDC_INC                         10003
#define IDC_DEC                         10004
#define IDC_LIST2                       10006
#define IDC_K4                          40001
#define IDC_K0                          40002
#define IDC_K3                          40003
#define IDC_PLI                         40004

// Next default values for new objects
// 
#ifdef APSTUDIO_INVOKED
#ifndef APSTUDIO_READONLY_SYMBOLS
#define _APS_NEXT_RESOURCE_VALUE        110
#define _APS_NEXT_COMMAND_VALUE         40005
#define _APS_NEXT_CONTROL_VALUE         10007
#define _APS_NEXT_SYMED_VALUE           101
#endif
#endif


#define S_DIMS 3
#define ALL_PT         100         // �����  ����� 

// ����� ��������
#define  RED           0
#define  GREEN         1
#define  BLUE          2
#define  WHITE         3
#define  BLACK		   4


 // ����������
struct CVect {
	double x, y;
};


// ������� �������������� 
struct CMatr {
	double m[S_DIMS][S_DIMS];
};


void Delay(UINT nMiliseconds);
void trans_coor(CVect* pvOld, CVect* pvNew, int all_pt, CMatr& mpr);
CMatr  make_matr_pr(RECT& cp);

void draw_polygon(HDC hDc, CVect* pVect, CVect* pVect1, CVect* pVect2, CVect* pVect3, CVect* pVect4, int nPoints, HBRUSH hbr);



// ��������� �������
void MatrZero(CMatr& m_);

// ��������� ��������� �������
void MatrIdentity(CMatr& m_);

// ��������� ������� ���������������
void MatrScale(CMatr& m_, double kx, double ky);

// ��������� ������� �����������
void MatrTransl(CMatr& m_,
	const CVect& v_);

// ��������� ������� ��������
void MatrRot(CMatr& m_,
	const double& dAng_);




// ��������� ������
CMatr operator * (const CMatr& m1_,
	const CMatr& m2_);

// ��������� ������� �� �������
CVect operator * (const CVect& v_,
	const CMatr& m_);

CVect operator + (const CVect& v1, const double& dz);

// ������ � ������
void MatrPrint(const CMatr& m1_, LPSTR pcBuff);

