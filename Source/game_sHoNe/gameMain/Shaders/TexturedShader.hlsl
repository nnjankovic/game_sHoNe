
Texture2D    gTexture : register(t0);

SamplerState gsamPointWrap        : register(s0);
SamplerState gsamPointClamp       : register(s1);
SamplerState gsamLinearWrap       : register(s2);
SamplerState gsamLinearClamp      : register(s3);
SamplerState gsamAnisotropicWrap  : register(s4);
SamplerState gsamAnisotropicClamp : register(s5);

cbuffer cbPerObject : register(b0)
{
	float4x4 gWorldViewProj;
	float4x4 gTexTransform;
};

struct VertexIn
{
	float3 PosL  : POSITION;
	float2 TexC  : TEX;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
	float2 TexC	 : TEX;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	// Transform to homogeneous clip space.
	vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);

	// Transform texture.
	vout.TexC = mul(float4(vin.TexC, 0.0f, 1.0f), gTexTransform).xy;

	return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	return gTexture.Sample(gsamAnisotropicWrap, pin.TexC);
}