#pragma once
#include <vector>
#include <memory>
#include <DirectXMath.h>
#include <RenderTargetState.h>
#include <D3DDescriptorHeap.h>
#include <D3DIndex.h>
#include <D3DAttribute.h>
#include <D3DConstant.h>
#include <export.h>

namespace DX {
    class dllexport D3DObject
    {
    public:
        D3DObject(
            std::vector<D3DAttribute*>&& attributes,
            D3DIndex* indices,
            std::vector<D3DConstant*>&& constants,
            D3DDescriptorHeap* textureHeap,
            D3DDescriptorHeap* samplerHeap,
            D3D12_PRIMITIVE_TOPOLOGY primitiveTopology = D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        D3DObject(
            const std::vector<D3DAttribute*>& attributes,
            D3DIndex* indices,
            const std::vector<D3DConstant*>& constants,
            D3DDescriptorHeap* textureHeap,
            D3DDescriptorHeap* samplerHeap,
            D3D12_PRIMITIVE_TOPOLOGY primitiveTopology = D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
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
        ~D3DObject();
    private:
        class Impl;
        std::unique_ptr<Impl> m_impl;
    };
}
