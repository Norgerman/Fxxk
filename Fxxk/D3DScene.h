#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <initializer_list>
#include <functional>
#include <mutex>

class D3DObject;

typedef struct VsConstant {
	DirectX::XMMATRIX projection;
} VsConstant;

class D3DScene
{
public:
	D3DScene();
	D3DScene(std::function<DirectX::XMMATRIX(D3DScene&)>&& buildProjection);
	void dispose();
	void resize(float width, float height);
	void init(HWND hwnd);
	ID3D11Device* getDevice() const;
	ID3D11DeviceContext* getContext() const;
	void updateProjection();
	const D3D11_VIEWPORT& getViewport() const;
	void render(std::initializer_list<D3DObject*>&& objs);
	~D3DScene();
private:
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_context;
	IDXGISwapChain* m_swapChain;
	ID3D11RenderTargetView* m_target;
	ID3D11Buffer* m_constantBuffer;
	VsConstant* m_constant;
	D3D11_VIEWPORT m_viewport;
	std::function<DirectX::XMMATRIX(D3DScene&)> m_buildProjection;
	bool m_inited;
	std::mutex m_mutex;
};

