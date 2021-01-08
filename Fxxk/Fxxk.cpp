// Fxxk.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "Fxxk.h"
#include "D3DObject.h"
#include "D3DScene.h"
#include "D3DAttribute.h"
#include "D3DConstant.h"
#include "Timer.hpp"

#pragma comment(lib, "pathcch.lib")
#pragma comment(lib, "d3dcompiler.lib" )

#define M_PI acosf(-1.0)

using namespace std;

constexpr auto WM_TICK = WM_USER + 1;
// Global Variables:
constexpr auto MAX_LOADSTRING = 100;
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
D3DScene* g_scene;
Timer* g_timer;
std::function<void(void)> render([]() -> void {});

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int, HWND& hwnd);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

inline float randf()
{
    return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
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

    g_scene = new D3DScene();
    g_scene->init(hwnd);

    ID3DBlob* vs = nullptr, * ps = nullptr, * error = nullptr;
#if _DEBUG
    auto compileFlags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG;
#else
    auto compileFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#endif

    auto hr = D3DCompileFromFile(
        (s + L"\\VertexShader.hlsl").data(),
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "main",
        "vs_5_0",
        compileFlags,
        0,
        &vs,
        &error);
    assert(SUCCEEDED(hr));

    hr = D3DCompileFromFile(
        (s + L"\\PixelShader.hlsl").data(),
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "main",
        "ps_5_0",
        compileFlags,
        0,
        &ps,
        &error);
    assert(SUCCEEDED(hr));

    if (error) {
        error->Release();
    }

    float w = 1600;
    float h = 450;

    float vertices[] = {
         0.0f, 0.0f, 0.0f,
         w, 0.0f, 0.0f,
         w, h, 0.0f,
         0.0f, h, 0.0f
    };
    float normals[] = {
        0.0f, 0.0f, 0.0f,
        w, 0.0f, 0.0f,
        w, h, 0.0f,
        0.0f, h, 0.0f
    };
    float texcoords[] = {
        0.0f, 1.0f,
        2.0f, 1.0f,
        2.0f, 0.0f,
        0.0f, 0.0f,
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
        1.0f, 1.0f, 1.0f, 0.5f,
    };

    // load textures
    std::vector<ID3D11Resource*> textures;
    std::vector<ID3D11ShaderResourceView*> textureViews;
    std::vector<wstring> textureFiles = { s + L"\\a.png", s + L"\\b.png" };
    ID3D11Resource* texture = nullptr;
    ID3D11ShaderResourceView* textureView = nullptr;

    for (auto& element : textureFiles)
    {
        hr = DirectX::CreateWICTextureFromFile(g_scene->getDevice(), element.data(), &texture, &textureView);
        assert(SUCCEEDED(hr));
        textures.push_back(texture);
        textureViews.push_back(textureView);
    }

    D3DObject obj(
        {
            D3DAttribute(vertices, sizeof(float), 12, "POS", sizeof(float) * 3, 0, D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE::D3D11_USAGE_DEFAULT, 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT),
            D3DAttribute(normals, sizeof(float), 12, "NORMAL", sizeof(float) * 3, 0, D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE::D3D11_USAGE_DEFAULT, 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT),
            D3DAttribute(texcoords, sizeof(float), 8, "TEXCOORD", sizeof(float) * 2, 0, D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE::D3D11_USAGE_DEFAULT, 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT),
            D3DAttribute(textureMatrixX, sizeof(float), 9, "TEXMATRIX", sizeof(float) * 9, 0, D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE::D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 3),
            D3DAttribute(indices, sizeof(uint32_t), 6, "INDICES", sizeof(uint32_t) * 2, 0, D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER, D3D11_USAGE::D3D11_USAGE_DEFAULT, 0, DXGI_FORMAT::DXGI_FORMAT_R32_UINT),
        },
        {
            D3DConstant(textureMatrixY, sizeof(float), 12)
        },
        {
             D3DConstant(color, sizeof(float), 4)
        },
        {
            D3D11_FILL_MODE::D3D11_FILL_SOLID,
            D3D11_CULL_MODE::D3D11_CULL_BACK,
            TRUE,
            0,
            0.0f,
            0.0f,
            TRUE,
            FALSE,
            FALSE,
            FALSE
        },
        {
            {
                .Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR,
                .AddressU = D3D11_TEXTURE_ADDRESS_WRAP,
                .AddressV = D3D11_TEXTURE_ADDRESS_WRAP,
                .AddressW = D3D11_TEXTURE_ADDRESS_WRAP,
                .MipLODBias = 0.0f,
                .MaxAnisotropy = 1,
                .ComparisonFunc = D3D11_COMPARISON_ALWAYS,
                .BorderColor = { 0, 0, 0, 0},
                .MinLOD = 0,
                .MaxLOD = D3D11_FLOAT32_MAX,
            }
        },
        D3D_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
        textures,
        textureViews
    );

    float angle = 0;
    float scale = 1.0f;
    float step = 0.01f;
    uint8_t count = 0;

    obj.updateTransform(DirectX::XMMatrixTranslation(g_scene->getViewport().Width / 2 - w / 2, g_scene->getViewport().Height / 2 - h / 2, 0.0f));

    obj.init(*g_scene, { vs->GetBufferPointer(), vs->GetBufferSize() }, { ps->GetBufferPointer(), ps->GetBufferSize() });

    ps->Release();
    vs->Release();

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSPROJECT1));

    MSG msg;

    render = [&obj, &w, &h, &scale, &count, &angle, &step, &textureMatrixX, &textureMatrixY, &color]() -> void
    {
        const auto& viewport = g_scene->getViewport();
        scale += step;
        angle += 1;
        count++;

        if (angle > 360)
        {
            angle -= 360;
        }

        if (scale > 1.0f || scale < 0.1)
        {
            step = -step;
        }

        auto r = DirectX::XMMatrixRotationZ(angle / 180 * M_PI);
        auto base = DirectX::XMMatrixTranslation(-viewport.Width / 2, -viewport.Height / 2, 0.0f);

        textureMatrixX[0] = scale;
        textureMatrixY[5] = scale;

        if (count == 10)
        {
            count = 0;
            color[0] = randf();
            color[1] = randf();
            color[2] = randf();
            color[3] = randf();
        }

        obj.updateAttribute(3, textureMatrixX);
        obj.updateVSConstant(0, textureMatrixY);
        obj.updatePSConstant(0, color);

        r = DirectX::XMMatrixMultiply(base, r);
        base.r[3] = DirectX::XMVectorMultiply(base.r[3], DirectX::XMVECTOR({ -1.0f, -1.0f, 0.0f, 1.0f }));
        r = DirectX::XMMatrixMultiply(r, base);

        auto s = DirectX::XMMatrixScaling(scale, scale, 1.0f);
        auto t = DirectX::XMMatrixTranslation(viewport.Width / 2 - w * scale / 2, viewport.Height / 2 - h * scale / 2, 0.0f);

        auto result = DirectX::XMMatrixMultiply(s, t);

        obj.updateTransform(DirectX::XMMatrixMultiply(result, r));
        g_scene->render({ &obj });
    };

    g_timer = new Timer([&hwnd]() -> void
        {
            SendMessage(hwnd, WM_TICK, 0, 0);
        }, 16);

    g_timer->start();

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

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
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        // TODO: Add any drawing code that uses hdc here...
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_SIZE:
    {
        if (g_scene)
        {
            UINT width = LOWORD(lParam);
            UINT height = HIWORD(lParam);
            if (width == 0 || height == 0)
            {
                g_timer->stop();
            }
            else
            {
                g_scene->resize(static_cast<float>(width), static_cast<float>(height));
                if (!g_timer->started())
                {
                    g_timer->start();
                }
            }
        }
    }
    break;
    case WM_TICK:
        render();
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
