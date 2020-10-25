#include "GFXContext.h"
#include <stdexcept>
namespace FraplesDev
{

	ID3D11DeviceContext* GfxContext::GetContext(Graphics& gfx) noexcept
	{
		return gfx._mpContext.Get();
	}

	ID3D11Device* GfxContext::GetDevice(Graphics& gfx) noexcept
	{
		return gfx._mpDevice.Get();
	}

	DxgiInfoManager& GfxContext::GetInfoManager(Graphics& gfx)noexcept(IS_DEBUG)
	{
#ifndef NDEBUG
		return gfx.infoManager;
#else
		throw std::logic_error("YouFuckedUp! (tried to access gfx.infoManager in Release config)");
#endif
	}
}