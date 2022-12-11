#type vertex
#version 450 core
layout(location = 0) in vec3 a_worldPosition;
layout(location = 1) in vec4 a_color;

out VertexData
{
    vec3 ws_pos;
    vec4 color;
} v2f;

layout(std140, binding = 0) uniform ub_camera
{
	mat4 u_view;
	mat4 u_projection;
	mat4 u_viewProjection;
};

void main()
{
	v2f.ws_pos = a_worldPosition;
	v2f.color = a_color;
	gl_Position = u_viewProjection * vec4(a_worldPosition, 1.0);	
}

#type fragment
#version 450 core
			
layout(location = 0) out vec4 o_color;

in VertexData
{
    vec3 ws_pos;
    vec4 color;
} v2f;

void main()
{
	o_color = v2f.color;
}