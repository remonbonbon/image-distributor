#ifndef _IMAGE_INFO_
#define _IMAGE_INFO_

/////////////////////////////////////////

#include <windows.h>
#include <gdiplus.h>
#include <string>

/////////////////////////////////////////

const std::string get_time_string(SYSTEMTIME st);
const std::string get_current_time_string();

/////////////////////////////////////////

//��{�I�ȃt�@�C�����
class FileInfo
{
public:
	SYSTEMTIME last_update;	//�X�V����
	LARGE_INTEGER size;		//�t�@�C���T�C�Y [byte]

	FileInfo()
	{
		ZeroMemory(&last_update, sizeof(SYSTEMTIME));
		ZeroMemory(&size, sizeof(LARGE_INTEGER));
	}
	FileInfo(const std::string &file_path) {set_file_info(file_path);}

	void set_file_info(const std::string &path);
	const std::string last_update_time();

};

/////////////////////////////////////////

//�摜�t�@�C�����
class ImageInfo : public FileInfo
{
public:
	UINT width, height;		//���C����
	GUID format;			//jpg�Cpng�Ȃǂ̃t�H�[�}�b�g
	std::string str_guid;	//GUID�𕶎���ɂ�������
	UINT gif_frame;			//gif�A�j���̃t���[����

	ImageInfo() : width(0), height(0), gif_frame(0) {ZeroMemory(&format, sizeof(GUID));}
	ImageInfo(Gdiplus::Image &img) {set_image_info(img);}
	ImageInfo(const std::string &file_path, Gdiplus::Image &img) : FileInfo(file_path) {set_image_info(img);}

	void set_image_info(Gdiplus::Image &img);
	const char *format_ext();
	const char *format_EXT();
	const char *format_desc();
};

/////////////////////////////////////////

#endif
