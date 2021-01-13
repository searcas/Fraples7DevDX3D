#pragma once
#include "RendererAPI/GFXContext.h"
#include <memory>
#include <optional>
#include <array>
namespace FraplesDev
{

	class Blending : public GfxContext
	{
	public:
		Blending(Graphics& gfx, bool blending, std::optional<float>factor = {});
		void Bind(Graphics& gfx)noexcept(!IS_DEBUG) override;
		void SetFactor(float factor)noexcept(!IS_DEBUG);
		float GetFactor()noexcept(!IS_DEBUG);
		static std::shared_ptr<Blending>Resolve(Graphics& gfx, bool blending, std::optional<float>factor = {});
		static std::string GenerateUID(bool blending,std::optional<float>factor);
		std::string GetUID()const noexcept override;
	protected:
		Microsoft::WRL::ComPtr<ID3D11BlendState>_mPBlender;
		bool _mBlending;
		std::optional<std::array<float, 4>>_mFactors;
	private:

	};
}