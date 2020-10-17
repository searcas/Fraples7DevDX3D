#pragma once
#include "../../RendererAPI/GFXContext.h"
#include "../Surface.h"


namespace FraplesDev
{
	class Texture : public GfxContext
	{
	public:
		Texture(Graphics& gfx, const Surface& surfcae);
		void Bind(Graphics& gfx)noexcept override;
		

	protected:
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _mpTextureView;
	};

}