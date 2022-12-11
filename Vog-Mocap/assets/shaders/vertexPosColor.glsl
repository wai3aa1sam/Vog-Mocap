#type vertex
#version 450 core
			
layout(location = 0) in vec3 a_position;
layout(location = 1) in vec4 a_color;

out vec3 v_position;
out vec4 v_color;

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
	v_color = a_color;
	gl_Position = u_viewProjection * u_transform * vec4(a_position, 1.0);	
}

#type fragment
#version 450 core
			
layout(location = 0) out vec4 color;

in vec3 v_position;
in vec4 v_color;

void main()
{
	color = v_color;
}