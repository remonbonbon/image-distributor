#include "FileInfo.h"

//////////////////////////////////////////////////////

//マルチバイト文字列をワイド文字列に変換
static void multin(const std::wstring &src, std::string &dest)
{
	const int length = src.length()*2+1;	//マルチバイトに変換するので最大2倍になる
	char *mbs = new char[length];
	wcstombs(mbs, src.c_str(), length);
	dest = mbs;
	delete []mbs;
}

//////////////////////////////////////////////////////

//ファイル情報をセット
void FileInfo::set_file_info(const std::string &path)
{
	HANDLE hFile = CreateFile(path.c_str(),
		GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(INVALID_HANDLE_VALUE != hFile)
	{
		FILETIME ftFileTime , ftLocalFileTime;
		GetFileTime(hFile, NULL, NULL, &ftFileTime);
		FileTimeToLocalFileTime(&ftFileTime , &ftLocalFileTime);
		FileTimeToSystemTime(&ftLocalFileTime , &last_update);

		GetFileSizeEx(hFile, &size);

		CloseHandle(hFile);
	}
}

//最終更新日時を文字列で取得
const std::string FileInfo::last_update_time() {return get_time_string(last_update);}
//現在日時を文字列で取得
const std::string get_current_time_string()
{
	SYSTEMTIME st;
	GetLocalTime(&st);
	return get_time_string(st);
}
//日時を文字列で取得
const std::string get_time_string(SYSTEMTIME st)
{
	TCHAR strFileTime[256];
	LPTSTR day = NULL;
	switch (st.wDayOfWeek)
	{
	case 0:day = "日";break;
	case 1:day = "月";break;
	case 2:day = "火";break;
	case 3:day = "水";break;
	case 4:day = "木";break;
	case 5:day = "金";break;
	case 6:day = "土";break;
	}
	wsprintf(strFileTime, TEXT("%04d/%02d/%02d(%s) %02d:%02d:%02d"),
		st.wYear, st.wMonth, st.wDay,
		day,
		st.wHour, st.wMinute, st.wSecond);
	return strFileTime;
}

//////////////////////////////////////////////////////

//画像情報をセット
void ImageInfo::set_image_info(Gdiplus::Image &img)
{
	width = img.GetWidth();
	height = img.GetHeight();

	img.GetRawFormat(&format);
	WCHAR strGuid[39];
	StringFromGUID2(format, strGuid, 39);
	multin(strGuid, str_guid);

	GUID guid;
	img.GetFrameDimensionsList(&guid, 1);	// 最初のディメンションのGUIDを取得
	gif_frame = img.GetFrameCount(&guid);	// コマ数を取得

	Gdiplus::ColorPalette cp[100];
	img.GetPalette(cp, 100);
}

//画像タイプの拡張子（小文字）を取得
const char *ImageInfo::format_ext()
{
	if(Gdiplus::ImageFormatBMP		== format) return "bmp";
	if(Gdiplus::ImageFormatEMF		== format) return "emf";
	if(Gdiplus::ImageFormatEXIF		== format) return "Exif";
	if(Gdiplus::ImageFormatGIF		== format) return "gif";
	if(Gdiplus::ImageFormatIcon		== format) return "ico";
	if(Gdiplus::ImageFormatJPEG		== format) return "jpg";
	if(Gdiplus::ImageFormatMemoryBMP== format) return "bmp";
	if(Gdiplus::ImageFormatPNG		== format) return "png";
	if(Gdiplus::ImageFormatTIFF		== format) return "tif";
	if(Gdiplus::ImageFormatUndefined== format) return "";
	if(Gdiplus::ImageFormatWMF		== format) return "wmf";
	return "";
}
//画像タイプの拡張子（大文字）を取得
const char *ImageInfo::format_EXT()
{
	if(Gdiplus::ImageFormatBMP		== format) return "BMP";
	if(Gdiplus::ImageFormatEMF		== format) return "EMF";
	if(Gdiplus::ImageFormatEXIF		== format) return "EXIF";
	if(Gdiplus::ImageFormatGIF		== format) return "GIF";
	if(Gdiplus::ImageFormatIcon		== format) return "ICO";
	if(Gdiplus::ImageFormatJPEG		== format) return "JPEG";
	if(Gdiplus::ImageFormatMemoryBMP== format) return "BMP";
	if(Gdiplus::ImageFormatPNG		== format) return "PNG";
	if(Gdiplus::ImageFormatTIFF		== format) return "TIFF";
	if(Gdiplus::ImageFormatUndefined== format) return "";
	if(Gdiplus::ImageFormatWMF		== format) return "WMF";
	return "";
}
//画像タイプの説明を取得
const char *ImageInfo::format_desc()
{
	if(Gdiplus::ImageFormatBMP		== format) return "Windows bitmap";
	if(Gdiplus::ImageFormatEMF		== format) return "Enhanced Metafile";
	if(Gdiplus::ImageFormatEXIF		== format) return "Exchangeable image file";
	if(Gdiplus::ImageFormatGIF		== format) return "Graphics Interchange Format";
	if(Gdiplus::ImageFormatIcon		== format) return "Window icon";
	if(Gdiplus::ImageFormatJPEG		== format) return "Joint Photographic Experts Group";
	if(Gdiplus::ImageFormatMemoryBMP== format) return "Memory bitmap";
	if(Gdiplus::ImageFormatPNG		== format) return "Portable Network Graphics";
	if(Gdiplus::ImageFormatTIFF		== format) return "Tagged Image File Format";
	if(Gdiplus::ImageFormatUndefined== format) return "";
	if(Gdiplus::ImageFormatWMF		== format) return "Windows Metafile";
	return "";
}

