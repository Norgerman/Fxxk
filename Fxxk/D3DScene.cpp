#include "D3DScene.h"
#include "D3DObject.h"

#pragma comment(lib, "d3d11.lib")

void D3DScene::init(HWND hwnd)
{
    DXGI_SWAP_CHAIN_DESC swapChainDescr = { 0 };
    swapChainDescr.BufferDesc.RefreshRate.Denominator = 1;
    swapChainDescr.BufferDesc.RefreshRate.Numerator = 0;
    swapChainDescr.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    swapChainDescr.SampleDesc.Count = 1;
    swapChainDescr.SampleDesc.Quality = 0;
    swapChainDescr.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDescr.BufferCount = 1;
    swapChainDescr.Windowed = true;
    swapChainDescr.OutputWindow = hwnd;

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

    RECT winRect;
    GetClientRect(hwnd, &winRect);

    m_viewport.Width = static_cast<float>(winRect.right - winRect.left);
    m_viewport.Height = static_cast<float>(winRect.bottom - winRect.top);

    this->m_constant = new VsConstant;
    m_constant->projection = m_buildProjection(*this);

    D3D11_SUBRESOURCE_DATA sr = { 0 };
    D3D11_BUFFER_DESC cbDesc = { };
    cbDesc.ByteWidth = sizeof(VsConstant);
    cbDesc.Usage = D3D11_USAGE_DYNAMIC;
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    cbDesc.MiscFlags = 0;
    cbDesc.StructureByteStride = 0;
    sr.pSysMem = m_constant;
    hr = m_device->CreateBuffer(&cbDesc, &sr,
        &m_constantBuffer);

    assert(SUCCEEDED(hr));
    m_context->RSSetViewports(1, &m_viewport);
    m_context->OMSetRenderTargets(1, &m_target, NULL);
    m_context->VSSetConstantBuffers(0, 1, &m_constantBuffer);
    m_inited = true;
}

void D3DScene::resize(float width, float height)
{
    const std::lock_guard lock(m_mutex);
    if (m_inited)
    {
        m_context->OMSetRenderTargets(0, 0, 0);
        m_target->Release();
        auto hr = m_swapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
        assert(SUCCEEDED(hr));

        ID3D11Texture2D* pBuffer = nullptr;
        hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
            reinterpret_cast<void**>(&pBuffer));
        assert(SUCCEEDED(hr));

        hr = m_device->CreateRenderTargetView(pBuffer, NULL, &m_target);
        assert(SUCCEEDED(hr));

        pBuffer->Release();
        m_context->OMSetRenderTargets(1, &m_target, NULL);
        m_viewport.Width = (float)width;
        m_viewport.Height = (float)height;

        m_context->RSSetViewports(1, &m_viewport);
        updateProjection();
    }
}

void D3DScene::updateProjection()
{
    m_constant->projection = m_buildProjection(*this);
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
    auto hr = m_context->Map(m_constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    assert(SUCCEEDED(hr));
    memcpy(mappedResource.pData, m_constant, sizeof(*m_constant));
    m_context->Unmap(m_constantBuffer, 0);
}

void D3DScene::render(std::initializer_list<D3DObject*>&& objs)
{
    const std::lock_guard lock(m_mutex);
    m_context->ClearRenderTargetView(m_target, m_background.data());

    for (auto obj : objs)
    {
        obj->render(*this);
    }

    m_swapChain->Present(1, 0);
}

ID3D11Device* D3DScene::getDevice() const
{
    return this->m_device;
}
ID3D11DeviceContext* D3DScene::getContext() const
{
    return this->m_context;
}

const D3D11_VIEWPORT& D3DScene::getViewport() const
{
    return this->m_viewport;
}

void D3DScene::dispose()
{
    if (m_device)
    {
        m_device->Release();
    }
    if (m_context)
    {
        m_context->Release();
    }
    if (m_swapChain)
    {
        m_swapChain->Release();
    }
    if (m_target)
    {
        m_target->Release();
    }
    if (m_constantBuffer)
    {
        m_constantBuffer->Release();
    }
    delete m_constant;
    m_device = nullptr;
    m_context = nullptr;
    m_swapChain = nullptr;
    m_target = nullptr;
    m_constantBuffer = nullptr;
    m_constant = nullptr;
}

D3DScene::~D3DScene()
{
    this->dispose();
}
