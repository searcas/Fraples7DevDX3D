#pragma once
#include <array>
#include "RendererAPI/GFXContextBase.h"
#include "GraphicAPI/Graphics.h"
#include "Job.h"
#include "Pass.h"
#include "RendererAPI/Stencil.h"
#include "RendererAPI/NullPixelShader.h"
#include "Utility/SpeedLog.h"
#include "RendererAPI/Stencil/DepthStencil.h"
#include "RendererAPI/RenderTarget.h"
namespace FraplesDev
{
	class FrameCommander
	{
	public:
		FrameCommander(Graphics& gfx)
			:_mDepthStencil(gfx, gfx.GetWidth(), gfx.GetHeight()),
			_mRenderTarget(gfx, gfx.GetWidth(), gfx.GetHeight())
		{
			MP::VertexLayout lay;
			lay.Append(MP::ElementType::Position2D);

			MP::VertexBuffer buffFull{ lay };
			buffFull.EmplaceBack(DirectX::XMFLOAT2{ -1, 1 });
			buffFull.EmplaceBack(DirectX::XMFLOAT2{  1, 1 });
			buffFull.EmplaceBack(DirectX::XMFLOAT2{ -1,-1 });
			buffFull.EmplaceBack(DirectX::XMFLOAT2{  1,-1 });
			_mPvertexBufferFull = VertexBuffer::Resolve(gfx, "$Full", std::move(buffFull));
			std::vector<unsigned short> indices = { 0, 1, 2, 1, 3, 2 };
			_mPindexBufferFull = IndexBuffer::Resolve(gfx, "$Full", std::move(indices));

			// setup fullscreen shaders
			_mPpixelShaderFull = PixelShader::Resolve(gfx, "BlurOutline_PS.cso");
			_mPvertexShaderFull = VertexShader::Resolve(gfx, "FullScreen_VS.cso");
			_mPlayoutFull = InputLayout::Resolve(gfx, lay, _mPvertexShaderFull->GetByteCode());
			_mPsamplerFull = Sampler::Resolve(gfx, false, true);
			_mPblenderFull = Blending::Resolve(gfx, true);
		}
		void Accept(Job job, size_t target)noexcept
		{
			_mPasses[target].Accept(job);
		}
		void Execute(Graphics& gfx)const noexcept(!IS_DEBUG)
		{
			// Normally this would be a loop with each pass
			// defining it setup / etc. and later on it would
			// be a complex graph with parallel execution contingent
			// on input / output requirements

			//Setup render target used for normal passes
			_mDepthStencil.Clear(gfx);
			_mRenderTarget.Clear(gfx);
			gfx.BindSwapBuffer(_mDepthStencil);
			
			// main phong lighting pass
			Blending::Resolve(gfx, false)->Bind(gfx);
			Stencil::Resolve(gfx, Stencil::Mode::Off)->Bind(gfx);
			_mPasses[0].Execute(gfx);
			
			//outline masking pass
			Stencil::Resolve(gfx, Stencil::Mode::Write)->Bind(gfx);
			NullPixelShader::Resolve(gfx)->Bind(gfx);
			_mPasses[1].Execute(gfx);

			// Outline drawing pass
			_mRenderTarget.BindAsTarget(gfx);
			Stencil::Resolve(gfx, Stencil::Mode::Off)->Bind(gfx);
			_mPasses[2].Execute(gfx);
			// fullscreen blur + blend pass
			gfx.BindSwapBuffer(_mDepthStencil);
			_mRenderTarget.BindAsTexture(gfx, 0);
			_mPvertexBufferFull->Bind(gfx);
			_mPindexBufferFull->Bind(gfx);
			_mPvertexShaderFull->Bind(gfx);
			_mPpixelShaderFull->Bind(gfx);
			_mPlayoutFull->Bind(gfx);
			_mPsamplerFull->Bind(gfx);
			_mPblenderFull->Bind(gfx);
			Stencil::Resolve(gfx, Stencil::Mode::Mask)->Bind(gfx);
			gfx.RenderIndexed(_mPindexBufferFull->GetCount());
		}
		void Reset()noexcept
		{
			for (auto& p : _mPasses)
			{
				p.Reset();
			}
		}
	private:
		std::array<Pass, 3>_mPasses;
		 DepthStencil _mDepthStencil;
	private:
		RenderTarget _mRenderTarget;
		std::shared_ptr<VertexBuffer>_mPvertexBufferFull;
		std::shared_ptr<IndexBuffer>_mPindexBufferFull;
		std::shared_ptr<VertexShader>_mPvertexShaderFull;
		std::shared_ptr<PixelShader>_mPpixelShaderFull;
		std::shared_ptr<InputLayout>_mPlayoutFull;
		std::shared_ptr<Sampler>_mPsamplerFull;
		std::shared_ptr<Blending>_mPblenderFull;
	};
}