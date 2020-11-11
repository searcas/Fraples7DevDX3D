#pragma once
#include "RendererAPI/GFXContextBase.h"
#include "RendererAPI/renderer.h"

namespace FraplesDev {
	class Mesh : public Renderer
	{
	public:
		using Renderer::Renderer;

		void Submit(FrameCommander& frame, DirectX::FXMMATRIX accumulatedTransform)const noexcept(!IS_DEBUG);
		const DirectX::XMMATRIX GetTransformXM()const noexcept override;

	private:
		mutable DirectX::XMFLOAT4X4 _mTransform;
	};
}