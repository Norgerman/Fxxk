#include <cstring>
#include <D3DData.h>
#include <D3DScene.h>
#include <GraphicsMemory.h>

namespace DX {
    class D3DData::Impl
    {
    public:
        Impl(D3DData* owner, D3DScene* scene, uint32_t elementSize, uint32_t size) :
            m_owner(owner),
            m_scene(scene),
            m_elementSize(elementSize),
            m_size(size)
        {
        }


        void Update(const void* data)
        {
            if (m_scene && m_buffer.Memory() == nullptr)
            {
                m_owner->Alloc();
            }
            if (data)
            {
                memcpy(m_buffer.Memory(), data, ByteSize());
            }
        }


        void Alloc(size_t size, size_t alignment)
        {
            m_buffer = DirectX::GraphicsMemory::Get(m_scene->Device()).Allocate(size, alignment);
        }

        uint32_t Size() const
        {
            return m_size;
        }

        uint32_t ElementSize() const
        {
            return m_elementSize;
        }

        uint32_t ByteSize() const
        {
            return m_size * m_elementSize;
        }

        D3D12_GPU_VIRTUAL_ADDRESS GpuAddress() const
        {
            return m_buffer.GpuAddress();
        }

        void Reset()
        {
            m_buffer.Reset();
        }
    private:
        uint32_t m_elementSize;
        uint32_t m_size;
        DirectX::GraphicsResource m_buffer;
        D3DData* m_owner;
        D3DScene* m_scene;
    };
    
    D3DData::D3DData(D3DScene* scene, uint32_t elementSize, uint32_t size) :
        m_impl(std::make_unique<Impl>(this, scene, elementSize, size))
    {
    }

    void D3DData::Update(const void* data)
    {
        m_impl->Update(data);
    }
    
    void D3DData::Alloc(size_t size, size_t alignment)
    {
        m_impl->Alloc(size, alignment);
    }

    uint32_t D3DData::Size() const
    {
        return m_impl->Size();
    }
    
    uint32_t D3DData::ElementSize() const
    {
        return m_impl->ElementSize();
    }

    uint32_t D3DData::ByteSize() const
    {
        return m_impl->ByteSize();
    }
    
    void D3DData::Reset()
    {
        m_impl->Reset();
    }

    D3D12_GPU_VIRTUAL_ADDRESS D3DData::GpuAddress() const
    {
        return m_impl->GpuAddress();
    }

    D3DData::~D3DData()
    {
    }
}
