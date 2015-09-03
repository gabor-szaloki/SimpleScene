#include "pch.h"
#include "Sphere.h"

#include "Common\DirectXHelper.h"

using namespace DirectX;

Sphere::Sphere()
{
}

void Sphere::GenerateMesh(std::shared_ptr<DX::DeviceResources> deviceResources)
{
	float PI;
	XMStoreFloat(&PI, g_XMPi);

	std::vector<VertexPositionNormal> verticesVector;
	std::vector<unsigned short> indicesVector;

	float latitudeBands = 30;
	float longitudeBands = 30;
	float radius = 0.5f;

	for (float latNumber = 0; latNumber <= latitudeBands; latNumber++) {
		float theta = latNumber * PI / latitudeBands;
		float sinTheta = sin(theta);
		float cosTheta = cos(theta);

		for (float longNumber = 0; longNumber <= longitudeBands; longNumber++) {
			float phi = longNumber * 2 * PI / longitudeBands;
			float sinPhi = sin(phi);
			float cosPhi = cos(phi);

			VertexPositionNormal vs;
			vs.normal = XMFLOAT3(cosPhi * sinTheta, cosTheta, sinPhi * sinTheta);
			vs.pos = XMFLOAT3(radius * vs.normal.x, radius * vs.normal.y, radius * vs.normal.z);

			verticesVector.push_back(vs);
		}

		for (int latNumber = 0; latNumber < latitudeBands; latNumber++) {
			for (int longNumber = 0; longNumber < longitudeBands; longNumber++) {
				unsigned short first = (latNumber * (longitudeBands + 1)) + longNumber;
				unsigned short second = first + longitudeBands + 1;

				indicesVector.push_back(first);
				indicesVector.push_back(second);
				indicesVector.push_back(first + 1);

				indicesVector.push_back(second);
				indicesVector.push_back(second + 1);
				indicesVector.push_back(first + 1);

			}
		}
	}

	//int verticesCount = verticesVector.size();
	//static VertexPositionNormal* vertices = new VertexPositionNormal[verticesCount];
	//int indicesCount = indicesVector.size();
	//static unsigned short* indices = new unsigned short[indicesCount];

	//std::copy(verticesVector.begin(), verticesVector.end(), vertices);
	//std::copy(indicesVector.begin(), indicesVector.end(), indices);

	VertexPositionNormal* vertices = &verticesVector[0];
	unsigned short* indices = &indicesVector[0];


	D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
	vertexBufferData.pSysMem = vertices;
	vertexBufferData.SysMemPitch = 0;
	vertexBufferData.SysMemSlicePitch = 0;
	CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(VertexPositionNormal) * verticesVector.size(), D3D11_BIND_VERTEX_BUFFER);
	DX::ThrowIfFailed(
		deviceResources->GetD3DDevice()->CreateBuffer(
			&vertexBufferDesc,
			&vertexBufferData,
			&m_vertexBuffer
			)
		);

	m_indexCount = indicesVector.size();

	D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
	indexBufferData.pSysMem = indices;
	indexBufferData.SysMemPitch = 0;
	indexBufferData.SysMemSlicePitch = 0;
	CD3D11_BUFFER_DESC indexBufferDesc(sizeof(unsigned short) * indicesVector.size(), D3D11_BIND_INDEX_BUFFER);
	DX::ThrowIfFailed(
		deviceResources->GetD3DDevice()->CreateBuffer(
			&indexBufferDesc,
			&indexBufferData,
			&m_indexBuffer
			)
		);
	
}

Sphere::~Sphere()
{
}
