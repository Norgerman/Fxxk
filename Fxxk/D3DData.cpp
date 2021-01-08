#include "D3DData.h"
#include "D3DScene.hpp"

D3DData::D3DData() :
    D3DData(nullptr, 0, 0)
{

}

D3DData::D3DData(const void* data, uint32_t elementSize, uint32_t size) :
    m_data(data),
    m_elementSize(elementSize),
    m_size(size),
    m_dirty(false)
{

}
void D3DData::updateData(const void* data)
{
    m_data = data;
    m_dirty = true;
}
void D3DData::upload(D3DScene& scene, ID3D11Buffer* buffer)
{
    if (buffer && shouldUpload())
    {
        D3D11_MAPPED_SUBRESOURCE mappedResource;
        ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
        auto hr = scene.getContext()->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
        assert(SUCCEEDED(hr));
        memcpy(mappedResource.pData, m_data, getByteSize());
        scene.getContext()->Unmap(buffer, 0);
        m_dirty = false;
    }
}
uint32_t D3DData::getSize() const
{
    return m_size;
}
uint32_t D3DData::getElementSize() const
{
    return m_elementSize;
}
uint32_t D3DData::getByteSize() const
{
    return m_size * m_elementSize;
}
const void* D3DData::getData() const {
    return m_data;
}

