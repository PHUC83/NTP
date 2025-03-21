  #include "skinned_mesh.hlsli"
#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
SamplerState sampler_states[3] : register(s0);
Texture2D texture_maps[4] : register(t0);
float4 main(VS_OUT pin) : SV_TARGET
{
    float4 color = texture_maps[0].Sample(sampler_states[ANISOTROPIC], pin.texcoord);
    float3 N = normalize(pin.world_normal.xyz);
    float3 L = normalize(-light_direction.xyz);
    float3 diffuse = color.rgb * sun_color * max(0, dot(N, L)) * sun_intensity;
    float3 ambient = color.rgb * 0.5f; 
    float3 final_color = ambient + diffuse;
    
    return float4(final_color, color.a) * pin.color;
}
