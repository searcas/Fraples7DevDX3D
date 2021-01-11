#pragma once
#include "imgui.h"
#include "RendererAPI/Mesh/ModelProbe.h"
#include "RendererAPI/Mesh/NodeSystem.h"
#include "Core/Math/FraplesXM.h"
#include <unordered_map>

namespace FraplesDev
{
	class Model;
	class Node;

	class ModelProbeBase : public ModelProbe
	{
	public:
		void SpawnWindow(Model& model);

	protected:
		Node* pSelectedNode = nullptr;
		struct TransformParameters
		{
			float xRot = 0.0f;
			float yRot = 0.0f;
			float zRot = 0.0f;
			float x = 0.0f;
			float y = 0.0f;
			float z = 0.0f;
		};
		std::unordered_map<int, TransformParameters>_mTransformParams;
		TransformParameters& ResolveTransform()noexcept
		{
			const auto id = pSelectedNode->GetId();
			auto i = _mTransformParams.find(id);
			if (i == _mTransformParams.end())
			{
				return LoadTransform(id);
			}
			return i->second;
		}
		TransformParameters& LoadTransform(int id)noexcept
		{
			const auto& applied = pSelectedNode->GetAppliedTransform();
			const auto angles = ExtractEulerAngles(applied);
			const auto translation = ExtractTranslation(applied);
			TransformParameters tp;
			tp.xRot = angles.x;
			tp.yRot = angles.y;
			tp.zRot = angles.z;
			tp.x = translation.x;
			tp.y = translation.y;
			tp.z = translation.z;
			return _mTransformParams.insert({ id,{ tp } }).first->second;
		}
		bool PushNode(Node& node)override;
		void PopNode(Node& node)override;

	};
}