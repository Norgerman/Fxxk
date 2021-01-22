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
    class D3DEffect;

    class dllexport D3DObject
    {
    public:
        D3DObject(
            std::vector<D3DAttribute*>&& attributes,
            D3DIndex* indices,
            std::vector<D3DConstant*>&& constants,
            D3DEffect* effect,
            D3DDescriptorHeap* textureHeap,
            D3DDescriptorHeap* samplerHeap,
            D3D12_PRIMITIVE_TOPOLOGY primitiveTopology = D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        D3DObject(
            const std::vector<D3DAttribute*>& attributes,
            D3DIndex* indices,
            const std::vector<D3DConstant*>& constants,
            D3DEffect* effect,
            D3DDescriptorHeap* textureHeap,
            D3DDescriptorHeap* samplerHeap,
            D3D12_PRIMITIVE_TOPOLOGY primitiveTopology = D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        virtual void Initialize(D3DScene& scene);
        virtual void UpdateTransform(const DirectX::XMMATRIX& transform);
        virtual void UpdateTransform(DirectX::XMMATRIX&& transform);
        void UpdateIndices(const void* data);
        void UpdateConstant(size_t index, const void* data);
        void UpdateAttribute(size_t index, const void* data);
        void Reset();
        void Render(D3DScene& scene);
        virtual ~D3DObject();
    private:
        class Impl;
        std::unique_ptr<Impl> m_impl;
    };
}
