#include <algorithm>
#include <DirectXMath.h>
#include <DirectXColors.h>
#include <wrl/event.h>
#include <wrl/client.h>
#include <wil/result.h>
#include <D3DScene.h>
#include <D3DObject.h>
#include <dxgidebug.h>
#include "d3dx12.h"
#include "StepTimer.h"

namespace DX {
    using namespace DirectX;
    using namespace std;
    using Microsoft::WRL::ComPtr;
    using Microsoft::WRL::Wrappers::Event;

    __declspec(align(16)) class D3DScene::Impl
    {
    public:
        Impl(D3DScene* owner, array<float, 4>&& backgroundColor, function<void(D3DScene&)>&& rebuildProjection, D3D_FEATURE_LEVEL featureLevel) noexcept :
            m_window(nullptr),
            m_owner(owner),
            m_backgroundColor(move(backgroundColor)),
            m_rebuildProjection(move(rebuildProjection)),
            m_featureLevel(featureLevel),
            m_backBufferIndex(0),
            m_rtvDescriptorSize(0),
            m_viewport({ 0.0f, 0.0f, 0.0f, 0.0f, D3D12_MIN_DEPTH, D3D12_MAX_DEPTH }),
            m_fenceValues{},
            m_projection(DirectX::XMMatrixIdentity()),
            m_suspended(false),
            m_objects(),
            m_update([](D3DScene&, double) -> void {}),
            m_debug(false),
            m_inactive(false),
            m_fixedTimeStep(false),
            m_targetSecounds(0.0),
            m_inactiveFixedTimeStep(false),
            m_inactiveTargetSeconds(0.0)
        {
        }

        Impl(D3DScene* owner, const array<float, 4>& backgroundColor, const function<void(D3DScene&)>& rebuildProjection, D3D_FEATURE_LEVEL featureLevel) noexcept :
            m_window(nullptr),
            m_owner(owner),
            m_backgroundColor(backgroundColor),
            m_rebuildProjection(rebuildProjection),
            m_featureLevel(featureLevel),
            m_backBufferIndex(0),
            m_rtvDescriptorSize(0),
            m_viewport({ 0.0f, 0.0f, 0.0f, 0.0f, D3D12_MIN_DEPTH, D3D12_MAX_DEPTH }),
            m_fenceValues{},
            m_projection(DirectX::XMMatrixIdentity()),
            m_suspended(false),
            m_objects(),
            m_update([](D3DScene&, double) -> void {}),
            m_debug(false),
            m_inactive(false),
            m_fixedTimeStep(false),
            m_targetSecounds(0.0),
            m_inactiveFixedTimeStep(false),
            m_inactiveTargetSeconds(0.0)
        {
        }

        // Initialization and management
        void Initialize(HWND window, float x, float y, float width, float height)
        {
            m_window = window;
            m_viewport.TopLeftX = x;
            m_viewport.TopLeftY = y;
            m_viewport.Width = std::max<float>(width, 1);
            m_viewport.Height = std::max<float>(height, 1);

            CreateDevice();
            CreateResources();

            m_timer.SetFixedTimeStep(m_fixedTimeStep);
            m_timer.SetTargetElapsedSeconds(m_targetSecounds);
        }

        void Tick()
        {
            if (m_suspended)
            {
                return;
            }

            m_timer.Tick([&]()
                {
                    Update(m_timer);
                });

            Render();
        }

        void SetTargetUpdateTimeout(bool fixedTimeStep, double targetSeconds)
        {
            m_fixedTimeStep = fixedTimeStep;
            m_targetSecounds = targetSeconds;
            if (!m_inactive)
            {
                m_timer.SetFixedTimeStep(m_fixedTimeStep);
                m_timer.SetTargetElapsedSeconds(m_targetSecounds);
            }
        }

        void SetInactiveTargetUpdateTimeout(bool fixedTimeStep, double targetSeconds)
        {
            m_inactiveFixedTimeStep = fixedTimeStep;
            m_inactiveTargetSeconds = targetSeconds;
            if (m_inactive)
            {
                m_timer.SetFixedTimeStep(m_inactiveFixedTimeStep);
                m_timer.SetTargetElapsedSeconds(m_inactiveTargetSeconds);
            }
        }

        // Messages
        void OnActivated()
        {
            m_inactive = false;
            m_timer.SetFixedTimeStep(m_fixedTimeStep);
            m_timer.SetTargetElapsedSeconds(m_targetSecounds);
        }

        void OnDeactivated()
        {
            m_inactive = true;
            m_timer.SetFixedTimeStep(m_inactiveFixedTimeStep);
            m_timer.SetTargetElapsedSeconds(m_inactiveTargetSeconds);
        }

        void OnSuspending()
        {
            m_suspended = true;
        }

        void OnResuming()
        {
            m_timer.ResetElapsedTime();

            m_suspended = false;
        }

        void OnWindowSizeChanged(float x, float y, float width, float height)
        {
            m_viewport.TopLeftX = x;
            m_viewport.TopLeftY = y;
            m_viewport.Width = std::max<float>(width, 1);
            m_viewport.Height = std::max<float>(height, 1);

            CreateResources();
        }

        void OnUpdate(const function<void(D3DScene&, double)>& update)
        {
            m_update = update;
        }

        void OnUpdate(function<void(D3DScene&, double)>&& update)
        {
            m_update = move(update);
        }

        ID3D12Device* Device() const
        {
            return m_d3dDevice.Get();
        }

        ID3D12CommandQueue* CommandQueue() const
        {
            return m_commandQueue.Get();
        }

        ID3D12GraphicsCommandList* CommandList() const
        {
            return m_commandList.Get();
        }

        const D3D12_VIEWPORT& Viewport() const
        {
            return m_viewport;
        }

        const D3DConstant& Projection() const
        {
            return *m_projectionConstant;
        }

        void SetRenderList(const std::vector<D3DObject*>& objects)
        {
            m_objects = objects;
        }

        void SetRenderList(std::vector<D3DObject*>&& objects)
        {
            m_objects = move(objects);
        }

        void UpdateProjection(const DirectX::XMMATRIX& projection)
        {
            m_projection = projection;
            m_projectionConstant->Update(&m_projection);
        }

        void UpdateProjection(DirectX::XMMATRIX&& projection)
        {
            m_projection = projection;
            m_projectionConstant->Update(&m_projection);
        }

        void EnableDebug()
        {
            m_debug = true;
        }

        void* operator new(size_t i)
        {
            return _mm_malloc(i, 16);
        }

        void operator delete(void* p)
        {
            _mm_free(p);
        }

        ~Impl()
        {
            WaitForGpu();
        }
    private:
        void Update(StepTimer const& timer)
        {
            m_update(*this->m_owner, timer.GetElapsedSeconds());
        }

        void Render()
        {
            // Don't try to render anything before the first Update.
            if (m_timer.GetFrameCount() == 0)
            {
                return;
            }

            // Prepare the command list to render a new frame.
            Clear();

            for (auto& obj : m_objects)
            {
                obj->Render(*this->m_owner);
            }

            // Show the new frame.
            Present();
        }

        void Clear()
        {
            // Reset command list and allocator.
            THROW_IF_FAILED(m_commandAllocators[m_backBufferIndex]->Reset());
            THROW_IF_FAILED(m_commandList->Reset(m_commandAllocators[m_backBufferIndex].Get(), nullptr));

            // Transition the render target into the correct state to allow for drawing into it.
            D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_backBufferIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
            m_commandList->ResourceBarrier(1, &barrier);

            // Clear the views.
            CD3DX12_CPU_DESCRIPTOR_HANDLE rtvDescriptor(
                m_rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
                static_cast<INT>(m_backBufferIndex), m_rtvDescriptorSize);
            CD3DX12_CPU_DESCRIPTOR_HANDLE dsvDescriptor(m_dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
            m_commandList->OMSetRenderTargets(1, &rtvDescriptor, FALSE, &dsvDescriptor);
            m_commandList->ClearRenderTargetView(rtvDescriptor, m_backgroundColor.data(), 0, nullptr);
            m_commandList->ClearDepthStencilView(dsvDescriptor, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

            // Set the viewport and scissor rect.
            D3D12_RECT scissorRect = { 0, 0, static_cast<LONG>(m_viewport.Width), static_cast<LONG>(m_viewport.Height) };
            m_commandList->RSSetViewports(1, &m_viewport);
            m_commandList->RSSetScissorRects(1, &scissorRect);
        }

        void Present()
        {
            // Transition the render target to the state that allows it to be presented to the display.
            D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_backBufferIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
            m_commandList->ResourceBarrier(1, &barrier);

            // Send the command list off to the GPU for processing.
            THROW_IF_FAILED(m_commandList->Close());
            m_commandQueue->ExecuteCommandLists(1, CommandListCast(m_commandList.GetAddressOf()));

            // The first argument instructs DXGI to block until VSync, putting the application
            // to sleep until the next VSync. This ensures we don't waste any cycles rendering
            // frames that will never be displayed to the screen.
            HRESULT hr = m_swapChain->Present(1, 0);

            // If the device was reset we must completely reinitialize the renderer.
            if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
            {
                OnDeviceLost();
            }
            else
            {
                THROW_IF_FAILED(hr);

                MoveToNextFrame();
            }
        }

        void CreateDevice()
        {
            DWORD dxgiFactoryFlags = 0;

            if (m_debug) {
                // Enable the debug layer (requires the Graphics Tools "optional feature").
                //
                // NOTE: Enabling the debug layer after device creation will invalidate the active device.
                {
                    ComPtr<ID3D12Debug> debugController;
                    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(debugController.GetAddressOf()))))
                    {
                        debugController->EnableDebugLayer();
                    }

                    ComPtr<IDXGIInfoQueue> dxgiInfoQueue;
                    if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(dxgiInfoQueue.GetAddressOf()))))
                    {
                        dxgiFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;

                        dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, true);
                        dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, true);
                    }
                }
            }

            THROW_IF_FAILED(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(m_dxgiFactory.ReleaseAndGetAddressOf())));

            ComPtr<IDXGIAdapter1> adapter;
            GetAdapter(adapter.GetAddressOf());

            // Create the DX12 API device object.
            THROW_IF_FAILED(D3D12CreateDevice(
                adapter.Get(),
                m_featureLevel,
                IID_PPV_ARGS(m_d3dDevice.ReleaseAndGetAddressOf())
            ));

            if (m_debug)
            {
                // Configure debug device (if active).
                ComPtr<ID3D12InfoQueue> d3dInfoQueue;
                if (SUCCEEDED(m_d3dDevice.As(&d3dInfoQueue)))
                {
                    d3dInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
                    d3dInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
                    D3D12_MESSAGE_ID hide[] =
                    {
                        D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,
                        D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE
                    };
                    D3D12_INFO_QUEUE_FILTER filter = {};
                    filter.DenyList.NumIDs = _countof(hide);
                    filter.DenyList.pIDList = hide;
                    d3dInfoQueue->AddStorageFilterEntries(&filter);
                }
            }


            // Create the command queue.
            D3D12_COMMAND_QUEUE_DESC queueDesc = {};
            queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
            queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

            THROW_IF_FAILED(m_d3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(m_commandQueue.ReleaseAndGetAddressOf())));

            // Create descriptor heaps for render target views and depth stencil views.
            D3D12_DESCRIPTOR_HEAP_DESC rtvDescriptorHeapDesc = {};
            rtvDescriptorHeapDesc.NumDescriptors = c_swapBufferCount;
            rtvDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

            D3D12_DESCRIPTOR_HEAP_DESC dsvDescriptorHeapDesc = {};
            dsvDescriptorHeapDesc.NumDescriptors = 1;
            dsvDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;

            THROW_IF_FAILED(m_d3dDevice->CreateDescriptorHeap(&rtvDescriptorHeapDesc, IID_PPV_ARGS(m_rtvDescriptorHeap.ReleaseAndGetAddressOf())));
            THROW_IF_FAILED(m_d3dDevice->CreateDescriptorHeap(&dsvDescriptorHeapDesc, IID_PPV_ARGS(m_dsvDescriptorHeap.ReleaseAndGetAddressOf())));

            m_rtvDescriptorSize = m_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

            // Create a command allocator for each back buffer that will be rendered to.
            for (UINT n = 0; n < c_swapBufferCount; n++)
            {
                THROW_IF_FAILED(m_d3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(m_commandAllocators[n].ReleaseAndGetAddressOf())));
            }

            // Create a command list for recording graphics commands.
            THROW_IF_FAILED(m_d3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocators[0].Get(), nullptr, IID_PPV_ARGS(m_commandList.ReleaseAndGetAddressOf())));
            THROW_IF_FAILED(m_commandList->Close());

            // Create a fence for tracking GPU execution progress.
            THROW_IF_FAILED(m_d3dDevice->CreateFence(m_fenceValues[m_backBufferIndex], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_fence.ReleaseAndGetAddressOf())));
            m_fenceValues[m_backBufferIndex]++;

            m_fenceEvent.Attach(CreateEventEx(nullptr, nullptr, 0, EVENT_MODIFY_STATE | SYNCHRONIZE));
            if (!m_fenceEvent.IsValid())
            {
                throw std::exception("CreateEvent");
            }

            // Initialize device dependent objects here (independent of window size).
            m_memory = std::make_unique<GraphicsMemory>(m_d3dDevice.Get());
            m_projectionConstant = std::make_unique<D3DConstant>(this->m_owner, 4, 16);
            m_projectionConstant->Update(&m_projection);
        }

        void CreateResources()
        {
            // Wait until all previous GPU work is complete.
            WaitForGpu();

            // Release resources that are tied to the swap chain and update fence values.
            for (UINT n = 0; n < c_swapBufferCount; n++)
            {
                m_renderTargets[n].Reset();
                m_fenceValues[n] = m_fenceValues[m_backBufferIndex];
            }

            const DXGI_FORMAT backBufferFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
            const DXGI_FORMAT depthBufferFormat = DXGI_FORMAT_D32_FLOAT;
            const UINT backBufferWidth = static_cast<UINT>(m_viewport.Width);
            const UINT backBufferHeight = static_cast<UINT>(m_viewport.Height);

            // If the swap chain already exists, resize it, otherwise create one.
            if (m_swapChain)
            {
                HRESULT hr = m_swapChain->ResizeBuffers(c_swapBufferCount, backBufferWidth, backBufferHeight, backBufferFormat, 0);

                if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
                {
                    // If the device was removed for any reason, a new device and swap chain will need to be created.
                    OnDeviceLost();

                    // Everything is set up now. Do not continue execution of this method. OnDeviceLost will reenter this method
                    // and correctly set up the new device.
                    return;
                }
                else
                {
                    THROW_IF_FAILED(hr);
                }
            }
            else
            {
                // Create a descriptor for the swap chain.
                DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
                swapChainDesc.Width = backBufferWidth;
                swapChainDesc.Height = backBufferHeight;
                swapChainDesc.Format = backBufferFormat;
                swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
                swapChainDesc.BufferCount = c_swapBufferCount;
                swapChainDesc.SampleDesc.Count = 1;
                swapChainDesc.SampleDesc.Quality = 0;
                swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
                swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
                swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;

                DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsSwapChainDesc = {};
                fsSwapChainDesc.Windowed = TRUE;

                // Create a swap chain for the window.
                ComPtr<IDXGISwapChain1> swapChain;
                THROW_IF_FAILED(m_dxgiFactory->CreateSwapChainForHwnd(
                    m_commandQueue.Get(),
                    m_window,
                    &swapChainDesc,
                    &fsSwapChainDesc,
                    nullptr,
                    swapChain.GetAddressOf()
                ));

                THROW_IF_FAILED(swapChain.As(&m_swapChain));

                // This template does not support exclusive fullscreen mode and prevents DXGI from responding to the ALT+ENTER shortcut
                THROW_IF_FAILED(m_dxgiFactory->MakeWindowAssociation(m_window, DXGI_MWA_NO_ALT_ENTER));
            }

            // Obtain the back buffers for this window which will be the final render targets
            // and create render target views for each of them.
            for (UINT n = 0; n < c_swapBufferCount; n++)
            {
                THROW_IF_FAILED(m_swapChain->GetBuffer(n, IID_PPV_ARGS(m_renderTargets[n].GetAddressOf())));

                wchar_t name[25] = {};
                m_renderTargets[n]->SetName(name);

                CD3DX12_CPU_DESCRIPTOR_HANDLE rtvDescriptor(
                    m_rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
                    static_cast<INT>(n), m_rtvDescriptorSize);
                m_d3dDevice->CreateRenderTargetView(m_renderTargets[n].Get(), nullptr, rtvDescriptor);
            }

            // Reset the index to the current back buffer.
            m_backBufferIndex = m_swapChain->GetCurrentBackBufferIndex();

            // Allocate a 2-D surface as the depth/stencil buffer and create a depth/stencil view
            // on this surface.
            CD3DX12_HEAP_PROPERTIES depthHeapProperties(D3D12_HEAP_TYPE_DEFAULT);

            D3D12_RESOURCE_DESC depthStencilDesc = CD3DX12_RESOURCE_DESC::Tex2D(
                depthBufferFormat,
                backBufferWidth,
                backBufferHeight,
                1, // This depth stencil view has only one texture.
                1  // Use a single mipmap level.
            );
            depthStencilDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

            D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
            depthOptimizedClearValue.Format = depthBufferFormat;
            depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
            depthOptimizedClearValue.DepthStencil.Stencil = 0;

            THROW_IF_FAILED(m_d3dDevice->CreateCommittedResource(
                &depthHeapProperties,
                D3D12_HEAP_FLAG_NONE,
                &depthStencilDesc,
                D3D12_RESOURCE_STATE_DEPTH_WRITE,
                &depthOptimizedClearValue,
                IID_PPV_ARGS(m_depthStencil.ReleaseAndGetAddressOf())
            ));

            m_depthStencil->SetName(L"Depth stencil");

            D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
            dsvDesc.Format = depthBufferFormat;
            dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

            m_d3dDevice->CreateDepthStencilView(m_depthStencil.Get(), &dsvDesc, m_dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

            // Initialize windows-size dependent objects here.
            m_rebuildProjection(*this->m_owner);
        }

        void WaitForGpu() noexcept
        {
            if (m_commandQueue && m_fence && m_fenceEvent.IsValid())
            {
                // Schedule a Signal command in the GPU queue.
                UINT64 fenceValue = m_fenceValues[m_backBufferIndex];
                if (SUCCEEDED(m_commandQueue->Signal(m_fence.Get(), fenceValue)))
                {
                    // Wait until the Signal has been processed.
                    if (SUCCEEDED(m_fence->SetEventOnCompletion(fenceValue, m_fenceEvent.Get())))
                    {
                        WaitForSingleObjectEx(m_fenceEvent.Get(), INFINITE, FALSE);

                        // Increment the fence value for the current frame.
                        m_fenceValues[m_backBufferIndex]++;
                    }
                }
            }
        }

        void MoveToNextFrame()
        {
            // Schedule a Signal command in the queue.
            const UINT64 currentFenceValue = m_fenceValues[m_backBufferIndex];
            THROW_IF_FAILED(m_commandQueue->Signal(m_fence.Get(), currentFenceValue));

            // Update the back buffer index.
            m_backBufferIndex = m_swapChain->GetCurrentBackBufferIndex();

            // If the next frame is not ready to be rendered yet, wait until it is ready.
            if (m_fence->GetCompletedValue() < m_fenceValues[m_backBufferIndex])
            {
                THROW_IF_FAILED(m_fence->SetEventOnCompletion(m_fenceValues[m_backBufferIndex], m_fenceEvent.Get()));
                WaitForSingleObjectEx(m_fenceEvent.Get(), INFINITE, FALSE);
            }

            // Set the fence value for the next frame.
            m_fenceValues[m_backBufferIndex] = currentFenceValue + 1;
        }

        void GetAdapter(IDXGIAdapter1** ppAdapter)
        {
            *ppAdapter = nullptr;

            ComPtr<IDXGIAdapter1> adapter;
            for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != m_dxgiFactory->EnumAdapters1(adapterIndex, adapter.ReleaseAndGetAddressOf()); ++adapterIndex)
            {
                DXGI_ADAPTER_DESC1 desc;
                THROW_IF_FAILED(adapter->GetDesc1(&desc));

                if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
                {
                    // Don't select the Basic Render Driver adapter.
                    continue;
                }

                // Check to see if the adapter supports Direct3D 12, but don't create the actual device yet.
                if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), m_featureLevel, _uuidof(ID3D12Device), nullptr)))
                {
                    break;
                }
            }

            if (!adapter && m_debug)
            {
                if (FAILED(m_dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(adapter.ReleaseAndGetAddressOf()))))
                {
                    throw std::exception("WARP12 not available. Enable the 'Graphics Tools' optional feature");
                }
            }

            if (!adapter)
            {
                throw std::exception("No Direct3D 12 device found");
            }

            *ppAdapter = adapter.Detach();
        }

        void OnDeviceLost()
        {
            for (UINT n = 0; n < c_swapBufferCount; n++)
            {
                m_commandAllocators[n].Reset();
                m_renderTargets[n].Reset();
            }

            m_depthStencil.Reset();
            m_fence.Reset();
            m_commandList.Reset();
            m_swapChain.Reset();
            m_rtvDescriptorHeap.Reset();
            m_dsvDescriptorHeap.Reset();
            m_commandQueue.Reset();
            m_d3dDevice.Reset();
            m_dxgiFactory.Reset();
            m_memory.reset();
            m_projectionConstant->Reset();
            for (auto& obj : m_objects)
            {
                obj->Reset();
            }
            m_objects.clear();

            CreateDevice();
            CreateResources();
        }

        // Application state
        HWND m_window;

        // Direct3D Objects
        D3D_FEATURE_LEVEL m_featureLevel;
        static const UINT c_swapBufferCount = 2;
        UINT m_backBufferIndex;
        UINT m_rtvDescriptorSize;
        ComPtr<ID3D12Device> m_d3dDevice;
        ComPtr<IDXGIFactory4> m_dxgiFactory;
        ComPtr<ID3D12CommandQueue> m_commandQueue;
        ComPtr<ID3D12DescriptorHeap> m_rtvDescriptorHeap;
        ComPtr<ID3D12DescriptorHeap> m_dsvDescriptorHeap;
        ComPtr<ID3D12CommandAllocator> m_commandAllocators[c_swapBufferCount];
        ComPtr<ID3D12GraphicsCommandList> m_commandList;
        ComPtr<ID3D12Fence> m_fence;
        UINT64 m_fenceValues[c_swapBufferCount];
        Event m_fenceEvent;
        D3D12_VIEWPORT m_viewport;
        XMMATRIX m_projection;
        unique_ptr<D3DConstant> m_projectionConstant;

        // Rendering resources
        ComPtr<IDXGISwapChain3> m_swapChain;
        ComPtr<ID3D12Resource> m_renderTargets[c_swapBufferCount];
        ComPtr<ID3D12Resource> m_depthStencil;

        unique_ptr<DirectX::GraphicsMemory> m_memory;

        function<void(D3DScene&)> m_rebuildProjection;
        function<void(D3DScene&, double)> m_update;
        array<float, 4> m_backgroundColor;

        StepTimer m_timer;
        vector<D3DObject*> m_objects;
        D3DScene* m_owner;
        bool m_suspended;
        bool m_inactive;
        bool m_debug;
        
        bool m_fixedTimeStep;
        double m_targetSecounds;

        bool m_inactiveFixedTimeStep;
        double m_inactiveTargetSeconds;
    };


    D3DScene::D3DScene(const std::array<float, 4>& backgroundColor, const std::function<void(D3DScene&)>& rebuildProjection, D3D_FEATURE_LEVEL featureLevel) noexcept :
        m_impl(make_unique<Impl>(this, backgroundColor, rebuildProjection, featureLevel))
    {

    }
    D3DScene::D3DScene(std::array<float, 4>&& backgroundColor, std::function<void(D3DScene&)>&& rebuildProjection, D3D_FEATURE_LEVEL featureLevel) noexcept :
        m_impl(make_unique<Impl>(this, move(backgroundColor), move(rebuildProjection), featureLevel))
    {

    }

    D3DScene::~D3DScene()
    {

    }

    // Initialize the Direct3D resources required to run.
    void D3DScene::Initialize(HWND window, float x, float y, float width, float height)
    {
        m_impl->Initialize(window, x, y, width, height);
    }

    void D3DScene::SetTargetUpdateTimeout(bool fixedTimeStep, double targetSeconds)
    {
        m_impl->SetTargetUpdateTimeout(fixedTimeStep, targetSeconds);
    }

    void D3DScene::SetInactiveTargetUpdateTimeout(bool fixedTimeStep, double targetSeconds)
    {
        m_impl->SetInactiveTargetUpdateTimeout(fixedTimeStep, targetSeconds);
    }

    void D3DScene::Tick()
    {
        m_impl->Tick();
    }

    // Message handlers
    void D3DScene::OnActivated()
    {
        m_impl->OnActivated();
    }

    void D3DScene::OnDeactivated()
    {
        m_impl->OnDeactivated();
    }

    void D3DScene::OnSuspending()
    {
        m_impl->OnSuspending();
    }

    void D3DScene::OnResuming()
    {
        m_impl->OnResuming();
    }

    void D3DScene::OnWindowSizeChanged(float x, float y, float width, float height)
    {
        m_impl->OnWindowSizeChanged(x, y, width, height);
    }

    void D3DScene::SetRenderList(const std::vector<D3DObject*>& objects)
    {
        m_impl->SetRenderList(objects);
    }

    void D3DScene::SetRenderList(std::vector<D3DObject*>&& objects)
    {
        m_impl->SetRenderList(objects);
    }

    void D3DScene::UpdateProjection(const XMMATRIX& projection)
    {
        m_impl->UpdateProjection(projection);
    }

    void D3DScene::UpdateProjection(XMMATRIX&& projection)
    {
        m_impl->UpdateProjection(move(projection));
    }

    void D3DScene::OnUpdate(const function<void(D3DScene&, double)>& update)
    {
        m_impl->OnUpdate(update);
    }

    void D3DScene::OnUpdate(function<void(D3DScene&, double)>&& update)
    {
        m_impl->OnUpdate(move(update));
    }

    void D3DScene::EnableDebug()
    {
        m_impl->EnableDebug();
    }

    ID3D12Device* D3DScene::Device() const
    {
        return m_impl->Device();
    }

    ID3D12CommandQueue* D3DScene::CommandQueue() const
    {
        return m_impl->CommandQueue();
    }

    ID3D12GraphicsCommandList* D3DScene::CommandList() const
    {
        return m_impl->CommandList();
    }

    const D3D12_VIEWPORT& D3DScene::Viewport() const
    {
        return m_impl->Viewport();
    }

    const D3DConstant& D3DScene::Projection() const
    {
        return m_impl->Projection();
    }
}
