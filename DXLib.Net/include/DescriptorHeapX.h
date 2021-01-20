#pragma once
#include <D3DDescriptorHeap.h>
#include <Enums.hpp>
#include <Structs.hpp>

namespace DX
{
    namespace Sharp
    {
        public ref class DescriptorHeap
        {
        public:
            property D3DDescriptorHeap* Value
            {
                D3DDescriptorHeap*  get()
                {
                    return m_value;
                }
            }

            DescriptorHeap(DX::Sharp::Direct3D12::Device device, size_t count);
            DescriptorHeap(DX::Sharp::Direct3D12::Device device, DX::Sharp::Direct3D12::DescriptorHeapType type, DX::Sharp::Direct3D12::DescriptorHeapFlags flags, size_t count);

            DX::Sharp::Direct3D12::GpuDescriptorHandle GetFirstGpuHandle();
            DX::Sharp::Direct3D12::CpuDescriptorHandle GetFirstCpuHandle();
            DX::Sharp::Direct3D12::GpuDescriptorHandle GetGpuHandle(size_t index);
            DX::Sharp::Direct3D12::CpuDescriptorHandle GetCpuHandle(size_t index);

            size_t Count();

            ~DescriptorHeap();
            !DescriptorHeap();

        private:
            D3DDescriptorHeap* m_value;
        };
    }
}

