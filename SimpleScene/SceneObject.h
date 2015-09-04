#pragma once

#include "Common\DeviceResources.h"
#include "Content\ShaderStructures.h"

#include <vector>

using namespace SimpleScene;
using namespace DirectX;

class SceneObject
{
public:
	SceneObject();

	virtual void Draw(std::shared_ptr<DX::DeviceResources> deviceResources);
	virtual void DrawDepthMap(std::shared_ptr<DX::DeviceResources> deviceResources);

	virtual void LoadVS(
		std::shared_ptr<DX::DeviceResources> deviceResources,
		const std::vector<byte>& fileData);
	virtual void LoadDepthVS(
		std::shared_ptr<DX::DeviceResources> deviceResources,
		const std::vector<byte>& fileData);
	virtual void LoadPS(
		std::shared_ptr<DX::DeviceResources> deviceResources,
		const std::vector<byte>& fileData);
	virtual void LoadCB(
		std::shared_ptr<DX::DeviceResources> deviceResources);
	virtual void GenerateMesh(std::shared_ptr<DX::DeviceResources> deviceResources) = 0;

	~SceneObject();

	WorldViewProjEyeConstantBuffer m_constantBufferData;

protected:
	// Direct3D resources for geometry.
	Microsoft::WRL::ComPtr<ID3D11InputLayout>	     m_inputLayout;
	Microsoft::WRL::ComPtr<ID3D11Buffer>		     m_vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>		     m_indexBuffer;
	Microsoft::WRL::ComPtr<ID3D11VertexShader>	     m_vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>	     m_pixelShader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader>	     m_depthVertexShader;
	Microsoft::WRL::ComPtr<ID3D11Buffer>		     m_constantBuffer;

	// System resources for geometry.
	uint32	m_indexCount;

};

