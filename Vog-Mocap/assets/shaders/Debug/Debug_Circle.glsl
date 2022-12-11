#type vertex
#version 450 core
layout(location = 0) in vec3 a_worldPosition;
layout(location = 1) in vec3 a_uv;
layout(location = 2) in vec4 a_color;
layout(location = 3) in float a_thickness;
layout(location = 4) in float a_fade;

out VertexData
{
    vec3 ws_pos;
    vec3 texCoord;
    vec4 color;
    float thickness;
    float fade;
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
	v2f.texCoord = a_uv;
	v2f.color = a_color;
	v2f.thickness = a_thickness;
	v2f.fade = a_fade;
	gl_Position = u_viewProjection * vec4(a_worldPosition, 1.0);	
}

#type fragment
#version 450 core
			
layout(location = 0) out vec4 o_color;

in VertexData
{
    vec3 ws_pos;
    vec3 texCoord;
    vec4 color;
    float thickness;
    float fade;
} v2f;

void main()
{
	const float epsilon = 0.000001;
	vec2 uv = v2f.texCoord.xy;
	// float dist = 1 - length(uv);
	// float alpha = smoothstep(0.0, v2f.fade, dist);
	// alpha = alpha * smoothstep(v2f.thickness + v2f.fade, v2f.thickness, dist);

	float dist = length(uv);
	float alpha = smoothstep(1.0, 1.0 - v2f.fade, dist);
	float t = 1 - v2f.thickness;
	alpha = alpha * smoothstep(t, t + v2f.fade, dist);

	if(alpha < epsilon)
		discard;

	o_color = v2f.color;
	o_color.a *= alpha;
}