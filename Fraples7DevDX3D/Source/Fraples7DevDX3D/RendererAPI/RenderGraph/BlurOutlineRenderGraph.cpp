#include "BlurOutlineRenderGraph.h"
#include "Passes/BufferClearPass.h"
#include "Passes/LambertianPass.h"
#include "Passes/OutlineMaskGenerationPass.h"
#include "Passes/HorizontalBlurPass.h"
#include "Passes/VerticalBlurPass.h"
#include "Core/Math/Math.h"
#include "Passes/BlurOutlineRenderPass.h"
#include "ImGui/imgui.h"
#include "Passes/WireFramePass.h"
#include "Passes/ShadowMappingPass.h"
#include "RendererAPI/RenderGraph/Passes/SkyBoxPass.h"
namespace FraplesDev
{
	BlurOutlineRenderGraph::BlurOutlineRenderGraph(Graphics& gfx)
		: RenderGraph(gfx)
	{
		{
			auto pass = std::make_unique<BufferClearPass>("clearRT");
			pass->SetSyncLinkage("buffer", "$.backbuffer");
			AppendPass(std::move(pass));
		}
		{
			auto pass = std::make_unique<BufferClearPass>("clearDS");
			pass->SetSyncLinkage("buffer", "$.masterDepth");
			AppendPass(std::move(pass));
		}
		{
			auto pass = std::make_unique<ShadowMappingPass>(gfx, "shadowMap");
			AppendPass(std::move(pass));
		}
		
		{
			auto pass = std::make_unique<LambertianPass>(gfx, "lambertian");
			pass->SetSyncLinkage("shadowMap", "shadowMap.map");
			pass->SetSyncLinkage("renderTarget", "clearRT.buffer");
			pass->SetSyncLinkage("depthStencil", "clearDS.buffer");
			AppendPass(std::move(pass));
		}
		// setup blur constant buffers
		{
			auto pass = std::make_unique<SkyBoxPass>(gfx, "skybox");
			pass->SetSyncLinkage("renderTarget", "lambertian.renderTarget");
			pass->SetSyncLinkage("depthStencil", "lambertian.depthStencil");
			AppendPass(std::move(pass));
		}
		{
			auto pass = std::make_unique<OutlineMaskGenerationPass>(gfx, "outlineMask");
			pass->SetSyncLinkage("depthStencil", "skybox.depthStencil");
			AppendPass(std::move(pass));
		}
		// setup blur constant buffers
		{
			{
				MP::RawLayout l;
				l.Add<MP::Integer>("nTaps");
				l.Add<MP::Array>("coefficients");
				l["coefficients"].Set<MP::Float>(maxRadius * 2 + 1);
				MP::Buffer buf{ std::move(l) };
				_mBlurKernel = std::make_shared<CachingPixelConstantBufferEx>(gfx, buf, 0);
				SetKernelGauss(radius, sigma);
				AddGlobalSource(DirectContextSource<CachingPixelConstantBufferEx>::Make("blurKernel", _mBlurKernel));
			}
			{
				MP::RawLayout l;
				l.Add<MP::Bool>("isHorizontal");
				MP::Buffer buf{ std::move(l) };
				_mBlurDirection = std::make_shared<CachingPixelConstantBufferEx>(gfx, buf, 1);
				AddGlobalSource(DirectContextSource<CachingPixelConstantBufferEx>::Make("blurDirection", _mBlurDirection));

			}
		}
		{
			auto pass = std::make_unique<BlurOutlineRenderPass>(gfx, "outlineDraw", gfx.GetWidth(), gfx.GetHeight());
			AppendPass(std::move(pass));
		}
		{
			auto pass = std::make_unique<HorizontalBlurPass>("horizontal", gfx, gfx.GetWidth(), gfx.GetHeight());
			pass->SetSyncLinkage("scratchIn", "outlineDraw.scratchOut");
			pass->SetSyncLinkage("kernel", "$.blurKernel");
			pass->SetSyncLinkage("direction", "$.blurDirection");
			AppendPass(std::move(pass));
		}
		{
			auto pass = std::make_unique<VerticalBlurPass>("vertical", gfx);
			pass->SetSyncLinkage("renderTarget", "skybox.renderTarget");
			pass->SetSyncLinkage("depthStencil", "outlineMask.depthStencil");
			pass->SetSyncLinkage("scratchIn", "horizontal.scratchOut");
			pass->SetSyncLinkage("kernel", "$.blurKernel");
			pass->SetSyncLinkage("direction", "$.blurDirection");
			AppendPass(std::move(pass));
		}
		{
			auto pass = std::make_unique<WireFramePass>(gfx, "wireframe");
			pass->SetSyncLinkage("renderTarget", "vertical.renderTarget");
			pass->SetSyncLinkage("depthStencil", "vertical.depthStencil");
			AppendPass(std::move(pass));
		}
		SetSinkTarget("backbuffer", "wireframe.renderTarget");
		Finalize();
	}

