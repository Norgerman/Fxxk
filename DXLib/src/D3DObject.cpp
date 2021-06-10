#include <EffectPipelineStateDescription.h>
#include <DirectXHelpers.h>
#include <winrt/base.h>
#include <wil/result.h>
#include <D3DScene.h>
#include <D3DObject.h>
#include <D3DEffect.h>
#include "d3dx12.h"


namespace DX {
    using namespace DirectX;
    using namespace std;
    using namespace winrt;

    typedef struct BasicObjectConstant
    {
        XMMATRIX Transform;
        XMMATRIX View;
        XMMATRIX ModelView;
        XMMATRIX ModelViewInverse;
    } BasicObjectConstant;

    __declspec(align(16)) class D3DObject::Impl
    {
    public:
        Impl(
            vector<D3DAttribute*>&& attributes,
            D3DIndex* indices,
            vector<D3DConstant*>&& constants,
            D3DEffect* effect,
            D3DDescriptorHeap* textureHeap,
            D3DDescriptorHeap* samplerHeap,
            uint32_t instanceCount,
            D3D12_PRIMITIVE_TOPOLOGY primitiveTopology) :
            m_attributes(move(attributes)),
            m_indices(indices),
            m_constants(move(constants)),
            m_effect(effect),
            m_textureHeap(textureHeap),
            m_samplerHeap(samplerHeap),
            m_instanceCount(instanceCount),
            m_primitiveTopology(primitiveTopology),
            m_basicConstant({ XMMatrixIdentity(), XMMatrixIdentity(), XMMatrixIdentity(), XMMatrixIdentity() })
        {
        }

        Impl(
            const vector<D3DAttribute*>& attributes,
            D3DIndex* indices,
            const vector<D3DConstant*>& constants,
            D3DEffect* effect,
            D3DDescriptorHeap* textureHeap,
            D3DDescriptorHeap* samplerHeap,
            uint32_t instanceCount,
            D3D12_PRIMITIVE_TOPOLOGY primitiveTopology) :
            m_attributes(attributes),
            m_indices(indices),
            m_constants(constants),
            m_effect(effect),
            m_textureHeap(textureHeap),
            m_samplerHeap(samplerHeap),
            m_instanceCount(instanceCount),
            m_primitiveTopology(primitiveTopology),
            m_basicConstant({ XMMatrixIdentity(), XMMatrixIdentity() })
        {
        }

        void Initialize(D3DScene& scene)
        {
            m_modelViewBuffer = make_unique<D3DConstant>(&scene, 4, 16 * 4);
            m_modelViewBuffer->Update(&m_basicConstant);
        }

        void UpdateModelView(const DirectX::XMMATRIX& transform, const DirectX::XMMATRIX& view)
        {
            m_basicConstant.Transform = transform;
            m_basicConstant.View = view;
            m_basicConstant.ModelView = XMMatrixMultiply(transform, view);
            m_basicConstant.ModelViewInverse = XMMatrixInverse(nullptr, m_basicConstant.ModelView);
            m_modelViewBuffer->Update(&m_basicConstant);
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
            vector<ID3D12DescriptorHeap*> heaps;
            if (m_textureHeap)
            {
                heaps.push_back(m_textureHeap->Heap());
            }

            if (m_samplerHeap)
            {
                heaps.push_back(m_samplerHeap->Heap());
            }

            m_effect->Apply(scene);

            // constants
            commandList->SetGraphicsRootConstantBufferView(idx, m_modelViewBuffer->GpuAddress());
            idx++;
            for (auto& constant : m_constants)
            {
                commandList->SetGraphicsRootConstantBufferView(idx, constant->GpuAddress());
                idx++;
            }

            // heaps
            if (heaps.size() > 0)
            {
                commandList->SetDescriptorHeaps(static_cast<uint32_t>(heaps.size()), heaps.data());
            }

            // textures
            if (m_textureHeap)
            {
                commandList->SetGraphicsRootDescriptorTable(idx++, m_textureHeap->GetFirstGpuHandle());
            }

            // samplers
            if (m_samplerHeap)
            {
                commandList->SetGraphicsRootDescriptorTable(idx++, m_samplerHeap->GetFirstGpuHandle());
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
            commandList->DrawIndexedInstanced(m_indices->Size(), m_instanceCount, 0, 0, 0);
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
            m_modelViewBuffer->Reset();
            m_effect->Reset();
        }

        void* operator new(size_t i)
        {
            return _mm_malloc(i, 16);
        }

        void operator delete(void* p)
        {
            _mm_free(p);
        }
    private:
        vector<D3DAttribute*> m_attributes;
        vector<D3DConstant*> m_constants;
        D3DEffect* m_effect;
        D3DDescriptorHeap* m_textureHeap;
        D3DDescriptorHeap* m_samplerHeap;
        unique_ptr<D3DConstant> m_modelViewBuffer;
        D3DIndex* m_indices;
        BasicObjectConstant m_basicConstant;
        D3D12_PRIMITIVE_TOPOLOGY m_primitiveTopology;
        uint32_t m_instanceCount;
    };

    D3DObject::D3DObject(
        vector<D3DAttribute*>&& attributes,
        D3DIndex* indices,
        std::vector<D3DConstant*>&& constants,
        D3DEffect* effect,
        D3DDescriptorHeap* textureHeap,
        D3DDescriptorHeap* samplerHeap,
        uint32_t instanceCount,
        D3D12_PRIMITIVE_TOPOLOGY primitiveTopology) :
        m_impl(make_unique<Impl>(move(attributes), indices, move(constants), effect, textureHeap, samplerHeap, instanceCount, primitiveTopology))

    {

    }

    D3DObject::D3DObject(
        const vector<D3DAttribute*>& attributes,
        D3DIndex* indices,
        const std::vector<D3DConstant*>& constants,
        D3DEffect* effect,
        D3DDescriptorHeap* textureHeap,
        D3DDescriptorHeap* samplerHeap,
        uint32_t instanceCount,
        D3D12_PRIMITIVE_TOPOLOGY primitiveTopology) :
        m_impl(make_unique<Impl>(attributes, indices, constants, effect, textureHeap, samplerHeap, instanceCount, primitiveTopology))
    {

    }

    void D3DObject::Initialize(D3DScene& scene)
    {
        m_impl->Initialize(scene);
    }

    void D3DObject::UpdateModelView(const DirectX::XMMATRIX& transform, const DirectX::XMMATRIX& view)
    {
        m_impl->UpdateModelView(transform, view);
    }

    void D3DObject::UpdateModelView(DirectX::XMMATRIX&& transform, DirectX::XMMATRIX&& view)
    {
        m_impl->UpdateModelView(transform, view);
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
