#pragma once

#include "DirectXContext.h"
#include "Core/Transform.h"
#include "Platform/Input.h"
#include "Debug/Log.h"

namespace Engine
{
	class DirectXCamera
	{

	public:
		DirectXCamera(float width, float height, float fovDegree = 45.0f, float nearZ = 0.1f, float farZ = 1000.0f);
		~DirectXCamera();

		void Resize(float width, float height);

		void Update();

		void GameUpdate(float dt);

		void MoveZAxis(float dt, float value);

		void MoveXAxis(float dt, float value);

		void Pitch(float dt, float value);

		void Rotate(float dt, float value);


	private:
		std::unique_ptr<Transform> m_Transform;

		float m_FovDegree;
		float m_NearZ;
		float m_FarZ;

		DirectX::XMFLOAT4X4 m_View = MathHelper::Identity4x4();
		DirectX::XMFLOAT4X4 m_Proj = MathHelper::Identity4x4();

		DirectX::XMFLOAT4X4 m_ViewProj = MathHelper::Identity4x4();
		DirectX::XMFLOAT4X4 m_ViewProjT = MathHelper::Identity4x4();

		friend class DirectXApi;
	};
}


