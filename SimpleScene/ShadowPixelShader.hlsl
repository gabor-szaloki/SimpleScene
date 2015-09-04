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

Texture2D shadowMap : register(t0);
SamplerComparisonState shadowSampler : register(s0);

float3 DplusS(float3 N, float3 L, float NdotL, float3 view, float distance);
float4 ComputeIllumination(float3 vLightTS, float3 vViewTS, float3 vNormalWS, float distance);

// Draw a shadow on top of an interpolated color.
//
// Takes the computed position of each pixel relative to the light source 
// and then compares the z value of the pixel with the value in the shadow
// buffer to determine whether that pixel is in shadow.
//
// Also applies Phong shading and hides z-fighting on self-shadowed surfaces.
// Draw a shadow on top of an interpolated color.
float4 main(PixelShaderInput input) : SV_TARGET
{
	const float3 ambient = float3(0.1f, 0.1f, 0.1f);

	// NdotL for shadow offset, lighting.
	float3 N = normalize(input.norm);
	float3 L = normalize(input.lRay);
	float distance = length(input.lRay);
	float NdotL = dot(N, L);

	// Compute texture coordinates for the current point's location on the shadow map.
	float2 shadowTexCoords;
	shadowTexCoords.x = 0.5f + (input.lightSpacePos.x / input.lightSpacePos.w * 0.5f);
	shadowTexCoords.y = 0.5f - (input.lightSpacePos.y / input.lightSpacePos.w * 0.5f);
	float pixelDepth = input.lightSpacePos.z / input.lightSpacePos.w;

	float lighting = 1;

	// Check if the pixel texture coordinate is in the view frustum of the 
	// light before doing any shadow work.
	if ((saturate(shadowTexCoords.x) == shadowTexCoords.x) &&
		(saturate(shadowTexCoords.y) == shadowTexCoords.y) &&
		(pixelDepth > 0))
	{
		// Use an offset value to mitigate shadow artifacts due to imprecise 
		// floating-point values (shadow acne).
		//
		// This is an approximation of epsilon * tan(acos(saturate(NdotL))):
		float margin = acos(saturate(NdotL));
		// The offset can be slightly smaller with smoother shadow edges.
		float epsilon = 0.0005 / margin;
		// Clamp epsilon to a fixed range so it doesn't go overboard.
		epsilon = clamp(epsilon, 0, 0.1);

		// Use the SampleCmpLevelZero Texture2D method (or SampleCmp) to sample from 
		// the shadow map, just as you would with Direct3D feature level 10_0 and
		// higher.  Feature level 9_1 only supports LessOrEqual, which returns 0 if
		// the pixel is in the shadow.
		lighting = float(shadowMap.SampleCmpLevelZero(
			shadowSampler,
			shadowTexCoords,
			pixelDepth + epsilon
			)
			);

		if (lighting == 0.f)
		{
			return float4(input.color * ambient, 1.f);
		}
		else if (lighting < 1.0f)
		{
			// Blends the shadow area into the lit area.
			float3 light = lighting * (ambient + DplusS(N, L, NdotL, input.view, distance));
			float3 shadow = (1.0f - lighting) * ambient;
			return float4(input.color * (light + shadow), 1.f);
		}
	}

	//return float4(input.color * (ambient + DplusS(N, L, NdotL, input.view, distance)), 1.f);
	float4 finalColor = ComputeIllumination(L, input.view, N, distance);
	return finalColor;
}

// Performs very basic Phong lighting for example purposes.
float3 DplusS(float3 N, float3 L, float NdotL, float3 view, float distance)
{
	const float3 Kdiffuse = float3(.5f, .5f, .4f);
	const float3 Kspecular = float3(.2f, .2f, .3f);
	const float exponent = 3.f;

	// Compute the diffuse coefficient.
	float diffuseConst = saturate(NdotL);

	// Compute the diffuse lighting value.
	float distanceCoeficient = saturate(3 / distance);
	float3 diffuse = saturate(Kdiffuse * diffuseConst * distanceCoeficient);

	// Compute the specular highlight.
	float3 R = reflect(-L, N);
	float3 V = normalize(view);
	float3 RdotV = dot(R, V);
	float3 specular = Kspecular * pow(saturate(RdotV), exponent);

	return (diffuse + specular);
}

float4 ComputeIllumination(float3 vLightTS, float3 vViewTS, float3 vNormalWS, float distance)
{
	// Sample the normal from the normal map for the given texture sample:
	float3 vNormalTS = normalize(vNormalWS);

	// Setting base color
	float4 cBaseColor = float4(0.8, 0.8, 0.8, 1);

	// Compute ambient component
	float ambientPower = 0.1f;
	float4 ambientColor = float4(1, 1, 1, 1);
	float4 cAmbient = ambientColor * ambientPower;

	// Compute diffuse color component:
	float distanceCoefficient = saturate(3 / distance);
	float4 cDiffuse = saturate(dot(vNormalTS, normalize(vLightTS) * distanceCoefficient));

	// Compute the specular component if desired:
	float3 H = normalize(vLightTS + vViewTS);
	float NdotH = max(0, dot(vNormalTS, H));
	float shininess = 50;
	float specularPower = 0.1;
	float4 specularColor = float4(1, 1, 1, 1);
	float4 cSpecular = pow(saturate(NdotH), shininess) * specularColor * specularPower;

	// Composite the final color:
	float4 cFinalColor = (cAmbient + cDiffuse) * cBaseColor + cSpecular;

	return cFinalColor;
}