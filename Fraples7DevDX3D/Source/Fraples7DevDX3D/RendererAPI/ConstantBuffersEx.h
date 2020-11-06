#pragma once
#include "GFXContext.h"
#include "Core/Debugging/Exceptions/Macros/GraphicsThrowMacros.h"
#include "Core/MetaProgramming/DynamicConstant.h"
namespace FraplesDev
{
	class PixelConstantBufferEx : public GfxContext
	{
	public:
		PixelConstantBufferEx(Graphics& gfx,const MP::LayoutElement& layout,UINT slot)
			:PixelConstantBufferEx(gfx,layout,slot,nullptr)
		{

		}
		PixelConstantBufferEx(Graphics& gfx, const MP::Buffer& buf, UINT slot)
			:PixelConstantBufferEx(gfx, buf.GetLayout(), slot, &buf)
		{

		}
		void Update(Graphics& gfx, const MP::Buffer& buf)
		{
			INFOMAN(gfx);
			D3D11_MAPPED_SUBRESOURCE msr;
			FPL_GFX_THROW_INFO(GetContext(gfx)->Map(_mPConstantBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &msr));
			memcpy(msr.pData, buf.GetData(), buf.GetSizeInBytes());
			GetContext(gfx)->Unmap(_mPConstantBuffer.Get(), 0u);
		}
		void Bind(Graphics& gfx)noexcept override
		{
			GetContext(gfx)->PSSetConstantBuffers(_mSlot, 1u, _mPConstantBuffer.GetAddressOf());
		}
	private:
		PixelConstantBufferEx(Graphics& gfx, const MP::LayoutElement& layout, UINT slot, const MP::Buffer* pBuffer)
			:_mSlot(slot)
		{
			INFOMAN(gfx);
			D3D11_BUFFER_DESC constantBufferDescriptor = {};
			constantBufferDescriptor.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			constantBufferDescriptor.Usage = D3D11_USAGE_DYNAMIC;
			constantBufferDescriptor.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			constantBufferDescriptor.MiscFlags = 0u;
			constantBufferDescriptor.ByteWidth = (UINT)layout.GetSizeInBytes();
			constantBufferDescriptor.StructureByteStride = 0u;
			if (pBuffer != nullptr)
			{
				D3D11_SUBRESOURCE_DATA constantBufferData = {};
				constantBufferData.pSysMem = pBuffer->GetData();
				FPL_GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&constantBufferDescriptor, &constantBufferData, &_mPConstantBuffer));
			}
			else
			{
				FPL_GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&constantBufferDescriptor, nullptr, &_mPConstantBuffer));
			}
		}
	private:
		UINT _mSlot = 0;
		Microsoft::WRL::ComPtr<ID3D11Buffer>_mPConstantBuffer;
	};

}