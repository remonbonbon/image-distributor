#include "PictureView.h"
#include <GdiPlusFlat.h>

void widen(const std::string &src, std::wstring &dest);

#define PV_TIMER_DISP_ZOOM_LEVEL		100
#define PV_TIMER_DISP_ZOOM_LEVEL_MSEC	1000

#define PV_TIMER_GIF_ANIME		200

static void My_DrawString(Gdiplus::Graphics &gr, int x, int y, wchar_t *text, const wchar_t *font_face, int font_size, const Gdiplus::Color &color)
{
	Gdiplus::StringFormat sf;
	Gdiplus::Font font(font_face, (float)font_size);
	Gdiplus::PointF pt((float)x, (float)y);
	Gdiplus::SolidBrush textbrush(color);

	gr.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
	gr.DrawString(text, wcslen(text), &font, pt, &sf, &textbrush);
}

//�A�j��gif�̑��t���[�������擾
//���ʂ̉摜�́C���t���[���� 1
int GetGif_frames(Gdiplus::Image &gif)
{
	GUID guid;
	gif.GetFrameDimensionsList(&guid, 1);	// �ŏ��̃f�B�����V������GUID���擾
	return gif.GetFrameCount(&guid);	// �R�}�����擾
}

//�A�j��gif�̃t���[���i�߂�
//�\�����ԁi�f�B���C�j��Ԃ�
//http://d.hatena.ne.jp/yu-hr/20090201/1233494522
//http://homepage1.nifty.com/MADIA/vb/vb_bbs/200709/200709_07090026.html
static int GetGif_delay(Gdiplus::Image &gif, int &frame_counter, bool gif_anime_next_frame)
{
	GUID guid = {0};
	gif.GetFrameDimensionsList(&guid, 1);	// �ŏ��̃f�B�����V������GUID���擾

	int frames = gif.GetFrameCount(&guid);		// �R�}�����擾
	//���t���[������1�̂Ƃ��͕��ʂ̉摜
	if(frames <= 1) return -1;

	int nSize = gif.GetPropertyItemSize(PropertyTagFrameDelay);
	Gdiplus::PropertyItem *propertyItem = (Gdiplus::PropertyItem *)(new byte[nSize]);
	gif.GetPropertyItem(PropertyTagFrameDelay, nSize, propertyItem);

	if(frame_counter < 0 || frames <= frame_counter)
	{
		delete []propertyItem;
		return -1;
	}

	// �A�j��GIF�ł̓R�}���Ƃ̑ҋ@���Ԃ��ݒ�ł���̂ŁA������擾����B
	int t = ((long*) propertyItem->value)[frame_counter] * 10;

	//�t���[����i�߂�
	if(true == gif_anime_next_frame)
	{
		gif.SelectActiveFrame(&guid, frame_counter);
		frame_counter++;
		if(frames <= frame_counter) frame_counter = 0;
	}

	delete []propertyItem;
	return t;
}

/******************************************************************************
	�摜���擾
******************************************************************************/
Gdiplus::Image *GetPicture(HWND hWnd)
{
	PictureViewInfo *pvi = (PictureViewInfo *)GetWindowLong(hWnd, GWL_USERDATA);
	if(NULL == pvi) return NULL;
	return pvi->img;
}

/******************************************************************************
	PictureView��������
******************************************************************************/
void InitPictureView()
{
	WNDCLASSEX WndClass = {0};
		WndClass.cbSize = sizeof(WNDCLASSEX);
		WndClass.style = CS_HREDRAW | CS_VREDRAW;
		WndClass.hInstance = GetModuleHandle(NULL);
		WndClass.lpszClassName = TEXT("PictureView");
		WndClass.lpfnWndProc = PictureViewProc;
		WndClass.lpszMenuName = NULL;
		WndClass.hIcon   = LoadIcon(NULL, IDI_APPLICATION);
		WndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
		WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		WndClass.hbrBackground = NULL;
	RegisterClassEx(&WndClass);
}

/******************************************************************************
	PictureView���쐬
******************************************************************************/
HWND CreatePictureView(HWND hParent, DWORD ExStyle)
{
	HWND hWnd = CreateWindowEx(ExStyle, TEXT("PictureView"), TEXT("No Image"),
			WS_CHILD | WS_VISIBLE | WS_TABSTOP | SS_NOTIFY,
			0, 0, 0, 0, hParent, NULL, GetModuleHandle(NULL), NULL);
	SendMessage(hWnd, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);

//	CallWindowProc(PictureViewProc, hWnd, WM_PV_INIT, 0, 0);

	return hWnd;
}

