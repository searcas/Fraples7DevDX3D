#pragma once
#include "Model.h"
#include <optional>
#include <unordered_map>
#include "Core/MetaProgramming/DynamicConstant.h"
#include "NodeSystem.h"
namespace FraplesDev
{
	class ModelWindow
	{
	public:
		void Show(Graphics& gfx, const char* windowName, const Node& root);
		const MP::Buffer& GetMaterial()const noexcept(!IS_DEBUG);


		bool TransformDirty()const noexcept(!IS_DEBUG);
		void ResetTransformDirty()noexcept(!IS_DEBUG);
		bool MaterialDirty()const noexcept(!IS_DEBUG);
		void ResetMaterialDirty()noexcept(!IS_DEBUG);
		bool IsDirty()const noexcept(!IS_DEBUG);

		DirectX::XMMATRIX GetTransform() const noexcept;
		void ApplyParameters() noexcept;
	private:
		Node* _mPselectedNode = nullptr;
		struct TransformParameters
		{
			float roll = 0.0f;
			float pitch = 0.0f;
			float yaw = 0.0f;
			float x = 0.0f;
			float y = 0.0f;
			float z = 0.0f;
		};
	};
}
