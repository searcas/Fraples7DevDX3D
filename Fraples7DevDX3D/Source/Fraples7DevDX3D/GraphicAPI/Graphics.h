#pragma once
#include "Core/Fraples7.h"
#include "Core/Common/Exceptions/FraplesException.h"
#include "Core/Common/DxgiInfoManager.h"
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <vector>
#include <d3d11.h>
#include <wrl.h>
#include "imgui_impl_win32.h"
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
		Graphics(HWND hWnd,int width,int height);
		Graphics(const Graphics&) = delete;
		Graphics& operator=(const Graphics&) = delete;

		~Graphics();
	public:
		void EndFrame();
		//clear buffer
		void BeginFrame(float red, float green, float blue) noexcept;
		void RenderIndexed(UINT count)noexcept(!IS_DEBUG);
		void SetProjection(DirectX::FXMMATRIX proj) noexcept;
		const DirectX::XMMATRIX& GetProjection() const noexcept;
		void SetCamera(DirectX::FXMMATRIX cameraView) noexcept;
		inline DirectX::XMMATRIX GetCamera()const noexcept
		{
			return _mCamera;
		}
	public:
		void EnableImGui()noexcept;
		void DisableImGui()noexcept;
		constexpr bool IsEnabled() noexcept
		{
			return IsImGuiEnabled;
		};
	private:
#ifndef NDEBUG
		DxgiInfoManager infoManager;
#endif // NDEBUG
		Microsoft::WRL::ComPtr<ID3D11Device> _mpDevice;
		Microsoft::WRL::ComPtr<IDXGISwapChain> _mpSwap;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> _mpContext;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> _mpTarget;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> _mpDSV;
		DirectX::XMMATRIX _mProjection;
		DirectX::XMMATRIX _mCamera;
		bool IsImGuiEnabled = true;
	};

	
}