#pragma once
#include <d3d11.h>
#include "D3DData.h"

class D3DConstant :
    public D3DData
{
public:
    D3DConstant();
    D3DConstant(const void* data, uint32_t elementSize, uint32_t size);
    virtual D3D11_BUFFER_DESC buildBufferDesc();
protected:
    virtual bool shouldUpload() const;
};

