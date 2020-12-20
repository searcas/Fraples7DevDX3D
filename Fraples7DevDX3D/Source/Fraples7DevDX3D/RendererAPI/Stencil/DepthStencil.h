#pragma once
#include "RendererAPI/GFXContext.h"
#include "RendererAPI/RenderGraph/BufferResource.h"


namespace FraplesDev
{
	class Graphics;

	class DepthStencil : public GfxContext, public BufferResource
	{
		friend class RenderTarget;
	public:
		DepthStencil(Graphics& gfx, UINT width, UINT height, bool canBindShaderInput);
		void BindAsBuffer(Graphics& gfx);
		void BindAsBuffer(Graphics& gfx, BufferResource* renderTarget)noexcept override;
		void BindAsBuffer(Graphics& gfx, RenderTarget* renderTarget)noexcept;
		void BindAsDepthStencil(Graphics& gfx)const noexcept;
		void Clear(Graphics& gfx)const noexcept;
	protected:
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView>_mPdepthStencilView;
	};

	class ShaderInputDepthStencil : public DepthStencil
	{
	public:
		ShaderInputDepthStencil(Graphics& gfx, UINT slot);
		ShaderInputDepthStencil(Graphics& gfx, UINT width, UINT height, UINT slot);
		void Bind(Graphics& gfx)noexcept override;
	private:
		UINT _mSlot;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>_mShaderResourceView;
	};
	class OutputOnlyDepthStencil : public DepthStencil
	{
	public:
		OutputOnlyDepthStencil(Graphics& gfx);
		OutputOnlyDepthStencil(Graphics& gfx, UINT width, UINT height);
		void Bind(Graphics& gfx)noexcept override;
	};
}