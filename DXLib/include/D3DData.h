#pragma once
#include <cstdint>
#include <memory>
#include <d3d12.h>
#include <functional>
#include <export.h>

namespace DX {
    class D3DScene;

    class dllexport D3DData
    {
    public:
        D3DData(D3DScene* scene, uint32_t elementSize, uint32_t size);
        virtual void Update(const void* data);
        uint32_t Size() const;
        uint32_t ElementSize() const;
        uint32_t ByteSize() const;
        virtual void Reset();
        virtual D3D12_GPU_VIRTUAL_ADDRESS GpuAddress() const;
        virtual ~D3DData();
    protected:
        void Alloc(size_t size, size_t alignment = 16U);
        virtual void Alloc() = 0;
    private:
        class Impl;
        std::unique_ptr<Impl> m_impl;
    };
}
