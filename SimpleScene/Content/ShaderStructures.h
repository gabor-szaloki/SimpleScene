#pragma once

namespace SimpleScene
{
	// Constant buffers used to send data to the vertex shaders.
	struct ShadowVSConstantBuffer
	{
		DirectX::XMFLOAT4X4 world;
		DirectX::XMFLOAT4   lightPos;
	};

	struct ShadowGSConstantBuffer
	{
		DirectX::XMFLOAT4X4 lightViews[6];
		DirectX::XMFLOAT4X4 projection;
	};

	struct SceneVSConstantBuffer
	{
		DirectX::XMFLOAT4X4 world;
		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT4X4 projection;
		DirectX::XMFLOAT4   lightPos;
		DirectX::XMFLOAT4   cameraPosition;
	};

	// Used to send per-vertex data to the vertex shader.
	struct VertexPositionNormal
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 normal;
	};
}