#pragma once
#include "RendererAPI/GFXContext.h"
#include "GraphicAPI/Graphics.h"
#include "RendererAPI/RenderGraph/BufferResource.h"
#include <d3d11.h>

namespace FraplesDev
{
	class DepthStencil : public BufferResource, public GfxContext
	{
		friend class RenderTarget;
	public:
		void BindAsBuffer(Graphics& gfx) noexcept(!IS_DEBUG) override;
		void BindAsBuffer(Graphics& gfx, BufferResource* renderTarget)noexcept(!IS_DEBUG) override;
		void BindAsBuffer(Graphics& gfx, RenderTarget* renderTarget)noexcept(!IS_DEBUG);
		void BindAsDepthStencil(Graphics& gfx)const noexcept(!IS_DEBUG);
		void Clear(Graphics& gfx)const noexcept(!IS_DEBUG);
	protected:
		DepthStencil(Graphics& gfx, UINT width, UINT height, bool canBindShaderInput);
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView>_mDepthStencilView;
	};

	class ShaderInputDepthStencil : public DepthStencil
	{
	public:
		ShaderInputDepthStencil(Graphics& gfx, UINT slot);
		ShaderInputDepthStencil(Graphics& gfx, UINT width, UINT height, UINT slot);
		void Bind(Graphics& gfx)noexcept(!IS_DEBUG) override;
	private:
		UINT _mSlot;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>_mShaderResourceView;
	};
	class OutputOnlyDepthStencil : public DepthStencil
	{
	public:
		OutputOnlyDepthStencil(Graphics& gfx);
		OutputOnlyDepthStencil(Graphics& gfx, UINT width, UINT height);
		void Bind(Graphics& gfx)noexcept(!IS_DEBUG) override;
	};
}