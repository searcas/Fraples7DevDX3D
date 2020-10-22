#pragma once
#include "GFXContext.h"
#include "../Core/Debugging/Exceptions/Macros/GraphicsThrowMacros.h"

namespace FraplesDev
{
	template<typename C>
	class ConstantBuffer : public GfxContext
	{
	public:
		void Update(Graphics& gfx, const C& consts) 
		{
			INFOMAN(gfx);

			D3D11_MAPPED_SUBRESOURCE msr;
			FPL_GFX_THROW_INFO(GetContext(gfx)->Map( 
				_mpConstantBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &msr
			));
			memcpy(msr.pData, &consts, sizeof(consts));
			GetContext(gfx)->Unmap(_mpConstantBuffer.Get(), 0u);
		}
		ConstantBuffer(Graphics& gfx,const C& consts,UINT slot = 0):_mSlot(slot)
		{
			INFOMAN(gfx);
			D3D11_BUFFER_DESC cbd;
			cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			cbd.Usage = D3D11_USAGE_DYNAMIC;
			cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			cbd.MiscFlags = 0u;
			cbd.ByteWidth = sizeof(C);
			cbd.StructureByteStride = 0u;

			D3D11_SUBRESOURCE_DATA csd = {  };
			csd.pSysMem = &consts;
			FPL_GFX_THROW_INFO( GetDevice(gfx)->CreateBuffer(&cbd, &csd, &_mpConstantBuffer));
		}
		ConstantBuffer(Graphics& gfx, UINT slot = 0u) : _mSlot(slot)
		{
			INFOMAN(gfx);
			D3D11_BUFFER_DESC cbd;
			cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			cbd.Usage = D3D11_USAGE_DYNAMIC;
			cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			cbd.MiscFlags = 0u;
			cbd.ByteWidth = sizeof(C);
			cbd.StructureByteStride = 0u;
			FPL_GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&cbd, nullptr,&_mpConstantBuffer));
		}
	protected:
		Microsoft::WRL::ComPtr<ID3D11Buffer> _mpConstantBuffer;
		UINT _mSlot;
	};

	template<typename C>
	class VertexConstantBuffer : public ConstantBuffer<C>
	{
		using ConstantBuffer<C>::_mpConstantBuffer;
		using ConstantBuffer<C>::_mSlot;
		using GfxContext::GetContext;
	public:
		using ConstantBuffer<C>::ConstantBuffer;
		void Bind(Graphics& gfx)noexcept override
		{
			GetContext(gfx)->VSSetConstantBuffers(_mSlot, 1u, _mpConstantBuffer.GetAddressOf());
		}
	};

	template<typename C>
	class PixelConstantBuffer : public ConstantBuffer<C>
	{
		//Template inharitance not letting you access to protected members directly 
		//So this is one of ways arounds
		using ConstantBuffer<C>::_mpConstantBuffer;
		using ConstantBuffer<C>::_mSlot;
		using GfxContext::GetContext;
	public:
		using ConstantBuffer<C>::ConstantBuffer;
		void Bind(Graphics& gfx)noexcept override
		{
			//or use this->
			GetContext(gfx)->PSSetConstantBuffers(_mSlot, 1u, _mpConstantBuffer.GetAddressOf());
		}
	};
}

