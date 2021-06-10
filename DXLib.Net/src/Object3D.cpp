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
        using namespace System::Collections::Generic;
        using namespace DX::Sharp::Direct3D;

        Object3D::Object3D(
            IEnumerable<Attribute^>^ attributes,
            Index^ index,
            IEnumerable<Constant^>^ constants,
            Effect3D^ effect,
            DescriptorHeap^ textureHeap,
            DescriptorHeap^ samplerHeap,
            System::UInt32 instanceCount,
            PrimitiveTopology primitiveTopology
        ) :
            m_attributes(attributes),
            m_index(index),
            m_constants(constants),
            m_effect(effect),
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

            m_value = new D3DObject(move(attributePtr), m_index->Value, move(constantPtr), m_effect->Value, m_textureHeap ? m_textureHeap->Value : nullptr, m_samplerHeap ? m_samplerHeap->Value : nullptr, instanceCount, static_cast<D3D12_PRIMITIVE_TOPOLOGY>(primitiveTopology));
        }

        Object3D::Object3D(
            IEnumerable<Attribute^>^ attributes,
            Index^ index,
            IEnumerable<Constant^>^ constants,
            Effect3D^ effect,
            DescriptorHeap^ textureHeap,
            DescriptorHeap^ samplerHeap) :
            Object3D(attributes, index, constants, effect, textureHeap, samplerHeap, 1, PrimitiveTopology::TriangleList)
        {

        }

        void Object3D::Initialize(Scene3D^ scene)
        {
            m_value->Initialize(*(scene->Value));
        }

        void Object3D::UpdateModelView(DX::Sharp::SimpleMath::XMMatrix^ transform, DX::Sharp::SimpleMath::XMMatrix^ view)
        {
            m_value->UpdateModelView(*transform->Value, *view->Value);
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
