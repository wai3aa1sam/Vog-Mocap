#type vertex
#version 450 core
			
layout(location = 0) in vec3 a_position;
layout(location = 1) in vec4 a_color;
layout(location = 2) in vec2 a_texCoord;

out vec3 v_position;
out vec4 v_color;
out vec2 v_texCoord;

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
	v_texCoord = a_texCoord;

	gl_Position = u_viewProjection * u_transform * vec4(a_position, 1.0);	
}

#type fragment
#version 450 core
layout(location = 0) out vec4 o_color;

in vec3 v_position;
in vec4 v_color;
in vec2 v_texCoord;

#define MAX_TEXTURE_SLOT 32
layout(binding = 0) uniform sampler2D u_textures[MAX_TEXTURE_SLOT];

uniform float u_tilingFactor;

void main()
{
	//o_color = vec4(v_position * 0.5 + 0.5, 1.0);
	//o_color = vec4(v_texCoord.x, v_texCoord.y, 0.0, 1.0);

	o_color = v_color;
	o_color = texture(u_textures[0], v_texCoord);

}