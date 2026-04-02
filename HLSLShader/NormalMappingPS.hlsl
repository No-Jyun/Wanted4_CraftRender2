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

// light constant buffer.
cbuffer Light : register(b0)
{
    float3 lightPosition;
    float lightIntensity;
    float3 lightColor;
    float lightPadding;
};

Texture2D diffuseMap : register(t0);
Texture2D specularMap : register(t1);
Texture2D normalMap : register(t2);
SamplerState mapSampler : register(s0);

float4 main(VSOutput input) : SV_TARGET
{
    float4 diffuseMapColor = diffuseMap.Sample(mapSampler, input.texCoord);
    float4 specularMapColor = specularMap.Sample(mapSampler, input.texCoord);
    
    // tangent normal.
    float4 tangentNormal = normalMap.Sample(mapSampler, input.texCoord);
    // !!!!!!!!!.
    tangentNormal = tangentNormal * 2 - 1;
    
    // light direction.
    //float3 lightDir = normalize(float3(500.0f, 500.0f, -500.0f));
    float3 lightDir = normalize(input.worldPosition - lightPosition);
    
    // world normal.
    //float3 worldNormal = normalize(input.normal);
    // tangent normal to world transformation matrix.
    float3x3 tangentToWorld = float3x3(
        normalize(input.tangent),
        normalize(input.bitangent),
        normalize(input.normal)
    );
    
    // world normal.
    float3 worldNormal = normalize(mul(tangentNormal.xyz, tangentToWorld));
    
    // NdotL.
    float NdotL = dot(worldNormal, -lightDir);
    
    // Specular ((Vanila)Phong-shader).
    float3 specular = 0.0f;
        
    // View Direction.
    float3 viewDir = normalize(input.worldPosition - input.cameraPosition);
        
    if (NdotL > 0)
    {
        // RdotV.
        // R: reflection vector of light direction vector.
        // V: View-Direction Vector.
        //float3 reflection = reflect(lightDir, worldNormal);
        // Half-Vector.
        float3 halfVector = normalize(-lightDir + -viewDir);

        // RdotV.
        //float RdotV = max(0, dot(reflection, viewDir));
        //float RdotV = saturate(dot(reflection, -viewDir));
        float NdotH = saturate(dot(halfVector, worldNormal));
        
        // shineness.
        float shineness = 64;
        
        // specular.
        //specular = pow(RdotV, shineness);
        specular = specularMapColor.rgb * pow(NdotH, shineness);
    }
    
    // Rim Lighting
    float NdotV = saturate(dot(worldNormal, -viewDir));
    float rimAmount = 0.8f;
    float rimConstant = 2.0f;   // control rim effect smoothness
    float rimWidth = 0.2f;
    
    // rim intensity
    float rimIntensity = smoothstep(rimAmount - rimWidth, rimAmount + rimWidth, 1 - NdotV);
    rimIntensity = pow(rimIntensity, rimConstant) * saturate(NdotL);
    
    // Diffuse + Specular + Ambient(Global-Illumination/Local-Illumination).
    
    // final color.
    float4 finalColor = float4(0, 0, 0, 1);
    
    // Diffuse.
    // Half-Lambert.
    NdotL = pow(NdotL * 0.5f + 0.5f, 2);
    float4 diffuse = diffuseMapColor * NdotL;
    
    // Specular.
    float4 specularColor = float4(specular, 1) * float4(lightColor, 1);
    
    finalColor = diffuse + specularColor + float4(float3(1.0f, 0.0f, 0.0f) * rimIntensity, 1.0f);
    
    //return float4(1.0f, 1.0f, 0.0f, 1.0f);
    //return diffuseMapColor * NdotL;
    //return float4(NdotL, NdotL, NdotL, 1);
    
    //return float4(specular, 1);
    return finalColor;
}