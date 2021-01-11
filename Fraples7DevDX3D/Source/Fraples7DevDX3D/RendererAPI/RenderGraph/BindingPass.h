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
	private:
		std::vector<std::shared_ptr<GfxContext>>_mBinds;
	};
}