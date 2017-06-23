#ifndef _COMMON_HEADER_
#define _COMMON_HEADER_

///////////////////////////////////////////////////////////////////////////////

//#pragma warning(disable: 4996)	//古い関数使用の警告を無視（sprintfなど）

///////////////////////////////////////////////////////////////////////////////

#include <windows.h>
//#include <tchar.h>

#include <commctrl.h>
#pragma comment(lib, "comctl32.lib")

//#include <shlwapi.h>
//#pragma comment(lib, "shlwapi.lib")

#include <fstream>
#include <sstream>
#include <time.h>

/*rc用
#include <winuser.h>
#include <winver.h>
#include <commctrl.h>
*/

#include "resource.h"
#include "Timer.h"
#include "PathList.h"
#include "PictureView.h"
#include "Separator.h"
#include "FileInfo.h"

///////////////////////////////////////////////////////////////////////////////

LRESULT CALLBACK MainProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void IniFileLoad(const char *ini_file_name);
void IniFileSave(const char *ini_file_name);

///////////////////////////////////////////////////////////////////////////////
//RECT構造体の幅、高さを取得
#define GET_RECT_X(rc) (rc.right - rc.left)
#define GET_RECT_Y(rc) (rc.bottom - rc.top)

//ウィンドウ位置、サイズを変更
#define SET_WND_POSIZE(hWnd, x, y, w, h)	SetWindowPos(hWnd, NULL, x, y, w, h, SWP_NOZORDER)

//コントロールにGUIデフォルトフォントを設定
#define SET_GUI_FONT(hWnd)	SendMessage(hWnd, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE)

//ステータスバーにテキストを表示
#define SET_STATUS_TEXT(parts, text)		SendMessage(mcs.hStatus, SB_SETTEXT, parts, (LPARAM)text)

//WM_NOTIFYを解読
//#define WM_NOTIFY_ID(lParam)	(((LPNMHDR)lParam)->idFrom)
//#define WM_NOTIFY_HWND(lParam)	(((LPNMHDR)lParam)->hwndFrom)
//#define WM_NOTIFY_CODE(lParam)	(((LPNMHDR)lParam)->code)

//valをmin～maxの間にする
//#define LIMIT(val, min, max) ((val < min) ? min : ( (val > max) ? max : val))

//お手軽に、アンチエイリアスフォントを作成
//#define QUICK_CREATE_FONT(hight, name) (CreateFont(hight, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, DEFAULT_PITCH | FF_DONTCARE, name))

///////////////////////////////////////////////////////////////////////////////

#define MENU_ID_COPY_DIR	1
#define MENU_ID_SEPARATOR	2
#define MENU_ID_DIR_START	3

///////////////////////////////////////////////////////////////////////////////

struct MyCommonStructure
{
	HWND hMain;
	HWND hStatus;
	HWND hPicView;
	HWND hBtn_next;
	HWND hBtn_back;
	HWND hBtn_copy;
	HWND hBtn_shuffle;
	HWND hList;
	HWND hSep;
	HWND hBtn_go;

	PicturesList list_pic;
	DirectoriesList list_dir;

	std::string copy_dir;
	std::vector<std::string> add_dir;

	std::string current_dir;
	std::string exe_dir;
	std::string log_filename;
	std::string ini_filename;
	RECT window_ini_size;
	int listview_width;
	bool window_maximamed;
	bool shuffle_list_pic;
	int listup_filenum_limit;

	HMENU hMenu;
	HFONT hFont;

	COLORREF LV_dir_bk_color;
	COLORREF LV_copy_font_color;

	int LV_width_copy;
	int LV_width_img;
	int LV_width_dir;
};

///////////////////////////////////////////////////////////////////////////////

#endif
