#pragma once

#include "GFXContext.h"
#include "../Core/Debugging/Exceptions/Macros/GraphicsThrowMacros.h"
#include "../Core/MetaProgramming/Vertex.h"
#include <memory>
namespace FraplesDev
{
	class VertexBuffer : public GfxContext
	{
	public:
		VertexBuffer(Graphics& gfx, const MP::VertexBuffer& vbuf);

		VertexBuffer(Graphics& gfx, const std::string& tag, const MP::VertexBuffer& vbuf);


		void Bind(Graphics& gfx)noexcept override;
		static std::shared_ptr<VertexBuffer>Resolve(Graphics& gfx, const std::string& tag, const MP::VertexBuffer& vbuf);
		template <typename... Ignore>
		static std::string GenerateUID(const std::string& tag, Ignore&&...ignore)
		{
			return GenerateUID_(tag);
		}
		std::string GetUID()const noexcept override;
		static std::string GenerateUID_(const std::string& tag);
	private:
		std::string _mTag;
		UINT _mStride;
		Microsoft::WRL::ComPtr<ID3D11Buffer> _mpVertexBuffer;

	};

}