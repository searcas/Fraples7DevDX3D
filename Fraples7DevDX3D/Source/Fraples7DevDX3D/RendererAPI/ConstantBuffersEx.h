#pragma once
#include "GFXContext.h"
#include "Core/Common/Exceptions/Macros/GraphicsThrowMacros.h"
#include "Core/MetaProgramming/DynamicConstant.h"
namespace FraplesDev
{
	class PixelConstantBufferEx : public GfxContext
	{
	public:
	
		void Update(Graphics& gfx, const MP::Buffer& buf)
		{
			assert(&buf.GetRootLayoutElement() == &GetRootLayoutElement());
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
		virtual const MP::LayoutElement& GetRootLayoutElement() const noexcept = 0;
	protected:
		PixelConstantBufferEx(Graphics& gfx, const MP::LayoutElement& layoutRoot, UINT slot, const MP::Buffer* pBuffer)
			:_mSlot(slot)
		{
			INFOMAN(gfx);
			D3D11_BUFFER_DESC constantBufferDescriptor = {};
			constantBufferDescriptor.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			constantBufferDescriptor.Usage = D3D11_USAGE_DYNAMIC;
			constantBufferDescriptor.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			constantBufferDescriptor.MiscFlags = 0u;
			constantBufferDescriptor.ByteWidth = (UINT)layoutRoot.GetSizeInBytes();
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
	class CachingPixelConstantBufferEx : public PixelConstantBufferEx
	{
	public:
		CachingPixelConstantBufferEx(Graphics& gfx, const MP::CookedLayout& layout, UINT slot)
			:PixelConstantBufferEx(gfx, *layout.ShareRoot(), slot, nullptr), _mBuf(MP::Buffer(layout))
		{

		}
		CachingPixelConstantBufferEx(Graphics& gfx, const MP::Buffer& buf, UINT slot)
			:PixelConstantBufferEx(gfx, buf.GetRootLayoutElement(), slot, &buf), _mBuf(buf)
		{

		}
		const MP::LayoutElement& GetRootLayoutElement() const noexcept override
		{
			return _mBuf.GetRootLayoutElement();
		}
		const MP::Buffer& GetBuffer()const noexcept
		{
			return _mBuf;
		}
		void SetBuffer(const MP::Buffer& buff_in)
		{
			_mBuf.CopyFrom(buff_in);
			dirty = true;
		}
		void Bind(Graphics& gfx) noexcept override
		{
			if (dirty)
			{
				Update(gfx, _mBuf);
				dirty = false;
			}
			PixelConstantBufferEx::Bind(gfx);
		}
	private:
		MP::Buffer _mBuf;
		bool dirty = false;
	};
	class NoCachePixelConstantBufferEx : public PixelConstantBufferEx
	{
	public:
		NoCachePixelConstantBufferEx(Graphics& gfx, const MP::CookedLayout& layout, UINT slot)
			: PixelConstantBufferEx(gfx, *layout.ShareRoot(), slot, nullptr),_mPLayoutRoot(layout.ShareRoot())
		{}
		NoCachePixelConstantBufferEx(Graphics& gfx, const MP::Buffer& buff, UINT slot)
			: PixelConstantBufferEx(gfx, buff.GetRootLayoutElement(), slot, nullptr), _mPLayoutRoot(buff.ShareLayoutRoot())
		{}
		const MP::LayoutElement& GetRootLayoutElement()const noexcept override
		{
			return *_mPLayoutRoot;
		}
	private:
		std::shared_ptr<MP::LayoutElement>_mPLayoutRoot;
	};
}