#type vertex
#version 450 core
layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_texCoord;
layout(location = 3) in vec3 a_tangent;
layout(location = 4) in ivec4 a_boneIDs;
layout(location = 5) in vec4 a_weights;

layout(std140, binding = 0) uniform ub_camera
{
	mat4 u_view;
	mat4 u_projection;
	mat4 u_viewProjection;
    mat4 u_lightSpace_matrix;
};
uniform mat4 u_transform;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 u_boneTransformations[MAX_BONES];


void main()
{
	mat4 boneTransform = u_boneTransformations[a_boneIDs[0]] * a_weights[0];
	boneTransform += u_boneTransformations[a_boneIDs[1]] * a_weights[1];
	boneTransform += u_boneTransformations[a_boneIDs[2]] * a_weights[2];
	boneTransform += u_boneTransformations[a_boneIDs[3]] * a_weights[3];

	gl_Position = u_lightSpace_matrix * u_transform * boneTransform * vec4(a_position, 1.0);	
}

#type fragment
#version 450 core

void main()
{
}