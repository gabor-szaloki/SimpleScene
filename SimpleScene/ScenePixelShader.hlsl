// Per-pixel color data passed to the pixel shader.
struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float3 norm : NORMAL0;
	float3 lightRay : NORMAL1;
	float3 viewRay : NORMAL2;
};

TextureCube shadowMap : register(t0);
SamplerComparisonState shadowSampler : register(s0);
SamplerState linearSampler : register(s1);

float3 DplusS(float3 N, float3 L, float NdotL, float3 view, float distance);

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
	const float4 diffuseColor = float4(1, 1, 1, 1);

	// NdotL for shadow offset, lighting.
	float3 N = normalize(input.norm);
	float3 L = normalize(input.lightRay);
	float distance = length(input.lightRay);
	float NdotL = dot(N, L);


	float pixelDepth = length(input.lightRay);

	float lighting = 1;


	// Use an offset value to mitigate shadow artifacts due to imprecise 
	// floating-point values (shadow acne).
	//
	// This is an approximation of epsilon * tan(acos(saturate(NdotL))):
	float margin = acos(saturate(NdotL));
	// The offset can be slightly smaller with smoother shadow edges.
	float epsilon = 0.000006 / margin;
	// Clamp epsilon to a fixed range so it doesn't go overboard.
	epsilon = clamp(epsilon, 0, 0.1);

	//float shadowMapTexel = shadowMap.Sample(linearSampler, -normalize(input.lightRay)).r;
	//lighting = abs(shadowMapTexel - pixelDepth);

	lighting = float(shadowMap.SampleCmpLevelZero(
		shadowSampler,
		-normalize(input.lightRay),
		pixelDepth/30.0f + epsilon
		)
		);

	if (lighting == 0.f)
	{
		return float4(diffuseColor * ambient, 1.f);
	}
	else if (lighting < 1.0f)
	{
		// Blends the shadow area into the lit area.
		float3 light = lighting * (ambient + DplusS(N, L, NdotL, input.viewRay, distance));
		float3 shadow = (1.0f - lighting) * ambient;
		return float4(diffuseColor * (light + shadow), 1.f);
	}
	

	float4 finalColor = float4(diffuseColor * (ambient + DplusS(N, L, NdotL, input.viewRay, distance)), 1.f);
	return finalColor;
	//return float4(1,1,1,1);
}

// Performs very basic Phong lighting for example purposes.
float3 DplusS(float3 N, float3 L, float NdotL, float3 view, float distance)
{
	const float3 Kdiffuse = float3(.8f, .8f, .8f);
	const float3 Kspecular = float3(.2f, .2f, .2f);
	const float exponent = 6.f;

	// Compute the diffuse coefficient.
	float diffuseConst = saturate(NdotL);

	// Compute the diffuse lighting value.
	float distanceCoeficient = saturate(2.f / distance);
	float3 diffuse = saturate(Kdiffuse * diffuseConst * distanceCoeficient);

	// Compute the specular highlight.
	float3 R = reflect(-L, N);
	float3 V = normalize(view);
	float3 RdotV = dot(R, V);
	float3 specular = Kspecular * pow(saturate(RdotV), exponent);

	return (diffuse + specular);
}