 #include "geometric_primitive.hlsli" 

VS_OUT main(float4 position : POSITION, float4 normal : NORMAL) 
{ 
    VS_OUT vout;
    vout.position = mul(position, mul(world, view_projection)); 
    
    normal.w = 0; 
 
    //float4 N = normalize(mul(normal, world_inverse_transpose));
    float4 N = normalize(normal);
    float4 L = normalize(-light_direction);
     
    vout.color.rgb = material_color.rgb * max(0, dot(L, N)); 
    vout.color.a = material_color.a;
    return vout;
}