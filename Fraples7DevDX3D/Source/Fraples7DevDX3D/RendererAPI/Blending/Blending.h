#pragma once
#include "RendererAPI/GFXContext.h"
#include <memory>
namespace FraplesDev
{

	class Blending : public GfxContext
	{
	public:
		Blending(Graphics& gfx, bool blending);
		void Bind(Graphics& gfx)noexcept override;
		static std::shared_ptr<Blending>Resolve(Graphics& gfx, bool blending);
		static std::string GenerateUID(bool blending);
		std::string GetUID()const noexcept override;
	protected:
		Microsoft::WRL::ComPtr<ID3D11BlendState>_mPBlender;
		bool _mBlending;

	private:

	};
}