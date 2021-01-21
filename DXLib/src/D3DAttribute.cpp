#include <D3DAttribute.h>
#include <D3DScene.h>

namespace DX {

    D3DAttribute::D3DAttribute(
        D3DScene* scene,
        uint32_t elementSize,
        uint32_t size,
        uint32_t stride
    ) :
        D3DData(scene, elementSize, size),
        m_stride(stride),
        m_view()
    {
        m_view.SizeInBytes = ByteSize();
        m_view.StrideInBytes = m_stride;
    }


    void D3DAttribute::Alloc()
    {
        D3DData::Alloc(ByteSize());
        m_view.BufferLocation = GpuAddress();
    }

    const D3D12_VERTEX_BUFFER_VIEW& D3DAttribute::BufferView() const
    {
        return m_view;
    }
}
