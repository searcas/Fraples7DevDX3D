#pragma once
#include "RendererAPI/GFXContextBase.h"
#include "RendererAPI/RendererBase.h"

namespace FraplesDev {
	class Mesh : public RendererBase<Mesh>
	{
	public:
		Mesh(Graphics& gfx, std::vector<std::unique_ptr<GfxContext>>bindPtrs);
		void Render(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform)const noexcept(!IS_DEBUG);
		const DirectX::XMMATRIX GetTransformXM()const noexcept override;

	private:
		mutable DirectX::XMFLOAT4X4 _mTransform;
	};
}