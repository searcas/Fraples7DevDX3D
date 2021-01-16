#pragma once
#include "GFXContext.h"
#include "Core/MetaProgramming/Vertex.h"
#include <memory>
namespace FraplesDev
{
	class InputLayout : public GfxContext
	{
	public:
		InputLayout(Graphics& gfx, MP::VertexLayout layout, const class VertexShader& vs) ;
		void Bind(Graphics& gfx)noexcept(!IS_DEBUG) override;
		const MP::VertexLayout GetLayout()const noexcept;
		static std::shared_ptr<InputLayout> Resolve(Graphics& gfx,const MP::VertexLayout& layout, const VertexShader& vs);
		static std::string GenerateUID(const MP::VertexLayout& layout, const VertexShader& vs);
		std::string GetUID()const noexcept;
	protected:
		std::string _mVertexShaderUID;
		MP::VertexLayout _mLayout;
		Microsoft::WRL::ComPtr<ID3D11InputLayout>_mInputLayout;
	};

	
}