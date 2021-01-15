#pragma once
#include <string>
#include <d3d12.h>
#include <vector>
#include <D3DData.h>

namespace DX {
    class __declspec(dllexport) D3DAttribute :
        public D3DData
    {
    public:
        D3DAttribute() : D3DAttribute(nullptr, 0, 0, 0, std::initializer_list<D3D12_INPUT_ELEMENT_DESC>())
        {

        }
        template<typename T>
        D3DAttribute(
            const void* data,
            uint32_t elementSize,
            uint32_t size,
            uint32_t stride,
            T&& elements
        ) :
            D3DData(data, elementSize, size),
            m_elements(std::forward<T>(elements)),
            m_stride(stride),
            m_view()
        {
            m_view.SizeInBytes = ByteSize();
            m_view.StrideInBytes = m_stride;
        }
        void AppendElement(uint32_t solt, std::vector<D3D12_INPUT_ELEMENT_DESC>& output) const;
        const D3D12_VERTEX_BUFFER_VIEW& BufferView() const;
    protected:
        virtual void Alloc(D3DScene& scene);
    private:
        uint32_t m_stride;
        D3D12_VERTEX_BUFFER_VIEW m_view;
        std::vector<D3D12_INPUT_ELEMENT_DESC> m_elements;
    };
}
