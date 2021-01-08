#include <WICTextureLoader.h>
#include "D3DObject.h"
#include "D3DScene.h"

using namespace std;

void D3DObject::updateTransform(DirectX::XMMATRIX&& transform)
{
    m_transform = transform;
    m_transformDesc.updateData(&m_transform);
}

void D3DObject::updateTransform(DirectX::XMMATRIX& transform)
{
    m_transform = transform;
    m_transformDesc.updateData(&m_transform);
}

void D3DObject::uploadTransform(D3DScene& scene)
{
    m_transformDesc.upload(scene, m_transformBuffer);
}

void D3DObject::uploadVertexBuffer(D3DScene& scene)
{
    size_t i = 0;
    for (auto& attr : m_attributes)
    {
        if (i < m_indexPosition)
        {
            attr.upload(scene, m_vertexBuffer[i]);
        }
        else if (i > m_indexPosition)
        {
            attr.upload(scene, m_vertexBuffer[i - 1]);
        }
        i++;
    }
}

void D3DObject::uploadConstantBuffer(D3DScene& scene)
{
    size_t i = 0;
    for (auto& desc : m_vsConstants)
    {
        desc.upload(scene, m_vsConstantBuffer[i]);
    }
    for (auto& desc : m_psConstants)
    {
        desc.upload(scene, m_psConstantBuffer[i]);
    }
}

void D3DObject::uploadIndex(D3DScene& scene)
{
    if (m_indexAttribute && m_indexBuffer)
    {
        m_indexAttribute->upload(scene, m_indexBuffer);
    }
}

void D3DObject::render(D3DScene& scene)
{
    if (!m_inited)
    {
        return;
    }

    uploadTransform(scene);
    uploadIndex(scene);
    uploadVertexBuffer(scene);
    uploadConstantBuffer(scene);
    updateBlend(scene);

    scene.getContext()->RSSetState(m_rs);
    scene.getContext()->PSSetSamplers(0, static_cast<uint32_t>(m_ss.size()), m_ss.data());
    scene.getContext()->OMSetBlendState(m_blendState, m_blend.blendFactor, m_blend.sampleMask);
    scene.getContext()->IASetPrimitiveTopology(m_topology);
    scene.getContext()->IASetInputLayout(m_layout);
    scene.getContext()->VSSetConstantBuffers(1, 1, &m_transformBuffer);
    scene.getContext()->VSSetConstantBuffers(2, static_cast<uint32_t>(m_vsConstantBuffer.size()), m_vsConstantBuffer.data());
    scene.getContext()->PSSetConstantBuffers(0, static_cast<uint32_t>(m_psConstantBuffer.size()), m_psConstantBuffer.data());
    scene.getContext()->IASetVertexBuffers(0, static_cast<uint32_t>(m_vertexBuffer.size()), m_vertexBuffer.data(), m_strides.data(), m_offsets.data());
    scene.getContext()->PSSetShaderResources(0, static_cast<uint32_t>(m_textureViews.size()), m_textureViews.data());
    scene.getContext()->VSSetShader(m_vs, NULL, 0);
    scene.getContext()->PSSetShader(m_ps, NULL, 0);

    if (m_indexAttribute && m_indexBuffer)
    {
        scene.getContext()->IASetIndexBuffer(m_indexBuffer, m_indexAttribute->getFormat(), 0);
        scene.getContext()->DrawIndexed(m_indexAttribute->getSize(), 0, m_indexAttribute->getOffset());
    }
    else if (m_firstAttribute)
    {
        scene.getContext()->Draw(m_firstAttribute->getSize(), 0);
    }
}

