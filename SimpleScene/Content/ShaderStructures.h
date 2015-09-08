#pragma once

namespace SimpleScene
{
	// Constant buffer used to send MVP matrices to the vertex shader.
	struct ModelViewProjectionConstantBuffer
	{
		DirectX::XMFLOAT4X4 model;
		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT4X4 projection;
	};

	struct WorldViewProjEyeConstantBuffer
	{
		DirectX::XMFLOAT4X4 world;
		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT4X4 projection;
		DirectX::XMFLOAT4 cameraPosition;
	};

	struct ViewProjectionConstantBuffer
	{
		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT4X4 projection;
		DirectX::XMFLOAT4   pos;
	};

	struct LightViewsProjectionConstantBuffer
	{
		DirectX::XMFLOAT4X4 views[6];
		DirectX::XMFLOAT4X4 projection;
		DirectX::XMFLOAT4   pos;
	};

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
	struct VertexPositionColor
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 color;
	};

	struct VertexPositionNormal
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 normal;
	};
}