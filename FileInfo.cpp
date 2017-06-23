#include "FileInfo.h"

//////////////////////////////////////////////////////

//�}���`�o�C�g����������C�h������ɕϊ�
static void multin(const std::wstring &src, std::string &dest)
{
	const int length = src.length()*2+1;	//�}���`�o�C�g�ɕϊ�����̂ōő�2�{�ɂȂ�
	char *mbs = new char[length];
	wcstombs(mbs, src.c_str(), length);
	dest = mbs;
	delete []mbs;
}

//////////////////////////////////////////////////////

//�t�@�C�������Z�b�g
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

//�ŏI�X�V�����𕶎���Ŏ擾
const std::string FileInfo::last_update_time() {return get_time_string(last_update);}
//���ݓ����𕶎���Ŏ擾
const std::string get_current_time_string()
{
	SYSTEMTIME st;
	GetLocalTime(&st);
	return get_time_string(st);
}
//�����𕶎���Ŏ擾
const std::string get_time_string(SYSTEMTIME st)
{
	TCHAR strFileTime[256];
	LPTSTR day = NULL;
	switch (st.wDayOfWeek)
	{
	case 0:day = "��";break;
	case 1:day = "��";break;
	case 2:day = "��";break;
	case 3:day = "��";break;
	case 4:day = "��";break;
	case 5:day = "��";break;
	case 6:day = "�y";break;
	}
	wsprintf(strFileTime, TEXT("%04d/%02d/%02d(%s) %02d:%02d:%02d"),
		st.wYear, st.wMonth, st.wDay,
		day,
		st.wHour, st.wMinute, st.wSecond);
	return strFileTime;
}

//////////////////////////////////////////////////////

//�摜�����Z�b�g
void ImageInfo::set_image_info(Gdiplus::Image &img)
{
	width = img.GetWidth();
	height = img.GetHeight();

	img.GetRawFormat(&format);
	WCHAR strGuid[39];
	StringFromGUID2(format, strGuid, 39);
	multin(strGuid, str_guid);

	GUID guid;
	img.GetFrameDimensionsList(&guid, 1);	// �ŏ��̃f�B�����V������GUID���擾
	gif_frame = img.GetFrameCount(&guid);	// �R�}�����擾

	Gdiplus::ColorPalette cp[100];
	img.GetPalette(cp, 100);
}

//�摜�^�C�v�̊g���q�i�������j���擾
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
//�摜�^�C�v�̊g���q�i�啶���j���擾
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
//�摜�^�C�v�̐������擾
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

