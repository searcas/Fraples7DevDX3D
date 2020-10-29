#define WIN_TAR
#include "Surface.h"
#include <sstream>
#include <algorithm>
namespace Gdiplus
{
	using std::min;
	using std::max;
}
#include <gdiplus.h>

#pragma comment(lib,"gdiplus.lib")

namespace FraplesDev
{
	Surface::Surface(unsigned int width, unsigned int height )noexcept :
		_mPBuffer(std::make_unique<Color[]>( width * height )),
		_mWidth(width),
		_mHeight(height)
	{

	}
	Surface::Surface(Surface&& source) noexcept
		:_mPBuffer(std::move(source._mPBuffer)),
		_mWidth(source._mWidth),
		_mHeight(source._mHeight)
	{
	}
	Surface& Surface::operator=(Surface&& donor) noexcept
	{
		_mWidth = donor._mWidth;
		_mHeight = donor._mHeight;
		_mPBuffer = std::move(donor._mPBuffer);
		donor._mPBuffer = nullptr;
		return *this;
	}
	void Surface::Clear(Color fillValue) noexcept
	{
		memset(_mPBuffer.get(), fillValue._mDword, _mWidth * _mHeight * sizeof(Color));
	}
	void Surface::PutPixel(unsigned int x, unsigned int y, Color c) noexcept(!IS_DEBUG)
	{
		assert(x >= 0);
		assert(y >= 0);
		assert(x < _mWidth);
		assert(y < _mHeight);
		_mPBuffer[y * _mWidth + x] = c;
	}
	Surface::Color Surface::GetPixel(unsigned int x, unsigned int y) const noexcept(!IS_DEBUG)
	{
		assert(x >= 0);
		assert(y >= 0);
		assert(x < _mWidth);
		assert(y < _mHeight);
		return _mPBuffer[y * _mWidth + x];
	}
	unsigned int Surface::GetWidth() const noexcept
	{
		return _mWidth;
	}
	unsigned int Surface::GetHeight() const noexcept
	{
		return _mHeight;
	}
	const Surface::Color* Surface::GetBufferPtr() const noexcept
	{
		return _mPBuffer.get();
	}
	const Surface::Color* Surface::GetBufferPtrConst() const noexcept
	{
		return _mPBuffer.get();
	}
	Surface Surface::FromFile(const std::string& name)
	{
		unsigned int  width = 0;
		unsigned int  height = 0;

		std::unique_ptr<Color[]> pBuffer;

		{
			//convert filenam to wide string (for Gdiplus)
			wchar_t wideName[512];

			mbstowcs_s(nullptr, wideName, name.c_str(), _TRUNCATE);
			
			Gdiplus::Bitmap bitmap(wideName);
			if (bitmap.GetLastStatus() != Gdiplus::Status::Ok)
			{
				std::stringstream ss;
				ss << "Loading Image [" << name << "]: failed to load.";
				throw Exception(__LINE__, __FILE__, ss.str());
			}
			width = bitmap.GetWidth();
			height = bitmap.GetHeight();
			pBuffer = std::make_unique<Color[]>(width * height);

			for (unsigned int y = 0; y < height; y++)
			{
				for (unsigned int x = 0; x < width; x++)
				{
					Gdiplus::Color c;
					bitmap.GetPixel(x, y, &c);
					pBuffer[y * width + x] = c.GetValue();
				}
			}
		}
		return Surface(width, height, std::move(pBuffer));
	}
	void Surface::Save(const std::string& filename) const
	{
		auto GetEncoderClsid = [&filename](const WCHAR* format, CLSID* pClsid)->void
		{
			UINT num = 0; //number of image encoders
			UINT size = 0;

			Gdiplus::ImageCodecInfo* pImageCodeInfo = nullptr;

			Gdiplus::GetImageEncodersSize(&num, &size);

			if (size == 0)
			{
				std::stringstream ss;
				ss << "Saving surface to [" << filename << "]: failed to get encoder size == 0.";
				throw Exception(__LINE__, __FILE__, ss.str());
			}
			pImageCodeInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
			if (pImageCodeInfo == nullptr)
			{
				std::stringstream ss;

				ss << "Saving surface to [" << filename << "]: failed to get encoder; failed to allocate memory.";
				throw Exception(__LINE__, __FILE__, ss.str());
			}
			GetImageEncoders(num, size, pImageCodeInfo);

			for (UINT j = 0; j < num; ++j)
			{
				if (wcscmp(pImageCodeInfo[j].MimeType, format) == 0)
				{
					*pClsid = pImageCodeInfo[j].Clsid;
					free(pImageCodeInfo);
					return;
				}
			}
			free(pImageCodeInfo);
			std::stringstream ss;
			ss << "Saving surface to [" << filename << "]: failed to get encoder; failed to find matching encoder.";
			throw Exception(__LINE__, __FILE__, ss.str());
		};
		CLSID bmpID;
		GetEncoderClsid(L"image/bmp", &bmpID);

		//convert filenam to wide string ( for Gdiplus)

		wchar_t widename[512];
		mbstowcs_s(nullptr, widename, filename.c_str(), _TRUNCATE);

		Gdiplus::Bitmap bitmap(_mWidth, _mHeight, _mWidth * sizeof(Color), PixelFormat32bppARGB, (BYTE*)_mPBuffer.get());
		if (bitmap.Save(widename,&bmpID,nullptr) != Gdiplus::Status::Ok)
		{
			std::stringstream ss;
			ss << "Saving Surface to [" << filename << "]: failed to save.";
			throw Exception(__LINE__, __FILE__, ss.str());
		}
	}
	void Surface::Copy(const Surface& src) noexcept(!IS_DEBUG)
	{
		assert(_mWidth == src._mWidth);
		assert(_mHeight == src._mHeight);
		memcpy(_mPBuffer.get(), src._mPBuffer.get(), _mWidth * _mHeight * sizeof(Color));
	}
	Surface::Surface(unsigned int width, unsigned int height, std::unique_ptr<Color[]> pBufferParam) noexcept
		: _mWidth(width),_mHeight(height),_mPBuffer(std::move(pBufferParam))
	{
	}
	Surface::Exception::Exception(int line, const char* file, std::string note) noexcept
		:FraplesException(line, file), _mNote(std::move(note))
	{

	}
	const char* Surface::Exception::what() const noexcept
	{
		std::ostringstream oss;
		oss << FraplesException::what() << std::endl << "[Note]" << GetNote();
		_mWhatBuffer = oss.str();
		return _mWhatBuffer.c_str();
	}
	const char* Surface::Exception::GetType() const noexcept
	{
		return "Fraples Graphics Exception";
	}
	const std::string& Surface::Exception::GetNote() const noexcept
	{
		return _mNote;
	}
}