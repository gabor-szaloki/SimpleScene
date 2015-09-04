#pragma once

#include "..\Common\DeviceResources.h"
#include "ShaderStructures.h"
#include "..\Common\StepTimer.h"

#include "SceneObject.h"
#include "PointLight.h"
#include "Camera.h"

namespace SimpleScene
{
	// This sample renderer instantiates a basic rendering pipeline.
	class Sample3DSceneRenderer
	{
	public:
		Sample3DSceneRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources);
		void CreateDeviceDependentResources();
		void CreateWindowSizeDependentResources();
		void ReleaseDeviceDependentResources();
		void Update(DX::StepTimer const& timer);
		void Render();
		void RenderShadowMap();
		void RenderSceneWithShadows();
		void StartTracking();
		void TrackingUpdate(float positionX);
		void StopTracking();
		bool IsTracking() { return m_tracking; }


	private:
		void Rotate(float radians);

	private:
		// Cached pointer to device resources.
		std::shared_ptr<DX::DeviceResources> m_deviceResources;

		// Shadow map resources
		float m_shadowMapDimension = 2048.f;
		Microsoft::WRL::ComPtr<ID3D11Texture2D>          m_shadowMap;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView>   m_shadowDepthView;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shadowResourceView;
		Microsoft::WRL::ComPtr<ID3D11SamplerState>       m_comparisonSampler;
		Microsoft::WRL::ComPtr<ID3D11SamplerState>       m_linearSampler;
		D3D11_VIEWPORT                                   m_shadowViewport;

		// Vector for scene objects
		std::vector<SceneObject*> m_sceneObjects;

		// Light
		std::shared_ptr<PointLight> m_light;

		// Camera
		std::shared_ptr<Camera> m_camera;

		// Variables used with the rendering loop.
		bool	m_loadingComplete;
		float	m_degreesPerSecond;
		bool	m_tracking;
	};
}

