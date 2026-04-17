struct VSOutput
{
    float4 position : SV_POSITION;
    float3 texCoord : TEXCOORD;
};

TextureCube cubemap : register(t0);
SamplerState cubemapSampler : register(s0);

float4 main(VSOutput input) : SV_TARGET
{
    return cubemap.Sample(cubemapSampler, input.texCoord);
}