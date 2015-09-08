#include "pch.h"
#include "SceneRenderer.h"

#include "..\Common\DirectXHelper.h"

#include "Cube.h"
#include "Sphere.h"
#include "Room.h"
#include "Camera.h"

#include <Windows.h>

using namespace SimpleScene;

using namespace DirectX;
using namespace Windows::Foundation;

// Loads vertex and pixel shaders from files and instantiates the cube geometry.
SceneRenderer::SceneRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
	m_loadingComplete(false),
	m_deviceResources(deviceResources)
{
	m_sceneObjects.push_back(new Cube());
	m_sceneObjects.push_back(new Sphere());
	m_sceneObjects.push_back(new Room());

	CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();
}

// Initializes view parameters when the window size changes.
void SceneRenderer::CreateWindowSizeDependentResources()
{
	m_camera = std::shared_ptr<Camera>(new Camera(
		XMFLOAT4(1.0f, 1.0f, 1.6f, 0.0f),
		XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f),
		XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f),
		m_deviceResources
		));

	for (auto object : m_sceneObjects)
	{
		XMStoreFloat4x4(&object->m_constantBufferData.view, m_camera->getView());
		
		XMStoreFloat4x4(
			&object->m_constantBufferData.projection, m_camera->getProjection());

		XMStoreFloat4(&object->m_constantBufferData.cameraPosition, m_camera->getEye());
	}
}

// Called once per frame, rotates the cube and calculates the model and view matrices.
void SceneRenderer::Update(DX::StepTimer const& timer)
{
	m_light->m_position.x = 3.0f * cosf(timer.GetTotalSeconds());
	//m_light->m_position.x = 0.0f;
	//m_light->m_position.y = 2.0f;
	m_light->m_position.z = 3.0f * sinf(timer.GetTotalSeconds());
	//m_light->m_position.z = 1.0f * sinf(timer.GetTotalSeconds());
	m_light->UpdateBuffer();

	m_camera->Update(timer, m_deviceResources);
	for (auto object : m_sceneObjects)
	{
		XMStoreFloat4(&object->m_constantBufferData.cameraPosition, m_camera->getEye());
	}
}

// Renders one frame using the vertex and pixel shaders.
void SceneRenderer::Render()
{
	// Loading is asynchronous. Only draw geometry after it's loaded.
	if (!m_loadingComplete)
	{
		return;
	}

	RenderShadowMap();
	RenderSceneWithShadows();
}

