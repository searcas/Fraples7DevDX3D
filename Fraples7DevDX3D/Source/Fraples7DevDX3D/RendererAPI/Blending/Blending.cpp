#include "Blending.h"
#include "Core/Debugging/Exceptions/Macros/GraphicsThrowMacros.h"
#include "RendererAPI/GFXContextCodex.h"

namespace FraplesDev
{
	Blending::Blending(Graphics& gfx, bool blending): _mBlending(blending)
	{
		INFOMAN(gfx);

		D3D11_BLEND_DESC blendDesc = {};

		auto& blenderDescriptor = blendDesc.RenderTarget[0];
		if (_mBlending)
		{
			blenderDescriptor.BlendEnable = TRUE;
			blenderDescriptor.SrcBlend = D3D11_BLEND_SRC_ALPHA;
			blenderDescriptor.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			blenderDescriptor.BlendOp = D3D11_BLEND_OP_ADD;
			blenderDescriptor.SrcBlendAlpha = D3D11_BLEND_ZERO;
			blenderDescriptor.DestBlendAlpha = D3D11_BLEND_ZERO;
			blenderDescriptor.BlendOpAlpha = D3D11_BLEND_OP_ADD;
			blenderDescriptor.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		}
		else
		{
			blenderDescriptor.BlendEnable = FALSE;
			blenderDescriptor.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		}
		FPL_GFX_THROW_INFO(GetDevice(gfx)->CreateBlendState(&blendDesc, &_mPBlender));
	}
	void Blending::Bind(Graphics& gfx) noexcept
	{
		GetContext(gfx)->OMSetBlendState(_mPBlender.Get(), nullptr, 0xFFFFFFFFu);
	}
	std::shared_ptr<Blending> Blending::Resolve(Graphics& gfx, bool blending)
	{
		return Codex::Resolve<Blending>(gfx, blending);
	}
	std::string Blending::GenerateUID(bool blending)
	{
		using namespace std::string_literals;
		return typeid(Blending).name() + "#"s + (blending ? "blending" : "noBlending");
	}
	std::string Blending::GetUID() const noexcept
	{
		return GenerateUID(_mBlending);
	}
}