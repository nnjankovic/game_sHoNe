
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

//The Fresnel equations mathematically describe the percentage of incoming light that is reflected
//Rf(i) = Rf(0) + (1 - Rf(0))*(1 - dot(n*L))^5
float3 SchlickFresnel(float3 R0, float3 normal, float3 lightVec)
{
	float cosIncidentAngle = saturate(dot(normal, lightVec));

	float f0 = 1.0f - cosIncidentAngle;
	float3 reflectPercent = R0 + (1.0f - R0)*(f0*f0*f0*f0*f0);

	return reflectPercent;
}

//[max(L*n, 0)*Bl]* (md + Rf* (m+8)/8*(n*h)^m
//[max(L*n, 0)*Bl] - lightStrength (calculated prior to calling this function
//L - light vector (from surface to light) L = -I
//Bl - lightStrength (R,G,B)
//md - diffuse albedo (practically a color of a pixel without light - usually we get this color from texture)
//Rf - SchlickFresnel aprox
//m - property of a medium, roughness factor (smooth surface larger m, rough surface smaller m)
//n - normal vector
//h - halfVector = L + toEyeVector
float3 BlinnPhong(float3 lightStrength, float3 lightVec, float3 normal, float3 toEye, Material mat)
{
	const float m = (1 - mat.Roughness) * 256.0f; //find out why multiply with 256 here
	float3 h = normalize(toEye + lightVec);

	float3 fresnelFactor = SchlickFresnel(mat.FresnelR0, normal, lightVec);
	float roughnessFactor = (m + 8.0f)*pow(max(dot(h, normal), 0.0f), m) / 8.0f;

	float3 specularAlbedo = fresnelFactor * roughnessFactor;
	specularAlbedo = specularAlbedo / (specularAlbedo + 1.0f); //scale down

	return (mat.DiffuseAlbedo.rgb + specularAlbedo)*lightStrength;
}

float3 ComputeDirectionalLight(Light dirLight, float3 normal)
{
	float3 L = -dirLight.Direction;
	L = normalize(L);

	//max(L*n, 0) * Bl
	float3 lightStrength = dirLight.Strength * max(dot(L, normal), 0.0f);

	return lightStrength;
}

float3 ComputePointLight(Light pointLight, float3 normal, float3 vertexWorldPosition, float3 toEye, Material material) {
	float3 L = pointLight.Position - vertexWorldPosition;

	float distance = length(L);

	if (distance > pointLight.FalloffEnd)
		return 0.0f;
	
	L = normalize(L);

	//max(L*n, 0) * Bl
	float3 lightStrength = pointLight.Strength * max(dot(L, normal), 0.0f);
	
	//calculate how much light strength is lost on distance
	float att = CalculateAttenuation(distance, pointLight.FalloffEnd, pointLight.FalloffStart);
	//lightStrength is a function of distance since this is a point light
	lightStrength *= att; 

	return BlinnPhong(lightStrength, L, normal, toEye, material);
}

float3 ComputeSpotLight(Light spotLight, float3 normal, float3 vertexWorldPosition, float3 toEye, Material material) {

	float3 L = spotLight.Position - vertexWorldPosition;

	float distance = length(L);

	if (distance > spotLight.FalloffEnd)
		return 0.0f;

	L = normalize(L);

	//max(L*n, 0) * Bl
	float3 lightStrength = spotLight.Strength * max(dot(L, normal), 0.0f);

	//calculate how much light strength is lost on distance
	float att = CalculateAttenuation(distance, spotLight.FalloffEnd, spotLight.FalloffStart);
	//lightStrength is a function of distance since this is a point light
	lightStrength *= att;

	float spotFactor = pow(max(dot(-L, spotLight.Direction), 0.0f), spotLight.SpotPower);
	//max(L*n, 0) * Bl, Bl (light strength) changes depending on position because this is a spotlight
	lightStrength *= spotFactor; 

	return BlinnPhong(lightStrength, L, normal, toEye, material);
}

float4 ComputeLighting(Light gLight[MAX_LIGHTS], int gNumOfLights,
					   float3 normal, float3 vertexWorldPosition,
						Material material, float3 toEye)
{
	int i = 0;
	float3 result = 0.0f;
	for (i = 0; i < gNumOfLights; i++)
	{
		if (gLight[i].ligthType.x == 0.0f)
			result += ComputeDirectionalLight(gLight[i], normal);
		else if (gLight[i].ligthType.x == 1.0f)
			result += ComputePointLight(gLight[i], normal, vertexWorldPosition, toEye, material);
		else if (gLight[i].ligthType.x == 2.0f)
			result += ComputeSpotLight(gLight[i], normal, vertexWorldPosition, toEye, material);
	}

	return float4(result, 0.0f);
}