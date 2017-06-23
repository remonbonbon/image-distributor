#ifndef _SEPARATOR_H_
#define _SEPARATOR_H_

////////////////////////////////////////////////////

#include <windows.h>

////////////////////////////////////////////////////

#define IDC_MAIN_SEP_H	0x1000
#define IDC_MAIN_SEP_V	0x1001

////////////////////////////////////////////////////

#define SEP_OFFSET		30		//�Z�p���[�^�̊p���B�����߂̃I�t�Z�b�g
#define SEP_WIDTH		(GetSystemMetrics(SM_CXSIZEFRAME))		//�����g�̕�
#define SEP_HIGHT		(GetSystemMetrics(SM_CYSIZEFRAME))		//�����g�̍���

////////////////////////////////////////////////////

#define UM_SEP_CHANGE		(WM_APP + 0x0000)
#define UM_SEP_SETRGN		(WM_APP + 0x0001)
#define UM_SEP_SETRANGE		(WM_APP + 0x0002)

////////////////////////////////////////////////////

struct SeparatorInfo
{
	SIZE size;	//Window�T�C�Y
	int RangeMin, RangeMax;	//�Z�p���[�^�̗L���ȃ����W�i��or�����j

	SeparatorInfo() : RangeMin(0), RangeMax(0) {size.cx=size.cy=0;}
};

////////////////////////////////////////////////////

void RegisterSeparatorClass();	//�Z�p���[�^�̃E�B���h�E�N���X��o�^
HWND CreateHorizonSeparatorWindow(HWND hParent, int Width);
HWND CreateVerticalSeparatorWindow(HWND hParent, int Hight);

////////////////////////////////////////////////////

#endif
