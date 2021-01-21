#include <EffectPipelineStateDescription.h>
#include <DirectXHelpers.h>
#include <wrl/client.h>
#include <wil/result.h>
#include <D3DScene.h>
#include <D3DObject.h>
#include <D3DEffect.h>
#include "d3dx12.h"


namespace DX {
    using namespace DirectX;
    using namespace std;
    using namespace Microsoft::WRL;

    class D3DObject::Impl {
    public:
        Impl(
            vector<D3DAttribute*>&& attributes,
            D3DIndex* indices,
            vector<D3DConstant*>&& constants,
            D3DEffect* effect,
            D3DDescriptorHeap* textureHeap,
            D3DDescriptorHeap* samplerHeap,
            D3D12_PRIMITIVE_TOPOLOGY primitiveTopology) :
            m_attributes(move(attributes)),
            m_indices(indices),
            m_constants(move(constants)),
            m_effect(effect),
            m_textureHeap(textureHeap),
            m_samplerHeap(samplerHeap),
            m_primitiveTopology(primitiveTopology),
            m_transform(DirectX::XMMatrixIdentity())
        {
        }

        Impl(
            const vector<D3DAttribute*>& attributes,
            D3DIndex* indices,
            const vector<D3DConstant*>& constants,
            D3DEffect* effect,
            D3DDescriptorHeap* textureHeap,
            D3DDescriptorHeap* samplerHeap,
            D3D12_PRIMITIVE_TOPOLOGY primitiveTopology) :
            m_attributes(attributes),
            m_indices(indices),
            m_constants(constants),
            m_effect(effect),
            m_textureHeap(textureHeap),
            m_samplerHeap(samplerHeap),
            m_primitiveTopology(primitiveTopology),
            m_transform(DirectX::XMMatrixIdentity())
        {
        }

        void Initialize(D3DScene& scene)
        {
            m_transformBuffer = make_unique<D3DConstant>(&scene, 4, 16);
            m_transformBuffer->Update(&m_transform);
        }

        void UpdateTransform(const XMMATRIX& transform)
        {
            m_transform = transform;
            m_transformBuffer->Update(&m_transform);
        }

        void UpdateIndices(const void* data)
        {
            m_indices->Update(data);
        }

        void UpdateConstant(size_t index, const void* data)
        {
            m_constants[index]->Update(data);
        }

        void UpdateAttribute(size_t index, const void* data)
        {
            m_attributes[index]->Update(data);
        }

        void Render(D3DScene& scene)
        {
            auto commandList = scene.CommandList();
            uint32_t idx = static_cast<uint32_t>(m_effect->ObjectConstantStartSolt());
            ID3D12DescriptorHeap* heaps[] = { m_textureHeap->Heap(), m_samplerHeap->Heap() };

            m_effect->Apply(scene);

            // constants
            commandList->SetGraphicsRootConstantBufferView(idx, m_transformBuffer->GpuAddress());
            idx++;
            for (auto& constant : m_constants)
            {
                commandList->SetGraphicsRootConstantBufferView(idx, constant->GpuAddress());
                idx++;
            }

            // heaps
            commandList->SetDescriptorHeaps(2, heaps);

            // textures
            auto count = m_textureHeap->Count();
            for (size_t i = 0; i < count; i++, idx++)
            {
                commandList->SetGraphicsRootDescriptorTable(idx, m_textureHeap->GetGpuHandle(i));
            }

            // samplers
            count = m_samplerHeap->Count();
            for (size_t i = 0; i < count; i++, idx++)
            {
                commandList->SetGraphicsRootDescriptorTable(idx, m_samplerHeap->GetGpuHandle(i));
            }

            // vertex
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

        void Reset()
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
            m_transformBuffer->Reset();
            m_effect->Reset();
        }

    private:
        vector<D3DAttribute*> m_attributes;
        vector<D3DConstant*> m_constants;
        D3DEffect* m_effect;
        D3DDescriptorHeap* m_textureHeap;
        D3DDescriptorHeap* m_samplerHeap;
        unique_ptr<D3DConstant> m_transformBuffer;
        D3DIndex* m_indices;
        XMMATRIX m_transform;
        D3D12_PRIMITIVE_TOPOLOGY m_primitiveTopology;
    };

    D3DObject::D3DObject(
        vector<D3DAttribute*>&& attributes,
        D3DIndex* indices,
        std::vector<D3DConstant*>&& constants,
        D3DEffect* effect,
        D3DDescriptorHeap* textureHeap,
        D3DDescriptorHeap* samplerHeap,
        D3D12_PRIMITIVE_TOPOLOGY primitiveTopology) :
        m_impl(make_unique<Impl>(move(attributes), indices, move(constants), effect, textureHeap, samplerHeap, primitiveTopology))

    {

    }

    D3DObject::D3DObject(
        const vector<D3DAttribute*>& attributes,
        D3DIndex* indices,
        const std::vector<D3DConstant*>& constants,
        D3DEffect* effect,
        D3DDescriptorHeap* textureHeap,
        D3DDescriptorHeap* samplerHeap,
        D3D12_PRIMITIVE_TOPOLOGY primitiveTopology) : 
        m_impl(make_unique<Impl>(attributes, indices, constants, effect, textureHeap, samplerHeap, primitiveTopology))
    {

    }

    void D3DObject::Initialize(D3DScene& scene)
    {
        m_impl->Initialize(scene);
    }

    void D3DObject::UpdateTransform(const XMMATRIX& transform)
    {
        m_impl->UpdateTransform(transform);
    }

    void D3DObject::UpdateTransform(XMMATRIX&& transform)
    {
        m_impl->UpdateTransform(transform);
    }

    void D3DObject::UpdateIndices(const void* data)
    {
        m_impl->UpdateIndices(data);
    }

    void D3DObject::UpdateConstant(size_t index, const void* data)
    {
        m_impl->UpdateConstant(index, data);
    }

    void D3DObject::UpdateAttribute(size_t index, const void* data)
    {
        m_impl->UpdateAttribute(index, data);
    }

    void D3DObject::Render(D3DScene& scene)
    {
        m_impl->Render(scene);
    }

    void D3DObject::Reset()
    {
        m_impl->Reset();
    }

    D3DObject::~D3DObject()
    {

    }
}
