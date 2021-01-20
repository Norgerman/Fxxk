#pragma once
#include <D3DIndex.h>

namespace DX
{
    namespace Sharp
    {
        ref class Scene3D;

        public ref class Index
        {
        public:
            property DX::D3DIndex* Value
            {
                DX::D3DIndex* get()
                {
                    return m_value;
                }
            }
            
            Index(Scene3D^ scene, void* data, System::UInt32 elementSize, System::UInt32 size);
            
            void Update(void* data);
            
            !Index();
            virtual ~Index();
        private:
            DX::D3DIndex* m_value;
            
        };

        generic<typename T>
            where T : value class
                public ref class MemoryIndex : public Index
            {
            public:
                MemoryIndex(Scene3D^ scene, System::Memory<T> data);
                void Update(System::Memory<T> data);
            };
    }
}