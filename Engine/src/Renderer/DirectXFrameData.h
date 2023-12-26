﻿#pragma once
#include <cstdint>

#include "UploadBuffer.h"

namespace Engine
{

    struct PassConstants
    {
        DirectX::XMFLOAT4X4 ViewProj = MathHelper::Identity4x4();
    };

    struct Vertex
    {
        DirectX::XMFLOAT3 Position;
        DirectX::XMFLOAT4 Color{1, 1, 1, 1};
    };
    
    struct DirectXFrameData
    {
        DirectXFrameData(ID3D12Device* pDevice, UINT pPassCount)
        {
            ThrowIfFailed(pDevice->CreateCommandAllocator(
                D3D12_COMMAND_LIST_TYPE_DIRECT,
                IID_PPV_ARGS(CmdListAlloc.GetAddressOf())));

            PassCB = std::make_unique<UploadBuffer<PassConstants>>(pDevice, pPassCount, true);
        }
        DirectXFrameData(const DirectXFrameData& pFrameData) = delete;
        DirectXFrameData& operator=(const DirectXFrameData& pFrameData) = delete;
        ~DirectXFrameData() = default;

        // We cannot reset the allocator until the GPU is done processing the commands.
        // So each frame needs their own allocator.
        Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CmdListAlloc;

        // We cannot update a cbuffer until the GPU is done processing the commands
        // that reference it.  So each frame needs their own cbuffers.
        std::unique_ptr<UploadBuffer<PassConstants>> PassCB = nullptr;

        // Fence value to mark commands up to this fence point.  This lets us
        // check if these frame resources are still in use by the GPU.
        UINT64 Fence = 0;
    };
    
}
