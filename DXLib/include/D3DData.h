#pragma once
#include <cstdint>
#include <memory>
#include <GraphicsMemory.h>

namespace DX {
    class D3DScene;

    class __declspec(dllexport) D3DData
    {
    public:
        D3DData();
        D3DData(const D3DData& other);
        D3DData(const void* data, uint32_t elementSize, uint32_t size);
        virtual void Update(const void* data);
        virtual void Upload(D3DScene& scene);
        uint32_t Size() const;
        uint32_t ElementSize() const;
        uint32_t ByteSize() const;
        const void* Data() const;
        virtual void Reset();
        virtual D3D12_GPU_VIRTUAL_ADDRESS GpuAddress() const;
    protected:
        virtual bool ShouldUpload() const;
        virtual void Alloc(D3DScene& scene) = 0;

        uint32_t m_elementSize;
        uint32_t m_size;
        DirectX::GraphicsResource m_buffer;
        const void* m_data;
        bool m_dirty;
    };
}
