#include "Surface.h"
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cassert>
#include "Platform/Windows/Window.h"
#include "Utility/Fraples7Utility.h"

namespace FraplesDev
{
	Surface::Surface(unsigned int width, unsigned int height ) 
	{
		HRESULT hr = _mScratch.Initialize2D(format, width, height, 1u, 1u);
		if (FAILED(hr))
		{
			throw Surface::Exception(__LINE__, __FILE__, "Failed to initialize ScratchImage", hr);
		}
	}

	void Surface::Clear(Color fillValue) noexcept
	{
		const auto width = GetWidth();
		const auto height = GetHeight();

		auto& imgData = *_mScratch.GetImage(0, 0, 0);

		for (size_t y = 0u; y < height; y++)
		{
			auto rowStart = reinterpret_cast<Color*>(imgData.pixels + imgData.rowPitch * y);
			std::fill(rowStart, rowStart + imgData.width, fillValue);
		}
	}
	void Surface::PutPixel(unsigned int x, unsigned int y, Color c) noexcept(!IS_DEBUG)
	{
		assert(x >= 0);
		assert(y >= 0);
		assert(x < GetWidth());
		assert(y < GetHeight());
		auto& imgData = *_mScratch.GetImage(0, 0, 0);
		reinterpret_cast<Color*>(&imgData.pixels[y * imgData.rowPitch])[x] = c;
	}
	Surface::Color Surface::GetPixel(unsigned int x, unsigned int y) const noexcept(!IS_DEBUG)
	{
		assert(x >= 0);
		assert(y >= 0);
		assert(x < GetWidth());
		assert(y < GetHeight());
		auto& imgData = *_mScratch.GetImage(0, 0, 0);
		return reinterpret_cast<Color*>(&imgData.pixels[y * imgData.rowPitch])[x];
	
	}
	unsigned int Surface::GetWidth() const noexcept
	{
		return (unsigned int)_mScratch.GetMetadata().width;
	}
	unsigned int Surface::GetHeight() const noexcept
	{
		return (unsigned int)_mScratch.GetMetadata().height;
	}
	Surface::Color* Surface::GetBufferPtr()  noexcept
	{
		 return reinterpret_cast<Color*>(_mScratch.GetPixels());
	}
	const Surface::Color* Surface::GetBufferPtr() const noexcept
	{
		 return reinterpret_cast<Color*>(_mScratch.GetPixels());
	}
	const Surface::Color* Surface::GetBufferPtrConst() const noexcept
	{
		return GetBufferPtr();
	}
	Surface Surface::FromFile(const std::string& name)
	{

		DirectX::ScratchImage scratch;
		HREFTYPE hr = DirectX::LoadFromWICFile(Utility::ToWide(name).c_str(), DirectX::WIC_FLAGS_IGNORE_SRGB, nullptr, scratch);
		if (FAILED(hr))
		{
			throw Surface::Exception(__LINE__, __FILE__, name, "Failed to load image", hr);
		}
		if (scratch.GetImage(0, 0, 0)->format != format)
		{
			DirectX::ScratchImage converted;
			hr = DirectX::Convert(*scratch.GetImage(0, 0, 0), format, 
				DirectX::TEX_FILTER_DEFAULT, 
				DirectX::TEX_THRESHOLD_DEFAULT, converted);
			if (FAILED(hr))
			{
				throw Surface::Exception(__LINE__, __FILE__, name, "Failed to convert image", hr);
			}
			return Surface(std::move(converted));
		}
		return Surface(std::move(scratch));
	}
	bool Surface::AlphaLoaded() const noexcept
	{
		return !_mScratch.IsAlphaAllOpaque();
	}
	void Surface::Save(const std::string& filename) const
	{
		const auto GetCodeID = [](const std::string& filename)
		{
			const std::filesystem::path path = filename;

			const auto ext = path.extension().string();
			if (ext == ".png")
			{
				return DirectX::WIC_CODEC_PNG;
			}
			else if (ext == ".jpg")
			{
				return DirectX::WIC_CODEC_JPEG;
			}
			else if (ext == ".bmp")
			{
				return DirectX::WIC_CODEC_BMP;
			}
			throw Exception(__LINE__, __FILE__, filename, "Image format not supported");
		};
	
		HRESULT hr = DirectX::SaveToWICFile(*_mScratch.GetImage(0, 0, 0),
		DirectX::WIC_FLAGS_NONE, DirectX::GetWICCodec(GetCodeID(filename)), Utility::ToWide(filename).c_str());
		
		if (FAILED(hr))
		{
			throw Surface::Exception(__LINE__, __FILE__, filename, "Failed to save Image",hr);
		}
	}

	Surface::Surface(DirectX::ScratchImage scratch)noexcept 
		: _mScratch(std::move(scratch))
	{
	}
	Surface::Exception::Exception(int line, const char* file, std::string note,std::optional<HRESULT>hr) noexcept
		:FraplesException(line, file), _mNote("[Note]" + note)
	{
		if (hr)
		{
			note = "[Error String]" + Window::Exception::TranslateErrorCode(*hr) + "\n" + note;
		}
	}
	Surface::Exception::Exception(int line, const char* file, std::string filename,std::string note, std::optional<HRESULT>hr) noexcept
		:FraplesException(line, file)
	{
		using namespace std::string_literals;
		note = "[File]"s + filename + "\n"s + "[Note]"s + note;
		if (hr)
		{
			note = "[Error String]" + Window::Exception::TranslateErrorCode(*hr)  + note;
		}
	}
	const char* Surface::Exception::what() const noexcept
	{
		std::ostringstream oss;
		oss << FraplesException::what() << std::endl << GetNote();
		_mWhatBuffer = oss.str();
		return _mWhatBuffer.c_str();
	}
	const char* Surface::Exception::GetType() const noexcept
	{
		return "Fraples Surface Exception";
	}
	const std::string& Surface::Exception::GetNote() const noexcept
	{
		return _mNote;
	}
}