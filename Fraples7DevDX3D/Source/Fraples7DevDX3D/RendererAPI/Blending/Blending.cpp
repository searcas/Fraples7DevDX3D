#include "Blending.h"
#include "Core/Common/Exceptions/Macros/GraphicsThrowMacros.h"
#include "RendererAPI/GFXContextCodex.h"
namespace FraplesDev
{
	Blending::Blending(Graphics& gfx, bool blending, std::optional<float>factor_in): _mBlending(blending)
	{
		INFOMAN(gfx);

		if (factor_in)
		{
			_mFactors.emplace();
			_mFactors->fill(*factor_in);
		}
		D3D11_BLEND_DESC blendDesc = CD3D11_BLEND_DESC{ CD3D11_DEFAULT{} };

		auto& blenderDescriptor = blendDesc.RenderTarget[0];
		if (_mBlending)
		{
			blenderDescriptor.BlendEnable = TRUE;
			if (factor_in)
			{
				blenderDescriptor.SrcBlend = D3D11_BLEND_BLEND_FACTOR;
				blenderDescriptor.DestBlend = D3D11_BLEND_INV_BLEND_FACTOR;
			}
			else
			{
				
				blenderDescriptor.SrcBlend = D3D11_BLEND_SRC_ALPHA;
				blenderDescriptor.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			}
			blenderDescriptor.BlendOp = D3D11_BLEND_OP_ADD;
			blenderDescriptor.SrcBlendAlpha = D3D11_BLEND_ZERO;
			blenderDescriptor.DestBlendAlpha = D3D11_BLEND_ZERO;
			blenderDescriptor.BlendOpAlpha = D3D11_BLEND_OP_ADD;
			blenderDescriptor.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		}
		
		FPL_GFX_THROW_INFO(GetDevice(gfx)->CreateBlendState(&blendDesc, &_mPBlender));
	}
	void Blending::SetFactor(float factor) noexcept(!IS_DEBUG)
	{
		assert(_mFactors);
		return _mFactors->fill(factor);
	}
	float Blending::GetFactor() noexcept(!IS_DEBUG)
	{
		assert(_mFactors);
		return _mFactors->front();
	}
	void Blending::Bind(Graphics& gfx) noexcept
	{
		const float* data = _mFactors ? _mFactors->data() : nullptr;
		GetContext(gfx)->OMSetBlendState(_mPBlender.Get(), data, 0xFFFFFFFFu);
	}
	std::shared_ptr<Blending> Blending::Resolve(Graphics& gfx, bool blending, std::optional<float>factor_in)
	{
		return Codex::Resolve<Blending>(gfx, blending,factor_in);
	}
	std::string Blending::GenerateUID(bool blending, std::optional<float>factor_in)
	{
		using namespace std::string_literals;
		return typeid(Blending).name() + "#"s + (blending ? "blending"s : "noBlending"s) + (factor_in ? "#f"s + std::to_string(*factor_in) :"");
	}
	std::string Blending::GetUID() const noexcept
	{
		return GenerateUID(_mBlending, _mFactors ? _mFactors->front() : std::optional<float>{});
	}
}