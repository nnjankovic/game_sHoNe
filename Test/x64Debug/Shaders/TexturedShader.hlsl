
#include "Lighting.hlsl"

Texture2D    gTexture : register(t0);

SamplerState gsamPointWrap        : register(s0);
SamplerState gsamPointClamp       : register(s1);
SamplerState gsamLinearWrap       : register(s2);
SamplerState gsamLinearClamp      : register(s3);
SamplerState gsamAnisotropicWrap  : register(s4);
SamplerState gsamAnisotropicClamp : register(s5);

cbuffer cbPerObject : register(b0)
{
	//Transform to world space
	float4x4 gWorld;
	
	float4x4 gTexTransform;
};

cbuffer cameraConstants : register(b1)
{
	//Transform to camera space
	float4x4 gCameraViewMatrix;
	
	//Projection matrix from camera view to 2d screen
	float4x4 gProjectionMatrix;
};

cbuffer lights : register(b2)
{
	float4 gAmbientLight;
	Light gLights[MAX_LIGHTS];
	int gNumOfLights;
};

cbuffer passConstants : register(b3)
{
	float3 gEyePosW;
};

cbuffer material : register(b4)
{
	float4   gDiffuseAlbedo;
	float3   gFresnelR0;
	float    gRoughness;
	//maybe I need to add matTransform matrix
};

struct VertexIn
{
	float3 PosL		: POSITION;
	float2 TexC		: TEX;
	//Normal local position
	float3 NormalL	: NORMAL;
};

struct VertexOut
{
	//Homogeneous coordinates (projected on 2d screen)
	float4 PosH		: SV_POSITION;
	//Vertex world position
	float3 PosW		: POSITION;
	//Normal world position
	float3 NormalW	: NORMAL;
	float2 TexC		: TEX;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout = (VertexOut)0.0f;

	float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);
	// Transform to world clip space.
	vout.PosW = posW.xyz;

	float4x4 viewProj = mul(gCameraViewMatrix, gProjectionMatrix);
	vout.PosH = mul(posW, viewProj);

	// Assumes nonuniform scaling; otherwise, need to use inverse-transpose of world matrix.
	vout.NormalW = mul(vin.NormalL, (float3x3)gWorld);

	// Transform texture.
	vout.TexC = mul(float4(vin.TexC, 0.0f, 1.0f), gTexTransform).xy;

	return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	// Interpolating normal can unnormalize it, so renormalize it.
	pin.NormalW = normalize(pin.NormalW);

	float4 diffuseAlbedo = gTexture.Sample(gsamAnisotropicWrap, pin.TexC)*gDiffuseAlbedo;

#ifdef ALPHA_TEST
	// Discard pixel if texture alpha < 0.1.  We do this test as soon 
	// as possible in the shader so that we can potentially exit the
	// shader early, thereby skipping the rest of the shader code.
	clip(diffuseAlbedo.a - 0.1f);
#endif

	float4 ambient = diffuseAlbedo*gAmbientLight;

	float3 toEye = normalize(gEyePosW - pin.PosW);
	Material material = { diffuseAlbedo, gFresnelR0, gRoughness };

	float4 lightAggregate = ComputeLighting(gLights, gNumOfLights, pin.NormalW, pin.PosW, material, toEye);

	//add ambient light
	float4 litColor = ambient + lightAggregate;
	litColor.a = diffuseAlbedo.a;

	return litColor; //applying ambient light
}