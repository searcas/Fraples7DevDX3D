#include "ViewFrustum.h"
#include "CameraProjection.h"
#include "Core/MetaProgramming/Vertex.h"
#include "RendererAPI/Topology.h"
#include "RendererAPI/VertexBuffer.h"
#include "RendererAPI/VertexShader.h"
#include "RendererAPI/PixelShader.h"
#include "RendererAPI/IndexBuffer.h"
#include "RendererAPI/InputLayout.h"
#include "RendererAPI/ConstantBuffers.h"
#include "RendererAPI/TransformCBuf/TransformCBuf.h"
#include "RendererAPI/Rasterizer/Rasterizer.h"
#include "RendererAPI/ShadowMapping/Channels.h"
namespace FraplesDev
{
	Frustum::Frustum(Graphics& gfx, float width, float height, float nearZ, float farZ)
	{
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
			indices.push_back(4);
			indices.push_back(5);
			indices.push_back(5);
			indices.push_back(6);
			indices.push_back(6);
			indices.push_back(7);
			indices.push_back(7);
			indices.push_back(4);
			indices.push_back(0);
			indices.push_back(4);
			indices.push_back(1);
			indices.push_back(5);
			indices.push_back(2);
			indices.push_back(6);
			indices.push_back(3);
			indices.push_back(7);
		}
		SetVertices(gfx, width, height, nearZ, farZ);
		_mPindices = IndexBuffer::Resolve(gfx, "$frustum", indices);
		_mPtopology = Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

		{
			Technique line{ Channel::main };
			{
				Step unoccluded("lambertian");


				auto pvs = VertexShader::Resolve(gfx, "Solid_VS.cso");
				unoccluded.AddContext(InputLayout::Resolve(gfx, _mPvertices->GetLayout(), *pvs));
				unoccluded.AddContext(std::move(pvs));

				unoccluded.AddContext(PixelShader::Resolve(gfx, "Solid_PS.cso"));

				struct PSColorConstant
				{
					DirectX::XMFLOAT3 color = { 0.6f,0.2f,0.2f };
					float padding;
				}colorConst;
				unoccluded.AddContext(PixelConstantBuffer<PSColorConstant>::Resolve(gfx, colorConst, 1u));
				unoccluded.AddContext(std::make_shared<TransformCBuf>(gfx));
				unoccluded.AddContext(Rasterizer::Resolve(gfx, false));
				line.AddStep(std::move(unoccluded));
			}
			{
				Step occluded("wireframe");

				auto pvs = VertexShader::Resolve(gfx, "Solid_VS.cso");
				occluded.AddContext(InputLayout::Resolve(gfx, _mPvertices->GetLayout(), *pvs));
				occluded.AddContext(std::move(pvs));
				occluded.AddContext(PixelShader::Resolve(gfx, "Solid_PS.cso"));

				struct PScolorConstantDark
				{
					DirectX::XMFLOAT3 color = { 0.25,0.8f,0.8f };
					float padding;
				}colorConst;

				occluded.AddContext(PixelConstantBuffer<PScolorConstantDark>::Resolve(gfx, colorConst, 1u));
				occluded.AddContext(std::make_shared<TransformCBuf>(gfx));
				occluded.AddContext(Rasterizer::Resolve(gfx, false));
				line.AddStep(std::move(occluded));
			}
			AddTechnique(std::move(line));
		}
	}
	void Frustum::SetVertices(Graphics& gfx, float width, float height, float nearZ, float farZ)
	{
		MP::VertexLayout layout;
		layout.Append(MP::ElementType::Position3D);
		MP::VertexBuffer vertices{ std::move(layout) };
		{
			const float zRatio = farZ / nearZ;
			const float nearX = width / 2.0f;
			const float nearY = height / 2.0f;
			const float farX = nearX * zRatio;
			const float farY = nearY * zRatio;
			vertices.EmplaceBack(DirectX::XMFLOAT3{ -nearX, nearY, nearZ });
			vertices.EmplaceBack(DirectX::XMFLOAT3{  nearX, nearY, nearZ });
			vertices.EmplaceBack(DirectX::XMFLOAT3{  nearX,-nearY, nearZ });
			vertices.EmplaceBack(DirectX::XMFLOAT3{ -nearX,-nearY, nearZ });
			vertices.EmplaceBack(DirectX::XMFLOAT3{ -farX, farY, farZ });
			vertices.EmplaceBack(DirectX::XMFLOAT3{  farX, farY, farZ });
			vertices.EmplaceBack(DirectX::XMFLOAT3{  farX,-farY, farZ });
			vertices.EmplaceBack(DirectX::XMFLOAT3{ -farX,-farY, farZ });
			_mPvertices = std::make_shared<VertexBuffer>(gfx, vertices);
		}
	}
	void Frustum::SetPosition(DirectX::XMFLOAT3 pos) noexcept
	{
		_mPosition = pos;
	}
	void Frustum::SetRotation(DirectX::XMFLOAT3 rot) noexcept
	{
		_mRotation = rot;
	}
	const DirectX::XMMATRIX Frustum::GetTransformXM() const noexcept
	{
		return DirectX::XMMatrixRotationRollPitchYawFromVector(DirectX::XMLoadFloat3(&_mRotation))
			* DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&_mPosition));
	}
}