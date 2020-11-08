#pragma once
#include "GFXContext.h"
#include "Core/Debugging/Exceptions/Macros/GraphicsThrowMacros.h"
#include "Core/MetaProgramming/DynamicConstant.h"
namespace FraplesDev
{
	class PixelConstantBufferEx : public GfxContext
	{
	public:
		PixelConstantBufferEx(Graphics& gfx,std::shared_ptr<MP::LayoutElement> playout,UINT slot)
			:PixelConstantBufferEx(gfx,std::move(pLayout),slot,nullptr)
		{

		}
		PixelConstantBufferEx(Graphics& gfx, const MP::Buffer& buf, UINT slot)
			:PixelConstantBufferEx(gfx, buf.ShareLayout(), slot, &buf)
		{

		}
		void Update(Graphics& gfx, const MP::Buffer& buf)
		{
			assert(&buf.GetLayout() == &*pLayout);
			INFOMAN(gfx);
			D3D11_MAPPED_SUBRESOURCE msr;
			FPL_GFX_THROW_INFO(GetContext(gfx)->Map(_mPConstantBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &msr));
			memcpy(msr.pData, buf.GetData(), buf.GetSizeInBytes());
			GetContext(gfx)->Unmap(_mPConstantBuffer.Get(), 0u);
		}
		const MP::LayoutElement& GetLayout() const noexcept
		{
			return *pLayout;
		}
		void Bind(Graphics& gfx)noexcept override
		{
			GetContext(gfx)->PSSetConstantBuffers(_mSlot, 1u, _mPConstantBuffer.GetAddressOf());
		}
	private:
		PixelConstantBufferEx(Graphics& gfx, std::shared_ptr<MP::LayoutElement> layout_in, UINT slot, const MP::Buffer* pBuffer)
			:_mSlot(slot),pLayout(std::move(layout_in))
		{
			INFOMAN(gfx);
			D3D11_BUFFER_DESC constantBufferDescriptor = {};
			constantBufferDescriptor.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			constantBufferDescriptor.Usage = D3D11_USAGE_DYNAMIC;
			constantBufferDescriptor.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			constantBufferDescriptor.MiscFlags = 0u;
			constantBufferDescriptor.ByteWidth = (UINT)pLayout->GetSizeInBytes();
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
		std::shared_ptr<MP::LayoutElement> pLayout;
		Microsoft::WRL::ComPtr<ID3D11Buffer>_mPConstantBuffer;
		UINT _mSlot = 0;
	};

}