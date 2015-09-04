#include "pch.h"
#include "Sample3DSceneRenderer.h"

#include "..\Common\DirectXHelper.h"

#include "Cube.h"
#include "Sphere.h"
#include "Room.h"
#include "Camera.h"

using namespace SimpleScene;

using namespace DirectX;
using namespace Windows::Foundation;

// Loads vertex and pixel shaders from files and instantiates the cube geometry.
Sample3DSceneRenderer::Sample3DSceneRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
	m_loadingComplete(false),
	m_degreesPerSecond(45),
	m_tracking(false),
	m_deviceResources(deviceResources)
{
	m_sceneObjects.push_back(new Cube());
	m_sceneObjects.push_back(new Sphere());
	m_sceneObjects.push_back(new Room());

	CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();
}

// Initializes view parameters when the window size changes.
void Sample3DSceneRenderer::CreateWindowSizeDependentResources()
{
	Size outputSize = m_deviceResources->GetOutputSize();
	float aspectRatio = outputSize.Width / outputSize.Height;
	float fovAngleY = 70.0f * XM_PI / 180.0f;

	// This is a simple example of change that can be made when the app is in
	// portrait or snapped view.
	if (aspectRatio < 1.0f)
	{
		fovAngleY *= 2.0f;
	}

	// Note that the OrientationTransform3D matrix is post-multiplied here
	// in order to correctly orient the scene to match the display orientation.
	// This post-multiplication step is required for any draw calls that are
	// made to the swap chain render target. For draw calls to other targets,
	// this transform should not be applied.

	// This sample makes use of a right-handed coordinate system using row-major matrices.
	XMMATRIX perspectiveMatrix = XMMatrixPerspectiveFovRH(
		fovAngleY,
		aspectRatio,
		0.01f,
		100.0f
		);

	XMFLOAT4X4 orientation = m_deviceResources->GetOrientationTransform3D();

	XMMATRIX orientationMatrix = XMLoadFloat4x4(&orientation);

	// Eye is at (0,0.7,1.5), looking at point (0,-0.1,0) with the up-vector along the y-axis.
	static const XMVECTORF32 eye = { 1.0f, 1.0f, 1.6f, 0.0f };
	static const XMVECTORF32 at = { 0.0f, 0.0f, 0.0f, 0.0f };
	static const XMVECTORF32 up = { 0.0f, 1.0f, 0.0f, 0.0f };

	for (auto object : m_sceneObjects)
	{
		XMStoreFloat4x4(&object->m_constantBufferData.view, XMMatrixTranspose(XMMatrixLookAtRH(eye, at, up)));
		
		XMStoreFloat4x4(
			&object->m_constantBufferData.projection,
			XMMatrixTranspose(perspectiveMatrix * orientationMatrix)
			);

		XMStoreFloat4(&object->m_constantBufferData.cameraPosition, eye);
	}

	m_view = m_sceneObjects[0]->m_constantBufferData.view;
	m_projection = m_sceneObjects[0]->m_constantBufferData.projection;
}

// Called once per frame, rotates the cube and calculates the model and view matrices.
void Sample3DSceneRenderer::Update(DX::StepTimer const& timer)
{
	m_light->m_position.z = 3.0f * sinf(timer.GetTotalSeconds());
	m_light->m_position.x = 3.0f * cosf(timer.GetTotalSeconds());
	for (auto object : m_sceneObjects)
	{
		//object->m_constantBufferData.lightPosition = m_light->m_position;
	}
	m_light->UpdateBuffer();

	/*if (!m_tracking)
	{
		// Convert degrees to radians, then convert seconds to rotation angle
		float radiansPerSecond = XMConvertToRadians(m_degreesPerSecond);
		double totalRotation = timer.GetTotalSeconds() * radiansPerSecond;
		float radians = static_cast<float>(fmod(totalRotation, XM_2PI));

		Rotate(radians);
	}*/
}

// Rotate the 3D cube model a set amount of radians.
void Sample3DSceneRenderer::Rotate(float radians)
{
	// Prepare to pass the updated model matrix to the shader
	//XMStoreFloat4x4(&m_constantBufferData.model, XMMatrixTranspose(XMMatrixRotationY(radians)));
}

void Sample3DSceneRenderer::StartTracking()
{
	m_tracking = true;
}

// When tracking, the 3D cube can be rotated around its Y axis by tracking pointer position relative to the output screen width.
void Sample3DSceneRenderer::TrackingUpdate(float positionX)
{
	if (m_tracking)
	{
		float radians = XM_2PI * 2.0f * positionX / m_deviceResources->GetOutputSize().Width;
		Rotate(radians);
	}
}

void Sample3DSceneRenderer::StopTracking()
{
	m_tracking = false;
}

// Renders one frame using the vertex and pixel shaders.
void Sample3DSceneRenderer::Render()
{
	// Loading is asynchronous. Only draw geometry after it's loaded.
	if (!m_loadingComplete)
	{
		return;
	}

	RenderShadowMap();
	RenderSceneWithShadows();
}

