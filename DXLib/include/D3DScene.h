//
// Game.h
//

#pragma once

#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <wrl/client.h>
#include <wrl/event.h>
#include <memory>
#include <GraphicsMemory.h>
#include <DirectXMath.h>
#include <functional>
#include <vector>
#include <initializer_list>
#include <array>
#include <StepTimer.h>
#include <D3DGlobal.h>
#include <D3DConstant.h>

namespace DX {
    class D3DObject;

    class __declspec(dllexport) D3DScene
    {
    public:

        D3DScene() noexcept :
            D3DScene(Global::backgroundColour, Global::Project, D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0)
        {

        }

        template<typename T1, typename T2>
        D3DScene(T1&& backgroundColor, T2&& buildProjection, D3D_FEATURE_LEVEL featureLevel) noexcept :
            m_window(nullptr),
            m_backgroundColor(backgroundColor),
            m_buildProjection(std::forward<T2>(buildProjection)),
            m_featureLevel(featureLevel),
            m_backBufferIndex(0),
            m_rtvDescriptorSize(0),
            m_viewport({ 0.0f, 0.0f, 0.0f, 0.0f, D3D12_MIN_DEPTH, D3D12_MAX_DEPTH }),
            m_fenceValues{},
            m_projection(DirectX::XMMatrixIdentity()),
            m_projectionConstant(nullptr, 4, 16),
            m_suspended(false),
            m_objects()
        {

        }

        ~D3DScene();

        D3DScene(D3DScene&&) = default;
        D3DScene& operator= (D3DScene&&) = default;

        D3DScene(D3DScene const&) = delete;
        D3DScene& operator= (D3DScene const&) = delete;

        // Initialization and management
        void Initialize(HWND window, float x, float y, float width, float height);

        void Tick();
        // Messages
        void OnActivated();
        void OnDeactivated();
        void OnSuspending();
        void OnResuming();
        void OnWindowSizeChanged(float x, float y, float width, float height);
        Microsoft::WRL::ComPtr<ID3D12Device> Device() const;
        Microsoft::WRL::ComPtr<ID3D12CommandQueue> CommandQueue() const;
        Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> CommandList() const;
        const D3D12_VIEWPORT& Viewport() const;
        const D3DConstant& Projection() const;
        template<typename T>
        void SetRenderList(T&& objects)
        {
            m_objects = std::forward<T>(objects);
        }

    private:

        void Update(StepTimer const& timer);
        void Render();
        void Clear();
        void Present();

        void CreateDevice();
        void CreateResources();

        void WaitForGpu() noexcept;
        void MoveToNextFrame();
        void GetAdapter(IDXGIAdapter1** ppAdapter);
        void OnDeviceLost();

        // Application state
        HWND m_window;

        // Direct3D Objects
        D3D_FEATURE_LEVEL m_featureLevel;
        static const UINT c_swapBufferCount = 2;
        UINT m_backBufferIndex;
        UINT m_rtvDescriptorSize;
        Microsoft::WRL::ComPtr<ID3D12Device> m_d3dDevice;
        Microsoft::WRL::ComPtr<IDXGIFactory4> m_dxgiFactory;
        Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_commandQueue;
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_rtvDescriptorHeap;
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_dsvDescriptorHeap;
        Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_commandAllocators[c_swapBufferCount];
        Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_commandList;
        Microsoft::WRL::ComPtr<ID3D12Fence> m_fence;
        UINT64 m_fenceValues[c_swapBufferCount];
        Microsoft::WRL::Wrappers::Event m_fenceEvent;
        D3D12_VIEWPORT m_viewport;
        DirectX::XMMATRIX m_projection;
        D3DConstant m_projectionConstant;

        // Rendering resources
        Microsoft::WRL::ComPtr<IDXGISwapChain3> m_swapChain;
        Microsoft::WRL::ComPtr<ID3D12Resource> m_renderTargets[c_swapBufferCount];
        Microsoft::WRL::ComPtr<ID3D12Resource> m_depthStencil;

        std::unique_ptr<DirectX::GraphicsMemory> m_memory;

        std::function<DirectX::XMMATRIX(D3DScene&)> m_buildProjection;
        std::array<float, 4> m_backgroundColor;

        StepTimer m_timer;
        std::vector<D3DObject*> m_objects;
        bool m_suspended;
    };
}
