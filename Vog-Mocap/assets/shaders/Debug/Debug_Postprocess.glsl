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

uniform vec4 u_screenParam;

void main()
{
    vec2 screenUv = gl_FragCoord.xy / u_screenParam.xy;
    //screenUv = v2f.texCoord;
    vec3 linearColor = texture(u_screen_map, screenUv).rgb;
    
    o_color = vec4(linearColor, 1.0);
}