cbuffer MyConstantBuffer : register(b0)
{
	matrix world;
	matrix view;
	matrix projection;
	float4 lightPosition;
	float4 lightColor;
	float4 eyePos;
};

cbuffer ViewProjectionConstantBuffer : register(b1)
{
	matrix lView;
	matrix lProjection;
	float4 lPos; // eye position
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
	float3 color : COLOR0;
	float4 lightSpacePos : POSITION1;
	float3 norm : NORMAL0;
	float3 lRay : NORMAL1;
	float3 view : NORMAL2;
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

	// Transform the vertex position into projected space from the POV of the light.
	float4 lightSpacePos = mul(worldPos, lView);
	lightSpacePos = mul(lightSpacePos, lProjection);
	output.lightSpacePos = lightSpacePos;

	// Light ray
	float3 lRay = lPos.xyz - worldPos.xyz;
	output.lRay = lRay;

	// Camera ray
	output.view = eyePos.xyz - worldPos.xyz;

	// Transform the vertex normal into world space.
	//float4 norm = float4(input.norm, 1.0f);
	//norm = mul(norm, world);
	//output.norm = norm.xyz;
	output.norm = mul(input.norm, world);

	output.color = float3(1, 1, 1);

	return output;
}