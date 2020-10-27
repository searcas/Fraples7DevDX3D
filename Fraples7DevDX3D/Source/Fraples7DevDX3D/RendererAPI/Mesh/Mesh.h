#pragma once
#include "RendererAPI/GFXContextBase.h"
#include "RendererAPI/renderer.h"

namespace FraplesDev {
	class Mesh : public Renderer
	{
	public:
		Mesh(Graphics& gfx, std::vector<std::shared_ptr<GfxContext>>bindPtrs);
		void Render(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform)const noexcept(!IS_DEBUG);
		const DirectX::XMMATRIX GetTransformXM()const noexcept override;

	private:
		mutable DirectX::XMFLOAT4X4 _mTransform;
	};
}