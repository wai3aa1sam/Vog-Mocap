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
layout(binding = 1) uniform sampler2D u_bright_map;

uniform vec4 u_screenParam;

// hdr
uniform float u_exposure;

void main()
{
    vec2 screenUv = gl_FragCoord.xy / u_screenParam.xy;
    //screenUv = v2f.texCoord;
    vec3 hdrColor = texture(u_screen_map, screenUv).rgb;
    vec3 bloomColor = texture(u_bright_map, screenUv).rgb;
    
    hdrColor += bloomColor;
    
    //vec3 tonedColor = hdrColor / (hdrColor + vec3(1.0));                // reinhard tone mapping
    vec3 tonedColor = vec3(1.0) - exp(-hdrColor * u_exposure);        // simple tone algorithm

    float gamma = 2.2;
    vec3 linearColor = pow(tonedColor.rgb, vec3(1.0 / gamma));
    o_color = vec4(linearColor, 1.0);
}