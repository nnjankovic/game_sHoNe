
#define MAX_LIGHTS 10

struct Light
{
	float3 Strength;
	float FalloffStart; // point/spot light only
	float3 Direction;   // directional/spot light only
	float FalloffEnd;   // point/spot light only
	float3 Position;    // point light only
	float SpotPower;    // spot light only
	float4 ligthType;	// DIRECTIONAL = 0000, POINT = 1000, SPOT = 2000, it is a float4 value because of padding
};

struct Material
{
	float4 DiffuseAlbedo;
	float3 FresnelR0;
	float Roughness;
};


//light strength falls as a function of distance
float CalculateAttenuation(float d, float FalloffEnd, float FalloffStart)
{
	return saturate((FalloffEnd - d) / (FalloffEnd - FalloffStart));
}

float3 ComputeDirectionalLight(Light dirLight, float3 normal)
{
	//We will assume diffuseAlbedo(color of the light reflected by the surface to be {1, 1, 1, 1} for now, until implementation of materials
	float3 diffuseAlbedo = { 1, 1, 1 };

	float3 L = -dirLight.Direction;
	L = normalize(L);
	float3 lightStrength = dirLight.Strength * max(dot(L, normal), 0.0f);

	return diffuseAlbedo * lightStrength;
}

float3 ComputePointLight(Light pointLight, float3 normal, float3 vertexWorldPosition){
	float3 L = pointLight.Position - vertexWorldPosition;

	float distance = length(L);

	if (distance > pointLight.FalloffEnd)
		return 0.0f;
	
	L = normalize(L);
	float3 lightStrength = pointLight.Strength * max(dot(L, normal), 0.0f);
	
	//calculate how much light strength is lost on distance
	float att = CalculateAttenuation(distance, pointLight.FalloffEnd, pointLight.FalloffStart);
	lightStrength *= att;

	return lightStrength;
}

float3 ComputeSpotLight(Light spotLight, float3 normal, float3 vertexWorldPosition) {

	float3 lightStrength = ComputePointLight(spotLight, normal, vertexWorldPosition);

	float3 L = spotLight.Position - vertexWorldPosition;
	float spotFactor = pow(max(dot(-L, spotLight.Direction), 0.0f), spotLight.SpotPower);
	lightStrength *= spotFactor;

	return lightStrength;
}

float4 ComputeLighting(Light gLight[MAX_LIGHTS], int gNumOfLights,
					   float3 normal, float3 vertexWorldPosition,
						Material material)
{
	int i = 0;
	float3 result = 0.0f;
	for (i = 0; i < gNumOfLights; i++)
	{
		if (gLight[i].ligthType.x == 0.0f)
			result += ComputeDirectionalLight(gLight[i], normal);
		else if (gLight[i].ligthType.x == 1.0f)
			result += ComputePointLight(gLight[i], normal, vertexWorldPosition);
		else if (gLight[i].ligthType.x == 2.0f)
			result += ComputeSpotLight(gLight[i], normal, vertexWorldPosition);
	}

	return float4(result, 0.0f);
}