void Sample3DSceneRenderer::RenderShadowMap()
{
	auto context = m_deviceResources->GetD3DDeviceContext();

	context->ClearRenderTargetView(m_deviceResources->GetBackBufferRenderTargetView(), DirectX::Colors::CornflowerBlue);
	context->ClearDepthStencilView(m_shadowDepthView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// Render all the objects in the scene that can cast shadows onto themselves or onto other objects.

	// Only bind the ID3D11DepthStencilView for output.
	context->OMSetRenderTargets(
		0,
		nullptr,
		m_shadowDepthView.Get()
		);

	// Set rendering state.
	context->RSSetViewports(1, &m_shadowViewport);

	// Draw scene objects
	for (int i = 0; i < m_sceneObjects.size(); i++)
	{
		m_sceneObjects[i]->m_constantBufferData.view = m_light->m_viewProjectionBufferData.view;
		m_sceneObjects[i]->m_constantBufferData.projection = m_light->m_viewProjectionBufferData.projection;
		m_sceneObjects[i]->DrawDepthMap(m_deviceResources);
	}
}

void Sample3DSceneRenderer::RenderSceneWithShadows()
{
	auto context = m_deviceResources->GetD3DDeviceContext();

	context->ClearRenderTargetView(m_deviceResources->GetBackBufferRenderTargetView(), DirectX::Colors::CornflowerBlue);
	context->ClearDepthStencilView(m_deviceResources->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// Set render targets to the screen.
	ID3D11RenderTargetView *const targets[1] = { m_deviceResources->GetBackBufferRenderTargetView() };
	context->OMSetRenderTargets(1, targets, m_deviceResources->GetDepthStencilView());
	
	D3D11_VIEWPORT view = m_deviceResources->GetScreenViewport();
	context->RSSetViewports(1, &view);

	context->PSSetSamplers(0, 1, m_comparisonSampler.GetAddressOf());
	context->PSSetShaderResources(0, 1, m_shadowResourceView.GetAddressOf());
	
	// Prepare the constant buffer to send it to the graphics device.
	context->UpdateSubresource(
		m_light->m_viewProjectionBuffer.Get(),
		0,
		NULL,
		&m_light->m_viewProjectionBufferData,
		0,
		0
		);

	context->VSSetConstantBuffers(
		1,
		1,
		m_light->m_viewProjectionBuffer.GetAddressOf()
		);

	// Draw scene objects
	for (auto object : m_sceneObjects)
	{
		object->m_constantBufferData.view = m_view;
		object->m_constantBufferData.projection = m_projection;
		object->Draw(m_deviceResources);
	}
}

void Sample3DSceneRenderer::CreateDeviceDependentResources()
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
	for (auto object : m_sceneObjects)
	{
		XMStoreFloat4(&object->m_constantBufferData.lightPosition,
			XMVectorSet(m_light->m_position.x, m_light->m_position.y, m_light->m_position.z, m_light->m_position.w));
		XMStoreFloat4(&object->m_constantBufferData.lightColor,
			XMVectorSet(m_light->m_color.x, m_light->m_color.y, m_light->m_color.z, m_light->m_color.w));
	}

	// Load shaders asynchronously.
	auto loadVSTask = DX::ReadDataAsync(L"ShadowVertexShader.cso");
	auto loadDepthVSTask = DX::ReadDataAsync(L"SampleVertexShader.cso");
	auto loadPSTask = DX::ReadDataAsync(L"ShadowPixelShader.cso");
	auto loadDepthPSTask = DX::ReadDataAsync(L"SamplePixelShader.cso");

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
			object->LoadRasterStates(m_deviceResources);
		}
	});

	// Once both shaders are loaded, create the mesh.
	auto createSceneObjectsTask = (createPSTask && createDepthPSTask && createVSTask && createDepthVSTask).then([this, cube]() {
		for (auto object : m_sceneObjects)
		{
			object->GenerateMesh(m_deviceResources);
		}
	});

	auto createShadowMapResources = createSceneObjectsTask.then([this]() {
		auto pD3DDevice = m_deviceResources->GetD3DDevice();

		D3D11_TEXTURE2D_DESC shadowMapDesc;
		ZeroMemory(&shadowMapDesc, sizeof(D3D11_TEXTURE2D_DESC));
		shadowMapDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
		shadowMapDesc.MipLevels = 1;
		shadowMapDesc.ArraySize = 1;
		shadowMapDesc.SampleDesc.Count = 1;
		shadowMapDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
		shadowMapDesc.Height = static_cast<UINT>(m_shadowMapDimension);
		shadowMapDesc.Width = static_cast<UINT>(m_shadowMapDimension);

		HRESULT hr = pD3DDevice->CreateTexture2D(
			&shadowMapDesc,
			nullptr,
			&m_shadowMap
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
			m_shadowMap.Get(),
			&depthStencilViewDesc,
			&m_shadowDepthView
			);

		hr = pD3DDevice->CreateShaderResourceView(
			m_shadowMap.Get(),
			&shaderResourceViewDesc,
			&m_shadowResourceView
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

	});

	// Once the objects and the shadow map are loaded, the objects are ready to be rendered.
	createShadowMapResources.then([this]() {
		m_loadingComplete = true;
	});
}

void Sample3DSceneRenderer::ReleaseDeviceDependentResources()
{
	for (auto it = m_sceneObjects.begin(); it != m_sceneObjects.end(); ++it)
	{
		delete (*it);
	}
	m_sceneObjects.clear();

	m_loadingComplete = false;
}