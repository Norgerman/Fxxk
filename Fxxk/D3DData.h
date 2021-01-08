#pragma once
#include <d3d11.h>
#include <cstdint>

class D3DScene;

class D3DData
{
public:
    D3DData();
    D3DData(const void* data, uint32_t elementSize, uint32_t size);
    virtual void updateData(const void* data);
    virtual void upload(D3DScene& scene, ID3D11Buffer* buffer);
    uint32_t getSize() const;
    uint32_t getElementSize() const;
    uint32_t getByteSize() const;
    const void* getData() const;
    virtual D3D11_BUFFER_DESC buildBufferDesc() = 0;
protected:
    virtual bool shouldUpload() const = 0;

    uint32_t m_elementSize;
    uint32_t m_size;
    const void* m_data;
    bool m_dirty;
};

