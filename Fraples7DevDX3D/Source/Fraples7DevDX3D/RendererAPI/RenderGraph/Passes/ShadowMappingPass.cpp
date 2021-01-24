#include "ShadowMappingPass.h"
#include "RendererAPI/Stencil/DepthStencil.h"
#include "RendererAPI/Stencil/Stencil.h"
#include "RendererAPI/Blending/Blending.h"
#include "RendererAPI/ShadowMapping/ShadowRasterizer.h"
#include "RendererAPI/VertexShader.h"
#include "RendererAPI/NullPixelShader.h"
#include "RendererAPI/RenderGraph/Source.h"
#include "RendererAPI/Camera/Camera.h"
#include "Core/Surface.h"
#include "Core/ViewPort.h"
#include "Core/Texture/CubeTexture.h"

namespace FraplesDev
{
	void ShadowMappingPass::BindShadowCamera(const Camera& cam) noexcept
	{
		_mShadowCamera = &cam;
	}
	ShadowMappingPass::ShadowMappingPass(Graphics& gfx, std::string name)
		:RenderQueuePass(std::move(name))
	{
		
		_mDepthCube = std::make_shared<DepthCubeTexture>(gfx, size, 3);
		AddBind(VertexShader::Resolve(gfx, "Shadow_VS.cso"));
		AddBind(NullPixelShader::Resolve(gfx));
		AddBind(Stencil::Resolve(gfx, Stencil::Mode::Off));
		AddBind(Blending::Resolve(gfx, false));
		AddBind(std::make_shared<ViewPort>(gfx, (float)size, (float)size));
		AddBind(std::make_shared<ShadowRasterizer>(gfx, 50, 2.0f, 0.1f));
		RegisterSource(DirectContextSource<DepthCubeTexture>::Make("map", _mDepthCube));
		DirectX::XMStoreFloat4x4(&_mProjection, DirectX::XMMatrixPerspectiveLH(1.0f, 1.0f, 0.5f, 100.0f));
		// +X
		DirectX::XMStoreFloat3(&_mCameraDirections[0], DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f));
		DirectX::XMStoreFloat3(&_mCameraUps[0], DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));

		// -x 
		DirectX::XMStoreFloat3(&_mCameraDirections[1], DirectX::XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f));
		DirectX::XMStoreFloat3(&_mCameraUps[1], DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));

		// +Y
		DirectX::XMStoreFloat3(&_mCameraDirections[2], DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
		DirectX::XMStoreFloat3(&_mCameraUps[2], DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f));

		// -y
		DirectX::XMStoreFloat3(&_mCameraDirections[3], DirectX::XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f));
		DirectX::XMStoreFloat3(&_mCameraUps[3], DirectX::XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f));

		// +z
		DirectX::XMStoreFloat3(&_mCameraDirections[4], DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f));
		DirectX::XMStoreFloat3(&_mCameraUps[4], DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));

		// -z
		DirectX::XMStoreFloat3(&_mCameraDirections[5], DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f));
		DirectX::XMStoreFloat3(&_mCameraUps[5], DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));

		//set the depth buffer to *something* so that the rg validation doesn't scream

		SetDepthBuffer(_mDepthCube->GetDepthBuffer(0));

	}
	void ShadowMappingPass::Execute(Graphics& gfx) const noexcept(!IS_DEBUG)
	{
		using namespace DirectX;
		const auto pos = DirectX::XMLoadFloat3(&_mShadowCamera->GetPos());
		gfx.SetProjection(DirectX::XMLoadFloat4x4(&_mProjection));
		for (size_t i = 0; i < 0b110; i++)
		{

			auto d = _mDepthCube->GetDepthBuffer(i);
			d->Clear(gfx);
			SetDepthBuffer(std::move(d));
			const auto lootAt = pos + DirectX::XMLoadFloat3(&_mCameraDirections[i]);
			gfx.SetCamera(DirectX::XMMatrixLookAtLH(pos, lootAt, DirectX::XMLoadFloat3(&_mCameraUps[i])));
			RenderQueuePass::Execute(gfx);
		}
	}
	void ShadowMappingPass::DumpShadowMap(Graphics&gfx, const std::string& path)
	{
		for (size_t i = 0; i < 0b110; i++)
		{
			auto d = _mDepthCube->GetDepthBuffer(i);
			d->ToSurface(gfx).Save(path + std::to_string(i) + ".png");
		}
	}
	void ShadowMappingPass::SetDepthBuffer(std::shared_ptr<DepthStencil> ds) const
	{
		const_cast<ShadowMappingPass*>(this)->_mDepthStencil = std::move(ds);
	}
}