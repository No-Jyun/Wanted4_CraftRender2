struct VSOutput
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD;
};

Texture2D diffuseMap : register(t0);
SamplerState diffuseSampler : register(s0);

float4 main(VSOutput input) : SV_TARGET
{
    // Sampling...
    float4 diffuseColor = diffuseMap.Sample(diffuseSampler, input.texCoord);
    
	//return float4(1.0f, 0.0f, 0.0f, 1.0f);
    return diffuseColor;
}