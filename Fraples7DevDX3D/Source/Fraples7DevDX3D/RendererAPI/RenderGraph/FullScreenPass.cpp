#include "FullScreenPass.h"
#include "Core/MetaProgramming/Vertex.h"
#include "RendererAPI/GFXContextBase.h"

namespace FraplesDev
{
	FullScreenPass::FullScreenPass(std::string name, Graphics& gfx) noexcept(!IS_DEBUG)
		:BindingPass(std::move(name))
	{
		//Setup Fullscreen geometry
		MP::VertexLayout lay;
		lay.Append(MP::ElementType::Position2D);
		MP::VertexBuffer buffFull{ lay };
		buffFull.EmplaceBack(DirectX::XMFLOAT2{-1,  1 });
		buffFull.EmplaceBack(DirectX::XMFLOAT2{ 1,  1 });
		buffFull.EmplaceBack(DirectX::XMFLOAT2{-1, -1 });
		buffFull.EmplaceBack(DirectX::XMFLOAT2{ 1, -1 });
		AddBind(VertexBuffer::Resolve(gfx, "$Full", std::move(buffFull)));
		std::vector<unsigned short> indices = { 0, 1, 2, 1 ,3, 2 };
		AddBind(IndexBuffer::Resolve(gfx, "$Full", std::move(indices)));

		//setup fullscreen shaders
		auto vs = VertexShader::Resolve(gfx, "FullScreen_VS.cso");
		AddBind(InputLayout::Resolve(gfx, lay, vs->GetByteCode()));
		AddBind(std::move(vs));
		AddBind(Topology::Resolve(gfx));
		AddBind(Rasterizer::Resolve(gfx, false));
	}

	void FullScreenPass::Execute(Graphics& gfx) const noexcept(!IS_DEBUG)
	{
		BindAll(gfx);
		gfx.RenderIndexed(6u);
	}
}