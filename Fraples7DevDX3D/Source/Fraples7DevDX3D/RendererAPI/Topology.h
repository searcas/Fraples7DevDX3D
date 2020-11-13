#pragma once
#include "GFXContext.h"
#include <memory>
namespace FraplesDev
{
	class Topology : public GfxContext
	{
	public:
		Topology(Graphics& gfx, D3D11_PRIMITIVE_TOPOLOGY type);
		void Bind(Graphics& gfx)noexcept override;
		static std::shared_ptr<Topology>Resolve(Graphics& gfx, D3D11_PRIMITIVE_TOPOLOGY type = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		static std::string GenerateUID(D3D11_PRIMITIVE_TOPOLOGY type) noexcept;
		std::string GetUID()const noexcept;
	private:
		D3D11_PRIMITIVE_TOPOLOGY _mType;
	};

}