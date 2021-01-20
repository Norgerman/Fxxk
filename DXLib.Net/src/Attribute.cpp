#include <vector>
#include <d3d12.h>
#include <Scene3D.h>
#include <Attribute.h>

namespace DX
{
    namespace Sharp
    {
        using namespace System;
        using namespace System::Collections::Generic;
        using namespace System::Buffers;
        using namespace DX::Sharp::Direct3D12;
        using namespace std;

        Attribute::Attribute(Scene3D^ scene, void* data, UInt32 elementSize, UInt32 size, UInt32 stride, IEnumerable<InputElement^>^ elements)
        {
            vector<D3D12_INPUT_ELEMENT_DESC> r;
            for each (auto e in elements)
            {
                r.push_back(static_cast<D3D12_INPUT_ELEMENT_DESC>(*e));
            }
            m_value = new D3DAttribute(scene->Value, elementSize, size, stride, move(r));
            m_value->Update(data);
        }

        void Attribute::Update(void* data)
        {
            m_value->Update(data);
        }

        Attribute::!Attribute()
        {
            delete m_value;
            m_value = nullptr;
        }

        Attribute::~Attribute()
        {
            this->!Attribute();
        }


        generic<typename T> where T : value class
            MemoryAttribute<T>::MemoryAttribute(Scene3D^ scene, Memory<T> data, UInt32 stride, IEnumerable<InputElement^>^ elements) :
            Attribute(scene, nullptr, static_cast<UInt32>(sizeof(T)), data.Length, stride, elements)
        {
            MemoryHandle^ handle = data.Pin();
            try
            {
                Attribute::Update(handle->Pointer);
            }
            finally
            {
                delete handle;
            }
        }

        generic<typename T> where T : value class
            void MemoryAttribute<T>::Update(Memory<T> data)
        {
            MemoryHandle^ handle = data.Pin();
            try
            {
                Attribute::Update(handle->Pointer);
            }
            finally
            {
                delete handle;
            }
        }
    }
}
