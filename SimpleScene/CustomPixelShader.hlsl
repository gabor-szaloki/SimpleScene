// A constant buffer that stores the three basic column-major matrices for composing geometry.
cbuffer MyConstantBuffer : register(b0)
{
	matrix world;
	matrix view;
	matrix projection;
	float4 lightPosition;
	float4 lightColor;
	float4 cameraPosition;
};

// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float3 worldPos : POSITION;
	float3 normal : NORMAL;
};

float4 ComputeIllumination(float3 vLightTS, float3 vViewTS, float3 vNormalWS)
{
	// Sample the normal from the normal map for the given texture sample:
	float3 vNormalTS = normalize(vNormalWS);

	// Setting base color
	float4 cBaseColor = float4(0.8, 0.8, 0.8, 1);

	// Compute ambient component
	float ambientPower = 0.05f;
	float4 ambientColor = float4(1, 1, 1, 1);
	float4 cAmbient = ambientColor * ambientPower;

	// Compute diffuse color component:
	float distance = saturate(3 / length(vLightTS));
	float4 cDiffuse = saturate(dot(vNormalTS, normalize(vLightTS) * distance));

	// Compute the specular component if desired:
	float3 H = normalize(vLightTS + vViewTS);
	float NdotH = max(0, dot(vNormalTS, H));
	float shininess = 30;
	float specularPower = 0.1;
	float4 specularColor = float4(1, 1, 1, 1);
	float4 cSpecular = pow(saturate(NdotH), shininess) * specularColor * specularPower;

	// Composite the final color:
	float4 cFinalColor = (cAmbient + cDiffuse) * cBaseColor + cSpecular;

	return cFinalColor;
}

// A pass-through function for the (interpolated) color data.
float4 main(PixelShaderInput input) : SV_TARGET
{
	return ComputeIllumination(lightPosition.xyz - input.worldPos, cameraPosition - input.worldPos, input.normal);
	//return lightColor * dot(normalize(lightPosition.xyz - input.worldPos), input.normal);
}
