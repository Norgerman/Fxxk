#pragma once
#include <D3DAttribute.h>
#include <Structs.hpp>
#include <Enums.hpp>

namespace DX
{
    namespace Sharp
    {
        ref class Scene3D;

        public value struct InputElement
        {
            System::String^ SemanticName;
            System::UInt32 SemanticIndex;
            DX::Sharp::DXGI::Format Format;
            System::UInt32 AlignedByteOffset;
            DX::Sharp::Direct3D12::InputClassification InputSlotClass;
            System::UInt32 InstanceDataStepRate;

            DX::InputElement ToNative();

            static System::UInt32 AppendAlignedElement = D3D12_APPEND_ALIGNED_ELEMENT;
        };

        public ref class Attribute
        {
        public:
            property DX::D3DAttribute* Value
            {
                DX::D3DAttribute* get()
                {
                    return m_value;
                }
            }
            
            Attribute(Scene3D^ scene, void* data, System::UInt32 elementSize, System::UInt32 size, System::UInt32 stride, System::Collections::Generic::IEnumerable<DX::Sharp::InputElement>^ elements);
            
            void Update(void* data);
            
            !Attribute();
            virtual ~Attribute();
        private:
            DX::D3DAttribute* m_value;
        };


        generic<typename T> where T : value class
            public ref class MemoryAttribute : public Attribute
        {
        public:
            MemoryAttribute(Scene3D^ scene, System::Memory<T> data, System::UInt32 stride, System::Collections::Generic::IEnumerable<DX::Sharp::InputElement>^ elements);
            void Update(System::Memory<T> data);
        };
    }
}

