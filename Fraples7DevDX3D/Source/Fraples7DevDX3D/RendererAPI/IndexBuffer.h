#pragma once
#include "GraphicAPI/Graphics.h"
#include "GFXContext.h"
#include <memory>

namespace FraplesDev
{
	class IndexBuffer : public GfxContext
	{
	public:
		IndexBuffer(Graphics& gfx,const std::vector<unsigned short>&indices);
		IndexBuffer(Graphics& gfx, std::string tag, const std::vector<unsigned short>& indices);
		
		void Bind(Graphics& gfx) noexcept override;
		UINT GetCount() const noexcept;
		static std::shared_ptr<IndexBuffer>Resolve(Graphics& gfx, const std::string& tag,const std::vector<unsigned short>& indices);
		template <typename ... Ignore>
		static std::string GenerateUID(const std::string&tag, Ignore&&...ignore)
		{
			return GenerateUID_(tag);
		}
		std::string GetUID()const noexcept;
		static std::string GenerateUID_(const std::string& tag);
	protected:
		std::string _mTag;
		UINT _mCount;
		Microsoft::WRL::ComPtr<ID3D11Buffer>_mpIndexBuffer;
	};
}