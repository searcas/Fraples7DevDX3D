#pragma once
#include "RendererAPI/GFXContextBase.h"
#include "Core/Math/Math.h"


namespace FraplesDev
{
	class BlurPack
	{
	public:
		BlurPack(Graphics& gfx, int radius = 7, float sigma = 2.6f)
			:_mShader(gfx, "Blur_PS.cso"),
			_mCtrlPixelConstBuf(gfx,1u), _mKernelPixConstBuf(gfx,0u),_mRadius(radius),_mSigma(sigma)
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
		void ShowWindow(Graphics& gfx)
		{
			ImGui::Begin("Blur");
			bool radChange = ImGui::SliderInt("Radius", &_mRadius, 0, 15);
			bool sigChange = ImGui::SliderFloat("Sigma", &_mSigma, 0.1f, 15.0f);
			if (radChange || sigChange)
			{
				SetKernel(gfx, _mRadius, _mSigma);
			}
			ImGui::End();
		}
		void SetVertical(Graphics& gfx)
		{
			_mCtrlPixelConstBuf.Update(gfx, { FALSE });
		}
		// nTaps should be 6 sigma - 1
		// for more accurate coefs, need to integrate, but no
		void SetKernel(Graphics& gfx, int radius, float sigma)
		{
			assert(radius <= 15);
			Kernel k;
			k.nTaps = radius * 2 + 1;
			float sum = 0.0f;

			for (int i = 0; i < k.nTaps; i++)
			{
				const auto x = float(i - radius);
				const auto g = gauss(x, sigma);
				sum += g;
				k.coefficients[i].x = g;
			}
			for (int i = 0; i < k.nTaps; i++)
			{
				k.coefficients[i].x /= sum;
			}
			_mKernelPixConstBuf.Update(gfx, k);
		}
		struct Kernel
		{
			int nTaps;
			float padding[3];
			DirectX::XMFLOAT4 coefficients[31];
		};
		struct Control
		{
			BOOL horizontal;
			float padding[3];
		};
	private:
		int _mRadius;
		float _mSigma;
		PixelShader _mShader;
		PixelConstantBuffer<Kernel> _mKernelPixConstBuf;
		PixelConstantBuffer<Control> _mCtrlPixelConstBuf;

	};
}
