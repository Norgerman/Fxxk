#pragma once
#include <D3DData.h>

namespace DX {
    class __declspec(dllexport) D3DConstant : public D3DData
    {
    public:
        D3DConstant();
        D3DConstant(const void* data, uint32_t elementSize, uint32_t size);
    protected:
        virtual void Alloc(D3DScene& scene);
    };
}