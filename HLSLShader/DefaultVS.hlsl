struct VSInput
{
    float3 position : POSITION;
    float2 texCoord : TEXCOORD;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD;
};

VSOutput main(VSInput input) 
{
    VSOutput output;
    output.position = float4(input.position, 1);
    output.texCoord = input.texCoord;
    
    return output;
}