#pragma once
#include "ConstantBuffers.h"
#include "Renderer.h"
#include "GraphicAPI/Graphics.h"
#include <DirectXMath.h>
namespace FraplesDev
{
	class TransformCBuf : public CloningContext
	{
	protected:
		struct Transforms
		{
			DirectX::XMMATRIX modelView;
			DirectX::XMMATRIX modelViewProj;
		};
	public:
		TransformCBuf(Graphics& gfx, UINT slot = 0u);
		void Bind(Graphics& gfx)noexcept override;
		void InitializeParentReference(const Renderer& parent)noexcept override;
		std::unique_ptr<CloningContext>Clone()const noexcept override;
	protected:
		void UpdateBindImpl(Graphics& gfx, const Transforms& tf)noexcept;
		Transforms GetTransforms(Graphics& gfx)noexcept;
	private:
		static std::unique_ptr<VertexConstantBuffer<Transforms>>_spVcbuf;
		const Renderer* _mPparent = nullptr;
	};

}