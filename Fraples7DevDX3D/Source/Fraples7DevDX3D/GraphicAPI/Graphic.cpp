#include "../../Fraples7DevDX3D/Core/Fraples7.h"
#include "../Core/Debugging/Exceptions/GraphicsThrowMacros.h"
#include "../Core/Debugging/dxerr.h"
#include "Graphic.h"
#include <sstream>
#include <d3dcompiler.h>
#include <DirectXMath.h>


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

		D3D11CreateDeviceAndSwapChain(
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
			&_mpContext);
		
		Microsoft::WRL::ComPtr<ID3D11Resource> pBackBuffer;
		FPL_GFX_THROW_INFO(_mpSwap->GetBuffer(0, __uuidof(ID3D11Resource),&pBackBuffer));
		_mpDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &_mpTarget);
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

	void Graphics::DrawTestTriangle(float angle, float x, float y)
	{
		DirectX::XMVECTOR v = DirectX::XMVectorSet(3.0f, 3.0f, 0.0f, 0.0f);
		auto result = DirectX::XMVector3Transform(v, DirectX::XMMatrixScaling(1.5f, 0.0f, 0.0f));
		auto xx = DirectX::XMVectorGetX(result);
		HRESULT hr;

		struct Vertex
		{
			
			struct {
				float x, y;
			}pos;
			
			struct {
				unsigned char r, g, b, a;
			}color;
		};
		const Vertex vertices[] =
		{
			{ 0.0f,  0.5f,  255, 000,   000,   0},
			{ 0.5f, -0.5f,  000, 255,   000,   0},
			{-0.5f, -0.5f,  000, 000,   255,   0},
			{-0.3f,  0.3f,  255, 000,   000,   0},
			{ 0.3f,  0.3f,  000, 255,   000,   0},
			{ 0.0f, -0.8f,  000, 000,   255,   0},

		};
		Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
		D3D11_BUFFER_DESC  bd = {};
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.CPUAccessFlags = 0u;
		bd.MiscFlags = 0u;
		bd.ByteWidth = sizeof(vertices);
		bd.StructureByteStride = sizeof(Vertex);
		D3D11_SUBRESOURCE_DATA sd = {};
		sd.pSysMem = vertices;
		FPL_GFX_THROW_INFO(_mpDevice->CreateBuffer(&bd, &sd, &pVertexBuffer));
		const UINT stride = sizeof(Vertex);
		const UINT offset = 0u;
		_mpContext->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset);

		const unsigned short indices[] = {
			
			0,1,2,
			0,2,3,
			0,4,1,
			2,1,5
		};
		Microsoft::WRL::ComPtr<ID3D11Buffer>pIndexBuffer;
		D3D11_BUFFER_DESC ibd = {};
		ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibd.Usage = D3D11_USAGE_DEFAULT;
		ibd.CPUAccessFlags = 0u;
		ibd.MiscFlags = 0u;
		ibd.ByteWidth = sizeof(indices);
		ibd.StructureByteStride = sizeof(unsigned short);
		D3D11_SUBRESOURCE_DATA isd = {};
		isd.pSysMem = indices;
		FPL_GFX_THROW_INFO(_mpDevice->CreateBuffer(&ibd, &isd, &pIndexBuffer));
		_mpContext->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT,0u);
		struct ConstantBuffer
		{
			DirectX::XMMATRIX transform;
		};
		const ConstantBuffer cb =
		{
			{
					DirectX::XMMatrixTranspose(
				    DirectX::XMMatrixRotationZ(angle) *
					DirectX::XMMatrixScaling(3.0f / 4.0f,1.0f,1.0f)*
					DirectX::XMMatrixTranslation(3.0f / 4.0f,1.0f,1.0f))
			}	
		};
		Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer;
		D3D11_BUFFER_DESC cbd;
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbd.MiscFlags = 0u;
		cbd.ByteWidth = sizeof(cb);
		cbd.StructureByteStride = 0u;
		D3D11_SUBRESOURCE_DATA csd = {};
		csd.pSysMem = &cb;
		FPL_GFX_THROW_INFO(_mpDevice->CreateBuffer(&cbd, &csd, &pConstantBuffer));
		_mpContext->VSSetConstantBuffers(0u, 1u, pConstantBuffer.GetAddressOf());
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

		const D3D11_INPUT_ELEMENT_DESC IelementDesc[] =
		{
			{"Position", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"Color", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,0}
		};

		_mpDevice->CreateInputLayout(IelementDesc, (UINT)std::size(IelementDesc), pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &pInputLayout);
		_mpContext->IASetInputLayout(pInputLayout.Get());


		//bind render target
		_mpContext->OMSetRenderTargets(1u, _mpTarget.GetAddressOf(), nullptr);

		_mpContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		D3D11_VIEWPORT vp;
		vp.Width = 1200;
		vp.Height = 600;
		vp.MinDepth = 0;
		vp.MaxDepth = 1;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		_mpContext->RSSetViewports(1u, &vp);
		_mpContext->DrawIndexed((UINT)std::size(indices), 0u,0u);
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