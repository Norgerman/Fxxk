#include <EffectPipelineStateDescription.h>
#include <DirectXHelpers.h>
#include <wil/result.h>
#include "d3dx12.h"
#include "D3DScene.h"
#include "D3DObject.h"


using namespace DirectX;
using namespace std;

void D3DObject::Initialize(
    D3DScene& scene,
    D3D12_SHADER_BYTECODE& vertexShader,
    D3D12_SHADER_BYTECODE& pixelShader,
    const D3D12_BLEND_DESC& blend,
    const D3D12_DEPTH_STENCIL_DESC& depthStencil,
    const D3D12_RASTERIZER_DESC& rasterizer,
    const RenderTargetState& renderTarget,
    D3D12_PRIMITIVE_TOPOLOGY_TYPE primitiveTopology,
    D3D12_INDEX_BUFFER_STRIP_CUT_VALUE stripCutValue)
{
    vector<D3D12_INPUT_ELEMENT_DESC> elements;
    uint32_t idx = 0;
    for (auto& attribute : m_attributes)
    {
        attribute->AppendElement(idx, elements);
        idx++;
    }
    D3D12_INPUT_LAYOUT_DESC layoutDesc = {
        elements.data(),
        static_cast<uint32_t>(elements.size())
    };

    EffectPipelineStateDescription pd = {
        &layoutDesc,
        blend,
        depthStencil,
        rasterizer,
        renderTarget,
        primitiveTopology,
        stripCutValue
    };

    D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;
    vector<CD3DX12_ROOT_PARAMETER> rootParameters(m_constants.size() + m_textureHeap->Count() + m_samplerHeap->Count() + 2);
    vector<CD3DX12_DESCRIPTOR_RANGE> textureRange(m_textureHeap->Count());
    vector<CD3DX12_DESCRIPTOR_RANGE> samplerRange(m_samplerHeap->Count());
    CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};

    rootParameters[0].InitAsConstantBufferView(0);
    rootParameters[1].InitAsConstantBufferView(1);
    idx = 2;

    // constants
    auto count = m_constants.size();
    for (size_t i = 0; i < count; i++, idx++)
    {
        rootParameters[idx].InitAsConstantBufferView(static_cast<uint32_t>(i + 2));
    }

    // textures
    count = m_textureHeap->Count();
    for (size_t i = 0; i < count; i++, idx++)
    {
        textureRange[i] = CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, static_cast<uint32_t>(i));
        rootParameters[idx].InitAsDescriptorTable(1, &textureRange[i], D3D12_SHADER_VISIBILITY_PIXEL);
    }

    // samplers
    count = m_samplerHeap->Count();
    for (size_t i = 0; i < count; i++, idx++)
    {
        samplerRange[i] = CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, static_cast<uint32_t>(i));
        rootParameters[idx].InitAsDescriptorTable(1, &samplerRange[i], D3D12_SHADER_VISIBILITY_PIXEL);
    }

    rootSignatureDesc.Init(static_cast<uint32_t>(rootParameters.size()), rootParameters.data(), 0, nullptr, rootSignatureFlags);
    auto hr = CreateRootSignature(scene.Device().Get(), &rootSignatureDesc, &m_rootSignature);

    THROW_IF_FAILED(hr);

    pd.CreatePipelineState(scene.Device().Get(), m_rootSignature.Get(), vertexShader, pixelShader, &m_pipelineState);
}
void D3DObject::UpdateTransform(const XMMATRIX& transform)
{
    m_transform = transform;
    m_transformBuffer.Update(&m_transform);
}

void D3DObject::UpdateTransform(XMMATRIX&& transform)
{
    UpdateTransform(transform);
}

void D3DObject::UpdateIndices(const void* data)
{
    m_indices->Update(data);
}

void D3DObject::UpdateConstant(size_t index, const void* data)
{
    m_constants[index]->Update(data);
}

void D3DObject::UpdateAttribute(size_t index, const void* data)
{
    m_attributes[index]->Update(data);
}

void D3DObject::Render(D3DScene& scene)
{
    m_transformBuffer.Upload(scene);
    m_indices->Upload(scene);

    for (auto& attribute : m_attributes)
    {
        attribute->Upload(scene);
    }
    for (auto& constant : m_constants)
    {
        constant->Upload(scene);
    }

    auto commandList = scene.CommandList();
    uint32_t idx = 2;
    ID3D12DescriptorHeap* heaps[] = { m_textureHeap->Heap(), m_samplerHeap->Heap() };

    commandList->SetGraphicsRootSignature(m_rootSignature.Get());
    commandList->SetPipelineState(m_pipelineState.Get());
    commandList->SetDescriptorHeaps(2, heaps);

    // constants
    commandList->SetGraphicsRootConstantBufferView(0, scene.Projection().GpuAddress());
    commandList->SetGraphicsRootConstantBufferView(1, m_transformBuffer.GpuAddress());
    for (auto& constant : m_constants)
    {
        commandList->SetGraphicsRootConstantBufferView(idx, constant->GpuAddress());
        idx++;
    }

    // textures
    auto count = m_textureHeap->Count();
    for (auto i = 0; i < count; i++, idx++)
    {
        commandList->SetGraphicsRootDescriptorTable(idx, m_textureHeap->GetGpuHandle(i));
    }

    // samplers
    count = m_samplerHeap->Count();
    for (auto i = 0; i < count; i++, idx++)
    {
        commandList->SetGraphicsRootDescriptorTable(idx, m_samplerHeap->GetGpuHandle(i));
    }

    commandList->IASetIndexBuffer(&m_indices->BufferView());

    vector<D3D12_VERTEX_BUFFER_VIEW> vertexView;
    for (auto& attribute : m_attributes)
    {
        vertexView.push_back(attribute->BufferView());
    }
    commandList->IASetVertexBuffers(0, static_cast<uint32_t>(vertexView.size()), vertexView.data());
    commandList->IASetPrimitiveTopology(m_primitiveTopology);
    commandList->DrawIndexedInstanced(m_indices->Size(), 1, 0, 0, 0);
}

void D3DObject::Reset()
{
    for (auto& attribute : m_attributes)
    {
        attribute->Reset();
    }
    for (auto& constant : m_constants)
    {
        constant->Reset();
    }
    m_indices->Reset();
    m_transformBuffer.Reset();
    m_pipelineState.Reset();
    m_rootSignature.Reset();
}
