#include <DescriptorHeap.h>
#include <D3DDescriptorHeap.h>

namespace DX
{
    class D3DDescriptorHeap::Impl
    {
    public:
        Impl(ID3D12Device* device, size_t count) : 
            m_heap(std::make_unique<DirectX::DescriptorHeap>(device, count))
        {

        }
        
        Impl(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flags, size_t count) :
            m_heap(std::make_unique<DirectX::DescriptorHeap>(device, type, flags, count))
        {
        }

        D3D12_GPU_DESCRIPTOR_HANDLE GetFirstGpuHandle() const
        {
            return m_heap->GetFirstGpuHandle();
        }

        D3D12_CPU_DESCRIPTOR_HANDLE GetFirstCpuHandle() const
        {
            return m_heap->GetFirstCpuHandle();
        }

        D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle(size_t index) const
        {
            return m_heap->GetGpuHandle(index);
        }

        D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle(size_t index) const
        {
            return m_heap->GetCpuHandle(index);
        }

        size_t Count() const
        {
            return m_heap->Count();
        }

        ID3D12DescriptorHeap* Heap() const
        {
            return m_heap->Heap();
        }
    private:
        std::unique_ptr<DirectX::DescriptorHeap> m_heap;
    };

    D3DDescriptorHeap::D3DDescriptorHeap(ID3D12Device* device, size_t count) : 
        m_impl(std::make_unique<Impl>(device, count))
    {

    }
    
    D3DDescriptorHeap::D3DDescriptorHeap(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flags, size_t count) : 
        m_impl(std::make_unique<Impl>(device, type, flags, count))
    {

    }
    
    D3D12_GPU_DESCRIPTOR_HANDLE D3DDescriptorHeap::GetFirstGpuHandle() const
    {
        return m_impl->GetFirstGpuHandle();
    }
    
    D3D12_CPU_DESCRIPTOR_HANDLE D3DDescriptorHeap::GetFirstCpuHandle() const
    {
        return m_impl->GetFirstCpuHandle();
    }
    
    D3D12_GPU_DESCRIPTOR_HANDLE D3DDescriptorHeap::GetGpuHandle(size_t index) const
    {
        return m_impl->GetGpuHandle(index);
    }
    
    D3D12_CPU_DESCRIPTOR_HANDLE D3DDescriptorHeap::GetCpuHandle(size_t index) const
    {
        return m_impl->GetCpuHandle(index);
    }
    
    size_t D3DDescriptorHeap::Count() const
    {
        return m_impl->Count();
    }
    
    ID3D12DescriptorHeap* D3DDescriptorHeap::Heap() const
    {
        return m_impl->Heap();
    }
    
    D3DDescriptorHeap::~D3DDescriptorHeap()
    {

    }
}


