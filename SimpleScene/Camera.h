#pragma once
#include "Common\DirectXHelper.h"
#include "Common\DeviceResources.h"
#include <vector>

using namespace DirectX;

class Camera
{
public:
	Camera();
	Camera(XMFLOAT4 eye, XMFLOAT4 at, XMFLOAT4 up, std::shared_ptr<DX::DeviceResources> deviceResources);

	inline XMVECTOR getEye() { return XMLoadFloat4(&eye); }
	inline XMVECTOR getAt() { return XMLoadFloat4(&at); }
	inline XMVECTOR getUp() { return XMLoadFloat4(&up); }
	inline XMVECTOR getDirection() { return XMVector3Normalize(getAt() - getEye()); }
	inline XMVECTOR getPlaneDirection() { return XMVector3Normalize(XMVectorSet(at.x - eye.x, 0, at.z - eye.z, 1)); }
	inline XMVECTOR getMovementDir() { return XMLoadFloat4(&movementDir); }
	inline void setMovementDir(XMVECTOR value) { XMStoreFloat4(&this->movementDir, value); }
	XMMATRIX getWorld();
	XMMATRIX getView();
	XMMATRIX getProjection();
	inline float getPitch() { return atan((at.y - eye.y) / (at.x - eye.x)); }
	inline float getYaw() { return 0.5 * atan((at.z - eye.z) / (at.x - eye.x)); }
	inline float getRoll() { return 0.f; }

	//void ProcessInput(std::vector<PlayerInputData>* playerActions);
	//void Update(DX::StepTimer const& timer);

	~Camera();

private:
	XMFLOAT4 eye;
	XMFLOAT4 at;
	XMFLOAT4 up;
	float fov;
	float aspectRatio;
	float nearClippingPane;
	float farClippingPane;

	float movementSpeed;
	XMFLOAT4 movementDir;

	XMFLOAT4X4 sceneOrientation;
};

