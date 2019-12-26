Texture2D    gTexture : register(t0);
SamplerState gSampler : register(s0);

cbuffer cbPerObject : register(b0)
{
	float4x4 gWorldViewProj;
};

struct VertexIn
{
	float3 PosL  : POSITION;
	float2 TexC  : TEXCOORD;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
	float2 TexC	 : TEXCOORD;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	// Transform to homogeneous clip space.
	vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);

	// Just pass vertex color into the pixel shader.
	vout.TexC = vin.TexC;

	return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	return gTexture.Sample(gSampler, pin.TexC);
}