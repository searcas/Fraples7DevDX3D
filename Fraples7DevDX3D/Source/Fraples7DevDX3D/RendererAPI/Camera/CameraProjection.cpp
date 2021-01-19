#include "CameraProjection.h"
#include "Core/MetaProgramming/Vertex.h"
#include "RendererAPI/Topology.h"
#include "RendererAPI/VertexBuffer.h"
#include "RendererAPI/VertexShader.h"
#include "RendererAPI/PixelShader.h"
#include "RendererAPI/IndexBuffer.h"
#include "RendererAPI/InputLayout.h"
#include "RendererAPI/ConstantBuffers.h"
#include "RendererAPI/TransformCBuf.h"
#include "RendererAPI/Rasterizer/Rasterizer.h"
#include "RendererAPI/ShadowMapping/Channels.h"
namespace FraplesDev
{
	CameraProjection::CameraProjection(Graphics& gfx)
	{
		const auto geometryTag = "$cam";
		MP::VertexLayout layout;
		layout.Append(MP::ElementType::Position3D);
		MP::VertexBuffer vertices{ std::move(layout) };
		
		{
			const float x = 4.0f / 3.0f * 0.75f;
			const float y = 1.0f * 0.75f;
			const float z = -2.0f;
			const float thalf = x * 0.5f;
			const float tspace = y * 0.2f;
			vertices.EmplaceBack(DirectX::XMFLOAT3{ -x,y,0.0f });
			vertices.EmplaceBack(DirectX::XMFLOAT3{ x,y,0.0f });
			vertices.EmplaceBack(DirectX::XMFLOAT3{ x,-y,0.0f });
			vertices.EmplaceBack(DirectX::XMFLOAT3{ -x,-y,0.0f });
			vertices.EmplaceBack(DirectX::XMFLOAT3{ 0.0f,0.0f, z });
			vertices.EmplaceBack(DirectX::XMFLOAT3{ -thalf,y + tspace,0.0f });
			vertices.EmplaceBack(DirectX::XMFLOAT3{ thalf,y + tspace,0.0f });
			vertices.EmplaceBack(DirectX::XMFLOAT3{ 0.0f,y + tspace + thalf,0.0f });
		}
		std::vector<unsigned short> indices;
		{
			indices.push_back(0);
			indices.push_back(1);
			indices.push_back(1);
			indices.push_back(2);
			indices.push_back(2);
			indices.push_back(3);
			indices.push_back(3);
			indices.push_back(0);
			indices.push_back(0);
			indices.push_back(4);
			indices.push_back(1);
			indices.push_back(4);
			indices.push_back(2);
			indices.push_back(4);
			indices.push_back(3);
			indices.push_back(4);
			indices.push_back(5);
			indices.push_back(6);
			indices.push_back(6);
			indices.push_back(7);
			indices.push_back(7);
			indices.push_back(5);
		}
		_mPvertices = VertexBuffer::Resolve(gfx, geometryTag, vertices);
		_mPindices = IndexBuffer::Resolve(gfx, geometryTag, indices);
		_mPtopology = Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
		{
			Technique line{ Channel::main };
			Step only("lambertian");
			auto pvs = VertexShader::Resolve(gfx, "Solid_VS.cso");
			only.AddContext(InputLayout::Resolve(gfx, vertices.GetLayout(),*pvs));
			only.AddContext(std::move(pvs));
			only.AddContext(PixelShader::Resolve(gfx, "Solid_PS.cso"));

			struct PSColorConstant
			{
				DirectX::XMFLOAT3 color = { 0.2f,0.2f,0.6f };
				float padding;
			}colorConstant;
			only.AddContext(PixelConstantBuffer<PSColorConstant>::Resolve(gfx, colorConstant, 1u));
			only.AddContext(std::make_shared<TransformCBuf>(gfx));
			only.AddContext(Rasterizer::Resolve(gfx, false));
			line.AddStep(std::move(only));
			AddTechnique(std::move(line));
		}
	}

	void CameraProjection::SetPosition(DirectX::XMFLOAT3 pos) noexcept
	{
		_mPos = pos;
	}

	void CameraProjection::SetRotation(DirectX::XMFLOAT3 rot)noexcept
	{
		_mRot = rot;
	}

	const DirectX::XMMATRIX CameraProjection::GetTransformXM() const noexcept
	{
		return DirectX::XMMatrixRotationRollPitchYawFromVector(DirectX::XMLoadFloat3(&_mRot)) * DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&_mPos));
	}
}