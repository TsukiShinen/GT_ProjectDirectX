﻿#include "DirectXPipeline.h"

#include "DirectXSwapchain.h"

namespace Engine
{

    DirectXPipeline::DirectXPipeline(const std::vector<D3D12_INPUT_ELEMENT_DESC>& pLayout, const std::wstring& pShaderPath)
    {
        const Microsoft::WRL::ComPtr<ID3DBlob> vsByteCode = d3dUtil::CompileShader(pShaderPath, nullptr, "VS", "vs_5_0");
        const Microsoft::WRL::ComPtr<ID3DBlob> psByteCode = d3dUtil::CompileShader(pShaderPath, nullptr, "PS", "ps_5_0");

        InitializeSignature();

        InitializePipelineState(pLayout, vsByteCode, psByteCode);
    }

    DirectXPipeline::~DirectXPipeline()
    {
    }

    void DirectXPipeline::InitializeSignature()
    {
        // Root parameter can be a table, root descriptor or root constants.
        CD3DX12_ROOT_PARAMETER slotRootParameter[1];

        // Create a single descriptor table of CBVs.
        CD3DX12_DESCRIPTOR_RANGE cbvTable;
        cbvTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
        slotRootParameter[0].InitAsDescriptorTable(1, &cbvTable);

        // A root signature is an array of root parameters.
        CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(1, slotRootParameter, 0, nullptr,
                                                D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

        // create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer
        Microsoft::WRL::ComPtr<ID3DBlob> serializedRootSig = nullptr;
        Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
        HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
                                                 serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

        if (errorBlob != nullptr)
        {
            ::OutputDebugStringA(static_cast<char*>(errorBlob->GetBufferPointer()));
        }
        ThrowIfFailed(hr);

        ThrowIfFailed(DirectXContext::Get()->m_Device->CreateRootSignature(
            0,
            serializedRootSig->GetBufferPointer(),
            serializedRootSig->GetBufferSize(),
            IID_PPV_ARGS(&m_RootSignature)));
    }
    
    void DirectXPipeline::InitializePipelineState(std::vector<D3D12_INPUT_ELEMENT_DESC> pLayout, Microsoft::WRL::ComPtr<ID3DBlob> pVsByteCode, Microsoft::WRL::ComPtr<ID3DBlob> pPsByteCode)
    {
        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
        ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
        psoDesc.InputLayout = {pLayout.data(), static_cast<UINT>(pLayout.size())};
        psoDesc.pRootSignature = m_RootSignature.Get();
        psoDesc.VS =
        {
            static_cast<BYTE*>(pVsByteCode->GetBufferPointer()),
            pVsByteCode->GetBufferSize()
        };
        psoDesc.PS =
        {
            static_cast<BYTE*>(pPsByteCode->GetBufferPointer()),
            pPsByteCode->GetBufferSize()
        };
        psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
        psoDesc.SampleMask = UINT_MAX;
        psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        psoDesc.NumRenderTargets = 1;
        psoDesc.RTVFormats[0] = DirectXSwapchain::k_BackBufferFormat;
        psoDesc.SampleDesc.Count = DirectXContext::Get()->m_4xMsaaState ? 4 : 1;
        psoDesc.SampleDesc.Quality = DirectXContext::Get()->m_4xMsaaState
                                         ? (DirectXContext::Get()->m_4xMsaaQuality - 1)
                                         : 0;
        psoDesc.DSVFormat = DirectXSwapchain::k_DepthStencilFormat;
        ThrowIfFailed(
            DirectXContext::Get()->m_Device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_PipelineState)));
    }
}
