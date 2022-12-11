// Flat Color Shader
#type vertex
#version 450 core
			
layout(location = 0) in vec3 a_position;

out vec3 v_position;

layout(std140, binding = 0) uniform ub_camera
{
	mat4 u_view;
	mat4 u_projection;
	mat4 u_viewProjection;
};
uniform mat4 u_transform;

void main()
{
	v_position = a_position;
	gl_Position = u_viewProjection * u_transform * vec4(a_position, 1.0);	
}

#type fragment
#version 450 core
			
layout(location = 0) out vec4 color;

in vec3 v_position;
uniform vec4 u_color;

void main()
{
	color = u_color;
}