void SceneRenderer::RenderShadowMap()
{
	auto context = m_deviceResources->GetD3DDeviceContext();

	context->ClearRenderTargetView(m_deviceResources->GetBackBufferRenderTargetView(), DirectX::Colors::CornflowerBlue);
	context->ClearDepthStencilView(m_frontShadowDepthView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	context->ClearDepthStencilView(m_backShadowDepthView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// Render all the objects in the scene that can cast shadows onto themselves or onto other objects.
	
	// Set rendering state.
	context->RSSetViewports(1, &m_shadowViewport);
	context->RSSetState(m_shadowRenderState.Get());

	// Only bind the ID3D11DepthStencilView for output.
	context->OMSetRenderTargets(
		0,
		nullptr,
		m_frontShadowDepthView.Get()
		);

	// Draw scene to front shadow paraboloid
	for (int i = 0; i < m_sceneObjects.size(); i++)
	{
		m_sceneObjects[i]->m_constantBufferData.view = m_light->m_viewsBufferData.frontView;
		//m_sceneObjects[i]->m_constantBufferData.projection = m_light->m_viewProjectionBufferData.projection;
		m_sceneObjects[i]->m_constantBufferData.cameraPosition = XMFLOAT4(1.0f, 0, 0, 0);
		m_sceneObjects[i]->DrawDepthMap(m_deviceResources);
	}

	// Only bind the ID3D11DepthStencilView for output.
	context->OMSetRenderTargets(
		0,
		nullptr,
		m_backShadowDepthView.Get()
		);

	// Draw scene to back shadow paraboloid
	for (int i = 0; i < m_sceneObjects.size(); i++)
	{
		m_sceneObjects[i]->m_constantBufferData.view = m_light->m_viewsBufferData.backView;
		//m_sceneObjects[i]->m_constantBufferData.projection = m_light->m_viewProjectionBufferData.projection;
		m_sceneObjects[i]->m_constantBufferData.cameraPosition = XMFLOAT4(-1.0f, 0, 0, 0);
		m_sceneObjects[i]->DrawDepthMap(m_deviceResources);
	}
}

void SceneRenderer::RenderSceneWithShadows()
{
	auto context = m_deviceResources->GetD3DDeviceContext();

	context->ClearRenderTargetView(m_deviceResources->GetBackBufferRenderTargetView(), DirectX::Colors::CornflowerBlue);
	context->ClearDepthStencilView(m_deviceResources->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// Set render targets to the screen.
	ID3D11RenderTargetView *const targets[1] = { m_deviceResources->GetBackBufferRenderTargetView() };
	context->OMSetRenderTargets(1, targets, m_deviceResources->GetDepthStencilView());
	
	D3D11_VIEWPORT view = m_deviceResources->GetScreenViewport();
	context->RSSetViewports(1, &view);
	context->RSSetState(m_drawingRenderState.Get());

	context->PSSetSamplers(0, 1, m_comparisonSampler.GetAddressOf());
	context->PSSetShaderResources(0, 1, m_frontShadowResourceView.GetAddressOf());
	context->PSSetShaderResources(1, 1, m_backShadowResourceView.GetAddressOf());
	
	// Prepare the constant buffer to send it to the graphics device.
	context->UpdateSubresource(
		m_light->m_viewsBuffer.Get(),
		0,
		NULL,
		&m_light->m_viewsBufferData,
		0,
		0
		);

	context->VSSetConstantBuffers(
		1,
		1,
		m_light->m_viewsBuffer.GetAddressOf()
		);

	// Draw scene objects
	for (auto object : m_sceneObjects)
	{
		XMStoreFloat4x4(&object->m_constantBufferData.view, m_camera->getView());
		XMStoreFloat4x4(&object->m_constantBufferData.projection, m_camera->getProjection());
		object->Draw(m_deviceResources);
	}
}

void SceneRenderer::CreateDeviceDependentResources()
{
	// Initialize scene objects
	Cube* cube = (Cube*)(m_sceneObjects[0]);
	XMStoreFloat4x4(&cube->m_constantBufferData.world, 
		XMMatrixTranspose(XMMatrixTranslation(-0.5f,0.0f,0.0f)) * 
		XMMatrixScaling(0.5f, 1.0f, 1.5f));
	Sphere* sphere = (Sphere*)(m_sceneObjects[1]);
	XMStoreFloat4x4(&sphere->m_constantBufferData.world,
		XMMatrixTranspose(XMMatrixTranslation(0.5f, 0.0f, 0.0f)));
	Room* room = (Room*)(m_sceneObjects[2]);
	XMStoreFloat4x4(&room->m_constantBufferData.world,
		XMMatrixTranspose(XMMatrixTranslation(0.0f, 9.5f, 0.0f)) *
		XMMatrixScaling(20, 20, 20));

	// Initialize light
	m_light = std::shared_ptr<PointLight>(new PointLight(
		DirectX::XMFLOAT4(2.f, 2.f, 2.f, 1.f), XMFLOAT4(1.f, 1.f, 1.f, 1.f)));
	m_light->LoadLightViewProjectionBuffer(m_deviceResources);

	// Load shaders asynchronously.
	auto loadVSTask = DX::ReadDataAsync(L"SceneVertexShader.cso");
	auto loadDepthVSTask = DX::ReadDataAsync(L"ShadowMapVertexShader.cso");
	auto loadPSTask = DX::ReadDataAsync(L"ScenePixelShader.cso");
	auto loadDepthPSTask = DX::ReadDataAsync(L"ShadowMapPixelShader.cso");

	// After the vertex shader file is loaded, create the shader and input layout.
	auto createVSTask = loadVSTask.then([this, cube](const std::vector<byte>& fileData) {
		for (auto object : m_sceneObjects)
		{
			object->LoadVS(m_deviceResources, fileData);
		}
	});
	auto createDepthVSTask = loadDepthVSTask.then([this, cube](const std::vector<byte>& fileData) {
		for (auto object : m_sceneObjects)
		{
			object->LoadDepthVS(m_deviceResources, fileData);
		}
	});

	// After the pixel shader file is loaded, create the shader and constant buffer.
	auto createPSTask = loadPSTask.then([this, cube](const std::vector<byte>& fileData) {
		for (auto object : m_sceneObjects)
		{
			object->LoadPS(m_deviceResources, fileData);
			object->LoadCB(m_deviceResources);
		}
	});
	auto createDepthPSTask = loadDepthPSTask.then([this, cube](const std::vector<byte>& fileData) {
		for (auto object : m_sceneObjects)
		{
			object->LoadDepthPS(m_deviceResources, fileData);
		}
	});

	// Once shaders are loaded, create the mesh.
	auto createSceneObjectsTask = (createPSTask && createDepthPSTask && createVSTask && createDepthVSTask).then([this, cube]() {
		for (auto object : m_sceneObjects)
		{
			object->GenerateMesh(m_deviceResources);
		}
	});

	// Initializing resources needed for shadow mapping
	auto createShadowMapResources = createSceneObjectsTask.then([this]() {
		auto pD3DDevice = m_deviceResources->GetD3DDevice();
		HRESULT hr;

		D3D11_TEXTURE2D_DESC shadowMapDesc;
		ZeroMemory(&shadowMapDesc, sizeof(D3D11_TEXTURE2D_DESC));
		shadowMapDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
		shadowMapDesc.MipLevels = 1;
		shadowMapDesc.ArraySize = 1;
		shadowMapDesc.SampleDesc.Count = 1;
		shadowMapDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
		shadowMapDesc.Height = static_cast<UINT>(m_shadowMapDimension);
		shadowMapDesc.Width = static_cast<UINT>(m_shadowMapDimension);

		hr = pD3DDevice->CreateTexture2D(
			&shadowMapDesc,
			nullptr,
			&m_frontShadowMap
			);
		hr = pD3DDevice->CreateTexture2D(
			&shadowMapDesc,
			nullptr,
			&m_backShadowMap
			);

		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
		ZeroMemory(&depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
		depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;

		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
		ZeroMemory(&shaderResourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderResourceViewDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		shaderResourceViewDesc.Texture2D.MipLevels = 1;

		hr = pD3DDevice->CreateDepthStencilView(
			m_frontShadowMap.Get(),
			&depthStencilViewDesc,
			&m_frontShadowDepthView
			);
		hr = pD3DDevice->CreateDepthStencilView(
			m_backShadowMap.Get(),
			&depthStencilViewDesc,
			&m_backShadowDepthView
			);

		hr = pD3DDevice->CreateShaderResourceView(
			m_frontShadowMap.Get(),
			&shaderResourceViewDesc,
			&m_frontShadowResourceView
			);
		hr = pD3DDevice->CreateShaderResourceView(
			m_backShadowMap.Get(),
			&shaderResourceViewDesc,
			&m_backShadowResourceView
			);

		D3D11_SAMPLER_DESC comparisonSamplerDesc;
		ZeroMemory(&comparisonSamplerDesc, sizeof(D3D11_SAMPLER_DESC));
		comparisonSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		comparisonSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
		comparisonSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
		comparisonSamplerDesc.BorderColor[0] = 1.0f;
		comparisonSamplerDesc.BorderColor[1] = 1.0f;
		comparisonSamplerDesc.BorderColor[2] = 1.0f;
		comparisonSamplerDesc.BorderColor[3] = 1.0f;
		comparisonSamplerDesc.MinLOD = 0.f;
		comparisonSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
		comparisonSamplerDesc.MipLODBias = 0.f;
		comparisonSamplerDesc.MaxAnisotropy = 0;
		comparisonSamplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
		comparisonSamplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
		
		DX::ThrowIfFailed(
			pD3DDevice->CreateSamplerState(
				&comparisonSamplerDesc,
				&m_comparisonSampler
				)
			);

		D3D11_SAMPLER_DESC linearSamplerDesc;
		ZeroMemory(&linearSamplerDesc, sizeof(D3D11_SAMPLER_DESC));
		linearSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		linearSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		linearSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		linearSamplerDesc.MinLOD = 0;
		linearSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
		linearSamplerDesc.MipLODBias = 0.f;
		linearSamplerDesc.MaxAnisotropy = 0;
		linearSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		linearSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;

		DX::ThrowIfFailed(
			pD3DDevice->CreateSamplerState(
				&linearSamplerDesc,
				&m_linearSampler
				)
			);

		// Init viewport for shadow rendering
		ZeroMemory(&m_shadowViewport, sizeof(D3D11_VIEWPORT));
		m_shadowViewport.Height = m_shadowMapDimension;
		m_shadowViewport.Width = m_shadowMapDimension;
		m_shadowViewport.MinDepth = 0.f;
		m_shadowViewport.MaxDepth = 1.f;

		// Init rasterizer states
		D3D11_RASTERIZER_DESC drawingRenderStateDesc;
		ZeroMemory(&drawingRenderStateDesc, sizeof(D3D11_RASTERIZER_DESC));
		drawingRenderStateDesc.CullMode = D3D11_CULL_BACK;
		drawingRenderStateDesc.FillMode = D3D11_FILL_SOLID;
		drawingRenderStateDesc.DepthClipEnable = true; // Feature level 9_1 requires DepthClipEnable == true

		DX::ThrowIfFailed(
			pD3DDevice->CreateRasterizerState(
				&drawingRenderStateDesc,
				&m_drawingRenderState
				)
			);

		D3D11_RASTERIZER_DESC shadowRenderStateDesc;
		ZeroMemory(&shadowRenderStateDesc, sizeof(D3D11_RASTERIZER_DESC));
		shadowRenderStateDesc.CullMode = D3D11_CULL_NONE;
		shadowRenderStateDesc.FillMode = D3D11_FILL_SOLID;
		shadowRenderStateDesc.DepthClipEnable = true;

		DX::ThrowIfFailed(
			pD3DDevice->CreateRasterizerState(
				&shadowRenderStateDesc,
				&m_shadowRenderState
				)
			);

	});

	// Once the objects and the shadow map are loaded, the scene is ready to be rendered.
	createShadowMapResources.then([this]() {
		m_loadingComplete = true;
	});
}

void SceneRenderer::ReleaseDeviceDependentResources()
{
	for (auto it = m_sceneObjects.begin(); it != m_sceneObjects.end(); ++it)
	{
		delete (*it);
	}
	m_sceneObjects.clear();

	m_loadingComplete = false;
}