void D3DObject::init(D3DScene& scene, D3DShader vertexShader, D3DShader pixelShader)
{
    auto hr = scene.getDevice()->CreateVertexShader(
        vertexShader.byteCode,
        vertexShader.size,
        NULL,
        &m_vs);
    assert(SUCCEEDED(hr));

    hr = scene.getDevice()->CreatePixelShader(
        pixelShader.byteCode,
        pixelShader.size,
        NULL,
        &m_ps);

    assert(SUCCEEDED(hr));

    // create vertex and index buffer
    D3D11_SUBRESOURCE_DATA sr = { 0 };
    D3D11_BUFFER_DESC desc = {};
    ID3D11Buffer* buffer = nullptr;
    size_t idx = 0;
    uint32_t solt = 0;
    std::vector<D3D11_INPUT_ELEMENT_DESC> inputDesc;

    for (auto& attr : m_attributes)
    {
        desc = attr.buildBufferDesc();
        sr.pSysMem = attr.getData();
        hr = scene.getDevice()->CreateBuffer(
            &desc,
            &sr,
            &buffer);

        assert(SUCCEEDED(hr));

        if (!m_indexBuffer && attr.isIndex())
        {
            m_indexAttribute = &attr;
            m_indexBuffer = buffer;
            m_indexPosition = idx;
        }
        else
        {
            m_vertexBuffer.push_back(buffer);
            attr.appendLayout(solt, inputDesc);
            m_strides.push_back(attr.getStride());
            m_offsets.push_back(attr.getOffset());
            solt++;
        }
        idx++;
    }

    hr = scene.getDevice()->CreateInputLayout(
        inputDesc.data(),
        static_cast<uint32_t>(inputDesc.size()),
        vertexShader.byteCode,
        vertexShader.size,
        &m_layout);

    assert(SUCCEEDED(hr));

    // create constant buffer
    desc = m_transformDesc.buildBufferDesc();
    sr.pSysMem = m_transformDesc.getData();
    hr = scene.getDevice()->CreateBuffer(
        &desc,
        &sr,
        &buffer);
    assert(SUCCEEDED(hr));
    m_transformBuffer = buffer;

    for (auto& constant : m_vsConstants)
    {
        desc = constant.buildBufferDesc();
        sr.pSysMem = constant.getData();
        hr = scene.getDevice()->CreateBuffer(
            &desc,
            &sr,
            &buffer);
        assert(SUCCEEDED(hr));
        m_vsConstantBuffer.push_back(buffer);
    }

    for (auto& constant : m_psConstants)
    {
        desc = constant.buildBufferDesc();
        sr.pSysMem = constant.getData();
        hr = scene.getDevice()->CreateBuffer(
            &desc,
            &sr,
            &buffer);
        assert(SUCCEEDED(hr));
        m_psConstantBuffer.push_back(buffer);
    }

    // load textures
    ID3D11Resource* texture = nullptr;
    ID3D11ShaderResourceView* textureView = nullptr;

    for (auto& element : m_textureFiles)
    {
        hr = DirectX::CreateWICTextureFromFile(scene.getDevice(), element.data(), &texture, &textureView);
        assert(SUCCEEDED(hr));
        m_textures.push_back(texture);
        m_textureViews.push_back(textureView);
    }

    hr = scene.getDevice()->CreateRasterizerState(&m_rasterizerDesc, &m_rs);
    assert(SUCCEEDED(hr));

    ID3D11SamplerState* sampleState = nullptr;

    for (auto& element : m_samplerDesc)
    {
        hr = scene.getDevice()->CreateSamplerState(&element, &sampleState);
        assert(SUCCEEDED(hr));
        m_ss.push_back(sampleState);
    }

    m_inited = true;
}

void D3DObject::disableBlend()
{
    if (m_blendState)
    {
        m_blendState->Release();
        m_blendState = nullptr;
    }
}

void D3DObject::updateAttribute(size_t index, const void* data)
{
    m_attributes[index].updateData(data);
}

void D3DObject::updateIndex(const void* data)
{
    if (m_indexAttribute)
    {
        m_indexAttribute->updateData(data);
    }
}

void D3DObject::updateVSConstant(size_t index, const void* data)
{
    m_vsConstants[index].updateData(data);
}

void D3DObject::updatePSConstant(size_t index, const void* data)
{
    m_psConstants[index].updateData(data);
}

void D3DObject::updateBlend(D3DScene& scene)
{
    if (m_blendNeedUpdate) 
    {
        disableBlend();
        scene.getDevice()->CreateBlendState(&m_blend.desc, &m_blendState);
        m_blendNeedUpdate = false;
    }
}

void D3DObject::dispose()
{
    if (m_indexBuffer)
    {
        m_indexBuffer->Release();
    }
    if (m_layout)
    {
        m_layout->Release();
    }
    if (m_vs)
    {
        m_vs->Release();
    }
    if (m_ps)
    {
        m_ps->Release();
    }
    if (m_rs)
    {
        m_rs->Release();
    }
    if (m_blendState)
    {
        m_blendState->Release();
    }
    m_indexBuffer = nullptr;
    m_layout = nullptr;
    m_vs = nullptr;
    m_ps = nullptr;
    m_blendState = nullptr;
    for (auto& element : m_vertexBuffer)
    {
        if (element)
        {
            element->Release();
        }
    }
    for (auto& element : m_psConstantBuffer)
    {
        if (element)
        {
            element->Release();
        }
    }
    for (auto& element : m_vsConstantBuffer)
    {
        if (element)
        {
            element->Release();
        }
    }
    for (auto& element : m_textures)
    {
        if (element)
        {
            element->Release();
        }
    }
    for (auto& element : m_textureViews)
    {
        if (element)
        {
            element->Release();
        }
    }
    for (auto& element : m_ss)
    {
        if (element)
        {
            element->Release();
        }
    }
    m_vertexBuffer.clear();
    m_psConstantBuffer.clear();
    m_vsConstantBuffer.clear();
    m_textures.clear();
    m_textureViews.clear();
    m_ss.clear();
}

D3DObject::~D3DObject()
{
    this->dispose();
}
