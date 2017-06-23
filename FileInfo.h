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

//基本的なファイル情報
class FileInfo
{
public:
	SYSTEMTIME last_update;	//更新日時
	LARGE_INTEGER size;		//ファイルサイズ [byte]

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

//画像ファイル情報
class ImageInfo : public FileInfo
{
public:
	UINT width, height;		//幅，高さ
	GUID format;			//jpg，pngなどのフォーマット
	std::string str_guid;	//GUIDを文字列にしたもの
	UINT gif_frame;			//gifアニメのフレーム数

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
