#pragma once
#include <vector>
#include <string>
#include "Mesh.h"

namespace FraplesDev
{
	class Node
	{
		friend class Model;
	public:
		Node(int id, const std::string& name, std::vector<Mesh*>meshPtrs, const DirectX::XMMATRIX& transform)noexcept(!IS_DEBUG);
		void Submit(DirectX::FXMMATRIX accumulatedTransform)const noexcept(!IS_DEBUG);
		void SetAppliedTransform(DirectX::FXMMATRIX transform)noexcept;
		inline const DirectX::XMFLOAT4X4& GetAppliedTransform() noexcept { return appliedTransform; }
		inline const int GetId()const noexcept { return _mID; }
		bool HasChildren()const noexcept;
		void Accept(class ModelProbe& probe);
		void Accept(class TechniqueProbe& probe);

		inline const std::string& GetName()const
		{
			return _mName;
		}
	private:
		void AddChild(std::unique_ptr<Node>pChild)noexcept(!IS_DEBUG);
	private:
		int _mID = 0;
		std::string _mName = {};
		std::vector<std::unique_ptr<Node>>_mChildPtrs = {};
		std::vector<Mesh*>_mMeshPtrs = {};
		DirectX::XMFLOAT4X4 _mTransform = {};
		DirectX::XMFLOAT4X4 appliedTransform = {};

	};
}