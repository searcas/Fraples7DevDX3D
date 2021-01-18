#pragma once
#include "RendererAPI/GFXContext.h"
#include "RendererAPI/GFXContextCodex.h"
#include "Core/Common/Exceptions/Macros/GraphicsThrowMacros.h"
namespace FraplesDev
{
	class Stencil : public GfxContext
	{
	public:
		enum class Mode
		{
			Off,
			Write,
			Mask,
			DepthOff,
			DepthReversed
		};
		Stencil(Graphics& gfx, Mode mode)
			:_mMode(mode)
		{
			D3D11_DEPTH_STENCIL_DESC dsDesc = CD3D11_DEPTH_STENCIL_DESC{ CD3D11_DEFAULT{} };
			if (mode == Mode::Write)
			{
				dsDesc.DepthEnable = FALSE;
				dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
				dsDesc.StencilEnable = TRUE;
				dsDesc.StencilWriteMask = 0xFF;
				dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
				dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
			}
			else if(mode == Mode::Mask)
			{
				dsDesc.DepthEnable = FALSE;
				dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
				dsDesc.StencilEnable = TRUE;
				dsDesc.StencilReadMask = 0xFF;
				dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_NOT_EQUAL;
				dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
				
			}
			else if (mode == Mode::DepthReversed)
			{
				dsDesc.DepthFunc = D3D11_COMPARISON_GREATER;
			}
			GetDevice(gfx)->CreateDepthStencilState(&dsDesc, &_mPstencil);
		}
		void Bind(Graphics& gfx)noexcept(!IS_DEBUG) override
		{
			INFOMAN_NOHR(gfx);
			FPL_GFX_THROW_INFO_ONLY(GetContext(gfx)->OMSetDepthStencilState(_mPstencil.Get(), 0xFF));
		}
		static std::shared_ptr<Stencil>Resolve(Graphics& gfx, Mode mode)
		{
			return Codex::Resolve<Stencil>(gfx, mode);
		}
		static std::string GenerateUID(Mode mode)
		{
			using namespace std::string_literals;
			const auto modeName = [mode]()
			{
				switch (mode)
				{
				case Stencil::Mode::Off:
					return "off"s;
					break;
				case Stencil::Mode::Write:
					return "write"s;
					break;
				case Stencil::Mode::Mask:
					return "mask"s;
					break;
				case Stencil::Mode::DepthOff:
					return "depthreversed"s;
				default:
					return "ERROR"s;
					break;
				}
			};
			return typeid(Stencil).name() + "#"s + modeName();
		}
		
	private:
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState>_mPstencil;
		Mode _mMode;
	};

}