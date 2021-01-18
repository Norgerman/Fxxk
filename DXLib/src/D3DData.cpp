#include <cstring>
#include <D3DData.h>
#include <D3DScene.h>
#include <GraphicsMemory.h>

namespace DX {
    class D3DData::Impl
    {
    public:
        Impl(Impl const&) = delete;
        Impl& operator= (Impl const&) = delete;

        Impl(D3DData* owner) :
            Impl(owner, nullptr, 0, 0)
        {

        }

        Impl(D3DData* owner, const void* data, uint32_t elementSize, uint32_t size) :
            m_owner(owner),
            m_data(data),
            m_elementSize(elementSize),
            m_size(size),
            m_dirty(true)
        {

        }

        Impl(D3DData* owner, const Impl& other) :
            Impl(owner, other.m_data, other.m_elementSize, other.m_size)
        {

        }

        void Update(const void* data)
        {
            m_data = data;
            m_dirty = true;
        }

        void Upload(D3DScene& scene)
        {
            if (m_buffer.Memory() == nullptr)
            {
                m_owner->Alloc(scene);
            }
            memcpy(m_buffer.Memory(), m_data, ByteSize());
            m_dirty = false;
        }

        void Alloc(D3DScene& scene, size_t size, size_t alignment)
        {
            m_buffer = DirectX::GraphicsMemory::Get(scene.Device()).Allocate(size, alignment);
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

        const void* Data() const {
            return m_data;
        }

        bool Dirty() const
        {
            return m_dirty;
        }

        D3D12_GPU_VIRTUAL_ADDRESS GpuAddress() const
        {
            return m_buffer.GpuAddress();
        }

        void Reset()
        {
            m_buffer.Reset();
            m_dirty = true;
        }
    private:
        uint32_t m_elementSize;
        uint32_t m_size;
        DirectX::GraphicsResource m_buffer;
        const void* m_data;
        D3DData* m_owner;
        bool m_dirty;
    };

    D3DData::D3DData() :
        m_impl(std::make_unique<Impl>(this))
    {

    }
    
    D3DData::D3DData(const D3DData& other)
        : m_impl(std::make_unique<Impl>(this, *m_impl))
    {
    }

    D3DData::D3DData(const void* data, uint32_t elementSize, uint32_t size) :
        m_impl(std::make_unique<Impl>(this, data, elementSize, size))
    {

    }

    void D3DData::Update(const void* data)
    {
        m_impl->Update(data);
    }
    
    void D3DData::Upload(D3DScene& scene)
    {
        if (ShouldUpload())
        {
            m_impl->Upload(scene);
        }
    }

    void D3DData::Alloc(D3DScene& scene, size_t size, size_t alignment)
    {
        m_impl->Alloc(scene, size, alignment);
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
    const void* D3DData::Data() const {
        return m_impl->Data();
    }

    bool D3DData::ShouldUpload() const
    {
        return m_impl->Dirty();
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
