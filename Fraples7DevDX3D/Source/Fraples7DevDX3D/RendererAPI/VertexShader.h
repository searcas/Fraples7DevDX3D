#pragma once
#include "GFXContext.h"
#include <memory>
namespace FraplesDev
{
	class VertexShader : public GfxContext
	{
	public:
		VertexShader(Graphics& gfx, const std::string& path);
		void Bind(Graphics& gfx)noexcept override;
		ID3DBlob* GetBytecode() const noexcept;
		static std::shared_ptr<VertexShader>Resolve(Graphics& gfx, const std::string& path);
		static std::string GenerateUID(const std::string& path);
		std::string GetUID()const noexcept override;

	protected:
		std::string _mPath;
		Microsoft::WRL::ComPtr<ID3DBlob>_mpBytecodeBlob;
		Microsoft::WRL::ComPtr<ID3D11VertexShader>_mpVertexShader;

	};

}