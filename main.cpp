#include "main.h"

MyCommonStructure mcs;


/******************************************************************************
	ボタンを作成
******************************************************************************/
HWND CreateButton(HWND hParent, LPCTSTR text)
{
	HWND hWnd = CreateWindow("BUTTON", text,
			WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON | BS_MULTILINE,
			0, 0, 0, 0, hParent, 0, GetModuleHandle(NULL), NULL);
	SendMessage(hWnd, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);
	return hWnd;
}

/******************************************************************************
	コピーボタンのテキストを設定
******************************************************************************/
void set_copy_btn_text()
{
	PictureData *pd = mcs.list_pic.current();
	if(NULL != pd)
	{
		std::string copy_btn_text = GetItemName(mcs.copy_dir);
		copy_btn_text = mcs.copy_dir;
		copy_btn_text = (false == pd->copy) ? (copy_btn_text + " へコピーする") : ("コピーしない");
		SetWindowText(mcs.hBtn_copy, copy_btn_text.c_str());
	}
}

/******************************************************************************
	シャッフルボタンのテキストを設定
******************************************************************************/
void set_shuffle_btn_text()
{
	std::string button_text;
	if(mcs.shuffle_list_pic)
	{
		button_text += "シャッフル ON";
		if(0 < mcs.listup_filenum_limit)
		{
			std::stringstream ss;
			ss << mcs.listup_filenum_limit;
			button_text += ", 上限枚数: " + ss.str();
		}
	}
	else
	{
		button_text += "シャッフル OFF";
	}
	SetWindowText(mcs.hBtn_shuffle, button_text.c_str());
}

/******************************************************************************
	次の画像へ
******************************************************************************/
void next_pic(bool refresh_only = false, bool go_backford = false)
{
	char buf[256];
	std::string str;

	if(0 == mcs.list_pic.count())
	{
		SET_STATUS_TEXT(2, "");
		SET_STATUS_TEXT(3, "");
		SET_STATUS_TEXT(4, "");
		SET_STATUS_TEXT(5, "");
	}
	else
	{
		//画像読み込み
		if(false == refresh_only)
		{
			if(false == go_backford) mcs.list_pic.next();
			else mcs.list_pic.back();
		}

		SetWindowText(mcs.hMain, (std::string(PROGRAM_NAME " - ") + GetItemName(mcs.list_pic.current_img_name())).c_str());

		Timer timer;
		timer.start();
		SendMessage(mcs.hPicView, WM_PV_SET_IMG, (WPARAM)(mcs.list_pic.current_img_name()), 0);
		timer.end();
		sprintf(buf, "Picture loading %.2lfms", timer.get());
		SET_STATUS_TEXT(5, buf);
		SendMessage(mcs.hPicView, WM_PV_FIT_ZOOM, 0, 0);

		//画像の情報を取得
		PictureData *pd = mcs.list_pic.current();
		if(NULL != pd)
		{
			ImageInfo ii(pd->path, *GetPicture(mcs.hPicView));
			str += ii.format_EXT();
			if(1 < ii.gif_frame)
			{
				sprintf(buf, "(%d)", ii.gif_frame);
				str += buf;
			}
			sprintf(buf, " %dx%d", ii.width, ii.height);
			str += buf;
			SET_STATUS_TEXT(2, str.c_str());
			SET_STATUS_TEXT(3, ii.last_update_time().c_str());

			if(ii.size.QuadPart < 1024)					sprintf(buf, "%d byte", ii.size.QuadPart);
			else if(ii.size.QuadPart < 1024*1024)		sprintf(buf, "%.1lf KB", ii.size.QuadPart/1024.0);
			else if(ii.size.QuadPart < 1024*1024*1024)	sprintf(buf, "%.1lf MB", ii.size.QuadPart/1024.0/1024.0);
			else										sprintf(buf, "%.1lf GB", ii.size.QuadPart/1024.0/1024.0/1024.0);
			SET_STATUS_TEXT(4, buf);
		}
	}

	set_copy_btn_text();

	//リストビューの更新
	ListView_EnsureVisible(mcs.hList, mcs.list_pic.current_index(), FALSE);
	InvalidateRect(mcs.hList, NULL, FALSE);
}

