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
#include "RendererAPI/Filters/BlurPack.h"
namespace FraplesDev
{
	class FrameCommander
	{
	public:
		FrameCommander(Graphics& gfx);
		void Accept(Job job, size_t target)noexcept;
		void Execute(Graphics& gfx) noexcept(!IS_DEBUG);
		void ShowWindow(Graphics& gfx);
		void Reset()noexcept;
	private:
		std::array<Pass, 3>_mPasses;
		int _mDownFactor = 1;
		DepthStencil _mDepthStencil;
		std::optional<RenderTarget>_mRenderTarget1;
		std::optional<RenderTarget>_mRenderTarget2;
		BlurPack blur;
	private:
		std::shared_ptr<VertexBuffer>_mPvertexBufferFull;
		std::shared_ptr<IndexBuffer>_mPindexBufferFull;
		std::shared_ptr<VertexShader>_mPvertexShaderFull;
		std::shared_ptr<PixelShader>_mPpixelShaderFull;
		std::shared_ptr<InputLayout>_mPlayoutFull;
		std::shared_ptr<Sampler>_mPsamplerFullPoint;
		std::shared_ptr<Sampler>_mPsamplerFullBilin;
		std::shared_ptr<Blending>_mPblenderMerge;
	};
}