	void BlurOutlineRenderGraph::SetKernelGauss(int radius, float sigma) noexcept(!IS_DEBUG)
	{
		assert(radius <= maxRadius);
		auto k = _mBlurKernel->GetBuffer();
		const int nTaps = radius * 2 + 1;
		k["nTaps"] = nTaps;
		float sum = 0.0f;
		for (int i = 0; i < nTaps; i++)
		{
			const auto x = float(i - radius);
			const auto g = gauss(x,sigma);
			sum += g;
			k["coefficients"][i] = g;
		}
		for (int i = 0; i < nTaps; i++)
		{
			k["coefficients"][i] = (float)k["coefficients"][i] / sum;
		}
		_mBlurKernel->SetBuffer(k);
	}
	void BlurOutlineRenderGraph::SetKernelBox(int radius) noexcept(!IS_DEBUG)
	{
		assert(radius <= maxRadius);
		auto k = _mBlurKernel->GetBuffer();
		const int nTaps = radius * 2 + 1;
		k["nTaps"] = nTaps;
		const float c = 1.0f / nTaps;

		for (int i = 0; i < nTaps; i++)
		{
			k["coefficients"][i] = c;
		}
		_mBlurKernel->SetBuffer(k);
	}
	void BlurOutlineRenderGraph::RenderKernelWindow(Graphics& gfx)
	{
		if (ImGui::Begin("Kernel"))
		{
			bool filterChanged = false;
			{
				const char* items[] = { "Gauss","Box" };
				static const char* curItem = items[0];
				if (ImGui::BeginCombo("Filter Type", curItem))
				{
					for (int n = 0; n < std::size(items); n++)
					{
						const bool isSelected = (curItem == items[n]);
						if (ImGui::Selectable(items[n], isSelected))
						{
							filterChanged = true;
							curItem = items[n];
							if (curItem == items[0])
							{
								_mKernelType = KernelType::Gauss;
							}
							else if (curItem == items[1])
							{
								_mKernelType = KernelType::Box;
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
			bool RadChange = ImGui::SliderInt("Radius", &radius, 0, maxRadius);
			bool sigChange = ImGui::SliderFloat("Sigma", &sigma, 0.1f, 10.0f);
			if (RadChange || sigChange || filterChanged)
			{
				if (_mKernelType == KernelType::Gauss)
				{
					SetKernelGauss(radius, sigma);
				}
				else if (_mKernelType == KernelType::Box)
				{
					SetKernelBox(radius);
				}
			}

		}
		ImGui::End();
	}
	void BlurOutlineRenderGraph::RenderShadowWindow(Graphics& gfx)
	{
		if (ImGui::Begin("Shadow"))
		{
			if (ImGui::Button("Dump CubeMap"))
			{
				DumpShadowMap(gfx, "Dumps\\shadow_");
			}
		}
		ImGui::End();
	}
	void BlurOutlineRenderGraph::RenderWindows(Graphics& gfx)
	{
		RenderKernelWindow(gfx);
		RenderShadowWindow(gfx);
	}
	void BlurOutlineRenderGraph::BindMainCamera(Camera& cam)
	{
		dynamic_cast<LambertianPass&>(FindPassByName("lambertian")).BindMainCamera(cam);
		dynamic_cast<SkyBoxPass&>(FindPassByName("skybox")).BindMainCamera(cam);
	}
	void BlurOutlineRenderGraph::BindShadowCamera(Camera& cam)
	{
		dynamic_cast<ShadowMappingPass&>(FindPassByName("shadowMap")).BindShadowCamera(cam);
		dynamic_cast<LambertianPass&>(FindPassByName("lambertian")).BindShadowCamera(cam);
	}
	void BlurOutlineRenderGraph::StoreDepth(Graphics& gfx, const std::string& path)
	{
	}
	void BlurOutlineRenderGraph::DumpShadowMap(Graphics& gfx, const std::string& path)
	{
		dynamic_cast<ShadowMappingPass&>(FindPassByName("shadowMap")).DumpShadowMap(gfx, path);
	}
}