/******************************************************************************
	画像リスト，ディレクトリリスト更新
******************************************************************************/
void update_list(bool img_enum_only)
{
	Timer timer;
	char buf[128];

	//画像ファイルの列挙
	{
		const char *extension_list[] = {"bmp", "gif", "jpg", "jpeg", "png", "tif", "tiff", "emf", "ico", "wmf"};
		mcs.list_pic.clear();
		timer.start();
		mcs.list_pic.enumerate(mcs.current_dir.c_str(), EF_ALL_FILES, extension_list, sizeof(extension_list)/sizeof(extension_list[0]));
		if(mcs.shuffle_list_pic)
		{
			// 画像リストをシャッフル
			mcs.list_pic.shuffle();
			mcs.list_pic.shuffle();	// 何回かシャッフル

			if(0 < mcs.listup_filenum_limit)
			{
				mcs.list_pic.cutoff(mcs.listup_filenum_limit);
			}
		}
		timer.end();
		sprintf(buf, "%d image(s) (%.1lfms)", mcs.list_pic.count(), timer.get());
		SET_STATUS_TEXT(0, buf);
		//リストビューに列挙
		LVITEM item = {0};
		std::string filename;
		ListView_DeleteAllItems(mcs.hList);
		for(std::vector<PictureData>::const_iterator itr = mcs.list_pic.get_begin(); itr != mcs.list_pic.get_end(); itr++)
		{
			item.mask = LVIF_TEXT;
			item.iItem = ListView_GetItemCount(mcs.hList);

			item.pszText = "×";
			item.iSubItem = 0;
			ListView_InsertItem(mcs.hList , &item);

			filename = GetItemName(itr->path);
			item.pszText = (char*)filename.c_str();
			item.iSubItem = 1;
			ListView_SetItem(mcs.hList , &item);
		}
	}

	//ディレクトリの列挙
	if(!img_enum_only)
	{
		mcs.list_dir.clear();
		mcs.list_dir.set_parent(mcs.hMain);
		timer.start();
		mcs.list_dir.enumerate(mcs.current_dir.c_str());
		for(std::vector<std::string>::iterator itr = mcs.add_dir.begin(); itr != mcs.add_dir.end(); itr++)
		{
			mcs.list_dir.add(*itr);
		}
		mcs.list_dir.refresh();
		timer.end();
		sprintf(buf, "%d directory(s) (%.1lfms)", mcs.list_dir.count(), timer.get());
		SET_STATUS_TEXT(1, buf);
	}


	//メニュー作成
	if(!img_enum_only)
	{
		if(NULL != mcs.hMenu) DestroyMenu(mcs.hMenu);
		mcs.hMenu = CreatePopupMenu();

		std::string dir;

		MENUITEMINFO mii = {0};
		mii.cbSize = sizeof(MENUITEMINFO);
		mii.fMask = MIIM_TYPE | MIIM_ID;

		mii.fType = MFT_STRING;
		dir = GetItemName(mcs.copy_dir);
		mii.dwTypeData = (LPTSTR)dir.c_str();
		mii.wID = MENU_ID_COPY_DIR;
		InsertMenuItem(mcs.hMenu, 0, TRUE, &mii);

		mii.fType = MFT_SEPARATOR;
		mii.wID = MENU_ID_SEPARATOR;
		InsertMenuItem(mcs.hMenu, 1, TRUE, &mii);

		mii.fMask = MIIM_TYPE | MIIM_ID | MIIM_DATA;
		mii.fType = MFT_STRING;
		int index = MENU_ID_DIR_START;
		for(std::vector<DirectoryData>::const_iterator itr = mcs.list_dir.get_begin(); itr != mcs.list_dir.get_end(); itr++)
		{
			dir = GetItemName(itr->path);
			mii.dwTypeData = (LPTSTR)dir.c_str();
			mii.wID = index;
			mii.dwItemData = (DWORD)(itr->hWnd);	//ユーザー定義データのボタンのウィンドウハンドルを入れる（どのディレクトリか特定できる）
			InsertMenuItem(mcs.hMenu, -1, TRUE, &mii);
			index++;
		}
	}

}

