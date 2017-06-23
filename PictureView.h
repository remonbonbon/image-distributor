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
//	WNDPROC DefProc;	//デフォルトプロシージャ

	int ZoomLevel;	//ズーム率 [%]
	bool displaying_ZoomLevel;	//ズーム率を表示中はtrue

	/////////// ↓追加予定機能 ///////////
	bool fit_zoom_no_mag;	//フィットズームのズーム率を最大100%にする（拡大ズームはしない）
	bool auto_fit_zoom;		//自動でフィットズームを使う
	bool mouse_func_on;		//マウス操作（ホイールでズーム，左クリックでフィットズーム）を使う
	bool display_ZoomLevel;	//ズーム率が変わった時に，ズーム率を画像に重ねて表示する

//	bool thumbnail_on;		//サムネイル機能ON
	/////////// ↑追加予定機能 ///////////

	Gdiplus::Image *img;	//画像
	std::string img_path;	//画像ファイルパス
	Gdiplus::Bitmap *bmp;	//バッファ

	int gif_frame_counter;	//アニメgif用フレームカウンタ

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
