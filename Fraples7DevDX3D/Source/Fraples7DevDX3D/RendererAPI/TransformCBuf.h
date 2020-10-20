#pragma once
#include "ConstantBuffers.h"
#include "Renderer.h"
#include <DirectXMath.h>
#include "../GraphicAPI/Graphics.h"

namespace FraplesDev
{
	class TransformCBuf : public GfxContext
	{
		struct Transforms
		{
			DirectX::XMMATRIX modelViewProj;
			DirectX::XMMATRIX model;
		};
	public:
		TransformCBuf(Graphics& gfx,const Renderer& parent);
		void Bind(Graphics& gfx)noexcept override;
	private:
		static std::unique_ptr<VertexConstantBuffer<Transforms>>_spVcbuf;
		const Renderer& parent;
	};

}