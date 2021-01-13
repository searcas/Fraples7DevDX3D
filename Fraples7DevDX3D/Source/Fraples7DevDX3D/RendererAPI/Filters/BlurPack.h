#pragma once
#include "RendererAPI/GFXContextBase.h"
#include "Core/Math/Math.h"


namespace FraplesDev
{
	class BlurPack
	{
	public:
		BlurPack(Graphics& gfx, int radius = 7, float sigma = 2.6f,const char* shader = "Blur_PS.cso")
			:_mShader(gfx, shader),
			_mCtrlPixelConstBuf(gfx,1u), _mKernelPixConstBuf(gfx,0u),_mRadius(radius),_mSigma(sigma)
		{
			SetKernelGauss(gfx, radius, sigma);
		}
		void Bind(Graphics& gfx)noexcept(!IS_DEBUG)
		{
			_mShader.Bind(gfx);
			_mKernelPixConstBuf.Bind(gfx);
			_mCtrlPixelConstBuf.Bind(gfx);
		}
		void SetHorizontal(Graphics& gfx)
		{
			_mCtrlPixelConstBuf.Update(gfx, { TRUE });
		}
		void RenderWidgets(Graphics& gfx)
		{
			ImGui::Begin("Blur");
			const char* items[] = { "Gauss","Box" };
			static const char* curItem = items[0];
			bool filterChange = false;
			{
				if (ImGui::BeginCombo("Filter Type",curItem))
				{
					for (int n = 0; n < std::size(items); n++)
					{
						const bool isSelected = (curItem == items[n]);
						if (ImGui::Selectable(items[n],isSelected))
						{
							filterChange = true;
							curItem = items[n];
							if (curItem == items[0])
							{
								kernelType = KernelType::Gauss;
							}
							else if (curItem == items[1])
							{
								kernelType = KernelType::Box;
							}
						}
						if (isSelected)
						{
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}
			}
			bool radChange = ImGui::SliderInt("Radius", &_mRadius, 0, 15);
			bool sigChange = (curItem == items[0]) ? ImGui::SliderFloat("Sigma", &_mSigma, 0.1f, 15.0f) : false;
			if (radChange || sigChange || filterChange)
			{
				if (kernelType == KernelType::Gauss)
				{
					SetKernelGauss(gfx, _mRadius, _mSigma);
				}
				else if (kernelType == KernelType::Box)
				{

					SetKernelBox(gfx, _mRadius);
				}
			}
			ImGui::End();
		}
		void SetVertical(Graphics& gfx)
		{
			_mCtrlPixelConstBuf.Update(gfx, { FALSE });
		}
		// for more accurate coefs, need to integrate, but no
		void SetKernelGauss(Graphics& gfx, int radius, float sigma)noexcept(!IS_DEBUG)
		{
			assert(radius <= maxRadius);
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
		void SetKernelBox(Graphics& gfx, int radius)noexcept(!IS_DEBUG)
		{
			assert(radius <= maxRadius);
			Kernel k;
			k.nTaps = radius * 2 + 1;
			const float c = 1.0f / k.nTaps;
			for (int i = 0; i < k.nTaps; i++)
			{
				k.coefficients[i].x = c;
			}
			_mKernelPixConstBuf.Update(gfx, k);
		}
		static constexpr int maxRadius = 15;
		struct Kernel
		{
			int nTaps;
			float padding[3];
			DirectX::XMFLOAT4 coefficients[maxRadius * 2 + 1];
		};
		struct Control
		{
			BOOL horizontal;
			float padding[3];
		};
		enum class KernelType
		{
			Gauss,
			Box,
		};
	private:
		UINT width;
		UINT height;
		KernelType kernelType = KernelType::Gauss;
		int _mRadius;
		float _mSigma;
		PixelShader _mShader;
		PixelConstantBuffer<Kernel> _mKernelPixConstBuf;
		PixelConstantBuffer<Control> _mCtrlPixelConstBuf;

	};
}
