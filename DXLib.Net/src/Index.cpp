#include <D3DIndex.h>
#include <Index.h>
#include <Scene3D.h>

namespace DX
{
    namespace Sharp
    {
        using namespace System;
        using namespace System::Buffers;

        Index::Index(Scene3D^ scene, void* data, UInt32 elementSize, UInt32 size)
        {
            m_value = new DX::D3DIndex(scene->Value, elementSize, size);
            m_value->Update(data);
        }

        void Index::Update(void* data)
        {
            m_value->Update(data);
        }

        Index::!Index()
        {
            delete m_value;
            m_value = nullptr;
        }

        Index::~Index()
        {
            this->!Index();
        }

        generic<typename T> where T : value class
            MemoryIndex<T>::MemoryIndex(Scene3D^ scene, Memory<T> data) :
            Index(scene, nullptr, static_cast<UInt32>(sizeof(T)), data.Length)
        {
            MemoryHandle^ handle = data.Pin();
            try
            {
             Index::Update(handle->Pointer);
            }
            finally
            {
                delete handle;
            }
        }

        generic<typename T> where T : value class
            void MemoryIndex<T>::Update(Memory<T> data)
        {
            MemoryHandle^ handle = data.Pin();
            try
            {
                Index::Update(handle->Pointer);
            }
            finally
            {
                delete handle;
            }
        }
    }
}