#pragma once
#include "ConstantBuffers.h"
#include "Renderer.h"
#include <DirectXMath.h>
#include "../GraphicAPI/Graphics.h"

namespace FraplesDev
{
	class TransformCBuf : public GfxContext
	{
	protected:
		struct Transforms
		{
			DirectX::XMMATRIX modelView;
			DirectX::XMMATRIX modelViewProj;
		};
	public:
		TransformCBuf(Graphics& gfx,const Renderer& parent, UINT slot = 0u);
		void Bind(Graphics& gfx)noexcept override;
	protected:
		void UpdateBindImpl(Graphics& gfx, const Transforms& tf)noexcept;
		Transforms GetTransforms(Graphics& gfx)noexcept;
	private:
		static std::unique_ptr<VertexConstantBuffer<Transforms>>_spVcbuf;
		const Renderer& parent;
	};

}