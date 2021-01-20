#include <D3DConstant.h>
#include <Constant.h>
#include <Scene3D.h>

namespace DX
{
    namespace Sharp
    {
        using namespace System;
        using namespace System::Buffers;

        Constant::Constant(Scene3D^ scene, void* data, UInt32 elementSize, UInt32 size)
        {
            m_value = new DX::D3DConstant(scene->Value, elementSize, size);
            m_value->Update(data);
        }

        void Constant::Update(void* data)
        {
            m_value->Update(data);
        }

        Constant::!Constant()
        {
            delete m_value;
            m_value = nullptr;
        }

        Constant::~Constant()
        {
            this->!Constant();
        }

        generic<typename T> where T : value class
            MemoryConstant<T>::MemoryConstant(Scene3D^ scene, Memory<T> data) :
            Constant(scene, nullptr, static_cast<UInt32>(sizeof(T)), data.Length)
        {
            MemoryHandle^ handle = data.Pin();
            try
            {
                Constant::Update(handle->Pointer);
            }
            finally
            {
                delete handle;
            }
        }

        generic<typename T> where T : value class
            void MemoryConstant<T>::Update(Memory<T> data)
        {
            MemoryHandle^ handle = data.Pin();
            try
            {
                Constant::Update(handle->Pointer);
            }
            finally
            {
                delete handle;
            }
        }

    }
}