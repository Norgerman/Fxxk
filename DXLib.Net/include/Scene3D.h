#pragma once
#include <D3DScene.h>
#include <XMMatrix.h>
#include <Structs.hpp>

namespace DX
{
    namespace Sharp
    {
        ref class Scene3D;        
        ref class Object3D;

        public delegate void SceneProjectionCallback(Scene3D^ scene);
        public delegate void SceneUpdateCallback(Scene3D^ scene, double elapsedSeconds, System::UInt32 frame);

        public ref class Scene3D
        {
        public:
            event SceneUpdateCallback^ OnUpdate;
            event SceneProjectionCallback^ OnReProject
            {
                void add(SceneProjectionCallback^ a)
                {
                    m_reproject = a;
                }

                void remove(SceneProjectionCallback^ b)
                {
                    m_reproject -= b;
                }
            }

            property DX::D3DScene* Value
            {
                DX::D3DScene* get()
                {
                    return m_value;
                }
            }

            property DX::Sharp::Direct3D12::Viewport Viewport
            {
                DX::Sharp::Direct3D12::Viewport get()
                {
                    return DX::Sharp::Direct3D12::Viewport::FromNative(m_value->Viewport());
                }
            }

            property DX::Sharp::Direct3D12::Device Device
            {
                DX::Sharp::Direct3D12::Device get()
                {
                    return DX::Sharp::Direct3D12::Device(m_value->Device());
                }
            }

            property DX::Sharp::Direct3D12::CommandQueue CommandQueue
            {
                DX::Sharp::Direct3D12::CommandQueue get()
                {
                    return DX::Sharp::Direct3D12::CommandQueue(m_value->CommandQueue());
                }
            }

            property DX::Sharp::Direct3D12::CommandList CommandList
            {
                DX::Sharp::Direct3D12::CommandList get()
                {
                    return DX::Sharp::Direct3D12::CommandList(m_value->CommandList());
                }
            }

            property System::UInt32 Fps
            {
                System::UInt32 get()
                {
                    return m_value->FramesPerSecond();
                }
            }

            Scene3D();
            Scene3D(Color bg, DX::Sharp::Direct3D::FeatureLevel featureLevel);

            void Initialize(System::IntPtr window, float x, float y, float width, float height);

            void SetTargetUpdateTimeout(bool fixedTimeStep, double targetSeconds);
            void SetInactiveTargetUpdateTimeout(bool fixedTimeStep, double targetSeconds);

            void Tick();
            void OnActivated();
            void OnDeactivated();
            void OnSuspending();
            void OnResuming();
            void OnWindowSizeChanged(float x, float y, float width, float height);

            void SetRenderList(System::Collections::Generic::IEnumerable<Object3D^>^ objects);
            void UpdateProjection(DX::Sharp::SimpleMath::XMMatrix^ projection);
            void EnableDebug();

            ~Scene3D();
            !Scene3D();
        private:
            delegate void NativeProjCallback(DX::D3DScene&);
            delegate void NativeUpdateCallback(DX::D3DScene&, double, uint32_t);

            void Project(DX::D3DScene& scene);
            void Update(DX::D3DScene& scene, double second, System::UInt32 frame);

            DX::D3DScene* m_value;
            SceneProjectionCallback^ m_reproject;
            NativeProjCallback^ m_nativeProj;
            NativeUpdateCallback^ m_nativeUpdate;
        };
    }
}

