#include <fstream>
#include <sstream>
#include <windows.h>

#include "PathList.h"

#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

#include "main.h"
extern MyCommonStructure mcs;

///////////////////////////////////////////////////////////////

void Conexec(std::stringstream &stream, char *filename, char *CurrentDir);

void ToLower(std::string &str)
{
	for(std::string::iterator itr = str.begin(); itr != str.end(); itr++)
	{
		if('A' <= (*itr) && (*itr) <= 'Z') (*itr) += 0x20;
	}
	return;
}

//マルチバイト文字列をワイド文字列に変換
void widen(const std::string &src, std::wstring &dest)
{
	const int length = src.length()+1;
	wchar_t *wcs = new wchar_t[length];
	mbstowcs(wcs, src.c_str(), length);
	dest = wcs;
	delete []wcs;
}

//マルチバイト文字列をワイド文字列に変換
void multin(const std::wstring &src, std::string &dest)
{
	const int length = src.length()*2+1;	//マルチバイトに変換するので最大2倍になる
	char *mbs = new char[length];
	wcstombs(mbs, src.c_str(), length);
	dest = mbs;
	delete []mbs;
}

//パスのアイテム部分の名前を取得
//（ワイド文字列に変換してから\を探索する）
std::string GetItemName(const std::string &text)
{
	std::wstring wstr;
	std::string str;
	widen(text, wstr);
	wstr.erase(0, wstr.rfind(L'\\')+1);
	multin(wstr, str);
	if(true == str.empty()) str = text;	//空になったのなら，元のまま
	return str;
}


///////////////////////////////////////////////////////////////

/******************************************************************************
	画像ファイルを移動&コピー
	処理したファイル数を返す
******************************************************************************/
int PicturesList::go(const std::string &copy_directory, PicturesList_GoLog *pgl, std::ostream *logout)
{
	int file_count = 0;
	std::string filename, to_path;

	bool copy = true;
	if(false == copy_directory.empty() && FALSE == PathFileExists(copy_directory.c_str()))
	{
		//コピー先ディレクトリが無い場合
		copy = false;
		if(logout) (*logout) << "[not found copy directory]" << copy_directory << std::endl;
	}

	bool copy_success, move_success;
	bool copy_skip, move_skip;
	for(std::vector<PictureData>::iterator itr = list.begin(); itr != list.end(); itr++)
	{
		copy_success = false;
		move_success = false;
		copy_skip = false;
		move_skip = false;

		if(FALSE == PathFileExists(itr->path.c_str()))
		{
			//画像ファイルが無い場合
			if(logout) (*logout) << "[not found image file]" << itr->path << std::endl;
		}
		else
		{
			filename = GetItemName(itr->path);

			//コピー
			copy_skip = (false == itr->copy);
			if(true == copy && false == copy_skip)
			{
				to_path = copy_directory + "\\" + filename;
				if(TRUE == PathFileExists(to_path.c_str()))
				{
					//コピー先に既に存在する場合
					if(logout) (*logout) << "[not copy(existing)]" << itr->path << " -> " << to_path << std::endl;
				}
				else
				{
					//上書きしない
					if(0 != CopyFile(itr->path.c_str(), to_path.c_str(), TRUE))
					{
						//コピー成功時
						if(logout) (*logout) << "[copy]" << itr->path << " -> " << to_path << std::endl;
						copy_success = true;
					}
					else
					{
						//コピー失敗時
						if(logout) (*logout) << "[not copy]" << itr->path << " -> " << to_path << std::endl;
					}
				}
			}

			//移動
			if(false == itr->to_dir.empty())
			{
				if(FALSE == PathFileExists(itr->to_dir.c_str()))
				{
					//移動先ディレクトリが無い場合
					if(logout) (*logout) << "[not found move directory]" << itr->to_dir << std::endl;
				}
				else
				{
					to_path = itr->to_dir + "\\" + filename;
					if(TRUE == PathFileExists(to_path.c_str()))
					{
						//移動先に既に存在する場合
						if(logout) (*logout) << "[not move(existing)]" << itr->path << " -> " << to_path << std::endl;
					}
					else
					{
						if(0 != MoveFile(itr->path.c_str(), to_path.c_str()))
						{
							//移動成功時
							if(logout) (*logout) << "[move]" << itr->path << " -> " << to_path << std::endl;
							move_success = true;
						}
						else
						{
							//移動失敗時
							if(logout) (*logout) << "[not move]" << itr->path << " -> " << to_path << std::endl;
						}
					}
				}
			}
			else
			{
				move_skip = true;
			}
		}

		if(pgl)
		{
			if(true == copy_skip) pgl->copy_skip++; else if(true == copy_success) pgl->copy++; else pgl->copy_fail++;
			if(true == move_skip) pgl->move_skip++; else if(true == move_success) pgl->move++; else pgl->move_fail++;
		}
	}
	return file_count;
}

