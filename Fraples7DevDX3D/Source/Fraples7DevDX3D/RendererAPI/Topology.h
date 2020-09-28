#pragma once
#include "GFXContext.h"

namespace FraplesDev
{
	class Topology : public GfxContext
	{
	public:
		Topology(Graphics& gfx, D3D11_PRIMITIVE_TOPOLOGY type);
		void Bind(Graphics& gfx)noexcept override;

	private:
		D3D11_PRIMITIVE_TOPOLOGY _mType;
	};

}