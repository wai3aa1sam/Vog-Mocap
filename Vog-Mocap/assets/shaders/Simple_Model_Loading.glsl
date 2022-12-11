#type vertex
#version 450 core
layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_texCoord;
layout(location = 3) in vec3 a_tangent;

out VertexData
{
    vec3 pos;
    vec3 ws_pos;
    vec2 texCoord;
    vec3 normal;
    vec3 tangent;
} v2f;

layout(std140, binding = 0) uniform ub_camera
{
	mat4 u_view;
	mat4 u_projection;
	mat4 u_viewProjection;
};
uniform mat4 u_transform;

void main()
{
	mat3 normal_matrix = transpose(inverse(mat3(u_transform)));

	v2f.pos = a_position;
	v2f.ws_pos = vec3(u_transform * vec4(a_position, 1.0));
	v2f.normal =  normal_matrix * a_normal;
	v2f.texCoord = a_texCoord;
	v2f.tangent = normal_matrix * a_tangent;

	gl_Position = u_viewProjection * vec4(v2f.ws_pos, 1.0);	
}

#type fragment
#version 450 core
			
layout(location = 0) out vec4 color;

in VertexData
{
    vec3 pos;
    vec3 ws_pos;
    vec2 texCoord;
    vec3 normal;
    vec3 tangent;
} v2f;

layout(binding = 0) uniform sampler2D u_albedo_map;
layout(binding = 1) uniform sampler2D u_normal_map;
layout(binding = 2) uniform sampler2D u_specular_map;
layout(binding = 3) uniform sampler2D u_emission_map;

uniform int u_isToogle;
uniform vec4 u_testVec4;

struct test
{
	vec4 vec4Param;
	vec4 vec4Params[4];
	vec3 vec3Param;
	vec3 vec3Params[4];
	vec2 vec2Param;
	vec2 vec2Params[4];
	float intensity;
	float intensities[10];
};
uniform test u_test;
uniform test u_test2;

uniform float u_color_scale;

uniform float u_scale;
uniform vec4 u_color;

void main()
{
	color = vec4(v2f.ws_pos * 0.5 + 0.5, 1.0);
	vec2 uv = v2f.texCoord / u_scale;

	vec4 albedo = texture(u_albedo_map, v2f.texCoord);
	vec4 normal = texture(u_normal_map, uv);
	vec4 specular = texture(u_specular_map, uv);
	vec4 emission = texture(u_emission_map, uv);

	color = vec4(v2f.texCoord.x, v2f.texCoord.x, v2f.texCoord.x, 1.0);
	//color = vec4(v2f.normal * 0.5 + 0.5, 1.0);
	//color = u_test.intensities[9] * u_test.vec4Params[3] * u_test.intensities[8] * u_test.vec4Params[3] * u_test.vec4Params[2];
	//color = color * u_test2.vec4Param; 
	//color = color * u_test2.vec4Param; 

	switch(u_isToogle)
	{
		case 0: color = albedo * emission * u_color_scale; break;
		case 1: color = normal; break;
		case 2: color = specular; break;
	}

	//color *= u_testVec4;
}