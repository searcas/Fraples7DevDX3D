#pragma once
#include "BindingPass.h"
#include "RendererAPI/GFXContext.h"
namespace FraplesDev
{
	BindingPass::BindingPass(std::string name, std::vector<GfxContext> binds)
		:Pass(std::move(name)), _mBinds(std::move(binds))
	{

	}
	void BindingPass::AddBind(std::shared_ptr<GfxContext> bind) noexcept
	{
		_mBinds.push_back(std::move(bind));
	}
	void BindingPass::BindAll(Graphics& gfx) const noexcept
	{
		for (auto& bind : _mBinds)
		{
			bind->Bind(gfx);
		}
		BindBufferResources(gfx);
	}
}
