#pragma once
#include "RendererAPI/GFXContext.h"

namespace FraplesDev
{
	class Camera;
	template<typename T>
	class VertexConstantBuffer;
	class SkyBoxTransformCBuf : public GfxContext
	{
	protected:
		struct Transforms
		{
			DirectX::XMMATRIX viewProj;
		};
	public:
		SkyBoxTransformCBuf(Graphics& gfx, UINT slot = 0u);
		void Bind(Graphics& gfx)noexcept(!IS_DEBUG)override;
		void UpdateBindImpl(Graphics& gfx, const Transforms& tf)noexcept(!IS_DEBUG);
		Transforms GetTransforms(Graphics& gfx)noexcept(!IS_DEBUG);
	private:
		std::unique_ptr<VertexConstantBuffer<Transforms>>_mVertexConstBuffer;
	};

	
}