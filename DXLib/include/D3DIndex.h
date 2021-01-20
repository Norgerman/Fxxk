#pragma once
#include <d3d12.h>
#include <D3DData.h>
#include <export.h>

namespace DX {
    class dllexport D3DIndex : public D3DData
    {
    public:
        D3DIndex(D3DScene* scene, uint32_t elementSize, uint32_t size);
        const D3D12_INDEX_BUFFER_VIEW& BufferView() const;
    protected:
        virtual void Alloc();
    private:
        D3D12_INDEX_BUFFER_VIEW m_view;
    };
}
