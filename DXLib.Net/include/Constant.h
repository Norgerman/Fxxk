#pragma once
#include <D3DConstant.h>

namespace DX
{
    namespace Sharp
    {
        ref class Scene3D;

        public ref class Constant
        {
        public:
            property DX::D3DConstant* Value
            {
                DX::D3DConstant* get()
                {
                    return m_value;
                }
            }

            Constant(Scene3D^ scene, void *data, System::UInt32 elementSize, System::UInt32 size);

            void Update(void *data);
            
            !Constant();
            virtual ~Constant();
        private:
            DX::D3DConstant* m_value;
           
        };

        generic<typename T>
            where T : value class
                public ref class MemoryConstant : public Constant
            {
            public:
                MemoryConstant(Scene3D^ scene, System::Memory<T> data);
                void Update(System::Memory<T> data);
            };
    }
}