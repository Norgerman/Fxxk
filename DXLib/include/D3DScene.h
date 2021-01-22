//
// Game.h
//

#pragma once

#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <memory>
#include <GraphicsMemory.h>
#include <DirectXMath.h>
#include <functional>
#include <vector>
#include <initializer_list>
#include <array>
#include <D3DConstant.h>
#include <export.h>

namespace DX {
    class D3DObject;

    class dllexport D3DScene
    {
    public:

        D3DScene() noexcept;

        D3DScene(const std::array<float, 4>& backgroundColor, const std::function<void(D3DScene&)>& rebuildProjection, D3D_FEATURE_LEVEL featureLevel) noexcept;
        D3DScene(std::array<float, 4>&& backgroundColor, std::function<void(D3DScene&)>&& rebuildProjection, D3D_FEATURE_LEVEL featureLevel) noexcept;

        virtual ~D3DScene();

        D3DScene(D3DScene const&) = delete;
        D3DScene& operator= (D3DScene const&) = delete;


        virtual void Tick();
        
        // Initialization and management
        virtual void Initialize(HWND window, float x, float y, float width, float height);
        virtual void SetRenderList(const std::vector<D3DObject*>& objects);
        virtual void SetRenderList(std::vector<D3DObject*>&& objects);
        virtual void UpdateProjection(const DirectX::XMMATRIX& projection);
        virtual void UpdateProjection(DirectX::XMMATRIX&& projection);

        void SetTargetUpdateTimeout(bool fixedTimeStep, double targetSeconds);
        void SetInactiveTargetUpdateTimeout(bool fixedTimeStep, double targetSeconds);

        // Messages
        void OnActivated();
        void OnDeactivated();
        void OnSuspending();
        void OnResuming();
        void OnWindowSizeChanged(float x, float y, float width, float height);
        ID3D12Device* Device() const;
        ID3D12CommandQueue* CommandQueue() const;
        ID3D12GraphicsCommandList* CommandList() const;
        const D3D12_VIEWPORT& Viewport() const;
        const D3DConstant& Projection() const;
        uint32_t FramesPerSecond() const;
        void OnUpdate(const std::function<void(D3DScene&, double, uint32_t)>& update);
        void OnUpdate(std::function<void(D3DScene&, double, uint32_t)>&& update);
        void EnableDebug();
    private:
        class Impl;
        std::unique_ptr<Impl> m_impl;
    };
}