/******************************************************************************
	画像ファイルを追加
******************************************************************************/
void PicturesList::add(const std::string &path)
{
	list.push_back(path);
	index = 0;
}

/******************************************************************************
	ファイルを列挙
******************************************************************************/
void IPathList::enumerate(const char *directory,
			unsigned int flags,
			const char *ext_list[], int ext_list_count,
			const std::set<std::string> *blacklist)
{
	std::stringstream stream;
	std::string filename, ext;
	std::string::size_type pos;

	//オプションフラグ
	bool show_dir   = (0 != (flags & EF_DIRECTORYS));
	bool show_hiden = (0 != (flags & EF_HIDEN));
	bool show_sys   = (0 != (flags & EF_SYSTEM));

	//dirコマンドでファイルを列挙
	filename = "cmd /c dir ";
	filename += directory;
	filename += " /B";
	if(false == show_dir) filename += " /A-D";
	else				  filename += " /AD";
	if(false == show_hiden)	filename += "-H";
	if(false == show_sys)	filename += "-S";

	Conexec(stream, (char*)filename.c_str(), NULL);
	stream.seekp(0);

	int line = 0;
	for(;false == stream.eof() && true == stream.good();)
	{
		line++;
		std::getline(stream, filename);
		if(true == filename.empty()) continue;

		pos = filename.rfind('\r');
		if(std::string::npos != pos) filename.erase(pos);
		pos = filename.rfind('\n');
		if(std::string::npos != pos) filename.erase(pos);

		filename = "\\" + filename;
		filename = directory + filename;

		//指定した拡張子以外なら無視（ファイル列挙時のみ）
		if(false == show_dir && 0 < ext_list_count)
		{
			pos = filename.rfind(".");
			if(std::string::npos == pos) continue;
			ext = filename.substr(pos+1);
			ToLower(ext);

			std::string ext_filter;
			for(int i=0; i<ext_list_count; i++)
			{
				ext_filter = ext_list[i];
				ToLower(ext_filter);
				//ブラックリストに載っていないファイルのみ追加
				if(ext_filter == ext
				&& (NULL == blacklist || blacklist->end() == blacklist->find(filename)))
				{
					this->add(filename);
					break;
				}
			}
		}
		else
		{
			this->add(filename);
		}
	}
}


