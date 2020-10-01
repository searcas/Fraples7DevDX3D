#pragma once
#include "GFXContext.h"


namespace FraplesDev
{
	class IndexBuffer : public GfxContext
	{
	public:
		IndexBuffer(Graphics& gfx, const std::vector<unsigned short>&indices);
		void Bind(Graphics& gfx) noexcept override;
		UINT GetCount() const noexcept;
	protected:
		UINT _mCount;
		Microsoft::WRL::ComPtr<ID3D11Buffer>_mpIndexBuffer;
	};
}