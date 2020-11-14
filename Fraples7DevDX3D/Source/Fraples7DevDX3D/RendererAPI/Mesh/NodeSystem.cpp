#include "NodeSystem.h"


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

	/*void Node::RenderTree(Node*& pSelectedNode) const noexcept
	{
		//if there is no selected node, set selectedId to an impossible value
		const int selectedId = (pSelectedNode == nullptr) ? -1 : pSelectedNode->GetId();
		//noindex serves as the uid for gui tree nodes, incremented troughout recursion
		const auto node_flags = ImGuiTreeNodeFlags_OpenOnArrow |
			((GetId() == selectedId) ?
				ImGuiTreeNodeFlags_Selected : 0) |
				((_mChildPtrs.size() == 0) ? ImGuiTreeNodeFlags_Leaf : 0);

		//if tree node expanded, recurively render all children
		auto ifClicked = ImGui::TreeNodeEx((void*)GetId(), node_flags, _mName.c_str());


		if (ImGui::IsItemClicked())
		{
			pSelectedNode = const_cast<Node*>(this);
		}
		if (ifClicked)
		{
			for (const auto& pChild : _mChildPtrs)
			{
				pChild->RenderTree(pSelectedNode);
			}
			ImGui::TreePop();
		}
	}
	*/
	void Node::AddChild(std::unique_ptr<Node>pChild)noexcept(!IS_DEBUG)
	{
		assert(pChild);
		_mChildPtrs.push_back(std::move(pChild));

	}
}