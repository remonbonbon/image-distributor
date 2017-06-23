#ifndef _PICTURE_VIEW_
#define _PICTURE_VIEW_

///////////////////////////////////////////////////////////////////////////////

#include <windows.h>

#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

#include <string>

/******************************************************************************
	Picture View
******************************************************************************/

struct PictureViewInfo
{
//	WNDPROC DefProc;	//�f�t�H���g�v���V�[�W��

	int ZoomLevel;	//�Y�[���� [%]
	bool displaying_ZoomLevel;	//�Y�[������\������true

	/////////// ���ǉ��\��@�\ ///////////
	bool fit_zoom_no_mag;	//�t�B�b�g�Y�[���̃Y�[�������ő�100%�ɂ���i�g��Y�[���͂��Ȃ��j
	bool auto_fit_zoom;		//�����Ńt�B�b�g�Y�[�����g��
	bool mouse_func_on;		//�}�E�X����i�z�C�[���ŃY�[���C���N���b�N�Ńt�B�b�g�Y�[���j���g��
	bool display_ZoomLevel;	//�Y�[�������ς�������ɁC�Y�[�������摜�ɏd�˂ĕ\������

//	bool thumbnail_on;		//�T���l�C���@�\ON
	/////////// ���ǉ��\��@�\ ///////////

	Gdiplus::Image *img;	//�摜
	std::string img_path;	//�摜�t�@�C���p�X
	Gdiplus::Bitmap *bmp;	//�o�b�t�@

	int gif_frame_counter;	//�A�j��gif�p�t���[���J�E���^

	PictureViewInfo() : /*DefProc(NULL), */
		img(NULL), bmp(NULL),
		ZoomLevel(100), displaying_ZoomLevel(false),
		fit_zoom_no_mag(true),
		auto_fit_zoom(false),
		mouse_func_on(true),
		display_ZoomLevel(true),
//		thumbnail_on(false),
		gif_frame_counter(0)
	{}
	~PictureViewInfo()
	{
		if(NULL != this->img) {delete img; img = NULL;}
		if(NULL != this->bmp) {delete bmp; bmp = NULL;}
	}

	Gdiplus::Bitmap *get_buffer(int w, int h)
	{
		if(NULL == bmp)
		{
			bmp = new Gdiplus::Bitmap(w, h);
		}
		else if(w != bmp->GetWidth() || h != bmp->GetHeight())
		{
			delete bmp;
			bmp = new Gdiplus::Bitmap(w, h);
		}
		return bmp;
	}
};

///////////////////////////////////////////////////////////////////////////////

enum PictureViewWindowMessage
{
	WM_PV_INIT = (WM_APP+0x300),
	WM_PV_WHATIAM,
	WM_PV_SET_IMG,

	WM_PV_SET_ZOOM_LEVEL,
	WM_PV_GET_ZOOM_LEVEL,
	WM_PV_FIT_ZOOM,

	WM_PV_FIT_ZOOM_NO_MAG,
	WM_PV_AUTO_FIT_ZOOM,
	WM_PV_USE_MOUSE_FUNC,
	WM_PV_DISPLAY_ZOOM_LEVLEL,


//	WM_PV_THUMBNAIL_ON,
};

///////////////////////////////////////////////////////////////////////////////

void InitPictureView();
HWND CreatePictureView(HWND hParent, DWORD ExStyle);
LRESULT CALLBACK PictureViewProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

Gdiplus::Image *GetPicture(HWND hWnd);
int GetGif_frames(Gdiplus::Image &gif);

#endif