/******************************************************************************
	�摜��`��
******************************************************************************/
static void DrawImg(HWND hWnd, Gdiplus::Graphics &gr, PictureViewInfo *pvi, int w, int h, bool gif_anime_next_frame)
{
/*	const int thum_size = 64;

	if(true == pvi->thumbnail_on)
	{
		Gdiplus::RectF rc(0, (float)thum_size, (float)w, (float)(h-thum_size));
		gr.SetClip(rc);
	}*/

	//�w�i���^�C����ɓh��Ԃ�
	Gdiplus::SolidBrush brush[2] = {0xFF202020, 0xFF303030};
	const int tile_size = 128;
	for(int y=0; y<h; y+= tile_size)
	{
		for(int x=0; x<w; x+= tile_size)
		{
			gr.FillRectangle(&brush[((x+y)/tile_size)%2], x, y, tile_size, tile_size);
		}
	}

	if(NULL == pvi || NULL == pvi->img) return;

	int t = GetGif_delay(*(pvi->img), pvi->gif_frame_counter, gif_anime_next_frame);
	if(0 <= t)
	{
		//http://www.hm.h555.net/~nosferatu/kouza/
		//Windows�ł�Internet�@Explorer��ꕔ�̃u���E�U�ł�
		//�ō����x��6�i100����6�b�j�����x�炵���̂ŁA
		//����ȏ㑬���ݒ肵�Ȃ��悤�C�����܂��傤�B
//		if(t < 60) t = 60;	//����ϓI�ɂ͏��������C������

		//http://d.hatena.ne.jp/yu-hr/20090211/1234341698
		//�ҋ@���Ԃ�0.05�b�ȉ��Ȃ� 0.1�b�ɕς��Ă���
		if(t < 50) t = 100;	//����������IE���ۂ��̂ō̗p

		SetTimer(hWnd, PV_TIMER_GIF_ANIME, t, NULL);
	}

	const float scale = pvi->ZoomLevel / 100.0f;
	if(100 != pvi->ZoomLevel) gr.ScaleTransform(scale, scale);

	const float img_w = (float)(pvi->img->GetWidth()),
				img_h = (float)(pvi->img->GetHeight());
	const float x = (w - img_w* scale)/2.0f,
				y = (h - img_h* scale/*+(true == pvi->thumbnail_on ? thum_size : 0)*/)/2.0f;

	if(100 == pvi->ZoomLevel)
	{
		//100%��
		gr.SetInterpolationMode(Gdiplus::InterpolationModeNearestNeighbor);
	}
	else if(0 <= t)
	{
		//gif�A�j����
		gr.SetInterpolationMode(Gdiplus::InterpolationModeLowQuality);
	}
	else if(pvi->ZoomLevel < 100)
	{
		//�k����
		gr.SetInterpolationMode(Gdiplus::InterpolationModeHighQuality);
	}
	else
	{
		//�g�厞
		gr.SetInterpolationMode(Gdiplus::InterpolationModeDefault);
	}

	gr.DrawImage(pvi->img, x/scale, y/scale, img_w, img_h);
	gr.ResetTransform();

/*	const float aspect = img_w / img_h;
	int thum_w, thum_h;
	if(1.0 <= aspect)
	{
		//�����̏ꍇ
		thum_w = thum_size;
		thum_h = (int)((float)thum_w/aspect);
	}
	else
	{
		//�c�̏ꍇ
		thum_h = thum_size;
		thum_w = (int)((float)thum_h*aspect);
	}

	//�T���l�C����`��
	if(true == pvi->thumbnail_on)
	{
		gr.ResetClip();
		Gdiplus::SolidBrush brush(Gdiplus::Color::Chartreuse);

		int x,y;

		Gdiplus::Image *thum_img = pvi->img->GetThumbnailImage(thum_w, thum_h);
		x = (thum_size-thum_w)/2;
		y = (thum_size-thum_h)/2;
		gr.FillRectangle(&brush, 0, 0, w, thum_size);
		gr.DrawImage(thum_img, x, y, thum_w, thum_h);
		delete thum_img;
	}*/

	//�Y�[������\��
	if(true == pvi->display_ZoomLevel && true == pvi->displaying_ZoomLevel)
	{
		wchar_t buf[128];
		swprintf_s(buf, L"%d%%", pvi->ZoomLevel);

		int x=10, y=10;
//		if(true == pvi->thumbnail_on) y+=64;
		My_DrawString(gr, x, y, buf, L"Tahoma", 24, Gdiplus::Color::Orange);
	}

}

