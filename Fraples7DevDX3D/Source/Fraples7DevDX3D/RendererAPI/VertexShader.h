#pragma once
#include "GFXContext.h"

namespace FraplesDev
{
	class VertexShader : public GfxContext
	{
	public:
		VertexShader(Graphics& gfx, const std::wstring& path);
		void Bind(Graphics& gfx)noexcept override;
		ID3DBlob* GetBytecode() const noexcept;
	

	protected:
		Microsoft::WRL::ComPtr<ID3DBlob>_mpBytecodeBlob;
		Microsoft::WRL::ComPtr<ID3D11VertexShader>_mpVertexShader;

	};

}