#pragma once
#include "RenderQueuePass.h"

namespace FraplesDev
{
	class Camera;
	class DepthCubeTexture;
	class ShadowMappingPass : public RenderQueuePass
	{
	public:
		void BindShadowCamera(const Camera& cam)noexcept;
		ShadowMappingPass(Graphics& gfx, std::string name);
		void Execute(Graphics& gfx)const noexcept(!IS_DEBUG) override;
		void DumpShadowMap(Graphics&, const std::string& path);
		void SetDepthBuffer(std::shared_ptr<DepthStencil>ds)const;
	private:
		static constexpr UINT size = 1000;
		const Camera* _mShadowCamera = nullptr;
		std::shared_ptr<DepthCubeTexture>_mDepthCube;
		DirectX::XMFLOAT4X4 _mProjection;
		std::vector<DirectX::XMFLOAT3>_mCameraDirections{ 6 };
		std::vector<DirectX::XMFLOAT3>_mCameraUps{ 6 };
	};
}
