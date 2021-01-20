#include <vector>
#include <RenderTargetState.h>
#include <Object3D.h>
#include <Scene3D.h>

namespace DX
{
    namespace Sharp
    {
        using namespace std;
        using namespace System;
        using namespace System::Buffers;
        using namespace System::Collections::Generic;
        using namespace DirectX;
        using namespace DX::Sharp::Direct3D;
        using namespace DX::Sharp::Direct3D12;
        using namespace DX::Sharp::DXGI;

        Object3D::Object3D(
            IEnumerable<Attribute^>^ attributes,
            Index^ index,
            IEnumerable<Constant^>^ constants,
            DescriptorHeap^ textureHeap,
            DescriptorHeap^ samplerHeap,
            PrimitiveTopology primitiveTopology
        ) :
            m_attributes(attributes),
            m_index(index),
            m_constants(constants),
            m_textureHeap(textureHeap),
            m_samplerHeap(samplerHeap)
        {
            vector<D3DAttribute*> attributePtr;
            vector<D3DConstant*> constantPtr;

            for each (auto attr in m_attributes)
            {
                attributePtr.push_back(attr->Value);
            }

            for each (auto constant in m_constants)
            {
                constantPtr.push_back(constant->Value);
            }

            m_value = new D3DObject(move(attributePtr), m_index->Value, move(constantPtr), m_textureHeap->Value, m_samplerHeap->Value, static_cast<D3D12_PRIMITIVE_TOPOLOGY>(primitiveTopology));
        }

        void Object3D::Initialize(
            Scene3D^ scene,
            Memory<Byte> vertexShader,
            Memory<Byte> pixelShader,
            BlendDescription% blend,
            DepthStencilDescription% depthStencil,
            RasteriazerDescription% rasterizer,
            Format rtFormat,
            Format dsFormat,
            PrimitiveTopologyType primitiveTopology,
            IndexBufferStripCutValue stripCutValue)
        {
            auto renderTarget = RenderTargetState(static_cast<DXGI_FORMAT>(rtFormat), static_cast<DXGI_FORMAT>(dsFormat));
            pin_ptr<BlendDescription> blendPtr = &blend;
            pin_ptr<DepthStencilDescription> depthPtr = &depthStencil;
            pin_ptr<RasteriazerDescription> rasterizerPtr = &rasterizer;
            MemoryHandle^ vertexShaderData = vertexShader.Pin();
            MemoryHandle^ pixelShaderData = pixelShader.Pin();
            D3D12_SHADER_BYTECODE vertex = { vertexShaderData->Pointer, static_cast<unsigned int>(vertexShader.Length) };
            D3D12_SHADER_BYTECODE pixel = { pixelShaderData->Pointer,  static_cast<unsigned int>(pixelShader.Length) };

            try {
                m_value->Initialize(
                    *(scene->Value),
                    vertex, pixel,
                    *reinterpret_cast<D3D12_BLEND_DESC*>(blendPtr),
                    *reinterpret_cast<D3D12_DEPTH_STENCIL_DESC*>(depthPtr),
                    *reinterpret_cast<D3D12_RASTERIZER_DESC*>(rasterizerPtr),
                    renderTarget,
                    static_cast<D3D12_PRIMITIVE_TOPOLOGY_TYPE>(primitiveTopology),
                    static_cast<D3D12_INDEX_BUFFER_STRIP_CUT_VALUE>(stripCutValue)
                );
            }
            finally
            {
                delete vertexShaderData;
                delete pixelShaderData;
            }
        }

        void Object3D::Initialize(
            Scene3D^ scene,
            Memory<Byte> vertexShader,
            Memory<Byte> pixelShader,
            BlendDescription% blend,
            DepthStencilDescription% depthStencil,
            RasteriazerDescription% rasterizer,
            Format rtFormat,
            Format dsFormat
        )
        {
            Initialize(scene, vertexShader, pixelShader, blend, depthStencil, rasterizer, rtFormat, dsFormat, PrimitiveTopologyType::Triangle, IndexBufferStripCutValue::Disabled);
        }

        void Object3D::UpdateTransform(DX::Sharp::SimpleMath::XMMatrix^ transform)
        {
            m_value->UpdateTransform(*transform->Value);
        }

        void Object3D::UpdateIndices(void* data)
        {
            m_value->UpdateIndices(data);
        }

        void Object3D::UpdateConstant(size_t index, void* data)
        {
            m_value->UpdateConstant(index, data);
        }

        void Object3D::UpdateAttribute(size_t index, void* data)
        {
            m_value->UpdateAttribute(index, data);
        }

        generic<typename T> where T : value class
            void Object3D::UpdateIndices(Memory<T> data)
        {
            safe_cast<MemoryIndex<T>^>(m_index)->Update(data);
        }

        generic<typename T> where T : value class
            void Object3D::UpdateConstant(size_t index, Memory<T> data)
        {
            size_t idx = 0;
            for each (auto constant in m_constants)
            {
                if (idx == index)
                {
                    safe_cast<MemoryConstant<T>^>(constant)->Update(data);
                    break;
                }
            }
        }

        generic<typename T> where T : value class
            void Object3D::UpdateAttribute(size_t index, Memory<T> data)
        {
            size_t idx = 0;
            for each (auto attr in m_attributes)
            {
                if (idx == index)
                {
                    safe_cast<MemoryAttribute<T>^>(attr)->Update(data);
                    break;
                }
            }
        }

        Object3D::~Object3D()
        {
            m_attributes = nullptr;
            m_index = nullptr;
            m_constants = nullptr;
            m_textureHeap = nullptr;
            m_samplerHeap = nullptr;
            this->!Object3D();
        }

        Object3D::!Object3D()
        {
            delete m_value;
            m_value = nullptr;
        }
    }
}
