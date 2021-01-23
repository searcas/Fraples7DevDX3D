#pragma once
#include "RendererAPI/GFXContext.h"
namespace FraplesDev
{
	class OutputOnlyDepthStencil;
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