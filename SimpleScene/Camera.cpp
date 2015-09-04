#include "pch.h"
#include "Camera.h"
#include "Windows.h"

Camera::Camera() { };

Camera::Camera(XMFLOAT4 eye, XMFLOAT4 at, XMFLOAT4 up,
	std::shared_ptr<DX::DeviceResources> deviceResources)
	: eye(eye), at(at), up(up)
{
	Windows::Foundation::Size outputSize = deviceResources->GetOutputSize();
	this->aspectRatio = outputSize.Width / outputSize.Height;
	this->fov = 70.0f * XM_PI / 180.0f;

	// This is a simple example of change that can be made when the app is in
	// portrait or snapped view.
	if (aspectRatio < 1.0f)
	{
		this->fov *= 2.0f;
	}

	this->nearClippingPane = 0.01f;
	this->farClippingPane = 100.0f;

	XMFLOAT4X4 orientation = deviceResources->GetOrientationTransform3D();
	this->sceneOrientation = orientation;

	this->movementSpeed = 2.0f;
	this->movementDir = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
}

XMMATRIX Camera::getWorld()
{
	return XMMatrixTranslationFromVector(getEye());
}

XMMATRIX Camera::getView()
{
	return XMMatrixTranspose(XMMatrixLookAtRH(getEye(), getAt(), getUp()));
}

XMMATRIX Camera::getProjection()
{
	// This sample makes use of a right-handed coordinate system using row-major matrices.
	XMMATRIX perspectiveMatrix = XMMatrixPerspectiveFovRH(
		fov,
		aspectRatio,
		nearClippingPane,
		farClippingPane);

	// Note that the OrientationTransform3D matrix is post-multiplied here
	// in order to correctly orient the scene to match the display orientation.
	// This post-multiplication step is required for any draw calls that are
	// made to the swap chain render target. For draw calls to other targets,
	// this transform should not be applied.
	return XMMatrixTranspose(perspectiveMatrix * XMLoadFloat4x4(&sceneOrientation));
}

void Camera::ProcessInput(DX::StepTimer const& timer, std::shared_ptr<DX::DeviceResources>& deviceResources)
{
	using namespace Windows::UI::Core;
	using namespace Windows::System;

	XMVECTOR md = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR forward = this->getDirection();
	XMVECTOR right = XMVector3Cross(forward, this->getUp());

	auto window = deviceResources->GetWindow();

	// Keyboard handling
	if (window->GetAsyncKeyState(VirtualKey::W) == CoreVirtualKeyStates::Down)
	{
		md += forward;
	}

	if (window->GetAsyncKeyState(VirtualKey::S) == CoreVirtualKeyStates::Down)
	{
		md += -forward;
	}

	if (window->GetAsyncKeyState(VirtualKey::A) == CoreVirtualKeyStates::Down)
	{
		md += -right;
	}

	if (window->GetAsyncKeyState(VirtualKey::D) == CoreVirtualKeyStates::Down)
	{
		md += right;
	}

	if (window->GetAsyncKeyState(VirtualKey::Space) == CoreVirtualKeyStates::Down)
	{
		md += this->getUp();
	}

	if (window->GetAsyncKeyState(VirtualKey::Control) == CoreVirtualKeyStates::Down)
	{
		md += -this->getUp();
	}

	this->setMovementDir(md);
}

void Camera::Update(DX::StepTimer const& timer, std::shared_ptr<DX::DeviceResources>& deviceResources)
{
	ProcessInput(timer, deviceResources);
	XMVECTOR newEye = getEye() += this->getMovementDir() * this->movementSpeed * timer.GetElapsedSeconds();
	XMStoreFloat4(&this->eye, newEye);
}



Camera::~Camera()
{
}
