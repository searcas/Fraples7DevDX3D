#pragma once
#include "FrameCommander.h"

namespace FraplesDev
{
		FrameCommander::FrameCommander(Graphics& gfx)
			:_mDepthStencil(gfx, gfx.GetWidth(), gfx.GetHeight()),
			_mRenderTarget1({ gfx, gfx.GetWidth() / 2, gfx.GetHeight() / 2 }),
			_mRenderTarget2({ gfx, gfx.GetWidth() / 2, gfx.GetHeight() / 2 }),
			blur(gfx, 7, 2.6f, "BlurOutline_PS.cso")
		{
			MP::VertexLayout lay;
			lay.Append(MP::ElementType::Position2D);

			MP::VertexBuffer buffFull{ lay };
			buffFull.EmplaceBack(DirectX::XMFLOAT2{ -1, 1 });
			buffFull.EmplaceBack(DirectX::XMFLOAT2{ 1, 1 });
			buffFull.EmplaceBack(DirectX::XMFLOAT2{ -1,-1 });
			buffFull.EmplaceBack(DirectX::XMFLOAT2{ 1,-1 });
			_mPvertexBufferFull = VertexBuffer::Resolve(gfx, "$Full", std::move(buffFull));
			std::vector<unsigned short> indices = { 0, 1, 2, 1, 3, 2 };
			_mPindexBufferFull = IndexBuffer::Resolve(gfx, "$Full", std::move(indices));

			// setup fullscreen shaders
			_mPvertexShaderFull = VertexShader::Resolve(gfx, "FullScreen_VS.cso");
			_mPlayoutFull = InputLayout::Resolve(gfx, lay, _mPvertexShaderFull->GetByteCode());
			_mPsamplerFullPoint = Sampler::Resolve(gfx, false, true);
			_mPsamplerFullBilin = Sampler::Resolve(gfx, true, true);
			_mPblenderMerge = Blending::Resolve(gfx, true);
		}
		void FrameCommander::Accept(Job job, size_t target)noexcept
		{
			_mPasses[target].Accept(job);
		}
		void FrameCommander::Execute(Graphics& gfx) noexcept(!IS_DEBUG)
		{
			// Normally this would be a loop with each pass
			// defining it's setup / etc. and later on it would
			// be a complex graph with parallel execution contingent
			// on input / output requirements

			//Setup render target used for normal passes
			_mDepthStencil.Clear(gfx);
			_mRenderTarget1->Clear(gfx);
			gfx.BindSwapBuffer(_mDepthStencil);


			// main phong lighting pass
			Blending::Resolve(gfx, false)->Bind(gfx);
			Stencil::Resolve(gfx, Stencil::Mode::Off)->Bind(gfx);
			_mPasses[0].Execute(gfx);
			// outline masking pass
			Stencil::Resolve(gfx, Stencil::Mode::Write)->Bind(gfx);
			NullPixelShader::Resolve(gfx)->Bind(gfx);
			_mPasses[1].Execute(gfx);
			//outline drawing pass
			_mRenderTarget1->BindAsTarget(gfx);
			Stencil::Resolve(gfx, Stencil::Mode::Off)->Bind(gfx);
			_mPasses[2].Execute(gfx);
			// Fullscreen blur h-pass
			_mRenderTarget2->BindAsTarget(gfx);
			_mRenderTarget1->BindAsTexture(gfx, 0);



			_mPvertexBufferFull->Bind(gfx);
			_mPindexBufferFull->Bind(gfx);
			_mPvertexShaderFull->Bind(gfx);
			_mPlayoutFull->Bind(gfx);
			_mPsamplerFullPoint->Bind(gfx);

			blur.Bind(gfx);
			blur.SetHorizontal(gfx);
			gfx.RenderIndexed(_mPindexBufferFull->GetCount());
			// fullscreen blur v-pass + combine
			gfx.BindSwapBuffer(_mDepthStencil);
			_mRenderTarget2->BindAsTexture(gfx, 0u);
			_mPblenderMerge->Bind(gfx);
			_mPsamplerFullBilin->Bind(gfx);
			Stencil::Resolve(gfx, Stencil::Mode::Mask)->Bind(gfx);
			blur.SetVertical(gfx);
			gfx.RenderIndexed(_mPindexBufferFull->GetCount());
		}
		void FrameCommander::ShowWindow(Graphics& gfx)
		{
			if (ImGui::Begin("Blur"))
			{
				if (ImGui::SliderInt("Down Factor", &_mDownFactor, 1, 16))
				{
					_mRenderTarget1.emplace(gfx, gfx.GetWidth() / _mDownFactor, gfx.GetHeight() / _mDownFactor);
					_mRenderTarget2.emplace(gfx, gfx.GetWidth() / _mDownFactor, gfx.GetHeight() / _mDownFactor);
				}
				blur.RenderWidgets(gfx);
			}
			ImGui::End();
		}
		void FrameCommander::Reset()noexcept
		{
			for (auto& p : _mPasses)
			{
				p.Reset();
			}
		}

}