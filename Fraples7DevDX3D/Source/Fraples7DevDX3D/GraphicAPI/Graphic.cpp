#include "../../Fraples7DevDX3D/Core/Fraples7.h"
#include "../Core/Debugging/Exceptions/Macros/GraphicsThrowMacros.h"
#include "../Core/Debugging/dxerr.h"
#include "../Core/Debugging/DxgiInfoManager.h"
#include "imgui_impl_dx11.h"
#include "Graphic.h"
#include <sstream>
#include <random>
#pragma comment(lib,"d3d11.lib")

namespace FraplesDev {
	FraplesDev::Graphics::Graphics(HWND hWnd)
	{
		DXGI_SWAP_CHAIN_DESC swap;
		HRESULT hr;
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

		UINT swapCreateFlags = 0u;
#ifndef NDEBUG
		swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif // !NDEBUG

		FPL_GFX_THROW_INFO(D3D11CreateDeviceAndSwapChain(
			nullptr, 
			D3D_DRIVER_TYPE_HARDWARE, 
			nullptr,
			0, 
			nullptr, 
			0, 
			D3D11_SDK_VERSION, 
			&swap, 
			&_mpSwap, 
			&_mpDevice, 
			nullptr, 
			&_mpContext
		));
		
		Microsoft::WRL::ComPtr<ID3D11Resource> pBackBuffer;
		FPL_GFX_THROW_INFO(_mpSwap->GetBuffer(0, __uuidof(ID3D11Resource),&pBackBuffer));
		FPL_GFX_THROW_INFO(_mpDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &_mpTarget));

		D3D11_DEPTH_STENCIL_DESC ddsd = {};
		ddsd.DepthEnable = true;
		ddsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		ddsd.DepthFunc = D3D11_COMPARISON_LESS;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> pDSState;
		FPL_GFX_THROW_INFO(_mpDevice->CreateDepthStencilState(&ddsd, &pDSState));

		//bind depth state
		_mpContext->OMSetDepthStencilState(pDSState.Get(), 1u);

		Microsoft::WRL::ComPtr<ID3D11Texture2D> pDepthStencil;
		D3D11_TEXTURE2D_DESC descDepth = {};
		descDepth.Width = 800u;
		descDepth.Height = 600u;
		descDepth.MipLevels = 1u;
		descDepth.ArraySize = 1u;
		descDepth.Format = DXGI_FORMAT_D32_FLOAT;
		descDepth.SampleDesc.Count = 1u;
		descDepth.SampleDesc.Quality = 0u;
		descDepth.Usage = D3D11_USAGE_DEFAULT;
		descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		FPL_GFX_THROW_INFO(_mpDevice->CreateTexture2D(&descDepth, nullptr, &pDepthStencil));

		//create view of depth stensil texture
		D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
		descDSV.Format = DXGI_FORMAT_D32_FLOAT;
		descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		descDSV.Texture2D.MipSlice = 0u;
		FPL_GFX_THROW_INFO(_mpDevice->CreateDepthStencilView(pDepthStencil.Get(), &descDSV, &_mpDSV));

		_mpContext->OMSetRenderTargets(1u, _mpTarget.GetAddressOf(), _mpDSV.Get());

		D3D11_VIEWPORT vp;
		vp.Width = 800.0f;
		vp.Height = 600.0f;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0.0f;
		vp.TopLeftY = 0.0f;
		_mpContext->RSSetViewports(1u, &vp);

		ImGui_ImplDX11_Init(_mpDevice.Get(), _mpContext.Get());
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

	void Graphics::RenderIndexed(UINT count)noexcept
	{
		_mpContext->DrawIndexed(count, 0u, 0u);
	}

	/*void Graphics::DrawTestTriangle(float angle, float x, float z)
	{
		DirectX::XMVECTOR v = DirectX::XMVectorSet(3.0f, 3.0f, 0.0f, 0.0f);
		auto result = DirectX::XMVector3Transform(v, DirectX::XMMatrixScaling(1.5f, 0.0f, 0.0f));
		auto xx = DirectX::XMVectorGetX(result);
		HRESULT hr;

	
		Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
		D3D11_BUFFER_DESC  bd = {};
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.CPUAccessFlags = 0u;
		bd.MiscFlags = 0u;
		bd.ByteWidth = sizeof(vertices);
		bd.StructureByteStride = sizeof(Vertex);
		D3D11_SUBRESOURCE_DATA sd = {};
		sd.pSysMem = vertices;
		FPL_GFX_THROW_INFO(_mpDevice->CreateBuffer(&bd, &sd, &pVertexBuffer));
	
		
		
		struct ConstantBuffer
		{
			DirectX::XMMATRIX transform;
		};
		const ConstantBuffer cb =
		{
			{
				DirectX::XMMatrixTranspose(
				DirectX::XMMatrixRotationZ(angle) *
				DirectX::XMMatrixRotationX(angle) *
				DirectX::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 10.0f))
			}	
		};

	

		Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer2;
		D3D11_BUFFER_DESC cbd2;
		cbd2.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd2.Usage = D3D11_USAGE_DEFAULT;
		cbd2.CPUAccessFlags = 0u;
		cbd2.MiscFlags = 0u;
		cbd2.ByteWidth = sizeof(cb2);
		cbd2.StructureByteStride = 0u;
		D3D11_SUBRESOURCE_DATA csd2 = {};
		csd2.pSysMem = &cb2;
		FPL_GFX_THROW_INFO(_mpDevice->CreateBuffer(&cbd2, &csd2, &pConstantBuffer2));
		_mpContext->VSSetConstantBuffers(0u, 1u, pConstantBuffer2.GetAddressOf());

		//create pixel shader
		Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
		Microsoft::WRL::ComPtr<ID3DBlob>pBlob;
		FPL_GFX_THROW_INFO(D3DReadFileToBlob(L"PixelShader.cso", &pBlob));
		FPL_GFX_THROW_INFO(_mpDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader));
	
		_mpContext->PSSetShader(pPixelShader.Get(), 0, 0);

		//create vertex shader
		Microsoft::WRL::ComPtr<ID3D11VertexShader>pVertexShader;
		FPL_GFX_THROW_INFO(D3DReadFileToBlob(L"VertexShader.cso", &pBlob));
		FPL_GFX_THROW_INFO(_mpDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pVertexShader));
		
		_mpContext->VSSetShader(pVertexShader.Get(), 0, 0);
		
		//input (vertex) layout (2d position only)
		Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;

	
		_mpDevice->CreateInputLayout(IelementDesc, 
			(UINT)std::size(IelementDesc), 
			pBlob->GetBufferPointer(), 
			pBlob->GetBufferSize(), &pInputLayout);


		_mpContext->IASetInputLayout(pInputLayout.Get());

		_mpContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		D3D11_VIEWPORT vp;
		vp.Width = 800;
		vp.Height = 600;
		vp.MinDepth = 0;
		vp.MaxDepth = 1;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		_mpContext->RSSetViewports(1u, &vp);
		_mpContext->DrawIndexed((UINT)std::size(indices), 0u,0u);
	}
	*/
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
}