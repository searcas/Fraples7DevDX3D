#pragma once
#include "GraphicAPI/Graphics.h"
#include "Projection.h"
#include "CameraProjection.h"
namespace FraplesDev
{
	class Graphics;
	class RenderGraph;
	class Camera
	{
	public:
		Camera(Graphics& gfx, std::string name, DirectX::XMFLOAT3 homePos,float homePitch, float homeYaw)noexcept;
		void Reset();
		void SpawnControllWindow() noexcept;
		DirectX::XMMATRIX GetMatrix() const noexcept;
		void Rotate(float dx, float dy) noexcept;
		void Translate(DirectX::XMFLOAT3 translation) noexcept;
		void BindGraphics(Graphics& gfx)const;
		void LinkTechniques(RenderGraph& rg);
		void Submit()const;
		inline DirectX::XMFLOAT3 GetPos()const noexcept { return pos; }
		inline const std::string& GetName()const noexcept { return _mName; }
	private:
		DirectX::XMFLOAT3 _mHomePos;
		float _mHomePitch;
		float _mHomeYaw;
		std::string _mName;
		DirectX::XMFLOAT3 pos;
		static constexpr float travelSpeed = 12.0f;
		static constexpr float rotationSpeed = 0.004;
		float pitch = 0.0f;
		float yaw = 0.0f;
		Projection _mProj;
		CameraProjection _mCamProj;
	};
}