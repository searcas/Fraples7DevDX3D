#pragma once
#include "../Core/Fraples7.h"
#include "../Core/Debugging/Exceptions/FraplesException.h"
#include "../Core/Debugging/DxgiInfoManager.h"
#include <vector>
#include <d3d11.h>


#define GFX_THROW_FAILED(hrcall) if(FAILED(hr = (hrcall))) throw FraplesDev::Graphics::HrException(__LINE__,__FILE__,hr)
namespace FraplesDev
{
	class Graphics
	{
	public:
		class Exception : public FraplesDev::FraplesException
		{
			using FraplesException::FraplesException;
		};
		class HrException : public Exception
		{
		public:
			HrException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs = {}) noexcept;
			const char* what() const noexcept override;
			const char* GetType() const noexcept override;
			HRESULT GetErrorCode() const noexcept;
			std::string GetErrorString() const noexcept;
			std::string GetErrorDescription() const noexcept;
			std::string GetErrorInfo() const noexcept;
		private:
			HRESULT _mHr;
			std::string _mInfo;
		};
		class InfoException : public Exception
		{
		public:
			InfoException(int line, const char* file, std::vector<std::string> infoMsgs) noexcept;
			const char* what() const noexcept override;
			const char* GetType() const noexcept override;
			std::string GetErrorInfo() const noexcept;
		private:
			std::string _mInfo;
		};
		class DeviceRemovedException : public HrException
		{
			using HrException::HrException;
		public:
			const char* GetType() const noexcept override;
		private:
			std::string reason;
		};
	public:
		Graphics(HWND hWnd);
		Graphics(const Graphics&) = delete;
		Graphics& operator=(const Graphics&) = delete;

		~Graphics();
	public:
		void EndFrame();
		inline void ClearBuffer(float red, float green, float blue)noexcept
		{
			const float color[] = { red,green,blue,1.0f };
			_mpContext->ClearRenderTargetView(_mpTarget, color);
		}
	private:
		ID3D11Device* _mpDevice = nullptr;
		IDXGISwapChain* _mpSwap = nullptr;
		ID3D11DeviceContext* _mpContext = nullptr;
		ID3D11RenderTargetView* _mpTarget = nullptr;

#ifndef NDEBUG
		DxgiInfoManager infoManager;
#endif // !NDEBUG

	};

	
}