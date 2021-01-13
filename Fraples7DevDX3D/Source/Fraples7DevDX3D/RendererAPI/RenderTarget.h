#pragma once
#include "GFXContext.h"
#include "RenderGraph/BufferResource.h"
#include <array>
namespace FraplesDev
{
	class RenderTarget : public GfxContext, public BufferResource
	{
	public:
		void BindAsBuffer(Graphics& gfx) noexcept(!IS_DEBUG) override;
		void BindAsBuffer(Graphics& gfx, BufferResource* depthStencil)noexcept(!IS_DEBUG) override;
		void BindAsBuffer(Graphics& gfx, class DepthStencil* depthStencil)noexcept(!IS_DEBUG);

		void Clear(Graphics& gfx)const noexcept(!IS_DEBUG) override;
		void Clear(Graphics& gfx, const std::array<float, 4>& color)const noexcept(!IS_DEBUG);
		UINT GetWidth()const noexcept;
		UINT GetHeight()const noexcept;
	private:
		void BindAsBuffer(Graphics& gfx, ID3D11DepthStencilView* pDepthStencilView)noexcept(!IS_DEBUG);
	protected:
		RenderTarget(Graphics& gfx, ID3D11Texture2D* pTexture);
		RenderTarget(Graphics& gfx, UINT width, UINT height);
	protected:
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView>_mTargetView;
		UINT _mWidth;
		UINT _mHeight;
	};

	class ShaderInputRenderTarget : public RenderTarget
	{
	public:
		ShaderInputRenderTarget(Graphics& gfx, UINT width, UINT height, UINT slot);
		void Bind(Graphics& gfx)noexcept(!IS_DEBUG) override;
	private: 
		UINT _mSlot;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>_mShaderResourceView;
	};

	// RT for Graphics to create RenderTarget for the back buffer
	class OutputOnlyRenderTarget : public RenderTarget 
	{
		friend Graphics;
	public:
		void Bind(Graphics& gfx)noexcept(!IS_DEBUG) override;
	private:
		OutputOnlyRenderTarget(Graphics& gfx, ID3D11Texture2D* pTexture);
	};
}
