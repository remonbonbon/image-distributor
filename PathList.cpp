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

//�}���`�o�C�g����������C�h������ɕϊ�
void widen(const std::string &src, std::wstring &dest)
{
	const int length = src.length()+1;
	wchar_t *wcs = new wchar_t[length];
	mbstowcs(wcs, src.c_str(), length);
	dest = wcs;
	delete []wcs;
}

//�}���`�o�C�g����������C�h������ɕϊ�
void multin(const std::wstring &src, std::string &dest)
{
	const int length = src.length()*2+1;	//�}���`�o�C�g�ɕϊ�����̂ōő�2�{�ɂȂ�
	char *mbs = new char[length];
	wcstombs(mbs, src.c_str(), length);
	dest = mbs;
	delete []mbs;
}

//�p�X�̃A�C�e�������̖��O���擾
//�i���C�h������ɕϊ����Ă���\��T������j
std::string GetItemName(const std::string &text)
{
	std::wstring wstr;
	std::string str;
	widen(text, wstr);
	wstr.erase(0, wstr.rfind(L'\\')+1);
	multin(wstr, str);
	if(true == str.empty()) str = text;	//��ɂȂ����̂Ȃ�C���̂܂�
	return str;
}


///////////////////////////////////////////////////////////////

/******************************************************************************
	�摜�t�@�C�����ړ�&�R�s�[
	���������t�@�C������Ԃ�
******************************************************************************/
int PicturesList::go(const std::string &copy_directory, PicturesList_GoLog *pgl, std::ostream *logout)
{
	int file_count = 0;
	std::string filename, to_path;

	bool copy = true;
	if(false == copy_directory.empty() && FALSE == PathFileExists(copy_directory.c_str()))
	{
		//�R�s�[��f�B���N�g���������ꍇ
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
			//�摜�t�@�C���������ꍇ
			if(logout) (*logout) << "[not found image file]" << itr->path << std::endl;
		}
		else
		{
			filename = GetItemName(itr->path);

			//�R�s�[
			copy_skip = (false == itr->copy);
			if(true == copy && false == copy_skip)
			{
				to_path = copy_directory + "\\" + filename;
				if(TRUE == PathFileExists(to_path.c_str()))
				{
					//�R�s�[��Ɋ��ɑ��݂���ꍇ
					if(logout) (*logout) << "[not copy(existing)]" << itr->path << " -> " << to_path << std::endl;
				}
				else
				{
					//�㏑�����Ȃ�
					if(0 != CopyFile(itr->path.c_str(), to_path.c_str(), TRUE))
					{
						//�R�s�[������
						if(logout) (*logout) << "[copy]" << itr->path << " -> " << to_path << std::endl;
						copy_success = true;
					}
					else
					{
						//�R�s�[���s��
						if(logout) (*logout) << "[not copy]" << itr->path << " -> " << to_path << std::endl;
					}
				}
			}

			//�ړ�
			if(false == itr->to_dir.empty())
			{
				if(FALSE == PathFileExists(itr->to_dir.c_str()))
				{
					//�ړ���f�B���N�g���������ꍇ
					if(logout) (*logout) << "[not found move directory]" << itr->to_dir << std::endl;
				}
				else
				{
					to_path = itr->to_dir + "\\" + filename;
					if(TRUE == PathFileExists(to_path.c_str()))
					{
						//�ړ���Ɋ��ɑ��݂���ꍇ
						if(logout) (*logout) << "[not move(existing)]" << itr->path << " -> " << to_path << std::endl;
					}
					else
					{
						if(0 != MoveFile(itr->path.c_str(), to_path.c_str()))
						{
							//�ړ�������
							if(logout) (*logout) << "[move]" << itr->path << " -> " << to_path << std::endl;
							move_success = true;
						}
						else
						{
							//�ړ����s��
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
	�摜�t�@�C����ǉ�
******************************************************************************/
void PicturesList::add(const std::string &path)
{
	list.push_back(path);
	index = 0;
}

/******************************************************************************
	�t�@�C�����
******************************************************************************/
void IPathList::enumerate(const char *directory,
			unsigned int flags,
			const char *ext_list[], int ext_list_count,
			const std::set<std::string> *blacklist)
{
	std::stringstream stream;
	std::string filename, ext;
	std::string::size_type pos;

	//�I�v�V�����t���O
	bool show_dir   = (0 != (flags & EF_DIRECTORYS));
	bool show_hiden = (0 != (flags & EF_HIDEN));
	bool show_sys   = (0 != (flags & EF_SYSTEM));

	//dir�R�}���h�Ńt�@�C�����
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

		//�w�肵���g���q�ȊO�Ȃ疳���i�t�@�C���񋓎��̂݁j
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
				//�u���b�N���X�g�ɍڂ��Ă��Ȃ��t�@�C���̂ݒǉ�
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
	�{�^���̃e�L�X�g�Ȃǂ��č\��
******************************************************************************/
void DirectoriesList::refresh()
{
	const int btn_count = this->list.size();
	if(0 == btn_count) return;

	int btn_line_count[4] = {0};	//1234�̒i�CQWER�̒i�CASDF�̒i�CZXCV�̒i
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
					//�E���\�L�[�iVK_OEM_102�j
					//�E����\�L�[�iVK_OEM_5�j
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
	�{�^���Ĕz�u
******************************************************************************/
void DirectoriesList::resize(RECT &rc)
{
	const int btn_count = this->list.size();
	if(0 == btn_count) return;

	//�L�[�{�[�h���Ƀ��C�A�E�g
	int btn_line_count[4] = {0};	//1234�̒i�CQWER�̒i�CASDF�̒i�CZXCV�̒i
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
	�w��L�[���V���[�g�J�b�g�L�[�Ƃ��Ď��{�^����T��
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
	�E�B���h�E�n���h���Ń{�^����T��
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
