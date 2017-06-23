#include "Separator.h"

///////////////////////////////////////////////////////////////////////////////

HWND CreateSeparatorWindow_bytype(HWND hParent, int Length, int type);
LRESULT CALLBACK SeparatorProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

///////////////////////////////////////////////////////////////////////////////

/***********************************************************************************
	�Z�p���[�^�̃E�B���h�E�N���X��o�^
***********************************************************************************/
void RegisterSeparatorClass()
{
	WNDCLASSEX WndClass = {0};
		WndClass.cbSize = sizeof(WNDCLASSEX);
		WndClass.style = CS_HREDRAW | CS_VREDRAW;
		WndClass.hInstance = GetModuleHandle(NULL);
		WndClass.hbrBackground = (HBRUSH)GetSysColorBrush(COLOR_3DFACE);

	//����
		WndClass.lpszClassName = "Separator_Horizon";
		WndClass.lpfnWndProc = SeparatorProc;
		RegisterClassEx(&WndClass);

	//����
		WndClass.lpszClassName = "Separator_Vertical";
		WndClass.lpfnWndProc = SeparatorProc;
		RegisterClassEx(&WndClass);

	return;
}

/***********************************************************************************
	�Z�p���[�^�E�B���h�E���쐬�itype�F����0�C����1�j
***********************************************************************************/
HWND CreateHorizonSeparatorWindow(HWND hParent, int Width)	{return CreateSeparatorWindow_bytype(hParent, Width, 0);}
HWND CreateVerticalSeparatorWindow(HWND hParent, int Hight)	{return CreateSeparatorWindow_bytype(hParent, Hight, 1);}

HWND CreateSeparatorWindow_bytype(HWND hParent, int Length, int type)
{
	if(0 == type)
	{
		return CreateWindowEx(0, "Separator_Horizon", NULL, WS_CHILD | WS_VISIBLE | WS_THICKFRAME,
									0, 0, 0, Length, hParent, (HMENU)IDC_MAIN_SEP_H, GetModuleHandle(NULL), NULL);
	}
	else
	{
		return CreateWindowEx(0, "Separator_Vertical", NULL, WS_CHILD | WS_VISIBLE | WS_THICKFRAME,
									0, 0, Length, 0, hParent, (HMENU)IDC_MAIN_SEP_V, GetModuleHandle(NULL), NULL);
	}
}

/***********************************************************************************
	�Z�p���[�^�̃v���V�[�W��
***********************************************************************************/
LRESULT CALLBACK SeparatorProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	SeparatorInfo *si = (SeparatorInfo *)GetWindowLong(hWnd, GWL_USERDATA);
	RECT rc;

	switch(msg)
	{
	case WM_CREATE:
		{
			if(NULL != si) delete si;
			si = new SeparatorInfo;
			SetWindowLong(hWnd, GWL_USERDATA, (LONG)si);

			GetWindowRect(hWnd, &rc);
			si->size.cx = rc.right-rc.left;
			si->size.cy = rc.bottom-rc.top;
			si->RangeMin = si->RangeMax = -1;
		}
		return 0;
	//�����W�ݒ�
	case UM_SEP_SETRANGE:
		if(si)
		{
			si->RangeMin = (int)wParam;
			si->RangeMax = (int)lParam;
		}
		return 0;
	//�Z�p���[�^�T�C�Y���ς����
	case UM_SEP_CHANGE:
//		LayoutWindow(mcs.hMain, 0, 0);
		{
			SendMessage(GetParent(hWnd), WM_SIZE, 0, 0);
		}
		return 0;
	//���[�W�����ݒ�
	case UM_SEP_SETRGN:
		if(si)
		{
			HRGN hRgn = NULL;
			SIZE sz = si->size;
			const int ID = GetWindowLong(hWnd, GWL_ID);
			//�����i���[�j
			if(IDC_MAIN_SEP_V == ID)
			{
				hRgn = CreateRectRgn(sz.cx-SEP_WIDTH, SEP_OFFSET, sz.cx+1, sz.cy+1-SEP_OFFSET);
			}
			//�����i���[�j
			else if(IDC_MAIN_SEP_H == ID)
			{
				hRgn = CreateRectRgn(SEP_OFFSET, sz.cy-SEP_HIGHT, sz.cx+1-SEP_OFFSET, sz.cy+1);
			}
			SetWindowRgn(hWnd, hRgn, TRUE);
		}
		return 0;
	//�����W����
	case WM_SIZING:
		if(si)
		{
			RECT *lprc = (RECT *)lParam;
			POINT org = {0,0};
			ClientToScreen(hWnd, &org);
			const int Min = si->RangeMin, Max = si->RangeMax;

			switch(GetWindowLong(hWnd, GWL_ID))
			{
			//����
			case IDC_MAIN_SEP_V:
				if(0 <= Min && lprc->right < Min + org.x) lprc->right = Min + org.x;
				if(0 <= Max && lprc->right > Max + org.x) lprc->right = Max + org.x;
				break;
			//����
			case IDC_MAIN_SEP_H:
				if(0 <= Min && lprc->bottom < Min + org.y) lprc->bottom = Min + org.y;
				if(0 <= Max && lprc->bottom > Max + org.y) lprc->bottom = Max + org.y;
				break;
			}
		}
		return 0;
	case WM_SIZE:
		if(si)
		{
			GetWindowRect(hWnd, &rc);
			//���Y�����̃T�C�Y���ς���Ă�����UM_SEP_CHANGE�𑗂�
			switch(GetWindowLong(hWnd, GWL_ID))
			{
			//����
			case IDC_MAIN_SEP_V:
				//���ύX����
				if((rc.right - rc.left) != si->size.cx)
				{
					SendMessage(hWnd, UM_SEP_CHANGE, 0 , 0);
				}
				break;
			//����
			case IDC_MAIN_SEP_H:
				//�����ύX����
				if((rc.bottom - rc.top) != si->size.cy)
				{
					SendMessage(hWnd, UM_SEP_CHANGE, 0 , 0);
				}
				break;
			}

			si->size.cx = (rc.right - rc.left);
			si->size.cy = (rc.bottom - rc.top);
			SendMessage(hWnd, UM_SEP_SETRGN, 0 , 0);
		}
		return 0;
	case WM_CLOSE:
		DestroyWindow(hWnd);
		return 0;
	case WM_DESTROY:
		if(si)
		{
			if(NULL != si) delete si;
		}
		break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}
