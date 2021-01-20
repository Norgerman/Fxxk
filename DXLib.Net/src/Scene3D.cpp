#include <Scene3D.h>
#include <Object3D.h>

namespace DX
{
    namespace Sharp
    {
        using namespace std;
        using namespace System;
        using namespace System::Collections::Generic;
        using namespace System::Runtime::InteropServices;
        using namespace DX::Sharp::Direct3D;
        using namespace DX::Sharp::SimpleMath;

        typedef void(__stdcall* native_proj_callback)(D3DScene&);
        typedef void(__stdcall* native_upd_callback)(D3DScene&, double);

        void BaseProject(Scene3D^ scene)
        {
            auto viewport = scene->Viewport;
            auto matrix = gcnew XMMatrix();
            matrix->Set(
                1 / viewport.Width * 2, 0.0f, 0.0f, 0.0f,
                0.0f, 1 / viewport.Height * 2, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                -1.0f, -1.0f, 0.0f, 1.0f);
            scene->UpdateProjection(matrix);
            delete matrix;
        }

        Scene3D::Scene3D() : Scene3D(Color::White, FeatureLevel::Level_11_0)
        {

        }

        Scene3D::Scene3D(Color bg, FeatureLevel featureLevel)
        {
            m_reproject = gcnew SceneProjectionCallback(BaseProject);
            m_nativeProj = gcnew Scene3D::NativeProjCallback(this, &Scene3D::Project);
            m_nativeUpdate = gcnew Scene3D::NativeUpdateCallback(this, &Scene3D::Update);
            native_proj_callback projPtr = static_cast<native_proj_callback>(Marshal::GetFunctionPointerForDelegate(m_nativeProj).ToPointer());
            native_upd_callback updPtr = static_cast<native_upd_callback>(Marshal::GetFunctionPointerForDelegate(m_nativeUpdate).ToPointer());
            m_value = new D3DScene({ bg.R, bg.G, bg.B, bg.A }, projPtr, static_cast<D3D_FEATURE_LEVEL>(featureLevel));
            m_value->OnUpdate(updPtr);
        }

        void Scene3D::Initialize(IntPtr window, float x, float y, float width, float height, bool fixedTimerStep, float targetSeconds)
        {
            m_value->Initialize(static_cast<HWND>(window.ToPointer()), x, y, width, height, fixedTimerStep, targetSeconds);
        }

        void Scene3D::Initialize(IntPtr window, float x, float y, float width, float height)
        {
            Initialize(window, x, y, width, height, false, 1.0f / 60);
        }

        void Scene3D::Tick()
        {
            m_value->Tick();
        }

        void Scene3D::OnActivated()
        {
            m_value->OnActivated();
        }

        void Scene3D::OnDeactivated()
        {
            m_value->OnDeactivated();
        }

        void Scene3D::OnSuspending()
        {
            m_value->OnSuspending();
        }

        void Scene3D::OnResuming()
        {
            m_value->OnResuming();
        }

        void Scene3D::OnWindowSizeChanged(float x, float y, float width, float height)
        {
            m_value->OnWindowSizeChanged(x, y, width, height);
        }

        void Scene3D::SetRenderList(IEnumerable<Object3D^>^ objects)
        {
            vector<D3DObject*> objPtr;
            for each (auto obj in objects)
            {
                objPtr.push_back(obj->Value);
            }
            m_value->SetRenderList(move(objPtr));
        }

        void Scene3D::UpdateProjection(XMMatrix^ projection)
        {
            m_value->UpdateProjection(*projection->Value);
        }

        void Scene3D::Project(DX::D3DScene& scene)
        {
            m_reproject(this);
        }

        void Scene3D::Update(DX::D3DScene& scene, double second)
        {
            OnUpdate(this, second);
        }

        Scene3D::~Scene3D()
        {
            this->!Scene3D();
        }

        Scene3D::!Scene3D()
        {
            delete m_value;
            m_value = nullptr;
        }
    }
}
