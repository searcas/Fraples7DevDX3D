#include "Graphics.h"
#include "Core/Common/Exceptions/Macros/GraphicsThrowMacros.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "Core/Common/dxerr.h"
#include "RendererAPI/RenderTarget.h"
#include <sstream>
namespace FraplesDev {

	Graphics::Graphics(HWND hWnd,int width,int height)
		: _mWidth(std::move(width)),_mHeight(std::move(height))
	{
		DXGI_SWAP_CHAIN_DESC swap;
		swap.BufferDesc.Width = _mWidth;
		swap.BufferDesc.Height = _mHeight;
		swap.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		swap.BufferDesc.RefreshRate.Numerator = 0;
		swap.BufferDesc.RefreshRate.Denominator = 0;
		swap.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swap.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swap.SampleDesc.Count = 1;
		swap.SampleDesc.Quality = 0;
		swap.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swap.BufferCount = 2;
		swap.OutputWindow = hWnd;
		swap.Windowed = TRUE;
		//DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL and DXGI_SWAP_EFFECT_FLIP_DISCARD NEW ONE
		//DXGI_SWAP_EFFECT_DISCARD OLD
		swap.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swap.Flags = 0;

		UINT swapCreateFlags = 0u;
#ifndef NDEBUG
		swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif // !NDEBUG
		HRESULT hr;

		FPL_GFX_THROW_INFO(D3D11CreateDeviceAndSwapChain(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			swapCreateFlags,
			nullptr,
			0,
			D3D11_SDK_VERSION,
			&swap,
			&_mpSwap,
			&_mpDevice,
			nullptr,
			&_mpContext
		));
		
		Microsoft::WRL::ComPtr<ID3D11Texture2D> pBackBuffer;
		FPL_GFX_THROW_INFO(_mpSwap->GetBuffer(0, __uuidof(ID3D11Texture2D),&pBackBuffer));
		_mTarget = std::shared_ptr<RenderTarget>{ new OutputOnlyRenderTarget(*this, pBackBuffer.Get()) };
	
		//viewport always fullscreen (for now)
		D3D11_VIEWPORT vp;
		vp.Width = _mWidth;
		vp.Height = _mHeight;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0.0f;
		vp.TopLeftY = 0.0f;
		_mpContext->RSSetViewports(1u, &vp);
		ImGui_ImplDX11_Init(_mpDevice.Get(), _mpContext.Get());
	}

	Graphics::~Graphics()
	{
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
	}
	Graphics::HrException::HrException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs) noexcept
		: Exception(line,file)
	{
		_mHr = hr;
		//join all info messages with newlines into single string

		for (const auto& m : infoMsgs)
		{
			_mInfo += m;
			_mInfo.push_back('\n');
		}
		//remove final newline if exist
		if (!_mInfo.empty())
		{
			_mInfo.pop_back();
		}
	}
	const char* Graphics::HrException::what()const noexcept
	{
		std::ostringstream oss;
		oss << GetType() << std::endl
			<< "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode()
			<< std::dec << " (" << (unsigned long)GetErrorCode() << ")" << std::endl
			<< "[Error String] " << GetErrorString() << std::endl
			<< "[Description] " << GetErrorDescription() << std::endl;
		if (!_mInfo.empty())
		{
			oss << "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
		}
		oss << GetOriginString();
		_mWhatBuffer = oss.str();
		return _mWhatBuffer.c_str();
	}
	const char* Graphics::HrException::GetType()const noexcept
	{
		return "Fraples7 Graphics Exception";
	}
	HRESULT Graphics::HrException::GetErrorCode() const noexcept
	{
		return _mHr;
	}
	std::string Graphics::HrException::GetErrorString() const noexcept
	{
		return DXGetErrorString(_mHr);
	}
	std::string Graphics::HrException::GetErrorDescription() const noexcept
	{
		char buf[512];
		DXGetErrorDescription(_mHr, buf, sizeof(buf));
		return buf;
	}
	std::string Graphics::HrException::GetErrorInfo() const noexcept
	{
		return _mInfo;
	}
	void Graphics::BeginFrame(float red, float green, float blue) noexcept
	{
		if (IsImGuiEnabled)
		{
			ImGui_ImplDX11_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();
		}
	}
	void FraplesDev::Graphics::EndFrame()
	{
		if (IsImGuiEnabled)
		{
			ImGui::Render();
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		}
		HRESULT hr;
#ifndef NDEBUG
		infoManager.Set();
#endif // !_DEBUG

		//Disabled Vysnc
		if (FAILED(hr = _mpSwap->Present(0u, 0u)))
		{
			if (hr == DXGI_ERROR_DEVICE_REMOVED)
			{
				throw FPL_GFX_DEVICE_REMOVED_EXCEPT(_mpDevice->GetDeviceRemovedReason());
			}
			else
			{
				throw FPL_GFX_EXCEPT(hr);
			}
		}
	}

	void Graphics::RenderIndexed(UINT count)noexcept(!IS_DEBUG)
	{
		FPL_GFX_THROW_INFO_ONLY(_mpContext->DrawIndexed(count, 0u, 0));
	}

	FraplesDev::Graphics::InfoException::InfoException(int line, const char* file, std::vector<std::string> infoMsgs) noexcept : Exception(line, file)
	{
		//join all info messages with newlines into single string
		for (const auto& m : infoMsgs)
		{
			_mInfo += m;
			_mInfo.push_back('\n');
		}
		//remove final newline if exist
		if (!_mInfo.empty())
		{
			_mInfo.pop_back();
		}
	}

	const char* FraplesDev::Graphics::InfoException::what() const noexcept
	{
		std::ostringstream oss;
		oss << GetType() << std::endl
			<< "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
		oss << GetOriginString();
		_mWhatBuffer = oss.str();
		return _mWhatBuffer.c_str();
	}

	const char* FraplesDev::Graphics::InfoException::GetType() const noexcept
	{
		return "Fraples7 Graphics Info Exception";
	}

	std::string FraplesDev::Graphics::InfoException::GetErrorInfo() const noexcept
	{
		return _mInfo;
	}

	const char* FraplesDev::Graphics::DeviceRemovedException::GetType() const noexcept
	{
		return "Fraples Graphics Eception [Device removed] (DXGI_ERROR_DEVICE_REMOVED)";
	}

	void Graphics::SetProjection(DirectX::FXMMATRIX proj) noexcept
	{
		_mProjection = proj;
	}

	const DirectX::XMMATRIX& Graphics::GetProjection() const noexcept
	{
		return _mProjection;
	}
	void Graphics::SetCamera(DirectX::FXMMATRIX cameraView) noexcept
	{
		_mCamera = cameraView;
	}
	void Graphics::EnableImGui() noexcept
	{
		IsImGuiEnabled = true;
	}
	void Graphics::DisableImGui() noexcept
	{
		IsImGuiEnabled = false;
	}
	UINT Graphics::GetWidth() const noexcept
	{
		return _mWidth;
	}
	UINT Graphics::GetHeight() const noexcept
	{
		return _mHeight;
	}
	std::shared_ptr<RenderTarget> Graphics::GetTarget()
	{
		return _mTarget;
	}
}