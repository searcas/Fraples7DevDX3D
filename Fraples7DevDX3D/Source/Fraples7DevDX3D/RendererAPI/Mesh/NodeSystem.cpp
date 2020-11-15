#include "NodeSystem.h"
#include "ModelProbe.h"

namespace FraplesDev
{
	Node::Node(int id, const std::string& name, std::vector<Mesh*>meshPtrs, const DirectX::XMMATRIX& transform)noexcept(!IS_DEBUG)
		:_mName(name), _mMeshPtrs(std::move(meshPtrs)), _mID(id)
	{
		DirectX::XMStoreFloat4x4(&_mTransform, transform);
		DirectX::XMStoreFloat4x4(&appliedTransform, DirectX::XMMatrixIdentity());
	}
	void Node::Submit(FrameCommander& frame, DirectX::FXMMATRIX accumulatedTransform)const noexcept(!IS_DEBUG)
	{
		const auto built = DirectX::XMLoadFloat4x4(&appliedTransform) * DirectX::XMLoadFloat4x4(&_mTransform) * accumulatedTransform;

		for (const auto pm : _mMeshPtrs)
		{
			pm->Submit(frame, built);
		}
		for (const auto& pc : _mChildPtrs)
		{
			pc->Submit(frame, built);
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

	void Node::AddChild(std::unique_ptr<Node>pChild) noexcept(!IS_DEBUG)
	{
		assert(pChild);
		_mChildPtrs.push_back(std::move(pChild));
	}
}