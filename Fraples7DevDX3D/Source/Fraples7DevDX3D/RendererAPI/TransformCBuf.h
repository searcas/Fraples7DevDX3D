#pragma once
#include "ConstantBuffers.h"
#include "Renderer.h"
#include <DirectXMath.h>


namespace FraplesDev
{
	class TransformCBuf : public GfxContext
	{
	public:
		TransformCBuf(Graphics& gfx,const Renderer& parent);
		void Bind(Graphics& gfx)noexcept override;
	private:
		static std::unique_ptr<VertexConstantBuffer<DirectX::XMMATRIX>>_spVcbuf;
		const Renderer& parent;
	};

}