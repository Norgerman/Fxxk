#include "D3DScene.h"
#include "D3DObject.h"

#pragma comment(lib, "d3d11.lib")

void D3DScene::init(DXGI_SWAP_CHAIN_DESC& swapChainDescr, float x, float y, float w, float h)
{
    auto hr = D3D11CreateDeviceAndSwapChain(
        NULL,
        m_driverType,
        NULL,
#if _DEBUG
        m_deviceFlags | D3D11_CREATE_DEVICE_FLAG::D3D11_CREATE_DEVICE_DEBUG,
#else
        m_deviceFlags,
#endif
        m_features.data(), static_cast<uint32_t>(m_features.size()),
        D3D11_SDK_VERSION,
        &swapChainDescr,
        &m_swapChain,
        &m_device,
        NULL,
        &m_context);

    assert(SUCCEEDED(hr));

    ID3D11Texture2D* framebuffer = nullptr;

    hr = m_swapChain->GetBuffer(
        0,
        __uuidof(ID3D11Texture2D),
        reinterpret_cast<void**>(&framebuffer));

    assert(SUCCEEDED(hr));

    hr = m_device->CreateRenderTargetView(framebuffer, 0, &m_target);
    framebuffer->Release();

    assert(SUCCEEDED(hr));

    m_viewport.TopLeftX = x;
    m_viewport.TopLeftY = y;
    m_viewport.Width = w;
    m_viewport.Height = h;

    m_projection = m_buildProjection(*this);

    D3D11_SUBRESOURCE_DATA sr = { 0 };
    D3D11_BUFFER_DESC cbDesc = { };
    cbDesc.ByteWidth = sizeof(m_projection);
    cbDesc.Usage = D3D11_USAGE_DYNAMIC;
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    cbDesc.MiscFlags = 0;
    cbDesc.StructureByteStride = 0;
    sr.pSysMem = &m_projection;
    hr = m_device->CreateBuffer(&cbDesc, &sr,
        &m_constantBuffer);

    assert(SUCCEEDED(hr));
    m_inited = true;
}

void D3DScene::init(DXGI_SWAP_CHAIN_DESC&& swapChainDescr, float x, float y, float w, float h)
{
    init(swapChainDescr, x, y, w, h);
}

void D3DScene::resize(float x, float y, float w, float h)
{
    if (m_inited)
    {
        m_context->OMSetRenderTargets(0, 0, 0);
        m_target = nullptr;

        auto hr = m_swapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
        assert(SUCCEEDED(hr));

        ID3D11Texture2D* pBuffer = nullptr;
        hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
            reinterpret_cast<void**>(&pBuffer));
        assert(SUCCEEDED(hr));

        hr = m_device->CreateRenderTargetView(pBuffer, NULL, &m_target);
        pBuffer->Release();

        assert(SUCCEEDED(hr));
        
        m_viewport.TopLeftX = x;
        m_viewport.TopLeftY = y;
        m_viewport.Width = w;
        m_viewport.Height = h;

       
        updateProjection();
    }
}

void D3DScene::updateProjection()
{
    m_projection = m_buildProjection(*this);
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
    auto hr = m_context->Map(m_constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    assert(SUCCEEDED(hr));
    memcpy(mappedResource.pData, &m_projection, sizeof(m_projection));
    m_context->Unmap(m_constantBuffer.Get(), 0);
}

void D3DScene::render(std::initializer_list<D3DObject*>&& objs)
{
    m_context->ClearRenderTargetView(m_target.Get(), m_background.data());

    for (auto obj : objs)
    {
        resetState();
        obj->render(*this);
    }

    m_swapChain->Present(1, 0);
}

void D3DScene::render(const std::vector<D3DObject*>& objs)
{
    m_context->ClearRenderTargetView(m_target.Get(), m_background.data());

    for (auto obj : objs)
    {
        resetState();
        obj->render(*this);
    }

    m_swapChain->Present(1, 0);
}

Microsoft::WRL::ComPtr<ID3D11Device> D3DScene::getDevice() const
{
    return this->m_device;
}
Microsoft::WRL::ComPtr<ID3D11DeviceContext> D3DScene::getContext() const
{
    return this->m_context;
}

Microsoft::WRL::ComPtr<IDXGISwapChain> D3DScene::getSwapChain() const
{
    return this->m_swapChain;
}

const D3D11_VIEWPORT& D3DScene::getViewport() const
{
    return this->m_viewport;
}

void D3DScene::resetState()
{
    m_context->ClearState();
    m_context->OMSetRenderTargets(1, m_target.GetAddressOf(), NULL);
    m_context->RSSetViewports(1, &m_viewport);
    m_context->VSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());
}

D3DScene::~D3DScene()
{
    m_device = nullptr;
    m_context = nullptr;
    m_swapChain = nullptr;
    m_target = nullptr;
    m_constantBuffer = nullptr;
}
