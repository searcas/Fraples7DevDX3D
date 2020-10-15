#pragma once
#include "Debugging\Exceptions\FraplesException.h"
#include <memory>
#include <assert.h>
#include <string>
namespace FraplesDev
{
	class Surface
	{
	public:
		class Color
		{
		public:
			constexpr Color() noexcept : _mDword()
			{

			}
			constexpr Color(const Color& other) noexcept : _mDword(other._mDword)
			{

			}
			constexpr Color(unsigned int dw) noexcept : _mDword(dw)
			{

			}
			constexpr Color(unsigned int x, unsigned int r, unsigned int g, unsigned int b)noexcept : _mDword((x << 24u) | (r << 16u))
			{

			}
			constexpr Color(unsigned int r, unsigned int g, unsigned int b)noexcept : _mDword((r << 16u) | (g << 8u) | b)
			{

			}
			constexpr Color(Color col, unsigned char x)noexcept : Color((x << 24u) | col._mDword)
			{

			}
			Color& operator = (Color color)noexcept
			{
				_mDword = color._mDword;
				return *this;
			}
			constexpr unsigned char GetX()const noexcept
			{
				return _mDword >> 24u;
			}
			constexpr unsigned char GetA()const noexcept
			{
				return GetX();
			}
			constexpr unsigned char GetR()const noexcept
			{
				return (_mDword >> 16u) & 0xFFu;
			}
			constexpr unsigned char GetG()const noexcept
			{
				return(_mDword >> 8u) & 0xFFu;
			}
			constexpr unsigned char GetB()const noexcept
			{
				return _mDword & 0xFFu;
			}
			void SetX(unsigned char x)noexcept
			{
				_mDword = (_mDword & 0xFFFFFFu);
			}
			void SetA(unsigned char a)noexcept
			{
				SetX(a);
			}
			void SetR(unsigned char r)noexcept
			{
				_mDword = (_mDword & 0xFF00FFFFu) | (r << 16u);
			}
			void SetG(unsigned char g)noexcept
			{
				_mDword = (_mDword & 0xFFFF00FFu) | (g << 8u);
			}
			void SetB(unsigned char b)noexcept
			{
				_mDword = (_mDword & 0xFFFFFF00u) | b;
			}
		public:
			unsigned int _mDword;
		};

		class Exception : public FraplesException
		{
		public:
			Exception(int line, const char* file, std::string note)noexcept;
			const char* what()const noexcept override;
			const char* GetType()const noexcept override;
			const std::string& GetNode()const noexcept;
		private:
			std::string note;
		};

		Surface(unsigned int width, unsigned int height, unsigned int pitch)noexcept;
		Surface(unsigned int width, unsigned int height) noexcept;
		Surface(Surface&& source) noexcept;
		Surface(Surface& cpyctr) = delete;
		Surface& operator = (Surface&& donor)noexcept;
		Surface& operator = (const Surface&) = delete;
		void Clear(Color fillValue) noexcept;
		void PutPixel(unsigned int x, unsigned int y, Color c)noexcept(!IS_DEBUG);
		Color GetPixel(unsigned int x, unsigned int y)const noexcept(!IS_DEBUG);
		unsigned int GetWidth()const noexcept;
		unsigned int GetHeight()const noexcept;
		const Color* GetBufferPtr()const noexcept;
		const Color* GetBufferPtrConst()const noexcept;
		static Surface FromFile(std::string& name);
		
		void Save(const std::string& filename)const;
		void Copy(const Surface& src)noexcept(!IS_DEBUG);
		private:
			Surface(unsigned int width, unsigned int height, std::unique_ptr<Color[]>pBufferParam)noexcept;

	private:
		std::unique_ptr<Color[]>_mPBuffer;
		unsigned int width;
		unsigned int height;
	};


}
