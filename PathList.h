#ifndef _PATH_LIST_H_
#define _PATH_LIST_H_

////////////////////////////////////////////////////////

#include <string>
#include <set>
#include <vector>
#include <algorithm>

////////////////////////////////////////////////////////

void widen(const std::string &src, std::wstring &dest);
void multin(const std::wstring &src, std::string &dest);
std::string GetItemName(const std::string &text);

////////////////////////////////////////////////////////

//enumerate()��flags
#define EF_FILES		0x0000	//�t�@�C�����
#define EF_DIRECTORYS	0x0001	//�f�B���N�g�����
#define EF_HIDEN		0x0002	//�B���t�@�C��or�f�B���N�g������
#define EF_SYSTEM		0x0004	//�V�X�e���t�@�C��or�f�B���N�g������

#define EF_ALL_FILES		(EF_FILES|EF_HIDEN|EF_SYSTEM)
#define EF_ALL_DIRECTORYS	(EF_DIRECTORYS|EF_HIDEN|EF_SYSTEM)

////////////////////////////////////////////////////////

//�p�X���X�g�̗v�f�̊��N���X
class base_PathData
{
public:
	std::string path;	//�t�@�C��or�f�B���N�g���p�X

	base_PathData() {}
	base_PathData(const std::string &Path) : path(Path) {}

	bool operator< (const base_PathData &a) const {return path < a.path;}
};

//���X�g�̃C���^�[�t�F�[�X
class IPathList
{
private:
public:
	IPathList() {}
	~IPathList() {}
	virtual void add(const std::string &path) = 0;	//�p�X�����X�g�ɒǉ�����
	virtual void enumerate(const char *directory,
				unsigned int flags = EF_FILES,
				const char *ext_list[] = NULL, int ext_list_count = 0,
				const std::set<std::string> *blacklist = NULL);
};

////////////////////////////////////////////////////////
////////////////////////////////////////////////////////

struct PicturesList_GoLog
{
	int copy;
	int copy_fail;
	int copy_skip;
	int move;
	int move_fail;
	int move_skip;

	PicturesList_GoLog() : copy(0), copy_fail(0), copy_skip(0), move(0), move_fail(0), move_skip(0) {}
};

//�摜�f�[�^
class PictureData : public base_PathData
{
public:
	std::string to_dir;	//�ړ���f�B���N�g��
	bool copy;			//�w��f�B���N�g���ɃR�s�[������Ƃ�true

	PictureData() : copy(false) {}
	PictureData(const std::string &Path) : base_PathData(Path), copy(false) {}
};

//�摜���X�g
class PicturesList : public IPathList
{
private:
	std::vector<PictureData> list;
	int index;	//�i���ݕ\�����Ă�j�摜�C���f�b�N�X
public:
	PicturesList() : index(0) {}
	void clear()
	{
		list.clear();
		index = 0;
	}

	std::vector<PictureData>::const_iterator get_begin(){return list.begin();}
	std::vector<PictureData>::const_iterator get_end(){return list.end();}

	void add(const std::string &path);
	int count() const {return (int)list.size();}

	PictureData *current() {return (0 == count()) ? NULL : &(list[index]);}
	PictureData *get(int Index){return (0 <= Index && Index < count()) ? &(list[Index]) : NULL;}
	int current_index() {return index;}
	const char *current_img_name() {return (0 == count()) ? NULL : current()->path.c_str();}
	void next()	{if(count() <= ++index) index = 0;}
	void back()	{if(--index < 0) index = count()-1;}
	void set_index(int Index) {if(0 <= Index && Index < count()) index = Index;}
	int go(const std::string &copy_directory, PicturesList_GoLog *pgl = NULL, std::ostream *logout = NULL);

	void shuffle() {std::random_shuffle(list.begin(), list.end());}

	// �擪����size�̃��X�g�ɂ���
	void cutoff(int size)
	{
		if(0 < size && size < (int)list.size())
		{
			std::vector<PictureData>::iterator s = list.begin() + size;
			std::vector<PictureData>::iterator e = list.end();
			list.erase(s, e);
		}
	}
};

////////////////////////////////////////////////////////

//�f�B���N�g���f�[�^
class DirectoryData : public base_PathData
{
public:
	int count;	//�ړ������t�@�C����
	HWND hWnd;	//�{�^����HWND
	unsigned char key;	//�V���[�g�J�b�g�L�[

	DirectoryData() : count(0), hWnd(NULL) {}
	DirectoryData(const std::string &Path) : base_PathData(Path), count(0), hWnd(NULL) {}
};

//�f�B���N�g�����X�g
class DirectoriesList : public IPathList
{
private:
	std::vector<DirectoryData> list;
	HWND hParent;	//�e�E�B���h�E
public:
	DirectoriesList() : hParent(NULL) {}
	~DirectoriesList() {clear();}

	void clear()
	{
		for(std::vector<DirectoryData>::iterator itr = list.begin(); itr != list.end(); itr++)
		{
			DestroyWindow(itr->hWnd);
		}
		list.clear();
	}

	std::vector<DirectoryData>::const_iterator get_begin(){return list.begin();}
	std::vector<DirectoryData>::const_iterator get_end(){return list.end();}


	void set_parent(HWND hWnd) {this->hParent = hWnd;}
	int count() const {return (int)list.size();}

	void add(const std::string &path)
	{
		list.push_back(path);

		HWND &hWnd = list.back().hWnd;
		hWnd = CreateWindow("BUTTON", "",
				WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON | BS_MULTILINE,
				0, 0, 0, 0, hParent, (HMENU)LOWORD((HMENU)(this)), GetModuleHandle(NULL), NULL);
		SendMessage(hWnd, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);
	}
	void enumerate(const char *directory) {IPathList::enumerate(directory, EF_DIRECTORYS);}

	void refresh();
	void resize(RECT &rc);
	const DirectoryData *find(const unsigned char shortcut_key) const;
	const DirectoryData *find(const HWND hWnd) const;
};

////////////////////////////////////////////////////////

#endif
