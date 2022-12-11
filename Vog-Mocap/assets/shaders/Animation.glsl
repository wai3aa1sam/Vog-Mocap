#type vertex
#version 450 core
layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_texCoord;
layout(location = 3) in vec3 a_tangent;
layout(location = 4) in ivec4 a_boneIDs;
layout(location = 5) in vec4 a_weights;

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
const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 u_boneTransformations[MAX_BONES];
uniform mat4 u_transform;

void main()
{
	mat4 boneTransform = u_boneTransformations[a_boneIDs[0]] * a_weights[0];
	boneTransform += u_boneTransformations[a_boneIDs[1]] * a_weights[1];
	boneTransform += u_boneTransformations[a_boneIDs[2]] * a_weights[2];
	boneTransform += u_boneTransformations[a_boneIDs[3]] * a_weights[3];

	v2f.ws_pos = vec3(u_transform * boneTransform * vec4(a_position, 1.0));

	mat3 normal_matrix = transpose(inverse(mat3(boneTransform)));
	//mat3 normal_matrix = transpose(inverse(mat3(u_transform)));

	v2f.pos = a_position;
	v2f.normal =  normal_matrix * a_normal;
	v2f.texCoord = a_texCoord;
	v2f.tangent = normal_matrix * a_tangent;

#if	1
	//v2f.ws_pos = vec3(u_transform * boneTransform * vec4(a_position, 1.0));
	gl_Position = u_viewProjection * vec4(v2f.ws_pos, 1.0);	
#else
	v2f.ws_pos = vec3(u_transform * vec4(a_position, 1.0));
	gl_Position = u_viewProjection * u_transform * vec4(a_position, 1.0);	
#endif
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

layout(binding = 0)uniform sampler2D u_albedo_map;
layout(binding = 1)uniform sampler2D u_normal_map;
layout(binding = 2)uniform sampler2D u_specular_map;

uniform int u_isToogle;

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

struct Light
{
	vec4 position;
	float intensity;
	vec4 color;
	vec4 colors[4];

};
uniform Light u_lights[3];
uniform Light u_lights2[10];
uniform Light u_lights3[10];
uniform test u_test2;
uniform test u_test3;

void main()
{
	o_color = vec4(v2f.ws_pos * 0.5 + 0.5, 1.0);

	vec4 albedo = texture(u_albedo_map, v2f.texCoord);
	vec4 normal = texture(u_normal_map, v2f.texCoord);
	vec4 specular = texture(u_specular_map, v2f.texCoord);

	// TODO: remove test
	//o_color = u_test.intensities[8] * u_test.vec4Params[3] * u_test.vec4Params[2] * u_lights[2].intensity;
	//o_color = u_test3.intensities[9] * u_test.intensities[9] * u_test2.vec4Params[3] * u_lights3[7].position * u_test.intensities[8]  * u_lights2[7].intensity * u_test.vec4Params[3] * u_lights2[4].position * u_test.vec4Params[2] * u_lights2[2].intensity * u_lights[2].intensity;
	//o_color = u_lights2[4].position;
	//o_color = u_lights3[7].colors[2];

	switch(u_isToogle)
	{
		case 0: o_color = albedo; break;
		case 1: o_color = normal; break;
		case 2: o_color = specular; break;
	}
	//o_color = vec4(1.0);

	//o_color = vec4(v2f.ws_pos * 0.5 + 0.5, 1.0);

}