/******************************************************************************
	DirectoriesList
	ボタンのテキストなどを再構成
******************************************************************************/
void DirectoriesList::refresh()
{
	const int btn_count = this->list.size();
	if(0 == btn_count) return;

	int btn_line_count[4] = {0};	//1234の段，QWERの段，ASDFの段，ZXCVの段
	btn_line_count[0] = btn_count/4 +1;
	btn_line_count[1] = (btn_count -btn_line_count[0])/3 +1;
	btn_line_count[2] = (btn_count -btn_line_count[0] -btn_line_count[1])/2 +1;
	btn_line_count[3] = btn_count -btn_line_count[0] -btn_line_count[1] -btn_line_count[2];

	int max_count = max(btn_line_count[0], btn_line_count[1]);
		max_count = max(btn_line_count[2], max_count);
		max_count = max(btn_line_count[3], max_count);
	if(max_count < 1) max_count = 1;

	const char *key[4] = 
	{
		{"1234567890-^\\"},
		{"QWERTYUIOP@["},
		{"ASDFGHJKL;:]"},
		{"ZXCVBNM,./\\"}
	};

	std::string text;

	std::vector<DirectoryData>::iterator itr = list.begin();
	for(int line=0; line<4; line++)
	{
		for(int i=0; i<btn_line_count[line]; i++)
		{
			if(itr == list.end()) return;

/*			std::wstring wstr;
			widen(itr->path, wstr);
			wstr.erase(0, wstr.rfind(L"\\")+1);
			multin(wstr, text);*/
			text = GetItemName(itr->path);
//			text = "hoge";
//			text += 'A'+line*btn_line_count[line]+i;
			if(i < (int)strlen(key[line]))
			{
				text += "(&";
				text += key[line][i];
				text += ")";

				if('\\' == key[line][i])
				{
					//右上の\キー（VK_OEM_102）
					//右下の\キー（VK_OEM_5）
					itr->key = (0 == line) ? VK_OEM_5 : VK_OEM_102;
				}
				else if('-' == key[line][i]) {itr->key = VK_OEM_MINUS;}
				else if('^' == key[line][i]) {itr->key = VK_OEM_7;}

				else if('@' == key[line][i]) {itr->key = VK_OEM_3;}
				else if('[' == key[line][i]) {itr->key = VK_OEM_4;}

				else if(';' == key[line][i]) {itr->key = VK_OEM_PLUS;}
				else if(':' == key[line][i]) {itr->key = VK_OEM_1;}
				else if(']' == key[line][i]) {itr->key = VK_OEM_6;}

				else if(',' == key[line][i]) {itr->key = VK_OEM_COMMA;}
				else if('.' == key[line][i]) {itr->key = VK_OEM_PERIOD;}
				else if('/' == key[line][i]) {itr->key = VK_OEM_2;}
				else
				{
					itr->key = key[line][i];
				}
			}
			SetWindowText(itr->hWnd, text.c_str());

		//	ShowWindow(itr->hWnd, SW_SHOW);

			itr++;
		}
	}
}


/******************************************************************************
	DirectoriesList
	ボタン再配置
******************************************************************************/
void DirectoriesList::resize(RECT &rc)
{
	const int btn_count = this->list.size();
	if(0 == btn_count) return;

	//キーボード風にレイアウト
	int btn_line_count[4] = {0};	//1234の段，QWERの段，ASDFの段，ZXCVの段
	btn_line_count[0] = btn_count/4 +1;
	btn_line_count[1] = (btn_count -btn_line_count[0])/3 +1;
	btn_line_count[2] = (btn_count -btn_line_count[0] -btn_line_count[1])/2 +1;
	btn_line_count[3] = btn_count -btn_line_count[0] -btn_line_count[1] -btn_line_count[2];

	int max_count = max(btn_line_count[0], btn_line_count[1]);
		max_count = max(btn_line_count[2], max_count);
		max_count = max(btn_line_count[3], max_count);
	if(max_count < 1) max_count = 1;

	const double offset_gain = 1/3.0;
	const int w = min((int)((double)(rc.right-rc.left) / (max_count + offset_gain * 3)), (rc.right-rc.left)/8);
	const int h = (rc.bottom-rc.top)/4;



	HDWP hDWP = BeginDeferWindowPos(btn_count);

	std::vector<DirectoryData>::iterator itr = list.begin();
	for(int line=0; line<4; line++)
	{
		for(int i=0; i<btn_line_count[line]; i++)
		{
			if(itr == list.end())
			{
				EndDeferWindowPos(hDWP);
				return;
			}

			hDWP = DeferWindowPos(hDWP, itr->hWnd, NULL, 
				rc.left+w*i + (int)(line*w*offset_gain),
				rc.top+line*h,
				w, h, SWP_NOZORDER);

			itr++;
		}
	}

	EndDeferWindowPos(hDWP);
}


/******************************************************************************
	指定キーをショートカットキーとして持つボタンを探す
******************************************************************************/
const DirectoryData *DirectoriesList::find(const unsigned char shortcut_key) const
{
	for(std::vector<DirectoryData>::const_iterator itr = list.begin(); itr != list.end(); itr++)
	{
		if(shortcut_key == itr->key)
		{
			const DirectoryData *p = &(*itr);
			return p;
		}
	}
	return NULL;
}

/******************************************************************************
	ウィンドウハンドルでボタンを探す
******************************************************************************/
const DirectoryData *DirectoriesList::find(const HWND hWnd) const
{
	for(std::vector<DirectoryData>::const_iterator itr = list.begin(); itr != list.end(); itr++)
	{
		if(hWnd == itr->hWnd)
		{
			const DirectoryData *p = &(*itr);
			return p;
		}
	}
	return NULL;
}
