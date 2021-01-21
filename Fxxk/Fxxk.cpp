// Fxxk.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "Fxxk.h"
#include <fstream>
#include <D3DDescriptorHeap.h>
#include <D3DScene.h>
#include <D3DObject.h>
#include <D3DConstant.h>
#include <D3DAttribute.h>
#include <D3DIndex.h>
#include <D3DEffect.h>
#include <TextureLoader.h>

#define M_PI acosf(-1.0)

using namespace std;
using namespace Microsoft::WRL;
using namespace DirectX;
using namespace DX;

// Global Variables:
constexpr auto MAX_LOADSTRING = 100;
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
unique_ptr<D3DScene> g_scene;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int, HWND& hwnd);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

inline float Randf()
{
    return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

inline DirectX::XMMATRIX Transform(const D3D12_VIEWPORT& viewport, float angle, float scale, float w, float h)
{
    auto r = DirectX::XMMatrixRotationZ(angle / 180 * M_PI);
    auto base = DirectX::XMMatrixTranslation(-viewport.Width / 2, -viewport.Height / 2, 0.0f);
    r = DirectX::XMMatrixMultiply(base, r);
    base.r[3] = DirectX::XMVectorMultiply(base.r[3], DirectX::XMVECTOR({ -1.0f, -1.0f, 0.0f, 1.0f }));
    r = DirectX::XMMatrixMultiply(r, base);

    auto s = DirectX::XMMatrixScaling(scale, scale, 1.0f);
    auto t = DirectX::XMMatrixTranslation(viewport.Width / 2 - w * scale / 2, viewport.Height / 2 - h * scale / 2, 0.0f);

    auto result = DirectX::XMMatrixMultiply(s, t);

    return DirectX::XMMatrixMultiply(result, r);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    wchar_t path[MAX_PATH];

    GetModuleFileNameW(NULL, path, MAX_PATH);
    PathCchRemoveFileSpec(path, MAX_PATH);

    wstring s = path;

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINDOWSPROJECT1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    HWND hwnd = NULL;


    // Perform application initialization:
    if (!InitInstance(hInstance, nCmdShow, hwnd))
    {
        return FALSE;
    }

    g_scene = make_unique<D3DScene>();
    SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(g_scene.get()));


    RECT winRect;
    GetClientRect(hwnd, &winRect);

#ifdef _DEBUG
    g_scene->EnableDebug();
#endif // DEBUG

    g_scene->Initialize(hwnd, 0.0f, 0.0f, static_cast<float>(winRect.right - winRect.left), static_cast<float>(winRect.bottom - winRect.top));

    D3D12_SAMPLER_DESC samplerState =
    {
        D3D12_FILTER_MIN_MAG_MIP_LINEAR,
        D3D12_TEXTURE_ADDRESS_MODE_WRAP,
        D3D12_TEXTURE_ADDRESS_MODE_WRAP,
        D3D12_TEXTURE_ADDRESS_MODE_WRAP,
        0.0f,
        1,
        D3D12_COMPARISON_FUNC_ALWAYS,
        { 0, 0, 0, 0},
        0,
        D3D12_FLOAT32_MAX,
    };

    ifstream vsStream(s + L"\\VertexShader.cso", ios::binary);
    ifstream psStream(s + L"\\PixelShader.cso", ios::binary);
    vector<char> vs = vector<char>(istreambuf_iterator<char>(vsStream), istreambuf_iterator<char>());
    vector<char> ps = vector<char>(istreambuf_iterator<char>(psStream), istreambuf_iterator<char>());

    vsStream.close();
    psStream.close();

    float w = 1600;
    float h = 450;

    HRESULT hr = S_OK;
    std::vector<ComPtr<ID3D12Resource>> textures;
    ComPtr<ID3D12Resource> texture = nullptr;
    std::vector<wstring> textureFiles = { s + L"\\a.png", s + L"\\b.png" };
    std::shared_ptr<D3DDescriptorHeap> textureHeap = make_shared<D3DDescriptorHeap>(g_scene->Device(), 2);
    std::shared_ptr<D3DDescriptorHeap> textureHeap2 = make_shared<D3DDescriptorHeap>(g_scene->Device(), 2);
    std::shared_ptr<D3DDescriptorHeap> samplerHeap = make_shared<D3DDescriptorHeap>(g_scene->Device(), D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, 1);
    size_t idx = 0;

    TextureLoader loader(g_scene->Device(), g_scene->CommandQueue());

    loader.Begin();

    for (auto& element : textureFiles)
    {
        loader.CreateTexture(element.data(), &texture, false);
        textures.push_back(texture);
        loader.CreateShaderResourceView(texture.Get(), textureHeap->GetCpuHandle(idx));
        loader.CreateShaderResourceView(texture.Get(), textureHeap2->GetCpuHandle((idx + 1) % 2));
        idx++;
    }

    loader.End().Wait();

    g_scene->Device()->CreateSampler(&samplerState, samplerHeap->GetCpuHandle(0));

    DirectX::VertexPositionNormalTexture vertices[] = {
        { XMFLOAT3({ 0.0f, 0.0f, 0.0f }), XMFLOAT3({ 0.0f, 0.0f, 1.0f }), XMFLOAT2({ 0.0f, 1.0f }) },
        { XMFLOAT3({ w, 0.0f, 0.0f }), XMFLOAT3({ 0.0f, 0.0f, 1.0f }), XMFLOAT2({  2.0f, 1.0f }) },
        { XMFLOAT3({ w, h, 0.0f }), XMFLOAT3({ 0.0f, 0.0f, 1.0f }), XMFLOAT2({ 2.0f, 0.0f }) },
        { XMFLOAT3({ 0.0f, h, 0.0f }), XMFLOAT3({ 0.0f, 0.0f, 1.0f }), XMFLOAT2({ 0.0f, 0.0f }) },
    };

    vector<InputElement> inputs =
    {
        { "POS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TEXMATRIX", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 0},
        { "TEXMATRIX", 1, DXGI_FORMAT_R32G32B32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 0},
        { "TEXMATRIX", 2, DXGI_FORMAT_R32G32B32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 0}
    };

    D3D12_SHADER_BYTECODE vertexShader = {
        vs.data(),
        vs.size()
    };

    D3D12_SHADER_BYTECODE pixelShader = {
      ps.data(),
      ps.size()
    };

    uint32_t indices[] = {
         0, 1, 2, 0, 2, 3
    };

    float textureMatrixX[] = {
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f
    };

    float textureMatrixY[] =
    {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
    };

    float color[] = {
        0.0f, 0.0f, 0.0f, 0.0f,
    };

    RenderTargetState renderTargetState(DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_FORMAT_D32_FLOAT);
    vector<shared_ptr<D3DAttribute>> attributes = {
        make_shared<D3DAttribute>(g_scene.get(), static_cast<uint32_t>(sizeof(VertexPositionNormalTexture)), 4, static_cast <uint32_t>(sizeof(VertexPositionNormalTexture))),
        make_shared<D3DAttribute>(g_scene.get(), static_cast<uint32_t>(sizeof(float)), 9, static_cast <uint32_t>(sizeof(float) * 9))
    };
    auto index = make_shared<D3DIndex>(g_scene.get(), 4, 6);
    shared_ptr<D3DConstant> textureMatrixYConstant = make_shared<D3DConstant>(g_scene.get(), 4, 16);
    shared_ptr<D3DConstant> colorConstant = make_shared<D3DConstant>(g_scene.get(), 4, 4);

    auto effect = make_shared<D3DEffect>(move(inputs), std::vector({ colorConstant.get() }), 1, 2, 1);

    attributes[0]->Update(vertices);
    attributes[1]->Update(textureMatrixX);
    index->Update(indices);
    textureMatrixYConstant->Update(textureMatrixY);
    colorConstant->Update(color);

    vector<D3DAttribute*> attributePtr;
    vector<D3DConstant*> constantPtr;

    for (auto& p : attributes)
    {
        attributePtr.push_back(p.get());
    }


    D3DObject obj1(
        attributePtr,
        index.get(),
        { textureMatrixYConstant.get() },
        effect.get(),
        textureHeap.get(),
        samplerHeap.get()
    );

    D3DObject obj2(
        attributePtr,
        index.get(),
        { textureMatrixYConstant.get() },
        effect.get(),
        textureHeap2.get(),
        samplerHeap.get()
    );

    effect->Initialize(*g_scene, vertexShader, pixelShader, CommonStates::Opaque, CommonStates::DepthDefault, Global::defaultRasterizerStateDesc, renderTargetState);

    obj1.Initialize(*g_scene);
    obj2.Initialize(*g_scene);

    g_scene->SetRenderList(std::initializer_list<D3DObject*>{ &obj1, & obj2 });

    uint32_t count = 0;
    float scale = 1;
    float step = 0.01f;
    float angle = 0;
    float angleStep = 1;

    g_scene->SetInactiveTargetUpdateTimeout(true, 1.0 / 30);

    g_scene->OnUpdate([&](D3DScene& scene, double time) -> void
        {
            scale += step;
            angle += angleStep;
            count++;

            if (angle > 360)
            {
                angle -= 360;
            }

            if (scale > 1.0f || scale < 0.1)
            {
                step = -step;
            }


            textureMatrixX[0] = scale;
            textureMatrixY[5] = scale;

            if (count == 10)
            {
                count = 0;
                color[0] = Randf();
                color[1] = Randf();
                color[2] = Randf();
                color[3] = Randf();
            }

            attributes[1]->Update(textureMatrixX);
            textureMatrixYConstant->Update(textureMatrixY);
            colorConstant->Update(color);

            obj1.UpdateTransform(Transform(g_scene->Viewport(), angle, scale, w, h));
            obj2.UpdateTransform(Transform(g_scene->Viewport(), angle * 2, scale, w, h));
        });

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSPROJECT1));

    MSG msg = {};

    while (WM_QUIT != msg.message)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            g_scene->Tick();
        }
    }
    g_scene.reset();
    return (int)msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECT1));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_WINDOWSPROJECT1);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow, HWND& hwnd)
{
    hInst = hInstance; // Store instance handle in our global variable

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    hwnd = hWnd;

    return TRUE;
}


