#include "NodeSystem.h"
#include "ModelProbe.h"
#include "RendererAPI/RenderPriority/TechniqueProbe.h"
namespace FraplesDev
{
	Node::Node(int id, const std::string& name, std::vector<Mesh*>meshPtrs, const DirectX::XMMATRIX& transform)noexcept(!IS_DEBUG)
		:_mName(name), _mMeshPtrs(std::move(meshPtrs)), _mID(id)
	{
		DirectX::XMStoreFloat4x4(&_mTransform, transform);
		DirectX::XMStoreFloat4x4(&appliedTransform, DirectX::XMMatrixIdentity());
	}
	void Node::Submit(size_t channels,DirectX::FXMMATRIX accumulatedTransform)const noexcept(!IS_DEBUG)
	{
		const auto built = DirectX::XMLoadFloat4x4(&appliedTransform) * DirectX::XMLoadFloat4x4(&_mTransform) * accumulatedTransform;

		for (const auto pm : _mMeshPtrs)
		{
			pm->Submit(channels,built);
		}
		for (const auto& pc : _mChildPtrs)
		{
			pc->Submit(channels, built);
		}
	}
	void Node::SetAppliedTransform(DirectX::FXMMATRIX transform) noexcept
	{
		DirectX::XMStoreFloat4x4(&appliedTransform, transform);
	}
	bool Node::HasChildren() const noexcept
	{
		return _mChildPtrs.size() > 0;
	}

	void Node::Accept( ModelProbe& probe)
	{
		if (probe.PushNode(*this))
		{
			for (auto& cp : _mChildPtrs)
			{
				cp->Accept(probe);
			}
			probe.PopNode(*this);
		}
	}

	void Node::Accept(TechniqueProbe& probe)
	{
		for (auto& mp : _mMeshPtrs)
		{
			mp->Accept(probe);
		}
	}

	void Node::AddChild(std::unique_ptr<Node>pChild) noexcept(!IS_DEBUG)
	{
		assert(pChild);
		_mChildPtrs.push_back(std::move(pChild));
	}
}