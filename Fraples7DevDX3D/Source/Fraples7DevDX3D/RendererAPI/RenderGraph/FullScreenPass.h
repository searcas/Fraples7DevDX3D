#pragma once
#include "BindingPass.h"

namespace FraplesDev
{
	
	
	class FullScreenPass : BindingPass
	{
	public:
		FullScreenPass( std::string name, Graphics& gfx)noexcept(!IS_DEBUG);
		void Execute(Graphics& gfx)const noexcept override;
		static std::shared_ptr<class VertexBuffer>_mVertexBuffer;
		static std::shared_ptr<class IndexBuffer>_mIndexBuffer;
		static std::shared_ptr<class VertexShader>_mVertexShader;
		static std::shared_ptr<class InputLayout>_mInputLayout;
	private:

	};
}
