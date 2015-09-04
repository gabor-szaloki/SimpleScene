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
	XMMATRIX perspectiveMatrix = XMMatrixPerspectiveFovRH(
		fov,
		aspectRatio,
		nearClippingPane,
		farClippingPane);

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
