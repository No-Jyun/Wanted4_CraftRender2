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
    // !!!!!!!!.
    float padding;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD;
    float3 normal : NORMAL;
    float3 cameraPosition : TEXCOORD1;
    float3 worldPosition : TEXCOORD2;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
};

VSOutput main(VSInput input)
{
    VSOutput output;
    //output.position = float4(input.position, 1);
    output.position = mul(float4(input.position, 1), world);
    output.worldPosition = output.position.xyz;
    
    output.position = mul(output.position, camera);
    output.texCoord = input.texCoord;
    
    output.normal = normalize(mul(input.normal, (float3x3) world));
    output.tangent = normalize(mul(input.tangent, (float3x3) world));
    output.bitangent = normalize(mul(input.bitangent, (float3x3) world));
    
    output.cameraPosition = cameraPosition;

    return output;
}