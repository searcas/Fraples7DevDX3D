#include "ShadowMappingPass.h"
#include "RendererAPI/Stencil/DepthStencil.h"
#include "RendererAPI/Stencil/Stencil.h"
#include "RendererAPI/Blending/Blending.h"
#include "RendererAPI/VertexShader.h"
#include "RendererAPI/NullPixelShader.h"
#include "RendererAPI/RenderGraph/Source.h"
#include "RendererAPI/Camera/Camera.h"
#include "Core/Surface.h"
namespace FraplesDev
{
	void ShadowMappingPass::BindShadowCamera(const Camera& cam) noexcept
	{
		_mShadowCamera = &cam;
	}
	ShadowMappingPass::ShadowMappingPass(Graphics& gfx, std::string name)
		:RenderQueuePass(std::move(name))
	{
		_mDepthStencil = std::make_unique<ShaderInputDepthStencil>(gfx, 3, DepthStencil::Usage::ShadowDepth);
		AddBind(VertexShader::Resolve(gfx, "Solid_VS.cso"));
		AddBind(NullPixelShader::Resolve(gfx));
		AddBind(Stencil::Resolve(gfx, Stencil::Mode::Off));
		AddBind(Blending::Blending::Resolve(gfx, false));
		RegisterSource(DirectContextSource<DepthStencil>::Make("map", _mDepthStencil));

	}
	void ShadowMappingPass::Execute(Graphics& gfx) const noexcept(!IS_DEBUG)
	{
		_mDepthStencil->Clear(gfx);
		_mShadowCamera->BindGraphics(gfx);
		RenderQueuePass::Execute(gfx);
	}
	void ShadowMappingPass::DumpShadowMap(Graphics&gfx, const std::string& path)
	{
		_mDepthStencil->ToSurface(gfx).Save(path);
	}
}