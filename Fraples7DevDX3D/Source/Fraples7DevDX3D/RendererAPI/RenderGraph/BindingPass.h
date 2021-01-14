#pragma once
#include "RendererAPI/RenderPriority/Pass.h"
#include <memory>
namespace FraplesDev
{
	class BindingPass : public Pass
	{
	public:
		BindingPass(std::string name, std::vector<std::shared_ptr<GfxContext>>binds = {});
		void AddBind(std::shared_ptr<GfxContext> bind)noexcept;
		void BindAll(Graphics& gfx)const noexcept;
		void Finalize()override;
	protected:
		std::shared_ptr<RenderTarget>_mRenderTarget;
		std::shared_ptr<DepthStencil>_mDepthStencil;
	private:
		void BindBufferResources(Graphics& gfx)const noexcept(!IS_DEBUG);
		std::vector<std::shared_ptr<GfxContext>>_mBinds;
	};
}