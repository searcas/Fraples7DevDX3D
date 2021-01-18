#pragma once
#include "RendererAPI/GFXContext.h"
#include "GraphicAPI/Graphics.h"
#include "RendererAPI/RenderGraph/BufferResource.h"
#include <d3d11.h>

namespace FraplesDev
{
	class Surface;

	class DepthStencil : public BufferResource, public GfxContext
	{
		friend class RenderTarget;
	public:
		enum class Usage
		{
			DepthStencil,
			ShadowDepth,
		};
	public:
		void BindAsBuffer(Graphics& gfx) noexcept(!IS_DEBUG) override;
		void BindAsBuffer(Graphics& gfx, BufferResource* renderTarget)noexcept(!IS_DEBUG) override;
		void BindAsBuffer(Graphics& gfx, RenderTarget* renderTarget)noexcept(!IS_DEBUG);
		void BindAsDepthStencil(Graphics& gfx)const noexcept(!IS_DEBUG);
		void Clear(Graphics& gfx)const noexcept(!IS_DEBUG);
		const Surface& ToSurface(Graphics& gfx)const;
		inline const unsigned int& GetWidth() const { return _mWidth; };
		inline const unsigned int& GetHeight()const { return _mHeight; };
	protected:
		DepthStencil(Graphics& gfx, UINT width, UINT height, bool canBindShaderInput,Usage usage);
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView>_mDepthStencilView;
		unsigned int _mWidth;
		unsigned int _mHeight;
	};

	class ShaderInputDepthStencil : public DepthStencil
	{
	public:
		ShaderInputDepthStencil(Graphics& gfx, UINT slot, Usage usage = DepthStencil::Usage::DepthStencil);
		ShaderInputDepthStencil(Graphics& gfx, UINT width, UINT height, UINT slot, Usage usage = DepthStencil::Usage::DepthStencil);
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