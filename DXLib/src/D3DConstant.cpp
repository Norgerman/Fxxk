#include <GraphicsMemory.h>
#include <D3DConstant.h>
#include <D3DScene.h>

namespace DX {
    D3DConstant::D3DConstant() : D3DData()
    {

    }

    D3DConstant::D3DConstant(const void* data, uint32_t elementSize, uint32_t size) : D3DData(data, elementSize, size)
    {

    }

    void D3DConstant::Alloc(D3DScene& scene)
    {
        const size_t alignment = D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT;
        const size_t alignedSize = (ByteSize() + alignment - 1) & ~(alignment - 1);
        m_buffer = DirectX::GraphicsMemory::Get(scene.Device().Get()).Allocate(alignedSize, alignment);
    }
}
