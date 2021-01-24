#include "ShadowMappingPass.h"
#include "RendererAPI/Stencil/DepthStencil.h"
#include "RendererAPI/Stencil/Stencil.h"
#include "RendererAPI/Blending/Blending.h"
#include "RendererAPI/Rasterizer/Rasterizer.h"
#include "RendererAPI/ShadowMapping/ShadowRasterizer.h"
#include "RendererAPI/NullPixelShader.h"
#include "RendererAPI/RenderGraph/Source.h"
#include "RendererAPI/Camera/Camera.h"
#include "Core/Surface.h"
#include "Core/ViewPort.h"
#include "Core/Texture/CubeTexture.h"
#include "Core/Math/Math.h"
#include "RendererAPI/VertexShader.h"
#include "RendererAPI/PixelShader.h"
#include "RendererAPI/RenderTarget.h"
namespace FraplesDev
{
	void ShadowMappingPass::BindShadowCamera(const Camera& cam) noexcept
	{
		_mShadowCamera = &cam;
	}
	ShadowMappingPass::ShadowMappingPass(Graphics& gfx, std::string name)
		:RenderQueuePass(std::move(name))
	{
		_mDepthStencil = std::make_shared<OutputOnlyDepthStencil>(gfx, size, size);
		_mDepthCube = std::make_shared<CubeTargetTexture>(gfx, size, size, 3, DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT);

		AddBind(VertexShader::Resolve(gfx, "Shadow_VS.cso"));
		AddBind(PixelShader::Resolve(gfx, "Shadow_PS.cso"));

		AddBind(Stencil::Resolve(gfx, Stencil::Mode::Off));
		AddBind(Blending::Resolve(gfx, false));
		AddBind(std::make_shared<ViewPort>(gfx, (float)size, (float)size));
		AddBind(std::make_shared<Rasterizer>(gfx, false));
		RegisterSource(DirectContextSource<CubeTargetTexture>::Make("map", _mDepthCube));
		DirectX::XMStoreFloat4x4(&_mProjection, DirectX::XMMatrixPerspectiveFovLH( PI / 2.0f, 1.0f, 0.5f, 100.0f));
		// +X
		DirectX::XMStoreFloat3(&_mCameraDirections[0], DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f));
		DirectX::XMStoreFloat3(&_mCameraUps[0], DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));

		// -x 
		DirectX::XMStoreFloat3(&_mCameraDirections[1], DirectX::XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f));
		DirectX::XMStoreFloat3(&_mCameraUps[1], DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));

		// +Y
		DirectX::XMStoreFloat3(&_mCameraDirections[2], DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
		DirectX::XMStoreFloat3(&_mCameraUps[2], DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f));

		// -y
		DirectX::XMStoreFloat3(&_mCameraDirections[3], DirectX::XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f));
		DirectX::XMStoreFloat3(&_mCameraUps[3], DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f));

		// +z
		DirectX::XMStoreFloat3(&_mCameraDirections[4], DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f));
		DirectX::XMStoreFloat3(&_mCameraUps[4], DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));

		// -z
		DirectX::XMStoreFloat3(&_mCameraDirections[5], DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f));
		DirectX::XMStoreFloat3(&_mCameraUps[5], DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
	}
	void ShadowMappingPass::Execute(Graphics& gfx) const noexcept(!IS_DEBUG)
	{
		using namespace DirectX;
		const auto pos = DirectX::XMLoadFloat3(&_mShadowCamera->GetPos());
		gfx.SetProjection(DirectX::XMLoadFloat4x4(&_mProjection));

		for (size_t i = 0; i < 0b110; i++)
		{
			auto rt = _mDepthCube->GetRenderTarget(i);
			rt->Clear(gfx);
			_mDepthStencil->Clear(gfx);
			SetRenderTarget(std::move(rt));
			const auto lootAt = pos + DirectX::XMLoadFloat3(&_mCameraDirections[i]);
			gfx.SetCamera(DirectX::XMMatrixLookAtLH(pos, lootAt, DirectX::XMLoadFloat3(&_mCameraUps[i])));
			RenderQueuePass::Execute(gfx);
		}
	}
	void ShadowMappingPass::DumpShadowMap(Graphics&gfx, const std::string& path)
	{
	//	for (size_t i = 0; i < 0b110; i++)
	//	{
	//		auto d = _mDepthCube->GetDepthBuffer(i);
	//		d->ToSurface(gfx).Save(path + std::to_string(i) + ".png");
	//		d->Dumpy(gfx, path + std::to_string(i) + ".npy");
	//	}
	}
	void ShadowMappingPass::SetRenderTarget(std::shared_ptr<RenderTarget> rt) const
	{
		const_cast<ShadowMappingPass*>(this)->_mRenderTarget = std::move(rt);
	}
}