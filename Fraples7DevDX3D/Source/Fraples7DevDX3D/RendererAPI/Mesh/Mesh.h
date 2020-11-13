#pragma once
#include "RendererAPI/GFXContextBase.h"
#include "RendererAPI/renderer.h"
#include "Material.h"

namespace FraplesDev {
	class Mesh : public Renderer
	{
	public:
		Mesh(Graphics& gfx, const Material& mat, const aiMesh& mesh)noexcept(!IS_DEBUG);
		void Submit(class FrameCommander& frame, DirectX::FXMMATRIX accumulatedTransform)const noexcept(!IS_DEBUG);
		const DirectX::XMMATRIX GetTransformXM()const noexcept override;

	private:
		mutable DirectX::XMFLOAT4X4 _mTransform;
	};
}