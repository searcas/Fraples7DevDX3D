#include "DepthStencil.h"
#include "Core/Common/Exceptions/Macros/GraphicsThrowMacros.h"
#include "RendererAPI/RenderTarget.h"
#include "GraphicAPI/Graphics.h"

namespace FraplesDev
{
	DepthStencil::DepthStencil(Graphics& gfx, UINT width, UINT height,bool canBindShaderInput)
	{
		INFOMAN(gfx);

		//create depth stencil texture
		Microsoft::WRL::ComPtr<ID3D11Texture2D>pDepthStencil;
		D3D11_TEXTURE2D_DESC descDepth = {};
		descDepth.Width = width;
		descDepth.Height = height;
		descDepth.MipLevels = 1u;
		descDepth.ArraySize = 1u;
		descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		descDepth.SampleDesc.Count = 1u;
		descDepth.SampleDesc.Quality = 0u;
		descDepth.Usage = D3D11_USAGE_DEFAULT;
		descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		FPL_GFX_THROW_INFO(GetDevice(gfx)->CreateTexture2D(&descDepth, nullptr, &pDepthStencil));

		// create target view of depth stencil texture
		FPL_GFX_THROW_INFO(GetDevice(gfx)->CreateDepthStencilView(pDepthStencil.Get(), nullptr, &_mPdepthStencilView));
	}
	void DepthStencil::BindAsBuffer(Graphics& gfx)
	{
		GetContext(gfx)->OMSetRenderTargets(0, nullptr, _mPdepthStencilView.Get());
	}
	void DepthStencil::BindAsBuffer(Graphics& gfx, BufferResource* renderTarget) noexcept
	{
		assert(dynamic_cast<RenderTarget*>(renderTarget) != nullptr);
		BindAsBuffer(gfx, static_cast<RenderTarget*>(renderTarget));
	}
	void DepthStencil::BindAsBuffer(Graphics& gfx, RenderTarget* renderTarget) noexcept
	{
		renderTarget->BindAsBuffer(gfx, this);
	}
	void DepthStencil::BindAsDepthStencil(Graphics& gfx) const noexcept
	{
		GetContext(gfx)->OMSetRenderTargets(0, nullptr, _mPdepthStencilView.Get());
	}
	void DepthStencil::Clear(Graphics& gfx) const noexcept
	{
		GetContext(gfx)->ClearDepthStencilView(_mPdepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0u);

	}
	ShaderInputDepthStencil::ShaderInputDepthStencil(Graphics& gfx, UINT slot)
		:ShaderInputDepthStencil(gfx, gfx.GetWidth(), gfx.GetHeight(), slot)
	{

	}
	ShaderInputDepthStencil::ShaderInputDepthStencil(Graphics& gfx, UINT width, UINT height, UINT slot)
		: DepthStencil(gfx, width, height, true), _mSlot(slot)
	{
		INFOMAN(gfx);
		Microsoft::WRL::ComPtr<ID3D11Resource>pRes;
		_mPdepthStencilView->GetResource(&pRes);
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // this will need to be fixed
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;
		FPL_GFX_THROW_INFO(GetDevice(gfx)->CreateShaderResourceView(pRes.Get(), &srvDesc, &_mShaderResourceView));
		
	}
	void ShaderInputDepthStencil::Bind(Graphics& gfx) noexcept
	{
		GetContext(gfx)->PSSetShaderResources(_mSlot, 1u, _mShaderResourceView.GetAddressOf());
	}
	OutputOnlyDepthStencil::OutputOnlyDepthStencil(Graphics& gfx)
		: OutputOnlyDepthStencil(gfx, gfx.GetWidth(), gfx.GetHeight())
	{
	}
	OutputOnlyDepthStencil::OutputOnlyDepthStencil(Graphics& gfx, UINT width, UINT height)
		: DepthStencil(gfx, width, height, false)
	{
	}
	void OutputOnlyDepthStencil::Bind(Graphics& gfx) noexcept
	{
		assert("OutputonlyDepthStencil cannot be bound as shader input" && false);
	}
}