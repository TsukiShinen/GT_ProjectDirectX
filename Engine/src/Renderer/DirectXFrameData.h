﻿#pragma once
#include <cstdint>

#include "DirectXContext.h"
#include "UploadBuffer.h"

namespace Engine
{
	struct DirectionalLight
	{
		alignas(16) DirectX::XMFLOAT3 Color = {1.0f, 1.0f, 1.0f};
		alignas(16) DirectX::XMFLOAT3 Direction = {0.0f, -1.0f, 0.0f};
	};

	struct PassConstants
	{
		alignas(16) DirectX::XMFLOAT4X4 ViewProj = MathHelper::Identity4x4();
		alignas(16) DirectX::XMFLOAT3 EyePosW{0.0f, 0.0f, 0.0f};

		alignas(16) DirectX::XMFLOAT4 AmbientLight{0.2f, 0.2f, 0.2f, 1.0f};

		alignas(16) int NumDirectionalLights = 0;

		alignas(16) DirectionalLight DirectionalLights[10];
	};

	struct Vertex
	{
	};

	struct VertexColor : Vertex
	{
		DirectX::XMFLOAT3 Position;
		DirectX::XMFLOAT4 Color{1, 1, 1, 1};

		VertexColor(const DirectX::XMFLOAT3 pPosition)
			: Vertex(), Position(pPosition)
		{
		}

		VertexColor(const DirectX::XMFLOAT3 pPosition, const DirectX::XMFLOAT4 pColor)
			: Vertex(), Position(pPosition), Color(pColor)
		{
		}

		static std::vector<D3D12_INPUT_ELEMENT_DESC> GetLayout()
		{
			return {
				{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
				{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
			};
		}
	};

	struct VertexTex : Vertex
	{
		DirectX::XMFLOAT3 Position;
		DirectX::XMFLOAT2 TexCoord;

		VertexTex(const DirectX::XMFLOAT3 pPosition, const DirectX::XMFLOAT2 pTexCoord)
			: Vertex(), Position(pPosition), TexCoord(pTexCoord)
		{
		}

		static std::vector<D3D12_INPUT_ELEMENT_DESC> GetLayout()
		{
			return {
				{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
				{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
			};
		}
	};

	struct VertexLit : Vertex
	{
		DirectX::XMFLOAT3 Position;
		DirectX::XMFLOAT2 TexCoord;
		DirectX::XMFLOAT3 Normal;

		VertexLit(const DirectX::XMFLOAT3 pPosition, const DirectX::XMFLOAT2 pTexCoord, const DirectX::XMFLOAT3 pNormal)
			: Vertex(), Position(pPosition), TexCoord(pTexCoord), Normal(pNormal)
		{
		}

		static std::vector<D3D12_INPUT_ELEMENT_DESC> GetLayout()
		{
			return {
				{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
				{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
				{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
			};
		}
	};

	struct DirectXFrameData
	{
		DirectXFrameData(ID3D12Device* pDevice, UINT pPassCount)
		{
			THROW_IF_FAILED(pDevice->CreateCommandAllocator(
				D3D12_COMMAND_LIST_TYPE_DIRECT,
				IID_PPV_ARGS(CmdListAlloc.GetAddressOf())));

			PassCB = std::make_unique<UploadBuffer<PassConstants>>(pDevice, pPassCount, true);
			m_IsDirty = true;
		}

		DirectXFrameData(const DirectXFrameData& pFrameData) = delete;
		DirectXFrameData& operator=(const DirectXFrameData& pFrameData) = delete;
		~DirectXFrameData() = default;

		// We cannot reset the allocator until the GPU is done processing the commands.
		// So each frame needs their own allocator.
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CmdListAlloc;

		PassConstants m_Constants;
		// We cannot update a cbuffer until the GPU is done processing the commands
		// that reference it.  So each frame needs their own cbuffers.
		std::unique_ptr<UploadBuffer<PassConstants>> PassCB = nullptr;

		// Fence value to mark commands up to this fence point.  This lets us
		// check if these frame resources are still in use by the GPU.
		UINT64 Fence = 0;

	public:
		void SetViewProj(const DirectX::XMFLOAT4X4& viewProj)
		{
			m_Constants.ViewProj = viewProj;
			m_IsDirty = true;
		}

		void SetEyePosition(const DirectX::XMFLOAT3& eyePos)
		{
			m_Constants.EyePosW = eyePos;
			m_IsDirty = true;
		}

		void SetAmbientLight(const DirectX::XMFLOAT4& ambientLight)
		{
			m_Constants.AmbientLight = ambientLight;
			m_IsDirty = true;
		}

		void SetDirectionalLight(int id, DirectionalLight light)
		{
			m_Constants.DirectionalLights[id] = light;
			m_IsDirty = true;
		}

		void SetNumDirectionalLights(int num)
		{
			m_Constants.NumDirectionalLights = num;
			m_IsDirty = true;
		}

		void Update()
		{
			if (m_IsDirty)
			{
				PassCB->CopyData(0, m_Constants);
				m_IsDirty = false;
			}
		}

		bool IsDirty() { return m_IsDirty; }

	private:
		bool m_IsDirty;
	};
}
