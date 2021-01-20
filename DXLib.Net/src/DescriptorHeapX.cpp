#include <DescriptorHeapX.h>

namespace DX
{
    namespace Sharp
    {
        using namespace DX::Sharp::Direct3D12;
        using namespace System;

        DescriptorHeap::DescriptorHeap(Device device, size_t count)
        {
            m_value = new D3DDescriptorHeap(device.Pointer, count);
        }

        DescriptorHeap::DescriptorHeap(Device device, DescriptorHeapType type, DescriptorHeapFlags flags, size_t count)
        {
            m_value = new D3DDescriptorHeap(device.Pointer, static_cast<D3D12_DESCRIPTOR_HEAP_TYPE>(type), static_cast<D3D12_DESCRIPTOR_HEAP_FLAGS>(flags), count);
        }

        GpuDescriptorHandle DescriptorHeap::GetFirstGpuHandle()
        {
            return GpuDescriptorHandle::FromNative(m_value->GetFirstGpuHandle());
        }
        
        CpuDescriptorHandle DescriptorHeap::GetFirstCpuHandle()
        {
            return CpuDescriptorHandle::FromNative(m_value->GetFirstCpuHandle());
        }
        
        GpuDescriptorHandle DescriptorHeap::GetGpuHandle(size_t index)
        {
            return GpuDescriptorHandle::FromNative(m_value->GetGpuHandle(index));
        }
        
        CpuDescriptorHandle DescriptorHeap::GetCpuHandle(size_t index)
        {
            return CpuDescriptorHandle::FromNative(m_value->GetCpuHandle(index));
        }

        size_t DescriptorHeap::Count()
        {
            return m_value->Count();
        }

        DescriptorHeap::~DescriptorHeap()
        {
            this->!DescriptorHeap();
        }
        
        DescriptorHeap::!DescriptorHeap()
        {
            delete m_value;
            m_value = nullptr;
        }
    }
}
