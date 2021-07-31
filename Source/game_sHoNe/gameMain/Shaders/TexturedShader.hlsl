
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
}

struct VertexIn
{
	float3 PosL  : POSITION;
	float2 TexC  : TEX;
};

struct VertexOut
{
	//Homogeneous coordinates (projected on 2d screen)
	float4 PosH  : SV_POSITION;
	//Vertex world position
	float3 PosW	 : POSITION;
	float2 TexC	 : TEX;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout = (VertexOut)0.0f;

	float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);
	// Transform to world clip space.
	vout.PosW = posW.xyz;

	float4x4 viewProj = mul(gCameraViewMatrix, gProjectionMatrix);
	vout.PosH = mul(posW, viewProj);

	// Transform texture.
	vout.TexC = mul(float4(vin.TexC, 0.0f, 1.0f), gTexTransform).xy;

	return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	return gTexture.Sample(gsamAnisotropicWrap, pin.TexC);
}