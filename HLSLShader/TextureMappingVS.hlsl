struct VSInput
{
    float3 position : POSITION;
    float2 texCoord : TEXCOORD;
    float3 normal : NORMAL;
};

// Constant Buffer (World Matrix)
// Local to World
// 1 - (ad - bc)
cbuffer Transform : register(b0)
{
    matrix worldMatrix;
}

// World to View, View to Projection
cbuffer Camera : register(b1)
{
    matrix cameraMatrix;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD;
    float3 normal : NORMAL;
};

VSOutput main(VSInput input)
{
    VSOutput output;
    //output.position = float4(input.position, 1);
    output.position = mul(float4(input.position, 1), worldMatrix);
    output.position = mul(output.position, cameraMatrix);
    
    output.texCoord = input.texCoord;
    
    // transform local normal to world normal
    output.normal = mul(input.normal, (float3x3) worldMatrix);
    
    return output;
}