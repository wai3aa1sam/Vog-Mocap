#type vertex
#version 450 core
layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_texCoord;
layout(location = 3) in vec3 a_tangent;

layout(std140, binding = 0) uniform ub_camera
{
	mat4 u_view;
	mat4 u_projection;
	mat4 u_viewProjection;
    mat4 u_lightSpace_matrix;
};
uniform mat4 u_transform;

void main()
{
	gl_Position = u_lightSpace_matrix * u_transform * vec4(a_position, 1.0);	
}

#type fragment
#version 450 core

void main()
{
}