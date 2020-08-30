#include "../../Fraples7DevDX3D/Core/Fraples7.h"
#include "../Core/Debugging/Exceptions/GraphicsThrowMacros.h"
#include "../Core/Debugging/dxerr.h"
#include "Graphic.h"
#include <sstream>
#pragma comment(lib,"d3d11.lib")

namespace FraplesDev {
	FraplesDev::Graphics::Graphics(HWND hWnd)
	{
		DXGI_SWAP_CHAIN_DESC swap;
		swap.BufferDesc.Width = 0;
		swap.BufferDesc.Height = 0;
		swap.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		swap.BufferDesc.RefreshRate.Numerator = 0;
		swap.BufferDesc.RefreshRate.Denominator = 0;
		swap.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swap.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swap.SampleDesc.Count = 1;
		swap.SampleDesc.Quality = 0;
		swap.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swap.BufferCount = 1;
		swap.OutputWindow = hWnd;
		swap.Windowed = TRUE;
		swap.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swap.Flags = 0;

		D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
			0, nullptr, 0, D3D11_SDK_VERSION, &swap, &_mpSwap, &_mpDevice, nullptr, &_mpContext);
		ID3D11Resource* pBackBuffer = nullptr;
		_mpSwap->GetBuffer(0, __uuidof(ID3D11Resource), (void**)&pBackBuffer);
		_mpDevice->CreateRenderTargetView(pBackBuffer, nullptr, &_mpTarget);
		pBackBuffer->Release();
	}

	FraplesDev::Graphics::~Graphics()
	{
		if (_mpTarget != nullptr)
		{
			_mpTarget->Release();
		}
		if (_mpContext != nullptr)
		{
			_mpContext->Release();
		}
		if (_mpSwap != nullptr)
		{
			_mpSwap->Release();
		}
		if (_mpDevice != nullptr)
		{
			_mpDevice->Release();
		}
	}
	Graphics::HrException::HrException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs) noexcept
		: Exception(line,file)
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
	void FraplesDev::Graphics::EndFrame()
	{
		HRESULT hr;
#ifndef NDEBUG
		infoManager.Set();
#endif // !_DEBUG

		if (FAILED(hr = _mpSwap->Present(1u, 0u)))
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


}