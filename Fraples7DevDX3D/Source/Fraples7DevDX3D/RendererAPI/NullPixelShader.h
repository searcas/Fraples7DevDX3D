#pragma once
#include "GFXContext.h"
#include <memory>

namespace FraplesDev
{
	class NullPixelShader : public GfxContext
	{
	public:
		NullPixelShader(Graphics& gfx);
		void Bind(Graphics& gfx)noexcept(!IS_DEBUG) override;
		static std::shared_ptr<NullPixelShader>Resolve(Graphics& gfx);
		static std::string GenerateUID();
		std::string GetUID()const noexcept override;
	private:

	};
}