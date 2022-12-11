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
    vec3 texCoord;
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
	//mat3 normal_matrix = transpose(inverse(mat3(u_transform)));

	//v2f.pos = a_position;
	//v2f.ws_pos = (u_transform * vec4(a_position, 1.0)).xyz;
	//v2f.normal =  normal_matrix * a_normal;
	v2f.texCoord = a_position;				// use 1 to 1 uv 
	//v2f.tangent = normal_matrix * a_tangent;

	vec4 ss_pos = u_projection * u_transform * vec4(a_position, 1.0);		// u_transform is camera translation only
	gl_Position = ss_pos.xyww;
	//gl_Position = vec4(ss_pos.xy, 1.0, 1.0);
}

#type fragment
#version 450 core
			
layout(location = 0) out vec4 o_color;

in VertexData
{
    vec3 pos;
    vec3 ws_pos;
    vec3 texCoord;
    vec3 normal;
    vec3 tangent;
} v2f;

layout(binding = 0) uniform samplerCube u_skybox_cubemap; 

void main()
{
	o_color = texture(u_skybox_cubemap, v2f.texCoord);

	// should load as srgb
	float gamma = 2.2;
    vec3 linearColor = pow(o_color.rgb, vec3(gamma));
    o_color = vec4(linearColor, 1.0);
}