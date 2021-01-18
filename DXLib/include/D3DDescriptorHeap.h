#pragma once
// c++/cli compatible implment of DirectX::DescriptorHeap
#include <memory>
#include <d3d12.h>
#include <export.h>

namespace DX
{
    class dllexport D3DDescriptorHeap
    {
    public:
        D3DDescriptorHeap(ID3D12Device* device, size_t count);
        D3DDescriptorHeap(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flags, size_t count);
        D3D12_GPU_DESCRIPTOR_HANDLE GetFirstGpuHandle() const;
        D3D12_CPU_DESCRIPTOR_HANDLE GetFirstCpuHandle() const;
        D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle(size_t index) const;
        D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle(size_t index) const;
        size_t Count() const;
        ID3D12DescriptorHeap* Heap() const;
        ~D3DDescriptorHeap();
    private:
        class Impl;
        std::unique_ptr<Impl> m_impl;
    };
}
