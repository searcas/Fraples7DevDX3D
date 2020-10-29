#pragma once
#include "GFXContext.h"
#include <memory>
namespace FraplesDev
{
	class PixelShader : public GfxContext
	{
	public:
		PixelShader(Graphics& gfx, const std::string& path);
		void Bind(Graphics& gfx) noexcept override;
		static std::shared_ptr<GfxContext>Resolve(Graphics& gfx, const std::string& path);
		static std::string GenerateUID(const std::string& path);
		std::string GetUID()const noexcept;
	protected:
		std::string _mPath;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>_mPixelShader;
	};


}