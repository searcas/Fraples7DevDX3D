#include "ModelProbeBase.h"
#include "RendererAPI/Mesh/Model.h"
#include "TechProbeBase.h"
namespace FraplesDev
{
	ModelProbeBase::ModelProbeBase(std::string name)
		:_mName(std::move(name))
	{
	}
	void ModelProbeBase::SpawnWindow(Model& model)
	{
		ImGui::Begin(_mName.c_str());
		ImGui::Columns(2, nullptr, true);
		model.Accept(*this);

		ImGui::NextColumn();

		if (pSelectedNode != nullptr)
		{
			bool dirty = false;
			const auto dcheck = [&dirty](bool changed) {dirty = dirty || changed; };
			auto& tf = ResolveTransform();

			ImGui::TextColored({ 1.0f,0.2f,0.2f,0.7f }, "Translation");
			dcheck(ImGui::SliderFloat("X", &tf.x, -60.0f, 60.0f));
			dcheck(ImGui::SliderFloat("Y", &tf.y, -60.0f, 60.0f));
			dcheck(ImGui::SliderFloat("Z", &tf.z, -60.0f, 60.0f));

			//So we gotta be careful here cuz We can't name again X Y Z
			//Gonna Trigger both childs and move both coords
			ImGui::TextColored({ 0.2f,1.0f,0.2f,0.7f }, "Orientation");
			dcheck(ImGui::SliderAngle("X-Rot", &tf.xRot, -180.0f, 180.0f));
			dcheck(ImGui::SliderAngle("Y-Rot", &tf.yRot, -180.0f, 180.0f));
			dcheck(ImGui::SliderAngle("Z-Rot", &tf.zRot, -180.0f, 180.0f));

			if (dirty)
			{
				pSelectedNode->SetAppliedTransform(
					DirectX::XMMatrixRotationX(tf.xRot) *
					DirectX::XMMatrixRotationY(tf.yRot) *
					DirectX::XMMatrixRotationZ(tf.zRot) *
					DirectX::XMMatrixTranslation(tf.x, tf.y, tf.z));
			}
			if (ImGui::Button("Reset"))
			{
				tf = {};
				pSelectedNode->SetAppliedTransform(
					DirectX::XMMatrixRotationX(tf.xRot) *
					DirectX::XMMatrixRotationY(tf.yRot) *
					DirectX::XMMatrixRotationZ(tf.zRot) *
					DirectX::XMMatrixTranslation(tf.x, tf.y, tf.z));

			}
			TechProbe probe{};
			pSelectedNode->Accept(probe);
		}
		ImGui::End();
	}
	bool ModelProbeBase::PushNode(Node& node)
	{
		// if there is no selected node,  set selectedId to an impossible value;
		const int selectedId = (pSelectedNode == nullptr) ? -1 : pSelectedNode->GetId();
		// build up flags for current node
		const auto node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ((node.GetId() == selectedId) ? ImGuiTreeNodeFlags_Selected : 0) |
			(node.HasChildren() ? 0 : ImGuiTreeNodeFlags_Leaf);
		//render this node
		const auto expanded = ImGui::TreeNodeEx((void*)(intptr_t)node.GetId(), node_flags, node.GetName().c_str());
		// processing for selectin node
		if (ImGui::IsItemClicked())
		{
			pSelectedNode = &node;
		}
		// signal if children should also be recursed
		return expanded;
	}
	void ModelProbeBase::PopNode(Node& node)
	{
		ImGui::TreePop();
	}
}