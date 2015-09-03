#include "pch.h"
#include "Sample3DSceneRenderer.h"

#include "..\Common\DirectXHelper.h"

#include "Cube.h"
#include "Sphere.h"
#include "Room.h"

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
}

// Called once per frame, rotates the cube and calculates the model and view matrices.
void Sample3DSceneRenderer::Update(DX::StepTimer const& timer)
{
	for (auto object : m_sceneObjects)
	{
		object->m_constantBufferData.lightPosition.z = 5.0f * sinf(timer.GetTotalSeconds());
	}

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

	// Draw scene objects
	for (auto object : m_sceneObjects)
	{
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
	for (auto object : m_sceneObjects)
	{
		XMStoreFloat4(&object->m_constantBufferData.lightPosition,
			XMVectorSet(m_light->m_position.x, m_light->m_position.y, m_light->m_position.z, m_light->m_position.w));
		XMStoreFloat4(&object->m_constantBufferData.lightColor,
			XMVectorSet(m_light->m_color.x, m_light->m_color.y, m_light->m_color.z, m_light->m_color.w));
	}

	// Load shaders asynchronously.
	auto loadVSTask = DX::ReadDataAsync(L"CustomVertexShader.cso");
	auto loadPSTask = DX::ReadDataAsync(L"CustomPixelShader.cso");

	// After the vertex shader file is loaded, create the shader and input layout.
	auto createVSTask = loadVSTask.then([this, cube](const std::vector<byte>& fileData) {
		for (auto object : m_sceneObjects)
		{
			object->LoadVS(m_deviceResources, fileData);
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

	// Once both shaders are loaded, create the mesh.
	auto createCubeTask = (createPSTask && createVSTask).then([this, cube]() {
		for (auto object : m_sceneObjects)
		{
			object->GenerateMesh(m_deviceResources);
		}
	});

	// Once the cube is loaded, the object is ready to be rendered.
	createCubeTask.then([this]() {
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