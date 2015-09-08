// Constant buffer that stores the 3 transformation matrices and the camera position
cbuffer SceneVSConstantBuffer : register(b0)
{
	matrix world;
	matrix view;
	matrix projection;
	float4 lightPos;
	float4 cameraPos;
};

// Per-vertex data used as input to the vertex shader.
struct VertexShaderInput
{
	float3 pos : POSITION;
	float3 norm : NORMAL0;
};

// Per-pixel color data passed to the pixel shader.
struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float3 norm : NORMAL0;
	float3 lightRay : NORMAL1;
	float3 viewRay : NORMAL2;
};

// Shader to do vertex processing for camera view position and light view position.
PixelShaderInput main(VertexShaderInput input)
{
	PixelShaderInput output;
	float4 pos = float4(input.pos, 1.0f);

	// Transform the vertex position into projected space.
	float4 worldPos = mul(pos, world);
	pos = mul(worldPos, view);
	pos = mul(pos, projection);
	output.pos = pos;

	// Light ray
	float3 lightRay = lightPos.xyz - worldPos.xyz;
	output.lightRay = lightRay;

	// Camera ray
	output.viewRay = cameraPos.xyz - worldPos.xyz;

	// Transform the vertex normal into world space.
	output.norm = mul(input.norm, world);

	return output;
}