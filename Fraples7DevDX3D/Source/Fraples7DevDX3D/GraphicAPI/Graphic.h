#pragma once
#include "../Core/Fraples7.h"
#include "../Core/Debugging/Exceptions/FraplesException.h"
#include "../Core/Debugging/DxgiInfoManager.h"
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <vector>
#include <d3d11.h>
#include <wrl.h>

#define GFX_THROW_FAILED(hrcall) if(FAILED(hr = (hrcall))) throw FraplesDev::Graphics::HrException(__LINE__,__FILE__,hr)
namespace FraplesDev
{
	class Graphics
	{
		friend class GfxContext;
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

		~Graphics() = default;
	public:
		void EndFrame();
		inline void ClearBuffer(float red, float green, float blue)noexcept
		{
			const float color[] = { red,green,blue,1.0f };
			_mpContext->ClearRenderTargetView(_mpTarget.Get(), color);
			_mpContext->ClearDepthStencilView(_mpDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);
		}
		void RenderIndexed(UINT count)noexcept;
		void SetProjection(DirectX::FXMMATRIX proj) noexcept;
		DirectX::XMMATRIX GetProjection() const noexcept;

	private:
		Microsoft::WRL::ComPtr<ID3D11Device> _mpDevice = nullptr;
		Microsoft::WRL::ComPtr<IDXGISwapChain> _mpSwap = nullptr;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> _mpContext = nullptr;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> _mpTarget = nullptr;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> _mpDSV;
		DirectX::XMMATRIX _mProjection;

#ifndef NDEBUG
		DxgiInfoManager infoManager;
#endif // NDEBUG

	};

	
}