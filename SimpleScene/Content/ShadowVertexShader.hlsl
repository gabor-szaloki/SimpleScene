// A constant buffer that stores the three basic column-major matrices for composing geometry.
cbuffer ShadowVSConstantBuffer : register(b0)
{
	matrix world;
	float4 lightPos;
};

// Per-vertex data used as input to the vertex shader.
struct VertexShaderInput
{
	float3 pos : POSITION;
	float3 normal : NORMAL;
};

// Per-pixel color data passed through the pixel shader.
struct VertexShaderOutput
{
	float4 worldPos : SV_POSITION;
	float3 worldLightDir : Attrib0;
};

// Simple shader to do vertex processing on the GPU.
VertexShaderOutput main(VertexShaderInput input)
{
	VertexShaderOutput output;
	float4 worldPos = mul(float4(input.pos, 1.0f), world);
	float3 worldLightDir = worldPos.xyz - lightPos.xyz;

	output.worldPos = worldPos;
	output.worldLightDir = worldLightDir;

	return output;
}
