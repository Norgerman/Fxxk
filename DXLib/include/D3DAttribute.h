#pragma once
#include <string>
#include <d3d12.h>
#include <vector>
#include <D3DData.h>
#include <export.h>

namespace DX {
    class dllexport D3DAttribute :
        public D3DData
    {
    public:
        D3DAttribute(D3DScene* scene, uint32_t elementSize, uint32_t size, uint32_t stride);
        const D3D12_VERTEX_BUFFER_VIEW& BufferView() const;
    protected:
        virtual void Alloc();
    private:
        uint32_t m_stride;
        D3D12_VERTEX_BUFFER_VIEW m_view;
    };
}
