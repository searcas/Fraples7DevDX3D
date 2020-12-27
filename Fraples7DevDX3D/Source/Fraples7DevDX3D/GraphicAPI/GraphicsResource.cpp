#include "GraphicsResource.h"
#include "GraphicAPI/Graphics.h"
#include "Core/Common/DxgiInfoManager.h"
#include <stdexcept>
namespace FraplesDev
{
	ID3D11DeviceContext* GraphicsResource::GetContext(Graphics& gfx) noexcept
	{
		return gfx._mpContext.Get();
	}
	ID3D11Device* GraphicsResource::GetDevice(Graphics& gfx) noexcept
	{
		return gfx._mpDevice.Get();
	}
	DxgiInfoManager& GraphicsResource::GetInfoManager(Graphics& gfx)
	{
#ifndef NDEBUG
		return gfx.infoManager;
#else 
		throw std::logic_error("You Did something wrong,  tried to access gfx.infomanager in Release mode");
#endif // !NDEBUG

	}
}