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

    vec4 lightSpace_pos;
} v2f;

layout(std140, binding = 0) uniform ub_camera
{
	mat4 u_view;
	mat4 u_projection;
	mat4 u_viewProjection;
    mat4 u_lightSpace_matrix;
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

    v2f.lightSpace_pos = u_lightSpace_matrix * vec4(v2f.ws_pos, 1.0);

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
			
layout(location = 0) out vec4 o_poisiton;
layout(location = 1) out vec4 o_normal;
layout(location = 2) out vec4 o_albedoSpec;

in VertexData
{
    vec3 pos;
    vec3 ws_pos;
    vec2 texCoord;
    vec3 normal;
    vec3 tangent;

    vec4 lightSpace_pos;
} v2f;

layout(binding = 0) uniform sampler2D u_albedo_map;
layout(binding = 1) uniform sampler2D u_normal_map;
layout(binding = 2) uniform sampler2D u_specular_map;
layout(binding = 3) uniform sampler2D u_emission_map;

struct Property
{
    float hasAlbedoMap;
    float hasNormalMap;
    float hasSpecularMap;
    float hasEmissionMap;

    vec4 albedo;
    float shininess;
    vec4 emission;
    vec2 tilling;
    vec2 offset;
};
uniform Property u_property;

void main()
{
    vec2 uv = v2f.texCoord * u_property.tilling + u_property.offset;
	vec4 position = vec4(v2f.ws_pos, 1.0);
	//vec4 normal = vec4(v2f.normal, 1.0);
	//vec4 albedo = texture(u_albedo_map, v2f.texCoord);

	vec3 albedo     = mix(u_property.albedo, texture(u_albedo_map, uv) * u_property.albedo, u_property.hasAlbedoMap).rgb;
	vec4 normal     = mix(vec4(v2f.normal, 1.0), texture(u_normal_map, uv), u_property.hasNormalMap);
    float spec = mix(u_property.shininess, texture(u_specular_map, uv).r, u_property.hasSpecularMap);
    spec = log2(spec) / 10.5;

	vec3 emission = mix(u_property.emission, texture(u_emission_map, uv) * u_property.emission, u_property.hasEmissionMap).rgb;
    emission = mix(vec3(0.0), emission, u_property.hasEmissionMap).rgb;
    
    o_poisiton = position;
    o_normal = normal;
    o_albedoSpec = vec4(albedo + emission, spec);
}