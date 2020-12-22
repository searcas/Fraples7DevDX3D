#pragma once
#include "GraphicAPI/Graphics.h"
#include "RenderGraph/BufferResource.h"
#include "RendererAPI/Stencil/DepthStencil.h"
#include <array>
namespace FraplesDev
{
	class RenderTarget : public BufferResource, public GfxContext
	{
	public:
		RenderTarget(Graphics& gfx, UINT width, UINT height);
		RenderTarget(Graphics& gfx, ID3D11Texture2D* pTexture);
		void BindAsBuffer(Graphics& gfx)noexcept override;
		void BindAsBuffer(Graphics& gfx, BufferResource* depthStencil)noexcept override;
		void BindAsBuffer(Graphics& gfx, DepthStencil* depthStencil)noexcept;
		

		void Clear(Graphics& gfx, const std::array<float, 4>& color)const noexcept;
		void Clear(Graphics& gfx)const noexcept;
		UINT GetWidth()const noexcept;
		UINT GetHeight()const noexcept;
	private:
		void BindAsBuffer(Graphics& gfx, ID3D11DepthStencilView* pDepthStencilView)noexcept;
	protected:
		RenderTarget(Graphics& gfx, ID3D11Texture2D* pTexture);
		RenderTarget(Graphics& gfx, UINT width, UINT height);
	protected:
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView>_mPtargetView;
		UINT _mWidth;
		UINT _mHeight;
	};

	class ShaderInputRenderTarget : public RenderTarget
	{
	public:
		ShaderInputRenderTarget(Graphics& gfx, UINT width, UINT height, UINT slot);
		void Bind(Graphics& gfx)noexcept override;
	private: 
		UINT _mSlot;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>_mShaderResourceView;
	};

	// RT for Graphics to create RenderTarget for the back buffer
	class OutputOnlyRenderTarget : public RenderTarget
	{
		friend Graphics;
	public:
		void Bind(Graphics& gfx)noexcept override;
	private:
		OutputOnlyRenderTarget(Graphics& gfx, ID3D11Texture2D* pTexture);

	private:
	};


	}
}
