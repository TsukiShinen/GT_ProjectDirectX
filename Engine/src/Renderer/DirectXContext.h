﻿#pragma once

#include "d3dUtil.h"
#include "DirectXFrameData.h"
#include "Debug/Log.h"

#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

namespace Engine
{
    class DirectXCommandObject;
    class DirectXSwapchain;
    class DirectXShader;
    class DirectXMaterial;
    class DirectXSimpleMaterial;
    class DirectXTextureMaterial;
    class DirectXLitMaterial;

    class DirectXSimpleShader;
    class DirectXTextureShader;
    class DirectXLitShader;

    class DirectXCamera;
    class DirectXResourceManager;

    class DirectXContext
    {
    public:
        
        static void Initialize();
        static void Shutdown();
        /*
        std::shared_ptr<DirectXShader> GetBaseShader() const;
        std::shared_ptr<DirectXShader> GetTextureShader() const;
        */
        DirectXResourceManager& GetResourceManager() const { return *m_ResourceManager; }

        static void LogErrorIfFailed(const HRESULT pHr, const char* pFile, int pLine)
        {
            if (pHr == S_OK)
                return;
            LPVOID errorMsg;
            FormatMessage(
                FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                nullptr,
                pHr,
                0, // Default language
                reinterpret_cast<LPWSTR>(&errorMsg),
                0,
                nullptr
            );
            std::wstring errorMessage(static_cast<LPCWSTR>(errorMsg));
            CORE_ERROR("Error 0x%08X: %ls    at : %s, ligne %d", pHr, errorMessage.c_str(), pFile, pLine);
            throw std::runtime_error("DirectX Failed");
        }
        
        static DirectXContext* Get() { return s_Instance; }
    private:
        void InitializeMsaa();
        DirectXFrameData& CurrentFrameData() const { return *m_FramesData[m_CurrentFrameData]; }
        
        Microsoft::WRL::ComPtr<IDXGIFactory4> m_Factory;
        std::unique_ptr<DirectXSwapchain> m_Swapchain;
        Microsoft::WRL::ComPtr<ID3D12Device> m_Device;

        std::unique_ptr<DirectXCommandObject> m_CommandObject;

        std::unique_ptr<DirectXMaterial> m_BaseMaterial; // TODO : Move this in mesh
        /*
        std::shared_ptr<DirectXSimpleShader> m_BaseShader;
        std::shared_ptr<DirectXTextureShader> m_TextureShader;
        std::shared_ptr<DirectXLitShader> m_LitShader;
        */
        std::unique_ptr<DirectXResourceManager> m_ResourceManager;
        
        UINT m_CbvSrvUavDescriptorSize = 0;
        
        bool m_4xMsaaState = false;
        UINT m_4xMsaaQuality = 0;
        
        D3D_DRIVER_TYPE m_DriverType = D3D_DRIVER_TYPE_HARDWARE;
        
        std::vector<std::unique_ptr<DirectXFrameData>> m_FramesData;
        int m_CurrentFrameData = 0;
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_PassConstantHeap = nullptr;

        // Camera
        std::unique_ptr<DirectXCamera> m_Camera;



    private:
        static DirectXContext* s_Instance;

        friend class DirectXApi;
        friend class DirectXSwapchain;
        friend class DirectXCommandObject;
        friend class DirectXShader;
        friend class DirectXSimpleShader;
        friend class DirectXTextureShader;
        friend class DirectXLitShader;
        friend class DirectXMesh;
        friend class DirectXMaterial;
        friend class DirectXSimpleMaterial;
        friend class DirectXTextureMaterial;
        friend class DirectXLitMaterial;
        friend class DirectXResourceManager;
        friend class MeshRenderer;
    };

#define THROW_IF_FAILED(hr) DirectXContext::LogErrorIfFailed(hr, __FILE__, __LINE__);
}