/******************************************************************************
	PictureView�R�[���o�b�N
******************************************************************************/
LRESULT CALLBACK PictureViewProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	PictureViewInfo *pvi = (PictureViewInfo *)GetWindowLong(hWnd, GWL_USERDATA);

	switch(msg)
	{
	case WM_PV_INIT:
	case WM_CREATE:
		{
			if(NULL != pvi) delete pvi;
			pvi = new PictureViewInfo;
			SetWindowLong(hWnd, GWL_USERDATA, (LONG)pvi);

//			pvi->DefProc = (WNDPROC)GetWindowLong(hWnd, GWL_WNDPROC);
//			SetWindowLong(hWnd, GWL_WNDPROC, (LONG)PictureViewProc);

			SendMessage(hWnd, WM_PAINT, 0, 0);
		}
		return 0;
	case WM_PV_WHATIAM:
		return 1;
	case WM_PV_SET_IMG:
		if(pvi)
		{
			if(NULL == wParam)
			{
				//�摜�����
				delete pvi->img;
				pvi->img = NULL;
				pvi->img_path.erase();
				InvalidateRect(hWnd, NULL, FALSE);
				return 0;
			}
			if(NULL != pvi->img) {delete pvi->img; pvi->img = NULL;}

			pvi->img_path = (const char *)wParam;

			std::wstring img_path;
			widen(pvi->img_path, img_path);
			pvi->img = new Gdiplus::Image(img_path.c_str());
			pvi->gif_frame_counter = 0;

			if(false == pvi->auto_fit_zoom)
			{
				InvalidateRect(hWnd, NULL, FALSE);
			}
			else
			{
				SendMessage(hWnd, WM_PV_FIT_ZOOM, 0, 0);
				InvalidateRect(hWnd, NULL, FALSE);
			}
		}
		return 0;
	case WM_PV_SET_ZOOM_LEVEL:
		if(pvi && 0 < wParam && (int)wParam != pvi->ZoomLevel)
		{
			pvi->ZoomLevel = (int)wParam;
			pvi->displaying_ZoomLevel = true;
			SetTimer(hWnd, PV_TIMER_DISP_ZOOM_LEVEL, PV_TIMER_DISP_ZOOM_LEVEL_MSEC, NULL);
			InvalidateRect(hWnd, NULL, FALSE);
		}
		return 0;
	case WM_PV_GET_ZOOM_LEVEL:
		if(pvi) return pvi->ZoomLevel;
		return 0;
	case WM_PV_FIT_ZOOM:
		if(pvi)
		{
			double zoom = 1.0;
			if(pvi->img)
			{
				RECT rc;
				GetClientRect(hWnd, &rc);
				const double w = (rc.right-rc.left),
							 h = (rc.bottom-rc.top)/* -(true == pvi->thumbnail_on ? 64 : 0)*/;
				const double img_w = pvi->img->GetWidth(),
							 img_h = pvi->img->GetHeight();

				//�摜���E�B���h�E�ɍ��킹��
				double a = w/img_w;
				double b = h/img_h;

				zoom = min(a,b);


				//�k������Ƃ������Y�[������
				if(true == pvi->fit_zoom_no_mag && 1.0 <= zoom) zoom = 1.0;
			}
			SendMessage(hWnd, WM_PV_SET_ZOOM_LEVEL, (WPARAM)(zoom*100), 0);
		}
		return 0;
	case WM_PV_FIT_ZOOM_NO_MAG:
		if(pvi) pvi->fit_zoom_no_mag = (0 != wParam);
		return 0;

	case WM_PV_AUTO_FIT_ZOOM:
		if(pvi) pvi->auto_fit_zoom = (0 != wParam);
		return 0;
	case WM_PV_USE_MOUSE_FUNC:
		if(pvi) pvi->mouse_func_on = (0 != wParam);
		return 0;
	case WM_PV_DISPLAY_ZOOM_LEVLEL:
		if(pvi) pvi->display_ZoomLevel = (0 != wParam);
		return 0;
/*	case WM_PV_THUMBNAIL_ON:
		if(pvi)
		{
			pvi->thumbnail_on = (0 != wParam);
			if(false == pvi->auto_fit_zoom)
			{
				InvalidateRect(hWnd, NULL, FALSE);
			}
			else
			{
				SendMessage(hWnd, WM_PV_FIT_ZOOM, 0, 0);
				InvalidateRect(hWnd, NULL, FALSE);
			}
		}
		return 0;*/
	case WM_LBUTTONDOWN:
		if(pvi && true == pvi->mouse_func_on)
		{
			SendMessage(hWnd, WM_PV_FIT_ZOOM,0,0);
		}
		return 0;
/*	case WM_RBUTTONDOWN:
		if(pvi && true == pvi->mouse_func_on)
		{
			SendMessage(hWnd, WM_PV_THUMBNAIL_ON, !pvi->thumbnail_on, 0);
		}
		return 0;*/
	case WM_MOUSEWHEEL:
		if(pvi && true == pvi->mouse_func_on)
		{
			short delta = (short)GET_WHEEL_DELTA_WPARAM(wParam);
			int zoom = pvi->ZoomLevel,d_zoom = 0;

			if(100 < zoom || (100 == zoom && (0 < delta)))
			{
				d_zoom = (int)(zoom * 0.1);
				d_zoom = max(d_zoom, 25);
				d_zoom = 25 * (int)(d_zoom/25);
				zoom += (0 < delta) ? (d_zoom) : (-d_zoom);
				zoom = d_zoom * (int)(zoom/d_zoom);
			}
			else
			{
				d_zoom = 10;
				zoom += (0 < delta) ? (d_zoom) : (-d_zoom);
				zoom = d_zoom * (int)(zoom/d_zoom);
			}
			if(zoom <= 0) zoom = 10;

			SendMessage(hWnd, WM_PV_SET_ZOOM_LEVEL, zoom, 0);
		}
		break;
	case WM_TIMER:
		if(PV_TIMER_DISP_ZOOM_LEVEL == wParam)
		{
			pvi->displaying_ZoomLevel = false;
			KillTimer(hWnd, PV_TIMER_DISP_ZOOM_LEVEL);
			InvalidateRect(hWnd, NULL, FALSE);
		}
		if(PV_TIMER_GIF_ANIME == wParam)
		{
			KillTimer(hWnd, PV_TIMER_GIF_ANIME);

			RECT rc;
			GetClientRect(hWnd, &rc);
			const int w = (rc.right-rc.left), h = (rc.bottom-rc.top);
			HDC hDC = GetDC(hWnd);
			{
				Gdiplus::Graphics gr(pvi->get_buffer(w,h));
				DrawImg(hWnd, gr, pvi, w, h, true);
				Gdiplus::Graphics gr_hDC(hDC);
				gr_hDC.DrawImage(pvi->bmp,0,0);
			}
			ReleaseDC(hWnd, hDC);
			

//			InvalidateRect(hWnd, NULL, FALSE);
		}		
		return 0;
	case WM_SIZE:
		if(pvi)
		{
			if(true == pvi->auto_fit_zoom)
			{
				SendMessage(hWnd, WM_PV_FIT_ZOOM, 0, 0);
			}
		}
		break;
	case WM_PAINT:
		if(pvi)
		{
			RECT rc;
			GetClientRect(hWnd, &rc);
			const int w = (rc.right-rc.left), h = (rc.bottom-rc.top);

			Gdiplus::Graphics gr(pvi->get_buffer(w,h));

			//�摜�\��
			DrawImg(hWnd, gr, pvi, w, h, false);
			PAINTSTRUCT ps = {0};
			HDC hDC = BeginPaint(hWnd, &ps);
			Gdiplus::Graphics gr_hDC(hDC);
			gr_hDC.DrawImage(pvi->bmp,0,0);
			EndPaint(hWnd, &ps);
		}
		return 0;
	case WM_DESTROY:
		if(pvi)
		{
			//PictureViewInfo���폜����O��
			//�Ō�ɌĂԃf�t�H���g�v���V�[�W����ۑ�
			//�T�u�N���X������������
//			WNDPROC def_proc = pvi->DefProc;
//			SetWindowLong(hWnd, GWL_WNDPROC, (LONG)def_proc);
			if(NULL != pvi) delete pvi;
//			return CallWindowProc(def_proc, hWnd, msg, wParam, lParam);
		}
		break;
	}
//	return CallWindowProc(pvi->DefProc, hWnd, msg, wParam, lParam);
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

/******************************************************************************
	�}���`�o�C�g����������C�h������ɕϊ�
******************************************************************************/
/*void widen(const std::string &src, std::wstring &dest)
{
	wchar_t *wcs = new wchar_t[src.length() + 1];
	mbstowcs(wcs, src.c_str(), src.length() + 1);
	dest = wcs;
	delete [] wcs;
}
*/