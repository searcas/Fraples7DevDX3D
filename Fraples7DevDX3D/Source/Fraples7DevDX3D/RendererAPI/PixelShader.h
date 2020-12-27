#pragma once
#include "GFXContext.h"
#include "Utility/Fraples7Utility.h"
#include <memory>
#include "GraphicAPI/Graphics.h"
namespace FraplesDev
{
	class PixelShader : public GfxContext
	{
	public:
		PixelShader(Graphics& gfx, const std::string& path);
		void Bind(Graphics& gfx) noexcept override;
		static std::shared_ptr<PixelShader>Resolve(Graphics& gfx, const std::string& path);
		static std::string GenerateUID(const std::string& path);
		std::string GetUID()const noexcept;
	protected:
		std::string _mPath;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>_mPixelShader;
	};
}