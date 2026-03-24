struct VSOutput
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD;
    float3 normal : NORMAL;
    float3 cameraPosition : TEXCOORD1;
};

Texture2D diffuseMap : register(t0);
SamplerState diffuseSampler : register(s0);

float4 main(VSOutput input) : SV_TARGET
{
    // Sampling...
    float4 diffuseColor = diffuseMap.Sample(diffuseSampler, input.texCoord);
    
    // light direction (hard-coded and normalized)
    float3 lightDir = normalize(float3(500.0f, 500.0f, -500.0f));
    
    // world normal
    float3 worldNormal = normalize(input.normal);
    
    // NdotL (Lambert)
    float NdotL = dot(worldNormal, lightDir);
    
    // Half-Lambert
    NdotL = pow(NdotL * 0.5f + 0.5f, 2);
    
    // Specular (Phong-Shader)
    float specular = 0.0f;
    if(NdotL > 0)
    {
        // RdotV
        // R : Reflection Vector of Light Direction vector
        // V : View-Direction Vector
        float3 reflection = reflect(lightDir, worldNormal);
        
        // View Direction
        float3 viewDir = input.cameraPosition - input.position.xyz;
        viewDir = normalize(viewDir);
        
        // RdotV
        //float RdotV = max(0, dot(reflection, viewDir));
        float RdotV = saturate(dot(reflection, viewDir));
        
        float shineness = 16;

        // specular
        specular = pow(RdotV, shineness);
    }
    
    // Diffuse + Specular + Ambient (Global-Illumination / Local-Illumination)

    // final color
    float4 finalColor = float4(0, 0, 0, 1);
    
    // Diffuse 
    float4 diffuse = diffuseColor * NdotL;
    
    // Specular
    float4 specularColor = specular * diffuseColor;
    
    finalColor = diffuse + specularColor;
    
	//return float4(1.0f, 0.0f, 0.0f, 1.0f);
    //return diffuseColor * NdotL;
    //return float4(NdotL, NdotL, NdotL, 1);
    
    //return float4(specular, specular, specular, 1);
    return finalColor;
}