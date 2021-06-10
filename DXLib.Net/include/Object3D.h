#pragma once
#include <D3DObject.h>
#include <Attribute.h>
#include <Constant.h>
#include <Index.h>
#include <Enums.hpp>
#include <Effect3D.h>
#include <DescriptorHeapX.h>
#include <XMMatrix.h>

namespace DX
{
    namespace Sharp
    {
        ref class Scene3D;

        public ref class Object3D
        {
        public:
            property DX::D3DObject* Value
            {
                DX::D3DObject* get()
                {
                    return m_value;
                }
            }
            Object3D(
                System::Collections::Generic::IEnumerable<Attribute^>^ attributes,
                Index^ index,
                System::Collections::Generic::IEnumerable<Constant^>^ constants,
                Effect3D^ effect,
                DescriptorHeap^ textureHeap,
                DescriptorHeap^ samplerHeap,
                System::UInt32 instanceCount,
                Direct3D::PrimitiveTopology primitiveTopology
            );
            Object3D(
                System::Collections::Generic::IEnumerable<Attribute^>^ attributes,
                Index^ index,
                System::Collections::Generic::IEnumerable<Constant^>^ constants,
                Effect3D^ effect,
                DescriptorHeap^ textureHeap,
                DescriptorHeap^ samplerHeap
            );
            virtual void Initialize(Scene3D^ scene);
            virtual void UpdateModelView(DX::Sharp::SimpleMath::XMMatrix^ transform, DX::Sharp::SimpleMath::XMMatrix^ view);
            void UpdateIndices(void* data);
            void UpdateConstant(size_t index, void* data);
            void UpdateAttribute(size_t index, void* data);
            generic<typename T> where T : value class
                void UpdateIndices(System::Memory<T> data);
            generic<typename T> where T : value class
                void UpdateConstant(size_t index, System::Memory<T> data);
            generic<typename T> where T : value class
                void UpdateAttribute(size_t index, System::Memory<T> data);
            virtual ~Object3D();
            !Object3D();
        private:
            DX::D3DObject* m_value;
            System::Collections::Generic::IEnumerable<Attribute^>^ m_attributes;
            Index^ m_index;
            System::Collections::Generic::IEnumerable<Constant^>^ m_constants;
            Effect3D^ m_effect;
            DescriptorHeap^ m_textureHeap;
            DescriptorHeap^ m_samplerHeap;
        };
    }
}

