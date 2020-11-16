#pragma once
#include "RendererAPI/GFXContextBase.h"
#include "Core/Math/Math.h"


namespace FraplesDev
{
	class BlurPack
	{
	public:
		BlurPack(Graphics& gfx, int radius = 5, float sigma = 1.0f)
			:_mShader(gfx, "Blur_PS.cso"),
			_mCtrlPixelConstBuf(gfx,1u), _mKernelPixConstBuf(gfx,0u)
		{
			SetKernel(gfx, radius, sigma);
		}
		void Bind(Graphics& gfx)noexcept
		{
			_mShader.Bind(gfx);
			_mKernelPixConstBuf.Bind(gfx);
			_mCtrlPixelConstBuf.Bind(gfx);
		}
		void SetHorizontal(Graphics& gfx)
		{
			_mCtrlPixelConstBuf.Update(gfx, { TRUE });
		}
		void SetVertical(Graphics& gfx)
		{
			_mCtrlPixelConstBuf.Update(gfx, { FALSE });
		}
		// nTaps should be 6 sigma - 1
		// for more accurate coefs, need to integrate, but no
		void SetKernel(Graphics& gfx, int radius, float sigma)
		{
			assert(radius <= 7);
			Kernel k;
			k.nTaps = radius * 2 + 1;
			float sum = 0.0f;

			for (int i = 1; i < k.nTaps + 1; i++)
			{
				const auto x = float(i - (radius + 1));
				const auto g = gauss(x, sigma);
				sum += g;
				k.coefficients[i].x = g;
			}
			for (int i = 1; i < k.nTaps + 1; i++)
			{
				k.coefficients[i].x /= sum;
			}
			_mKernelPixConstBuf.Update(gfx, k);
		}
		struct Kernel
		{
			int nTaps;
			DirectX::XMFLOAT4 coefficients[15];
		};
		struct Control
		{
			BOOL horizontal;
		};
	private:
		PixelShader _mShader;
		PixelConstantBuffer<Kernel> _mKernelPixConstBuf;
		PixelConstantBuffer<Control> _mCtrlPixelConstBuf;

	};
}
