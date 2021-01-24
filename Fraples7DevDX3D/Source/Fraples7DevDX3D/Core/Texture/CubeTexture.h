#pragma once
#include "RendererAPI/GFXContext.h"
namespace FraplesDev
{
	class OutputOnlyDepthStencil;
	class OutputOnlyRenderTarget;
	class CubeTexture : public GfxContext
	{
	public:
		CubeTexture(Graphics& gfx,const std::string& path, UINT slot = 0);
		void Bind(Graphics& gfx)noexcept(!IS_DEBUG) override;
	private:
		unsigned int _mSlot;
	protected:
		std::string _mPath;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>_mTextureView;
	};
	class CubeTargetTexture : public GfxContext
	{
	public:
		CubeTargetTexture(Graphics& gfx, UINT width, UINT height, UINT slot = 0, DXGI_FORMAT format = DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM);
		void Bind(Graphics& gfx)noexcept(!IS_DEBUG)override;
		std::shared_ptr<OutputOnlyRenderTarget>GetRenderTarget(size_t index)const;
	private:
		unsigned int _mSlot;
	protected:
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>_mTextureView;
		std::vector<std::shared_ptr<OutputOnlyRenderTarget>>_mRenderTargets;

	};
	class DepthCubeTexture : public GfxContext
	{
	public:
		DepthCubeTexture(Graphics& gfx, UINT size, UINT slot = 0);
		void Bind(Graphics& gfx)noexcept(!IS_DEBUG);
		std::shared_ptr<OutputOnlyDepthStencil> GetDepthBuffer(size_t index)const;
	private:
		unsigned int _mSlot;
	protected:
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>_mTextureView;
		std::vector<std::shared_ptr<OutputOnlyDepthStencil>>_mDepthBuffers;
	};
}