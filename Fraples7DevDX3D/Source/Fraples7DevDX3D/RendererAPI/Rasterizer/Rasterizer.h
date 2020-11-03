#pragma once
#include "RendererAPI/GFXContext.h"
#include <memory>
namespace FraplesDev
{
	class Rasterizer : public GfxContext
	{
	public:
		Rasterizer(Graphics& gfx,bool twoSided);
		void Bind(Graphics& gfx)noexcept override;
		static std::shared_ptr<Rasterizer>Resolve(Graphics& gfx, bool twoSided);
		static std::string GenerateUID(bool twoSided);
		std::string GetUID()const noexcept override;
	private:

	protected:
		Microsoft::WRL::ComPtr<ID3D11RasterizerState>_mRasterizerState;
		bool _mTwoSide = false;
	};
}