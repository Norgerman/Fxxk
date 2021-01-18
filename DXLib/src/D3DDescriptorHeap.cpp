#include <stdexcept>
#include <wrl/client.h>
#include <wil/result.h>
#include <cassert>
#include <D3DDescriptorHeap.h>

namespace DX
{
    class D3DDescriptorHeap::Impl
    {
    public:
        Impl(ID3D12Device* device, size_t count) : 
            Impl(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, count)
        {

        }
        
        Impl(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flags, size_t count) :
            m_desc{},
            m_hCPU{},
            m_hGPU{},
            m_increment(0)
        {
            if (count > UINT32_MAX)
                throw std::invalid_argument("Too many descriptors");

            D3D12_DESCRIPTOR_HEAP_DESC desc = {};
            desc.Flags = flags;
            desc.NumDescriptors = static_cast<UINT>(count);
            desc.Type = type;
            Create(device, &desc);
        }

        D3D12_GPU_DESCRIPTOR_HANDLE GetFirstGpuHandle() const
        {
            assert(m_desc.Flags & D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
            assert(m_pHeap != nullptr);
            return m_hGPU;
        }

        D3D12_CPU_DESCRIPTOR_HANDLE GetFirstCpuHandle() const
        {
            assert(m_pHeap != nullptr);
            return m_hCPU;
        }

        D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle(size_t index) const
        {
            assert(m_pHeap != nullptr);
            if (index >= m_desc.NumDescriptors)
            {
                throw std::out_of_range("D3DX12_GPU_DESCRIPTOR_HANDLE");
            }
            assert(m_desc.Flags & D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);

            D3D12_GPU_DESCRIPTOR_HANDLE handle;
            handle.ptr = m_hGPU.ptr + UINT64(index) * UINT64(m_increment);
            return handle;
        }

        D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle(size_t index) const
        {
            assert(m_pHeap != nullptr);
            if (index >= m_desc.NumDescriptors)
            {
                throw std::out_of_range("D3DX12_CPU_DESCRIPTOR_HANDLE");
            }

            D3D12_CPU_DESCRIPTOR_HANDLE handle;
            handle.ptr = static_cast<SIZE_T>(m_hCPU.ptr + UINT64(index) * UINT64(m_increment));
            return handle;
        }

        size_t Count() const
        {
            return m_desc.NumDescriptors;
        }

        ID3D12DescriptorHeap* Heap() const
        {
            return m_pHeap.Get();
        }
    private:
        void Create(_In_ ID3D12Device* pDevice, _In_ const D3D12_DESCRIPTOR_HEAP_DESC* pDesc)
        {
            assert(pDesc != nullptr);

            m_desc = *pDesc;
            m_increment = pDevice->GetDescriptorHandleIncrementSize(pDesc->Type);

            if (pDesc->NumDescriptors == 0)
            {
                m_pHeap.Reset();
                m_hCPU.ptr = 0;
                m_hGPU.ptr = 0;
            }
            else
            {
                THROW_IF_FAILED(pDevice->CreateDescriptorHeap(
                    pDesc,
                    IID_PPV_ARGS(m_pHeap.ReleaseAndGetAddressOf())));

                m_hCPU = m_pHeap->GetCPUDescriptorHandleForHeapStart();

                if (pDesc->Flags & D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE)
                    m_hGPU = m_pHeap->GetGPUDescriptorHandleForHeapStart();

            }
        }

        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>    m_pHeap;
        D3D12_DESCRIPTOR_HEAP_DESC                      m_desc;
        D3D12_CPU_DESCRIPTOR_HANDLE                     m_hCPU;
        D3D12_GPU_DESCRIPTOR_HANDLE                     m_hGPU;
        uint32_t                                        m_increment;
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


