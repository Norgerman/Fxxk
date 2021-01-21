#include <vector>
#include <d3d12.h>
#include <Scene3D.h>
#include <Attribute.h>

namespace DX
{
    namespace Sharp
    {
        using namespace std;
        using namespace System;
        using namespace System::Buffers;

        Attribute::Attribute(Scene3D^ scene, void* data, UInt32 elementSize, UInt32 size, UInt32 stride)
        {
            m_value = new D3DAttribute(scene->Value, elementSize, size, stride);
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
            MemoryAttribute<T>::MemoryAttribute(Scene3D^ scene, Memory<T> data, UInt32 stride) :
            Attribute(scene, nullptr, static_cast<UInt32>(sizeof(T)), data.Length, stride)
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
