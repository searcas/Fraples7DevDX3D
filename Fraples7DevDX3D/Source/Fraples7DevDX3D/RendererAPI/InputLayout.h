#pragma once
#include "GFXContext.h"
#include "Core/MetaProgramming/Vertex.h"
#include <memory>
namespace FraplesDev
{
	class InputLayout : public GfxContext
	{
	public:
		InputLayout(Graphics& gfx, MP::VertexLayout layout, ID3DBlob* pVertexShaderByteCode) ;
		void Bind(Graphics& gfx)noexcept override;
		static std::shared_ptr<GfxContext> Resolve(Graphics& gfx, const MP::VertexLayout& layout, ID3DBlob* pVertexShaderByteCode);
		static std::string GenerateUID(const MP::VertexLayout& layout, ID3DBlob* pVertexShaderByteCode = nullptr);
		std::string GetUID()const noexcept;
	protected:
		MP::VertexLayout _mLayout;
		Microsoft::WRL::ComPtr<ID3D11InputLayout>_mInputLayout;
	};

	
}