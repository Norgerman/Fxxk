#pragma once
#include <vector>
#include <memory>
#include <wrl/client.h>
#include <DirectXMath.h>
#include <RenderTargetState.h>
#include <DescriptorHeap.h>
#include "D3DIndex.h"
#include "D3DAttribute.h"
#include "D3DConstant.h"

class D3DObject
{
public:
    template<typename T1, typename T2, typename T3>
    D3DObject(
        T1&& attributes, 
        T2&& indices, 
        T3&& constants, 
        std::shared_ptr<DirectX::DescriptorHeap> textureHeap, 
        std::shared_ptr<DirectX::DescriptorHeap> samplerHeap,
        D3D12_PRIMITIVE_TOPOLOGY primitiveTopology = D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST) :
        m_attributes(std::forward<T1>(attributes)),
        m_indices(std::forward<T2>(indices)),
        m_constants(std::forward<T3>(constants)),
        m_textureHeap(textureHeap),
        m_samplerHeap(samplerHeap),
        m_primitiveTopology(primitiveTopology),
        m_transform(DirectX::XMMatrixIdentity()),
        m_transformBuffer(nullptr, 4, 16)
    {
        m_transformBuffer.Update(&m_transform);
    }
    void Initialize(
        D3DScene& scene, 
        D3D12_SHADER_BYTECODE& vertexShader, 
        D3D12_SHADER_BYTECODE& pixelShader, 
        const D3D12_BLEND_DESC& blend, 
        const D3D12_DEPTH_STENCIL_DESC& depthStencil, 
        const D3D12_RASTERIZER_DESC& rasterizer, 
        const DirectX::RenderTargetState& renderTarget, 
        D3D12_PRIMITIVE_TOPOLOGY_TYPE primitiveTopology = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
        D3D12_INDEX_BUFFER_STRIP_CUT_VALUE stripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED);
    void UpdateTransform(const DirectX::XMMATRIX& transform);
    void UpdateTransform(DirectX::XMMATRIX&& transform);
    void UpdateIndices(const void* data);
    void UpdateConstant(size_t index, const void* data);
    void UpdateAttribute(size_t index, const void* data);
    void Reset();
    void Render(D3DScene& scene);
private:

    Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pipelineState;
    Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;
    std::vector<D3DAttribute> m_attributes;
    std::vector<D3DConstant> m_constants;
    std::shared_ptr<DirectX::DescriptorHeap> m_textureHeap;
    std::shared_ptr<DirectX::DescriptorHeap> m_samplerHeap;
    D3DConstant m_transformBuffer;
    D3DIndex m_indices;
    DirectX::XMMATRIX m_transform;
    D3D12_PRIMITIVE_TOPOLOGY m_primitiveTopology;
};