/******************************************************************************
	移動＆コピーを実行
******************************************************************************/
void move_copy(PicturesList_GoLog &pgl)
{
	SendMessage(mcs.hPicView, WM_PV_SET_IMG, 0, 0);	//画像を解放

	//移動&コピー処理
//	std::ofstream logout(mcs.log_filename.c_str(), std::ios::out | std::ios::app);
//	logout << "[" << get_current_time_string() << "]" << std::endl;
//	mcs.list_pic.go(mcs.copy_dir, &pgl, &logout);
	mcs.list_pic.go(mcs.copy_dir, &pgl, NULL);
//	logout << "------------ ";
//	logout << "copy: success=" << pgl.copy << " fail=" << pgl.copy_fail << " skip=" << pgl.copy_skip;
//	logout << ", move: success=" << pgl.move << " fail=" << pgl.move_fail << " skip=" << pgl.move_skip;
//	logout << " ------------" << std::endl;
//	logout.close();

	//リストなどを更新
	update_list(true);
	SendMessage(mcs.hMain, WM_SIZE, 0, 0);
	next_pic(true);
}

/***********************************************************************************
	ウィンドウプロシージャ
***********************************************************************************/
LRESULT CALLBACK MainProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static bool on_initialize = false;

	switch(msg)
	{
	case WM_CREATE:
		{
			Timer timer;
			timer.start();

			on_initialize = true;
			InitCommonControls();


			mcs.hMain = hWnd;
			//↓リストビュー用強調表示フォント
			mcs.hFont = CreateFont(12, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
				SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
				PROOF_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "MS UI Gothic");

			mcs.hBtn_back    = CreateButton(hWnd, "戻る");
			mcs.hBtn_next    = CreateButton(hWnd, "次へ");
			mcs.hBtn_copy    = CreateButton(hWnd, "");
			mcs.hBtn_go      = CreateButton(hWnd, "実行");
			mcs.hBtn_shuffle = CreateButton(hWnd, "シャッフル");
			set_shuffle_btn_text();

			// コピー先が空なら、コピーボタンを無効化
			if(mcs.copy_dir.empty())
			{
				EnableWindow(mcs.hBtn_copy, FALSE);
			}

			//-------------------------- リストビュー作成
			{
				mcs.hList = CreateWindowEx(WS_EX_CLIENTEDGE, WC_LISTVIEW, 0, 
						WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER | LVS_REPORT | LVS_SINGLESEL,
						0, 0, 0, 0,
						hWnd, 0, GetModuleHandle(NULL), NULL);
				SendMessage(mcs.hList, LVM_SETEXTENDEDLISTVIEWSTYLE, 0,
					LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP | LVS_EX_GRIDLINES
					| LVS_EX_ONECLICKACTIVATE | LVS_EX_UNDERLINEHOT | LVS_EX_DOUBLEBUFFER);
				LVCOLUMN col;
				col.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
				col.fmt = LVCFMT_LEFT;
				const int cx[] = {mcs.LV_width_copy, mcs.LV_width_img, mcs.LV_width_dir};
				std::string filename = GetItemName(mcs.copy_dir);
				LPTSTR text[] = {(char*)filename.c_str(), TEXT("ファイル名"), TEXT("移動先")};
				for(int iSubItem =0; iSubItem < sizeof(cx)/sizeof(cx[0]); iSubItem++)
				{
					col.cx = cx[iSubItem];
					col.pszText = text[iSubItem];
					col.iSubItem = iSubItem;
					ListView_InsertColumn(mcs.hList, iSubItem, &col);
				}
			}

			//セパレータ
			RegisterSeparatorClass();
			{
				RECT rc;
				GetClientRect(hWnd, &rc);
				mcs.hSep = CreateVerticalSeparatorWindow(hWnd, GET_RECT_Y(rc));
				SET_WND_POSIZE(mcs.hSep, 0, -SEP_OFFSET, mcs.listview_width, GET_RECT_Y(rc)+SEP_OFFSET*2);
			}

			//ステータスバー
			const int parts[] = {150, 300, 450, 600, 700, 850, -1};
			mcs.hStatus = CreateStatusWindow(WS_CHILD | WS_VISIBLE | CCS_BOTTOM | SBARS_SIZEGRIP, NULL, hWnd, NULL);
			SendMessage(mcs.hStatus, SB_SETPARTS, sizeof(parts)/sizeof(parts[0]), (LPARAM)parts);

			//------------ PictureView ------------//
			InitPictureView();
			mcs.hPicView = CreatePictureView(hWnd, WS_EX_CLIENTEDGE);
			SendMessage(mcs.hPicView, WM_PV_SET_IMG, (WPARAM)(mcs.list_pic.current_img_name()), 0);
		/*	{
				RECT rc;
				GetClientRect(hWnd, &rc);
				SendMessage(hWnd, WM_SIZE, 0, 0);
			}*/
			SendMessage(mcs.hPicView, WM_PV_FIT_ZOOM_NO_MAG, true, 0);
			SendMessage(mcs.hPicView, WM_PV_AUTO_FIT_ZOOM, true, 0);
			SendMessage(mcs.hPicView, WM_PV_USE_MOUSE_FUNC, true, 0);
			SendMessage(mcs.hPicView, WM_PV_DISPLAY_ZOOM_LEVLEL, true, 0);

			//カレントディレクトリの画像&フォルダを列挙
			update_list(false);

			next_pic(true);

			on_initialize = false;

			timer.end();
			char buf[128];
			sprintf(buf, "Initialize %dms", (int)timer.get());
			SET_STATUS_TEXT(6, buf);
		}
		return 0;
	case WM_COMMAND:
		//ディレクトリボタンが押されたとき
		if(LOWORD((DWORD)(&(mcs.list_dir))) == LOWORD(wParam))
		{
			SetFocus(hWnd);
			const DirectoryData *dd = mcs.list_dir.find((HWND)lParam);
			PictureData *pd = mcs.list_pic.current();
			if(NULL != dd && NULL != pd)
			{
				pd->to_dir = dd->path;

				LVITEM item = {0};
				item.mask = LVIF_TEXT;
				item.iItem = mcs.list_pic.current_index();

				std::string filename = GetItemName(dd->path);
				item.pszText = (char*)filename.c_str();
				item.iSubItem = 2;
				ListView_SetItem(mcs.hList , &item);

				next_pic();
				return 0;
			}
		}
		//次へボタン
		if((HWND)lParam == mcs.hBtn_next)
		{
			SetFocus(hWnd);
			next_pic();
			return 0;
		}
		//戻るボタン
		if((HWND)lParam == mcs.hBtn_back)
		{
			SetFocus(hWnd);
			next_pic(false, true);
			return 0;
		}
		//コピーボタン
		if((HWND)lParam == mcs.hBtn_copy)
		{
			SetFocus(hWnd);
			PictureData *pd = mcs.list_pic.current();
			if(NULL != pd)
			{
	 			pd->copy = !pd->copy;
				set_copy_btn_text();

				LVITEM item = {0};
				item.mask = LVIF_TEXT;
				item.pszText = (false == pd->copy) ? ("×") : ("○");;
				item.iItem = mcs.list_pic.current_index();
				item.iSubItem = 0;
				ListView_SetItem(mcs.hList, &item);

				InvalidateRect(mcs.hList, NULL, FALSE);
			}
			return 0;
		}
		//Goボタン
		if((HWND)lParam == mcs.hBtn_go)
		{
			SetFocus(hWnd);

			PicturesList_GoLog pgl;
			Timer timer;
			timer.start();
			move_copy(pgl);
			timer.end();
			char buf[256];
			sprintf(buf, "copy %d image(s), move %d image(s) (%dms)", pgl.copy, pgl.move, (int)timer.get());
			SET_STATUS_TEXT(6, buf);
			return 0;
		}
		//シャッフルボタン
		if((HWND)lParam == mcs.hBtn_shuffle)
		{
			SetFocus(hWnd);
			mcs.shuffle_list_pic = !mcs.shuffle_list_pic;
			set_shuffle_btn_text();
			update_list(true);
			next_pic(true);
			return 0;
		}
		break;
	case WM_KEYDOWN:
		if(VK_RETURN == wParam)
		{
			if(0 == (BST_PUSHED & SendMessage(mcs.hBtn_next, BM_GETSTATE, TRUE, 0)))
			{
				SendMessage(mcs.hBtn_next, BM_SETSTATE, TRUE, 0);
			}
		}
		else if(VK_SPACE == wParam)
		{
			if(0 == (BST_PUSHED & SendMessage(mcs.hBtn_copy, BM_GETSTATE, TRUE, 0)))
			{
				SendMessage(mcs.hBtn_copy, BM_SETSTATE, TRUE, 0);
			}
		}
		else
		{
			const DirectoryData *dd = mcs.list_dir.find((const char)wParam);
			if(NULL != dd)
			{
				if(0 == (BST_PUSHED & SendMessage(dd->hWnd, BM_GETSTATE, TRUE, 0)))
				{
					SendMessage(dd->hWnd, BM_SETSTATE, TRUE, 0);
				}
			}
		}
		return 0;
	case WM_KEYUP:
		if(VK_RETURN == wParam)
		{
			SendMessage(mcs.hBtn_next, BM_SETSTATE, FALSE, 0);
			SendMessage(mcs.hBtn_next, BM_CLICK, 0, 0);
		}
		else if(VK_SPACE == wParam)
		{
			SendMessage(mcs.hBtn_copy, BM_SETSTATE, FALSE, 0);
			SendMessage(mcs.hBtn_copy, BM_CLICK, 0, 0);
		}
		else
		{
			const DirectoryData *dd = mcs.list_dir.find((const char)wParam);
			if(NULL != dd)
			{
				SendMessage(dd->hWnd, BM_SETSTATE, FALSE, 0);
				SendMessage(dd->hWnd, BM_CLICK, 0, 0);
			}
		}
		return 0;
	case WM_NOTIFY:
		{
			LPNMHDR lpnmhdr = (LPNMHDR)lParam;
			LPNMLISTVIEW lplv = (LPNMLISTVIEW)lParam;
			LPNMLVCUSTOMDRAW lplvcd = (LPNMLVCUSTOMDRAW)lParam;
			if(lpnmhdr->hwndFrom == mcs.hList)
			{
				switch(lplv->hdr.code)
				{
				case NM_CUSTOMDRAW:
					if(CDDS_PREPAINT == lplvcd->nmcd.dwDrawStage)
					{
						return CDRF_NOTIFYITEMDRAW;
					}
					if(CDDS_ITEMPREPAINT == lplvcd->nmcd.dwDrawStage)
					{
						PictureData *pd = mcs.list_pic.get(lplvcd->nmcd.dwItemSpec);
						if(mcs.list_pic.current_index() == lplvcd->nmcd.dwItemSpec)
						{
							//現在の画像
							SelectObject(lplvcd->nmcd.hdc, mcs.hFont);
						}
						if(pd && false == pd->to_dir.empty())
						{
							//移動先の決まっている画像
							lplvcd->clrTextBk = mcs.LV_dir_bk_color;
							lplvcd->clrText = RGB(0,0,0);
						}
						if(pd && true == pd->copy)
						{
							//コピーする画像
							lplvcd->clrText = mcs.LV_copy_font_color;
						}
						return CDRF_NEWFONT;
					}
					break;
				case NM_DBLCLK:
					{
						LPNMITEMACTIVATE lpnmitem = (LPNMITEMACTIVATE)lParam;
						mcs.list_pic.set_index(lpnmitem->iItem);
						SendMessage(hWnd, WM_COMMAND, 0, (LPARAM)mcs.hBtn_copy);
					}
					break;
				case NM_SETFOCUS:
					SetFocus(hWnd);
					break;
				case LVN_ITEMCHANGED:
					{
						const int index = ((LPNMLISTVIEW)lParam)->iItem;
						//違う画像に移るときのみリフレッシュをかける
						if(mcs.list_pic.current_index() != index)
						{
							mcs.list_pic.set_index(index);
							next_pic(true);
						}
					}
					break;
		/*		case NM_RCLICK:
					{
						LPNMITEMACTIVATE lpnmitem = (LPNMITEMACTIVATE)lParam;
						if(lpnmitem->iItem < 0) return 0;
						mcs.list_pic.set_index(lpnmitem->iItem);
						PictureData *pd = mcs.list_pic.current();
						if(NULL != pd)
						{
							MENUITEMINFO mii = {0};
							mii.cbSize = sizeof(MENUITEMINFO);
							mii.fMask = MIIM_STATE;
							mii.fState = (true == pd->copy) ? MFS_CHECKED : MFS_UNCHECKED;
							SetMenuItemInfo(mcs.hMenu, 0, TRUE, &mii);

							ClientToScreen(mcs.hList, &lpnmitem->ptAction);
							int cmd = TrackPopupMenu(mcs.hMenu,
								TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RETURNCMD,
								lpnmitem->ptAction.x, lpnmitem->ptAction.y, 0, hWnd, NULL);
							if(MENU_ID_COPY_DIR == cmd)
							{
								//コピーディレクトリの場合
								SendMessage(hWnd, WM_COMMAND, 0, (LPARAM)mcs.hBtn_copy);
							}
							else if(MENU_ID_DIR_START <= cmd)
							{
								//移動ディレクトリの場合
								//ボタンをエミュレート
								mii.fMask = MIIM_DATA;
								GetMenuItemInfo(mcs.hMenu, cmd, FALSE, &mii);
								SendMessage(hWnd, WM_COMMAND, LOWORD((DWORD)(&(mcs.list_dir))), (LPARAM)mii.dwItemData);
							}
						}
					}
					break;*/
				}
			}
		}
		break;
	case WM_PAINT:
		{
			PAINTSTRUCT ps = {0};
			HDC hDC = BeginPaint(hWnd, &ps);
			if(false == on_initialize)
			{
				RECT rc;
				GetClientRect(hWnd, &rc);
				FillRect(hDC, &rc, (HBRUSH)GetSysColorBrush(COLOR_3DFACE));
			}
			EndPaint(hWnd, &ps);
		}
		return 0;
	case WM_SIZE:
		{
			if(TRUE == IsIconic(mcs.hMain)) return 0;
			if(false == on_initialize && FALSE == IsIconic(mcs.hMain))
			{
				if(FALSE == IsZoomed(mcs.hMain))
				{
					GetWindowRect(mcs.hMain, &mcs.window_ini_size);
					mcs.window_maximamed = false;
				}
				else
				{
					mcs.window_maximamed = true;
				}
			}


			RECT rc;
			SIZE sz = {LOWORD(lParam), HIWORD(lParam)};
			if(0 == lParam)
			{
				GetClientRect(hWnd, &rc);
				sz.cx = GET_RECT_X(rc);
				sz.cy = GET_RECT_Y(rc);
			}
			const SIZE sz_all = sz;
			const int sep_rest = 50;
			SendMessage(mcs.hSep, UM_SEP_SETRANGE, sep_rest, sz.cx-sep_rest);
			RECT rc_st;
			GetClientRect(mcs.hStatus, &rc_st);
			sz.cy -= GET_RECT_Y(rc_st);

			GetWindowRect(mcs.hSep, &rc);
			const int sep_x = GET_RECT_X(rc);
			const int sep_y = GET_RECT_Y(rc);
			if(false == on_initialize)
			{
				if(sz.cx-sep_rest < sep_x)
				{
					SET_WND_POSIZE(mcs.hSep, rc.left, rc.top, sz.cx-sep_rest, sep_y);
					return 0;
				}
				else if(sep_x < sep_rest)
				{
					SET_WND_POSIZE(mcs.hSep, rc.left, rc.top, sep_rest, sep_y);
					return 0;
				}
			}
			const int list_width = sz.cx - sep_x;
			sz.cx -= (list_width+SEP_WIDTH);
			mcs.listview_width = sep_x;

			int x,y,w,h;

			const int dir_btn_height = min(sz.cy/3, 50*4);
			const int enter_key_width = 64;
			const int copy_btn_height = 40;
			const int shuffle_btn_height = 36;
			const int go_btn_height = 64;

			//PictureView
			x = 1; y = 1;
			w = sz.cx-x*2;
			h = sz.cy-y*2-dir_btn_height;
			SET_WND_POSIZE(mcs.hPicView, x, y, w, h);

			//dirボタン
			x = 1;
			RECT rc_dir_btn = {x, sz.cy-dir_btn_height, sz.cx-x-enter_key_width, sz.cy-x-copy_btn_height};
			mcs.list_dir.resize(rc_dir_btn);

			//戻るボタン
			x = rc_dir_btn.right;
			y = rc_dir_btn.top;
			w = enter_key_width;
			h = GET_RECT_Y(rc_dir_btn) / 2;
			SET_WND_POSIZE(mcs.hBtn_back, x, y, w, h);

			//次へボタン
			x = rc_dir_btn.right;
			y = rc_dir_btn.top + h;
			SET_WND_POSIZE(mcs.hBtn_next, x, y, w, h);

			//コピーボタン
			x = 0; y += h;
			w = sz.cx;
			h = copy_btn_height;
			SET_WND_POSIZE(mcs.hBtn_copy, x, y, w, h);

			//セパレータ
			x = 0; y = -SEP_OFFSET;
			w = sep_x;
			h = sz_all.cy+SEP_OFFSET*2;
			SET_WND_POSIZE(mcs.hSep, x, y, w, h);

			//シャッフルボタン
			x = sz.cx+SEP_WIDTH; y = 0;
			w = list_width-SEP_WIDTH;
			h = shuffle_btn_height;
			SET_WND_POSIZE(mcs.hBtn_shuffle, x, y, w, h);

			//ファイルリスト
			x = sz.cx+SEP_WIDTH; y += h;
			w = list_width-SEP_WIDTH;
			h = sz.cy -go_btn_height -shuffle_btn_height;
			SET_WND_POSIZE(mcs.hList, x, y, w, h);

			//Goボタン
			y += h;
			h = go_btn_height;
			SET_WND_POSIZE(mcs.hBtn_go, x, y, w, h);


			SendMessage(mcs.hStatus, WM_SIZE, wParam, lParam);
			InvalidateRect(mcs.hList, NULL, TRUE);
		}
		return 0;
	case WM_CLOSE:
		//iniファイル書き込み
		IniFileSave(mcs.ini_filename.c_str());
		DestroyMenu(mcs.hMenu);
		DeleteObject(mcs.hFont);
		DestroyWindow(hWnd);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

/***********************************************************************************
	WinMain
***********************************************************************************/
int WINAPI WinMain(HINSTANCE hCurInst, HINSTANCE hPrevInst, LPSTR lpsCmdLine, int nCmdShow)
{
	setlocale(LC_ALL, "Japanese_Japan.932");//ロケールを日本語に設定（Windows用）

	// シャッフル用の乱数を初期化
	srand((unsigned int)time(NULL));

	//GDI+初期化
	ULONG_PTR gdiplusToken;
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	//カレントディレクトリを取得
	char current[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, current);
	mcs.current_dir = current;

	//exeファイルのあるディレクトリを取得
	char exe_dir[MAX_PATH+1];
	GetModuleFileName(GetModuleHandle(NULL), exe_dir, MAX_PATH);
	mcs.ini_filename = exe_dir;
	mcs.ini_filename.erase(mcs.ini_filename.rfind('\\'));
	mcs.exe_dir = mcs.ini_filename;
	mcs.ini_filename += "\\" PROGRAM_NAME ".ini";
	mcs.log_filename = mcs.exe_dir + "\\move&copy.log";

	//iniファイル読み込み
	IniFileLoad(mcs.ini_filename.c_str());

	MSG msg;
	HWND hWnd;

	WNDCLASSEX WndClass = {0};
		WndClass.cbSize = sizeof(WNDCLASSEX);
		WndClass.style = CS_HREDRAW | CS_VREDRAW;
		WndClass.hInstance = hCurInst;
		WndClass.lpszClassName = PROGRAM_NAME;
		WndClass.lpfnWndProc = MainProc;
		WndClass.lpszMenuName = NULL;
		WndClass.hIcon   = LoadIcon(NULL, IDI_APPLICATION);
		WndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
		WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		WndClass.hbrBackground = NULL;//(HBRUSH)GetSysColorBrush(COLOR_3DFACE);
	if(!RegisterClassEx(&WndClass)) return FALSE;

	hWnd = 	CreateWindow(	PROGRAM_NAME	//クラス名
						,	PROGRAM_NAME	//ウィンドウ名
						,	WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN	//ウィンドウスタイル
						,	mcs.window_ini_size.left, mcs.window_ini_size.top	//座標
						,	GET_RECT_X(mcs.window_ini_size), GET_RECT_Y(mcs.window_ini_size)	//幅・高さ
						,	NULL, NULL, hCurInst, NULL	//hParentWnd、hMenu、hInst、ウィンドウ作成データ
						);
	if(!hWnd) return FALSE;

	ShowWindow(hWnd, (false == mcs.window_maximamed)?nCmdShow:SW_SHOWMAXIMIZED);
	UpdateWindow(hWnd);

	BOOL bRet;
	while((bRet = GetMessage(&msg, NULL, 0, 0)) != 0)
	{
		if(-1 == bRet) break;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	//GDI+終了
	Gdiplus::GdiplusShutdown(gdiplusToken);

	return (int)msg.wParam;
}

