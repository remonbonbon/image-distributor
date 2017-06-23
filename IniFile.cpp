#include "main.h"
#include <sstream>

extern MyCommonStructure mcs;

#define MAX_INI_BUF		1024
//#define INI_FILE_NAME	TEXT("./") PROGRAM_NAME TEXT(".ini")


/***********************************************************************************
	iniファイルロード
***********************************************************************************/
void IniFileLoad(const char *ini_file_name)
{
	//iniファイルが無ければ作成
	HANDLE hIniFile = CreateFile(ini_file_name, 0, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);CloseHandle(hIniFile);

	char buf[MAX_INI_BUF], key_name[128];


	//ウィンドウサイズ
	mcs.window_ini_size.left  = GetPrivateProfileInt("Window", "pos_x", 0, ini_file_name);
	mcs.window_ini_size.top   = GetPrivateProfileInt("Window", "pos_y", 0, ini_file_name);
	mcs.window_ini_size.right = GetPrivateProfileInt("Window", "width", 640, ini_file_name) + mcs.window_ini_size.left;
	mcs.window_ini_size.bottom= GetPrivateProfileInt("Window", "height",480, ini_file_name) + mcs.window_ini_size.top;

	if(mcs.window_ini_size.right < mcs.window_ini_size.left) mcs.window_ini_size.right = mcs.window_ini_size.left+640;
	if(mcs.window_ini_size.bottom < mcs.window_ini_size.top) mcs.window_ini_size.bottom = mcs.window_ini_size.top+480;

	mcs.listview_width = GetPrivateProfileInt("Window", "listview_width", 320, ini_file_name);
	mcs.window_maximamed = (0 != GetPrivateProfileInt("Window", "maximamed", 0, ini_file_name));


	//追加ディレクトリ
	for(int i=0; i<64; i++)
	{
		sprintf(key_name, "add%d", i);
		GetPrivateProfileString("AddDirectory", key_name, "", buf, MAX_PATH, ini_file_name);
		if(0 < strlen(buf))
		{
			mcs.add_dir.push_back(buf);
		}
		else break;
	}

	//コピーディレクトリ
	GetPrivateProfileString("CopyDirectory", "path", "", buf, MAX_PATH, ini_file_name);	mcs.copy_dir = buf;

	std::stringstream ss;
	GetPrivateProfileString("Listview", "dir_bk_color", "0xDDDDDD", buf, MAX_PATH, ini_file_name);
	ss << buf << std::ends;
	ss >> std::hex >> mcs.LV_dir_bk_color;
	GetPrivateProfileString("Listview", "copy_font_color", "0x0000FF", buf, MAX_PATH, ini_file_name);
	ss.str("");
	ss << buf << std::ends;
	ss >> std::hex >> mcs.LV_copy_font_color;

	GetPrivateProfileString("Listview", "column_width_copy", "40", buf, MAX_PATH, ini_file_name);	mcs.LV_width_copy = atoi(buf);
	GetPrivateProfileString("Listview", "column_width_img", "250", buf, MAX_PATH, ini_file_name);	mcs.LV_width_img = atoi(buf);
	GetPrivateProfileString("Listview", "column_width_dir", "250", buf, MAX_PATH, ini_file_name);	mcs.LV_width_dir = atoi(buf);

	// シャッフルするかどうか
	// 0    : シャッフルしない
	// 0以外: シャッフルする
	GetPrivateProfileString("Listview", "shuffle_list", "0", buf, MAX_PATH, ini_file_name);			mcs.shuffle_list_pic = (0!=atoi(buf));

	// シャッフル後、リストアップするファイル数の上限
	// 0以下  : 上限無し
	// 0より大: リストアップの上限数
	GetPrivateProfileString("Listview", "filenum_limit", "-1", buf, MAX_PATH, ini_file_name);
	mcs.listup_filenum_limit = atoi(buf);
}

/***********************************************************************************
	iniファイルセーブ
***********************************************************************************/
void IniFileSave(const char *ini_file_name)
{
	char buf[MAX_INI_BUF], key_name[128];

	sprintf(buf, "%d", (int)mcs.window_ini_size.left);	WritePrivateProfileString("Window", "pos_x", buf, ini_file_name);
	sprintf(buf, "%d", (int)mcs.window_ini_size.top);	WritePrivateProfileString("Window", "pos_y", buf, ini_file_name);
	sprintf(buf, "%d", (int)GET_RECT_X(mcs.window_ini_size));	WritePrivateProfileString("Window", "width", buf, ini_file_name);
	sprintf(buf, "%d", (int)GET_RECT_Y(mcs.window_ini_size));	WritePrivateProfileString("Window", "height", buf, ini_file_name);

	sprintf(buf, "%d", mcs.listview_width);	WritePrivateProfileString("Window", "listview_width", buf, ini_file_name);
	sprintf(buf, "%d", true==mcs.window_maximamed?1:0);	WritePrivateProfileString("Window", "maximamed", buf, ini_file_name);

	int i=0;
	WritePrivateProfileString("AddDirectory", "add0", "", ini_file_name);	//キー名認知用
	for(std::vector<std::string>::iterator itr = mcs.add_dir.begin(); itr != mcs.add_dir.end(); itr++)
	{
		sprintf(key_name, "add%d", i);
		WritePrivateProfileString("AddDirectory", key_name, itr->c_str(), ini_file_name);
		i++;
	}

	WritePrivateProfileString("CopyDirectory", "path", mcs.copy_dir.c_str(), ini_file_name);

	sprintf(buf, "0x%06X", mcs.LV_dir_bk_color);	WritePrivateProfileString("Listview", "dir_bk_color", buf, ini_file_name);
	sprintf(buf, "0x%06X", mcs.LV_copy_font_color);	WritePrivateProfileString("Listview", "copy_font_color", buf, ini_file_name);

	sprintf(buf, "%d", ListView_GetColumnWidth(mcs.hList, 0));	WritePrivateProfileString("Listview", "column_width_copy", buf, ini_file_name);
	sprintf(buf, "%d", ListView_GetColumnWidth(mcs.hList, 1));	WritePrivateProfileString("Listview", "column_width_img", buf, ini_file_name);
	sprintf(buf, "%d", ListView_GetColumnWidth(mcs.hList, 2));	WritePrivateProfileString("Listview", "column_width_dir", buf, ini_file_name);	

	WritePrivateProfileString("Listview", "shuffle_list", (mcs.shuffle_list_pic?"1":"0"), ini_file_name);	

	sprintf(buf, "%d", mcs.listup_filenum_limit);
	WritePrivateProfileString("Listview", "filenum_limit", buf, ini_file_name);
}
