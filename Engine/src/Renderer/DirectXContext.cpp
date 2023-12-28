﻿#include "DirectXContext.h"

#include "DirectXCommandObject.h"
#include "DirectXSwapchain.h"
#include "Core/Application.h"
#include "DirectXCamera.h"
#include "Resource/DirectXResourceManager.h"

#include "Shaders/DirectXSimpleShader.h"
#include "Shaders/DirectXTextureShader.h"
#include "Shaders/DirectXLitShader.h"

#include "Materials/DirectXMaterial.h"
#include "Materials/DirectXTextureMaterial.h"
#include "Materials/DirectXLitMaterial.h"

const int gNumFrameResources = 3;

namespace Engine
{
    DirectXContext* DirectXContext::s_Instance = nullptr;

    void DirectXContext::Initialize()
    {
        assert(!s_Instance, "DirectX context already initialized!");
        s_Instance = new DirectXContext();

        // ===== Factory =====
        ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&s_Instance->m_Factory)));

        // ===== Device =====
        if (const HRESULT hardwareResult = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0,
                                                             IID_PPV_ARGS(&s_Instance->m_Device));
            FAILED(hardwareResult))
        {
            Microsoft::WRL::ComPtr<IDXGIAdapter> warpAdapter;
            ThrowIfFailed(s_Instance->m_Factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)));
            ThrowIfFailed(D3D12CreateDevice(warpAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&s_Instance->m_Device)));
        }
        s_Instance->m_CbvSrvUavDescriptorSize = s_Instance->m_Device->GetDescriptorHandleIncrementSize(
            D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        s_Instance->InitializeMsaa();

        s_Instance->m_Camera = std::make_unique<DirectXCamera>(Application::Get()->GetWindow()->GetWidth(), Application::Get()->GetWindow()->GetHeight(), 90.f, 0.1f, 1000.f);
        s_Instance->m_CommandObject = std::make_unique<DirectXCommandObject>();
        s_Instance->m_Swapchain = std::make_unique<DirectXSwapchain>(Application::Get()->GetWindow()->GetWidth(),
                                                                     Application::Get()->GetWindow()->GetHeight());
        s_Instance->m_Swapchain->Resize(Application::Get()->GetWindow()->GetWidth(),
                                        Application::Get()->GetWindow()->GetHeight());
        s_Instance->m_ResourceManager = std::make_unique<DirectXResourceManager>(1000);
        /*
        s_Instance->m_BaseShader = std::make_shared<DirectXSimpleShader>(VertexColor::GetLayout(), L"Shaders\\color.hlsl");
        s_Instance->m_TextureShader = std::make_shared<DirectXTextureShader>(VertexTex::GetLayout(), L"Shaders\\Builtin.Texture.hlsl");
        s_Instance->m_LitShader = std::make_shared<DirectXLitShader>(VertexTex::GetLayout(), L"Shaders\\Builtin.Lit.hlsl");
        */
        // ===== Frame Resources =====
        for(int i = 0; i < gNumFrameResources; ++i)
        {
            s_Instance->m_FramesData.push_back(std::make_unique<DirectXFrameData>(s_Instance->m_Device.Get(), 1));
        }
    }

    void DirectXContext::Shutdown()
    {
    }
    /*
    std::shared_ptr<DirectXShader> DirectXContext::GetBaseShader() const
    {
        return m_BaseShader;
    }

    std::shared_ptr<DirectXShader> DirectXContext::GetTextureShader() const
    {
        return m_TextureShader;
    }*/


    void DirectXContext::InitializeMsaa()
    {
        // Check 4X MSAA quality support for our back buffer format.
        // All Direct3D 11 capable devices support 4X MSAA for all render 
        // target formats, so we only need to check quality support.

        D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
        msQualityLevels.Format = DirectXSwapchain::k_BackBufferFormat;
        msQualityLevels.SampleCount = 4;
        msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
        msQualityLevels.NumQualityLevels = 0;
        ThrowIfFailed(DirectXContext::Get()->m_Device->CheckFeatureSupport(
            D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
            &msQualityLevels,
            sizeof(msQualityLevels)));

        m_4xMsaaQuality = msQualityLevels.NumQualityLevels;
        assert(m_4xMsaaQuality > 0 && "Unexpected MSAA quality level.");
    }
}
