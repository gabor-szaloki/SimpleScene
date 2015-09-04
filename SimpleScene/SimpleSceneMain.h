#pragma once

#include "Common\StepTimer.h"
#include "Common\DeviceResources.h"
#include "Content\SceneRenderer.h"
#include "Content\FpsTextRenderer.h"

// Renders Direct2D and 3D content on the screen.
namespace SimpleScene
{
	class SimpleSceneMain : public DX::IDeviceNotify
	{
	public:
		SimpleSceneMain(const std::shared_ptr<DX::DeviceResources>& deviceResources);
		~SimpleSceneMain();
		void CreateWindowSizeDependentResources();
		void Update();
		bool Render();

		// IDeviceNotify
		virtual void OnDeviceLost();
		virtual void OnDeviceRestored();

	private:
		// Cached pointer to device resources.
		std::shared_ptr<DX::DeviceResources> m_deviceResources;

		// TODO: Replace with your own content renderers.
		std::unique_ptr<SceneRenderer> m_sceneRenderer;
		std::unique_ptr<FpsTextRenderer> m_fpsTextRenderer;

		// Rendering loop timer.
		DX::StepTimer m_timer;
	};
}