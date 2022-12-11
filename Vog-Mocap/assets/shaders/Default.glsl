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
	v2f.ws_pos = (u_transform * vec4(a_position, 1.0)).xyz;
	v2f.normal =  normal_matrix * a_normal;
	v2f.texCoord = a_texCoord;
	v2f.tangent = normal_matrix * a_tangent;

	gl_Position = u_viewProjection * vec4(v2f.ws_pos, 1.0);	
}

#type fragment
#version 450 core
			
layout(location = 0) out vec4 o_color;

in VertexData
{
    vec3 pos;
    vec3 ws_pos;
    vec2 texCoord;
    vec3 normal;
    vec3 tangent;
} v2f;

void main()
{
	o_color = vec4(1, 0, 1, 1);
}