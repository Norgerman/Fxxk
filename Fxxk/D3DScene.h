#pragma once
#include <initializer_list>
#include <functional>
#include <mutex>
#include <array>
#include <vector>
#include <d3d11.h>
#include <DirectXMath.h>
#include "D3DGlobal.h"

class D3DObject;

typedef struct VsConstant {
    DirectX::XMMATRIX projection;
} VsConstant;

class D3DScene
{
public:
    D3DScene() : 
        D3DScene(D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_HARDWARE, 
            D3D11_CREATE_DEVICE_FLAG::D3D11_CREATE_DEVICE_SINGLETHREADED, 
            { D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0 },
            D3DGlobal::backgroundColour, D3DGlobal::project)
    {

    }
   
    template<typename T1 = std::vector<D3D_FEATURE_LEVEL>, typename T2 = std::array<float, 4>, typename T3 = std::function<DirectX::XMMATRIX(D3DScene&)>>
    D3DScene(D3D_DRIVER_TYPE driverType, uint32_t deviceflags, T1&& features, T2&& backgorund, T3& buildProjection):
        m_device(nullptr),
        m_context(nullptr),
        m_swapChain(nullptr),
        m_target(nullptr),
        m_constantBuffer(nullptr),
        m_constant(nullptr),
        m_inited(false),
        m_viewport({ 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f }),
        m_driverType(driverType),
        m_deviceFlags(deviceflags),
        m_features(features),
        m_buildProjection(buildProjection),
        m_background(backgorund),
        m_mutex()
    {

    }
    void dispose();
    void resize(float width, float height);
    void init(HWND hwnd);
    ID3D11Device* getDevice() const;
    ID3D11DeviceContext* getContext() const;
    void updateProjection();
    const D3D11_VIEWPORT& getViewport() const;
    void render(std::initializer_list<D3DObject*>&& objs);
    ~D3DScene();
private:
    ID3D11Device* m_device;
    ID3D11DeviceContext* m_context;
    IDXGISwapChain* m_swapChain;
    ID3D11RenderTargetView* m_target;
    ID3D11Buffer* m_constantBuffer;
    VsConstant* m_constant;
    D3D11_VIEWPORT m_viewport;
    std::array<float, 4> m_background;
    std::function<DirectX::XMMATRIX(D3DScene&)> m_buildProjection;
    std::vector<D3D_FEATURE_LEVEL> m_features;
    D3D_DRIVER_TYPE m_driverType;
    uint32_t m_deviceFlags;
    bool m_inited;
    std::mutex m_mutex;
};
