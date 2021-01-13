#include <dxgi.h>
#include "D3DIndex.h"
#include "D3DScene.h"

D3DIndex::D3DIndex() : D3DIndex(nullptr, 0, 0)
{

}

D3DIndex::D3DIndex(const void* data, uint32_t elementSize, uint32_t size) :
    D3DData(data, elementSize, size),
    m_view()
{
    m_view.SizeInBytes = ByteSize();
    m_view.Format = m_elementSize == 4 ? DXGI_FORMAT::DXGI_FORMAT_R32_UINT : DXGI_FORMAT::DXGI_FORMAT_R16_UINT;
}

void D3DIndex::Alloc(D3DScene& scene)
{
    m_buffer = DirectX::GraphicsMemory::Get(scene.Device().Get()).Allocate(ByteSize());
    m_view.BufferLocation = m_buffer.GpuAddress();
}

const D3D12_INDEX_BUFFER_VIEW& D3DIndex::BufferView() const
{
    return m_view;
}
