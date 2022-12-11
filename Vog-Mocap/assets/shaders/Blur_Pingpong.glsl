#type vertex
#version 450 core
layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_texCoord;
layout(location = 3) in vec3 a_tangent;

out VertexData
{
    vec3 pos;
    vec2 texCoord;
    vec3 normal;
} v2f;

layout(std140, binding = 0) uniform ub_camera
{
	mat4 u_view;
	mat4 u_projection;
	mat4 u_viewProjection;
};

void main()
{
	v2f.pos = a_position;
	v2f.normal =  a_normal;
	v2f.texCoord = a_texCoord;

	gl_Position = vec4(a_position.xy, 0.0, 1.0);	
}
#type fragment
#version 450 core
			
layout(location = 0) out vec4 o_color;

in VertexData
{
    vec3 pos;
    vec2 texCoord;
    vec3 normal;
} v2f;

layout(binding = 0) uniform sampler2D u_screen_map;

uniform float u_isHorizontal;

// discrete sample
//uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

// linear sample, since gpu will take linear sample while texturing
// offset =  (offset1 * w1 + offset2 * w2) / (w1 + w2)
uniform float offset[3] = float[](0.0, 1.3846153846, 3.2307692308);
uniform float weight[3] = float[](0.2270270270, 0.3162162162, 0.0702702703);

void main()
{
	vec2 tex_offset = 1.0 / textureSize(u_screen_map, 0); // gets size of single texel
    vec3 result = texture(u_screen_map, v2f.texCoord).rgb * weight[0]; // current fragment's contribution

	vec2 currentTexOffset = mix(vec2(tex_offset.x, 0.0), vec2(0.0, tex_offset.y), vec2(u_isHorizontal, u_isHorizontal));

	for(int i = 1; i < 3; ++i)
	{
		result += texture(u_screen_map, v2f.texCoord + currentTexOffset * offset[i]).rgb * weight[i];
		result += texture(u_screen_map, v2f.texCoord - currentTexOffset * offset[i]).rgb * weight[i];
	}
    
    o_color = vec4(result, 1.0);
}