#pragma once
#include "GFXContext.h"
#include "RenderGraph/BufferResource.h"
#include <optional>
#include <array>
namespace FraplesDev
{
	class Surface;

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
		void Dumpy(Graphics& gfx, const std::string& path)const;
		Surface ToSurface(Graphics& gfx)const;
		std::pair<Microsoft::WRL::ComPtr<ID3D11Texture2D>, D3D11_TEXTURE2D_DESC>MakeStaging(Graphics& gfx)const;
	private:
		void BindAsBuffer(Graphics& gfx, ID3D11DepthStencilView* pDepthStencilView)noexcept(!IS_DEBUG);
	protected:
		RenderTarget(Graphics& gfx, ID3D11Texture2D* pTexture, std::optional<UINT>face);
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
		OutputOnlyRenderTarget(Graphics& gfx, ID3D11Texture2D* pTexture, std::optional<UINT>face = {});
	};
}
