#include <D3DAttribute.h>
#include <D3DScene.h>

namespace DX {
    void D3DAttribute::AppendElement(uint32_t solt, std::vector<D3D12_INPUT_ELEMENT_DESC>& output) const
    {
        for (auto& element : m_elements)
        {
            D3D12_INPUT_ELEMENT_DESC desc = element;
            desc.InputSlot = solt;
            output.push_back(desc);
        }
    }

    void D3DAttribute::Alloc(D3DScene& scene)
    {
        m_buffer = DirectX::GraphicsMemory::Get(scene.Device().Get()).Allocate(ByteSize());
        m_view.BufferLocation = m_buffer.GpuAddress();
    }

    const D3D12_VERTEX_BUFFER_VIEW& D3DAttribute::BufferView() const
    {
        return m_view;
    }
}
