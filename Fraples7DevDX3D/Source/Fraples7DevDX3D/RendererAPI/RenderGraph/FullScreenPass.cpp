#include "FullScreenPass.h"
#include "Core/MetaProgramming/Vertex.h"
#include "RendererAPI/GFXContextBase.h"

namespace FraplesDev
{
	FullScreenPass::FullScreenPass(std::string name, Graphics& gfx) noexcept(!IS_DEBUG)
		:BindingPass(std::move(name))
	{

		if (!_mIndexBuffer)
		{
			MP::VertexLayout lay;
			lay.Append(MP::ElementType::Position2D);
			MP::VertexBuffer buffFull{ lay };
			buffFull.EmplaceBack(DirectX::XMFLOAT2{ -1, 1 });
			buffFull.EmplaceBack(DirectX::XMFLOAT2{ 1, 1 });
			buffFull.EmplaceBack(DirectX::XMFLOAT2{ -1,-1 });
			buffFull.EmplaceBack(DirectX::XMFLOAT2{ 1,-1 });
			_mVertexBuffer = VertexBuffer::Resolve(gfx, "$Full", std::move(buffFull));
			std::vector<unsigned short> indices = { 0, 1, 2, 1 ,3, 2 };
			_mIndexBuffer = IndexBuffer::Resolve(gfx, "$Full", std::move(indices));

			//setup fullscreen shaders
			_mVertexShader = VertexShader::Resolve(gfx, "FullScreen_VS.cso");
			_mInputLayout = InputLayout::Resolve(gfx, lay, _mVertexShader->GetByteCode());
		}
	}

	void FullScreenPass::Execute(Graphics& gfx) const noexcept
	{
		BindAll(gfx);
		_mVertexBuffer->Bind(gfx);
		_mIndexBuffer->Bind(gfx);
		_mVertexBuffer->Bind(gfx);
		_mInputLayout->Bind(gfx);
		gfx.RenderIndexed(_mIndexBuffer->GetCount());
	}

	std::shared_ptr<VertexBuffer> FullScreenPass::_mVertexBuffer;
	std::shared_ptr<IndexBuffer> FullScreenPass::_mIndexBuffer;
	std::shared_ptr<VertexShader> FullScreenPass::_mVertexShader;
	std::shared_ptr<InputLayout> FullScreenPass::_mInputLayout;

}