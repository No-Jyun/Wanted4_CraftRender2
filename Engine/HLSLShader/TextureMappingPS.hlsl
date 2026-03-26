struct VSOutput
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD;
    float3 normal : NORMAL;
    float3 cameraPosition : TEXCOORD1;
    float3 worldPosition : TEXCOORD2;
};

// light constant buffer.
cbuffer Light : register(b0)
{
    float3 lightPosition;
    float lightIntensity;
    float3 lightColor;
    float lightPadding;
};

Texture2D map;
SamplerState mapSampler;

float4 main(VSOutput input) : SV_TARGET
{
    float4 diffuseMapColor = map.Sample(mapSampler, input.texCoord);
    
    // light direction.
    //float3 lightDir = normalize(float3(500.0f, 500.0f, -500.0f));
    float3 lightDir = normalize(input.worldPosition - lightPosition);
    
    // world normal.
    float3 worldNormal = normalize(input.normal);
    
    // NdotL.
    float NdotL = dot(worldNormal, -lightDir);
    
    // Specular (Phong-shader).
    float specular = 0.0f;
    if (NdotL > 0)
    {
        // RdotV.
        // R: reflection vector of light direction vector.
        // V: View-Direction Vector.
        float3 reflection = reflect(lightDir, worldNormal);
        
        // View Direction.
        float3 viewDir = normalize(input.worldPosition - input.cameraPosition);
        
        // Half-Vector
        float3 halfVector = normalize(-lightDir + -viewDir);

        // RdotV.
        //float RdotV = max(0, dot(reflection, viewDir));
        //float RdotV = saturate(dot(reflection, -viewDir));
        float NdotH = saturate(dot(halfVector, worldNormal));
        
        // shineness.
        float shineness = 16;
        
        // specular.
        specular = pow(NdotH, shineness);
    }
    
    // Diffuse + Specular + Ambient(Global-Illumination/Local-Illumination).
    
    // final color.
    float4 finalColor = float4(0, 0, 0, 1);
    
    // Diffuse.
    // Half-Lambert.
    NdotL = pow(NdotL * 0.5f + 0.5f, 2);
    float4 diffuse = diffuseMapColor * NdotL;
    
    // Specular.
    float4 specularColor = specular * float4(lightColor, 1);
    
    finalColor = diffuse + specularColor;
    
    //return float4(1.0f, 1.0f, 0.0f, 1.0f);
    //return diffuseMapColor * NdotL;
    //return float4(NdotL, NdotL, NdotL, 1);
    
    //return float4(specular, specular, specular, 1);
    return finalColor;
}