#pragma once
#include "../../RendererAPI/GFXContext.h"
#include "../Surface.h"

#include "RendererAPI/GFXContextCodex.h"
#include <memory>
namespace FraplesDev
{
	class Texture : public GfxContext
	{
	public:
		Texture(Graphics& gfx,const std::string& path, UINT slot = 0);
		void Bind(Graphics& gfx)noexcept override;
		
		static std::shared_ptr<Texture>Resolve(Graphics& gfx, const std::string& path, UINT slot = 0);
		static std::string GenerateUID(const std::string& path, UINT slot = 0);
		std::string GetUID()const noexcept override;
		inline bool HasAlhpa()const noexcept { return hasAlpha;	}
	protected:
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _mpTextureView;
		std::string _mPath;
		bool hasAlpha = false;
	private:
		static UINT CalculateNumberOfMipLevels(UINT width, UINT height)noexcept;
	private:
		unsigned int _mSlot;
	};

}