//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static bool s_in_sizemove = false;
    static bool s_in_suspend = false;
    static bool s_minimized = false;
    static bool s_fullscreen = false;

    auto scene = reinterpret_cast<D3DScene*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Parse the menu selections:
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    {
        if (s_in_sizemove && scene)
        {
            scene->Tick();
        }
        else
        {
            PAINTSTRUCT ps;
            (void)BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
        }
    }
    break;
    case WM_SIZE:
    {
        if (wParam == SIZE_MINIMIZED)
        {
            if (!s_minimized)
            {
                s_minimized = true;
                if (!s_in_suspend && scene)
                    scene->OnSuspending();
                s_in_suspend = true;
            }
        }
        else if (s_minimized)
        {
            s_minimized = false;
            if (s_in_suspend && scene)
                scene->OnResuming();
            s_in_suspend = false;
        }
        else if (!s_in_sizemove && scene)
        {
            scene->OnWindowSizeChanged(0.0f, 0.0f, static_cast<float>(LOWORD(lParam)), static_cast<float>(HIWORD(lParam)));
        }
    }
    break;
    case WM_ENTERSIZEMOVE:
        s_in_sizemove = true;
        break;

    case WM_EXITSIZEMOVE:
        s_in_sizemove = false;
        if (scene)
        {
            RECT rc;
            GetClientRect(hWnd, &rc);

            scene->OnWindowSizeChanged(0.0f, 0.0f, static_cast<float>(rc.right - rc.left), static_cast<float>(rc.bottom - rc.top));
        }
        break;
    case WM_ACTIVATEAPP:
        if (scene)
        {
            if (wParam)
            {
                scene->OnActivated();
            }
            else
            {
                scene->OnDeactivated();
            }
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
