#include "SolidSphere.h"
#include "Objects/Sphere.h"
#include "Core/Common/Exceptions/Macros/GraphicsThrowMacros.h"
#include "RendererAPI/GFXContextBase.h"
#include "Core/MetaProgramming/Vertex.h"
#include "RendererAPI/Stencil/Stencil.h"
namespace FraplesDev
{
	SolidSphere::SolidSphere(Graphics& gfx, float radius)
	{
			auto model = Sphere::Make();
			model.Transform(DirectX::XMMatrixScaling(radius, radius, radius));
			const auto geometryTag = "$sphere." + std::to_string(radius);
			_mPvertices = VertexBuffer::Resolve(gfx, geometryTag,model._mVertices);
			_mPindices = IndexBuffer::Resolve(gfx, geometryTag,model._mIndices);
			_mPtopology = Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			
			{
				Technique solid;
				Step only("lambertian");
				auto pvs = VertexShader::Resolve(gfx, "Solid_VS.cso");
				only.AddContext(InputLayout::Resolve(gfx, model._mVertices.GetLayout(), *pvs));
				only.AddContext(std::move(pvs));
				only.AddContext(PixelShader::Resolve(gfx, "Solid_PS.cso"));

				struct PSColorConstant
				{
					DirectX::XMFLOAT3 color = { 1.0f,1.0f,1.0f };
					float padding = 0;
				}colorConst;
				
				only.AddContext(PixelConstantBuffer<PSColorConstant>::Resolve(gfx, colorConst, 1u));
				only.AddContext(std::make_shared<TransformCBuf>(gfx));

				only.AddContext(Rasterizer::Resolve(gfx, false));

				solid.AddStep(std::move(only));
				AddTechnique(std::move(solid));
			}

	}


	void SolidSphere::SetPos(DirectX::XMFLOAT3 pos)noexcept
	{
		_mPos = pos;
	}
	const DirectX::XMMATRIX SolidSphere::GetTransformXM() const noexcept
	{
		return DirectX::XMMatrixTranslation(_mPos.x, _mPos.y, _mPos.z);
	}
}