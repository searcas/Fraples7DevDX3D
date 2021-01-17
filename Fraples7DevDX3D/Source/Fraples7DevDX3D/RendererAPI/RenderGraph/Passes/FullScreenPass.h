#pragma once
#include "BindingPass.h"

namespace FraplesDev
{
	
	
	class FullScreenPass : public BindingPass
	{
	public:
		FullScreenPass( std::string name, Graphics& gfx) noexcept(!IS_DEBUG);
		void Execute(Graphics& gfx)const noexcept(!IS_DEBUG) override;
	private:

	};
}
