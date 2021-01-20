#include <dxgi.h>
#include <D3DIndex.h>
#include <D3DScene.h>

namespace DX {
    D3DIndex::D3DIndex(D3DScene* scene, uint32_t elementSize, uint32_t size) :
        D3DData(scene, elementSize, size),
        m_view()
    {
        m_view.SizeInBytes = ByteSize();
        m_view.Format = ElementSize() == 4 ? DXGI_FORMAT::DXGI_FORMAT_R32_UINT : DXGI_FORMAT::DXGI_FORMAT_R16_UINT;
    }

    void D3DIndex::Alloc()
    {
        D3DData::Alloc(ByteSize());
        m_view.BufferLocation = GpuAddress();
    }

    const D3D12_INDEX_BUFFER_VIEW& D3DIndex::BufferView() const
    {
        return m_view;
    }
}
