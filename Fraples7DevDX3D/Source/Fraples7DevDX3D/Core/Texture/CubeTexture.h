#pragma once
#include "RendererAPI/GFXContext.h"
namespace FraplesDev
{
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
}