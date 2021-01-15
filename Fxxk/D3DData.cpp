#include <cstring>
#include "D3DData.h"
#include "D3DScene.h"

D3DData::D3DData() :
    D3DData(nullptr, 0, 0)
{

}
D3DData::D3DData(const D3DData& other)
{
    m_data = other.m_data;
    m_elementSize = other.m_elementSize;
    m_size = other.m_size;
    m_dirty = true;
}

D3DData::D3DData(const void* data, uint32_t elementSize, uint32_t size) :
    m_data(data),
    m_elementSize(elementSize),
    m_size(size),
    m_dirty(true)
{

}
void D3DData::Update(const void* data)
{
    m_data = data;
    m_dirty = true;
}
void D3DData::Upload(D3DScene& scene)
{
    if (ShouldUpload())
    {
        if (m_buffer.Memory() == nullptr)
        {
            Alloc(scene);
        }
        memcpy(m_buffer.Memory(), m_data, ByteSize());
        m_dirty = false;
    }
}
uint32_t D3DData::Size() const
{
    return m_size;
}
uint32_t D3DData::ElementSize() const
{
    return m_elementSize;
}
uint32_t D3DData::ByteSize() const
{
    return m_size * m_elementSize;
}
const void* D3DData::Data() const {
    return m_data;
}

bool D3DData::ShouldUpload() const
{
    return m_dirty;
}

void D3DData::Reset() 
{
    m_buffer.Reset();
    m_dirty = true;
}

D3D12_GPU_VIRTUAL_ADDRESS D3DData::GpuAddress() const
{
    return m_buffer.GpuAddress();
}


