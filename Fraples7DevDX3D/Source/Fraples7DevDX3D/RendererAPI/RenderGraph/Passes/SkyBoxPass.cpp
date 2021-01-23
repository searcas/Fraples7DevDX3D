#include "SkyBoxPass.h"
#include "RendererAPI/RenderTarget.h"
#include "RendererAPI/Stencil/DepthStencil.h"
#include "RendererAPI/Stencil/Stencil.h"
#include "RendererAPI/Rasterizer/Rasterizer.h"
#include "Core/Sampler.h"
#include "RendererAPI/Camera/Camera.h"
#include "RendererAPI/PixelShader.h"
#include "RendererAPI/VertexShader.h"
#include "RendererAPI/VertexBuffer.h"
#include "RendererAPI/IndexBuffer.h"
#include "RendererAPI/InputLayout.h"
#include "RendererAPI/Topology.h"
#include "Objects/Cube.h"
#include "RendererAPI/TransformCBuf/SkyBoxTransformCBuf.h"
#include "Core/Texture/CubeTexture.h"
namespace FraplesDev
{
	SkyBoxPass::SkyBoxPass(Graphics& gfx, std::string name)
		:BindingPass(std::move(name))
	{
		RegisterSync(DirectBufferSync<RenderTarget>::Make("renderTarget", _mRenderTarget));
		RegisterSync(DirectBufferSync<DepthStencil>::Make("depthStencil", _mDepthStencil));
		AddBind(std::make_shared<CubeTexture>(gfx, "Images\\SpaceBox"));
		AddBind(Stencil::Resolve(gfx, Stencil::Mode::DepthFirst));
		AddBind(Sampler::Resolve(gfx, Sampler::Type::Bilinear));
		AddBind(Rasterizer::Resolve(gfx, true));
		AddBind(std::make_shared<SkyBoxTransformCBuf>(gfx));
		AddBind(PixelShader::Resolve(gfx, "SkyBox_PS.cso"));
		AddBind(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
		{
			auto pvs = VertexShader::Resolve(gfx, "SkyBox_VS.cso");
			auto model = Cube::Make();
			model.Transform(DirectX::XMMatrixScaling(3.0f, 3.0f, 3.0f));
			const auto geometryTag = "$cube_map";
			AddBind(VertexBuffer::Resolve(gfx, geometryTag, std::move(model._mVertices)));
			_mCount = (UINT)model._mIndices.size();
			AddBind(IndexBuffer::Resolve(gfx, geometryTag, std::move(model._mIndices)));
			AddBind(InputLayout::Resolve(gfx, model._mVertices.GetLayout(), *pvs));
			AddBind(std::move(pvs));
		}
		RegisterSource(DirectBufferSource<RenderTarget>::Make("renderTarget", _mRenderTarget));
		RegisterSource(DirectBufferSource<DepthStencil>::Make("depthStencil", _mDepthStencil));
	}
	void SkyBoxPass::BindMainCamera(const Camera& cam) noexcept
	{
		_mMainCam = &cam;
	}
	void SkyBoxPass::Execute(Graphics& gfx) const noexcept
	{
		assert(_mMainCam);
		_mMainCam->BindGraphics(gfx);
		BindAll(gfx);
		gfx.RenderIndexed(_mCount);
	}
}