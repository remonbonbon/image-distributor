#ifndef _SEPARATOR_H_
#define _SEPARATOR_H_

////////////////////////////////////////////////////

#include <windows.h>

////////////////////////////////////////////////////

#define IDC_MAIN_SEP_H	0x1000
#define IDC_MAIN_SEP_V	0x1001

////////////////////////////////////////////////////

#define SEP_OFFSET		30		//セパレータの角を隠すためのオフセット
#define SEP_WIDTH		(GetSystemMetrics(SM_CXSIZEFRAME))		//垂直枠の幅
#define SEP_HIGHT		(GetSystemMetrics(SM_CYSIZEFRAME))		//水平枠の高さ

////////////////////////////////////////////////////

#define UM_SEP_CHANGE		(WM_APP + 0x0000)
#define UM_SEP_SETRGN		(WM_APP + 0x0001)
#define UM_SEP_SETRANGE		(WM_APP + 0x0002)

////////////////////////////////////////////////////

struct SeparatorInfo
{
	SIZE size;	//Windowサイズ
	int RangeMin, RangeMax;	//セパレータの有効なレンジ（幅or高さ）

	SeparatorInfo() : RangeMin(0), RangeMax(0) {size.cx=size.cy=0;}
};

////////////////////////////////////////////////////

void RegisterSeparatorClass();	//セパレータのウィンドウクラスを登録
HWND CreateHorizonSeparatorWindow(HWND hParent, int Width);
HWND CreateVerticalSeparatorWindow(HWND hParent, int Hight);

////////////////////////////////////////////////////

#endif
