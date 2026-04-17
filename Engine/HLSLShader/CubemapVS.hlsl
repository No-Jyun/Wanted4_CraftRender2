struct VSInput
{
    float3 position : POSITION;
    float2 texCoord : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
};

cbuffer Transform : register(b0)
{
    matrix world;
};

// Constant-buffer must be aligned by 16bytes..
cbuffer Camera : register(b1)
{
    matrix camera;
    float3 cameraPosition;
    float padding;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float3 texCoord : TEXCOORD;
};

VSOutput main(VSInput input)
{
    VSOutput output;
    //output.position = float4(input.position, 1);
    output.position = mul(float4(input.position, 1), world);
    output.position = mul(output.position, camera);
    
    output.texCoord = input.position;

    return output;
}