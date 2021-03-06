#include <GraphicsMemory.h>
#include <D3DConstant.h>
#include <D3DScene.h>

namespace DX {
    D3DConstant::D3DConstant(D3DScene* scene, uint32_t elementSize, uint32_t size) : D3DData(scene, elementSize, size)
    {

    }

    void D3DConstant::Alloc()
    {
        const size_t alignment = D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT;
        const size_t alignedSize = (ByteSize() + alignment - 1) & ~(alignment - 1);
        D3DData::Alloc(alignedSize, alignment);
    }
}
