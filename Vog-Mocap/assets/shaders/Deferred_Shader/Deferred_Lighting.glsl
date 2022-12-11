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

    mat4 u_lightSpace_matrix;
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
layout(location = 1) out vec4 o_brightColor;  

in VertexData
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

    mat4 u_lightSpace_matrix;
};

layout(binding = 0) uniform sampler2D u_position_map;
layout(binding = 1) uniform sampler2D u_normal_map;
layout(binding = 2) uniform sampler2D u_albedoSpec_map;

layout(binding = 3) uniform sampler2D u_shadow_map;

uniform vec4 u_screenParam;
uniform vec3 u_camera_position;
// Lights
const int max_lights_count = 50;
uniform int u_current_light_count;
uniform vec4 u_light_positions      [max_lights_count];
uniform vec3 u_light_directions     [max_lights_count];
uniform vec4 u_light_colors         [max_lights_count];
uniform vec4 u_light_params         [max_lights_count];

float shadowCalculation(vec4 lightSpace_pos_, vec3 ws_pos_, vec3 normal_, vec3 lightPosition_)
{
    // lightSpace_pos_ is world space now
    //lightSpace_pos_ = u_lightSpace_matrix * vec4(v2f.lightSpace_pos_.xyz, 1.0);

    // perform perspective divide
    vec3 projCoords = lightSpace_pos_.xyz / lightSpace_pos_.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    if(projCoords.z > 1.0)
       return 0.0;

    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(u_shadow_map, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow

    vec3 normal = normalize(normal_);
    vec3 lightDir = normalize(lightPosition_ - ws_pos_);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(u_shadow_map, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(u_shadow_map, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;

    return shadow;
}

vec4 directionalLight(vec3 ws_pos_, vec3 normal_, vec4 albedo_)
{
 vec3 o_lighting = vec3(0.0, 0.0, 0.0);
    vec3 viewDir = normalize(u_camera_position - ws_pos_);

    float shadow = shadowCalculation(u_lightSpace_matrix * vec4(ws_pos_, 1.0), ws_pos_, normal_, u_light_positions[0].xyz);

    vec3 ambient = albedo_.rgb * 0.1; // hard-coded ambient component
    vec3 diffuseColor = albedo_.rgb;
    vec3 specularColor = vec3(1, 1, 1);

    //float shiness = albedo_.a;
    float shiness = pow(2.0, albedo_.a * 10.5);
    shiness = 32.0f;

    vec3 lightColor 		= u_light_colors[0].rgb;
    float lightIntensity 	= u_light_colors[0].a;

    vec3 lightPos       	= u_light_positions[0].xyz;
    vec3 lightDir       	= u_light_directions[0].xyz;
    float isDirectional  	= 1 - u_light_positions[0].w;

    float lightRange          = u_light_params[0].x;
    float isSpotLight 	      = u_light_params[0].y;
    float lightInnerSpotAngle = u_light_params[0].z;
    float lightOuterSpotAngle = u_light_params[0].w;

    vec3 toLight = ws_pos_ - lightPos;
    vec3 L = mix(toLight, lightDir, vec3(isDirectional));

    float lengthSq = dot(L, L) * (1 - isDirectional);
    lengthSq = max(lengthSq, 0.000001);

    L = normalize(L);

    float diffuseIntensity = max(dot(-L, normal_), 0.0);
    vec3 diffuse = diffuseIntensity * diffuseColor;

    vec3 reflectDir = reflect(L, normal_);
    vec3 H = normalize(L + viewDir);
    float specularIntensity = mix(0.0, 0.4, diffuseIntensity);
    //vec3 specular = specularIntensity * specularColor * pow(max(dot(viewDir, reflectDir), 0.0), shiness);
    vec3 specular = specularIntensity * specularColor * pow(max(dot(normal_, H), 0.0), shiness);

    //float attenuation = 1.0 - clamp(lengthSq / (lightRange * lightRange + 1), 0.0, 1.0);
    float attenuation = 1.0 - lengthSq / (lightRange * lightRange + 1);
    attenuation = 1.0 / (lengthSq + 1.0);

    float intensity = lightIntensity * attenuation;

    ambient *= intensity;
    specular *= intensity;
    diffuse *= intensity * lightColor;

    o_lighting.rgb += ambient + (specular + diffuse) * (1 - shadow);

    return vec4(o_lighting , 1.0);
}

vec4 phongLight(vec3 ws_pos_, vec3 normal_, vec4 albedo_)
{
    vec3 o_lighting = vec3(0.0, 0.0, 0.0);
    vec3 viewDir = normalize(u_camera_position - ws_pos_);

    //float shadow = shadowCalculation(u_lightSpace_matrix * vec4(ws_pos_, 1.0), ws_pos_, normal_, u_light_positions[0].xyz);

    vec3 ambient = albedo_.rgb * 0.1; // hard-coded ambient component
    vec3 diffuseColor = albedo_.rgb;
    vec3 specularColor = vec3(1, 1, 1);

    //float shiness = albedo_.a;
    float shiness = pow(2.0, albedo_.a * 10.5);
    shiness = 32.0f;

    for(int i = 1; i < u_current_light_count; ++i)
    {
        vec3 lightColor 		= u_light_colors[i].rgb;
        float lightIntensity 	= u_light_colors[i].a;

        vec3 lightPos       	= u_light_positions[i].xyz;
        vec3 lightDir       	= u_light_directions[i].xyz;
        float isDirectional  	= 1 - u_light_positions[i].w;

        float lightRange          = u_light_params[i].x;
        float isSpotLight 	      = u_light_params[i].y;
        float lightInnerSpotAngle = u_light_params[i].z;
        float lightOuterSpotAngle = u_light_params[i].w;

        vec3 toLight = ws_pos_ - lightPos;
        vec3 L = mix(toLight, lightDir, vec3(isDirectional));

        float lengthSq = dot(L, L) * (1 - isDirectional);
        lengthSq = max(lengthSq, 0.000001);

        L = normalize(L);

        float diffuseIntensity = max(dot(-L, normal_), 0.0);
        vec3 diffuse = diffuseIntensity * diffuseColor;

        vec3 reflectDir = reflect(L, normal_);
        vec3 H = normalize(L + viewDir);
        float specularIntensity = mix(0.0, 0.4, diffuseIntensity);
        //vec3 specular = specularIntensity * specularColor * pow(max(dot(viewDir, reflectDir), 0.0), shiness);
        vec3 specular = specularIntensity * specularColor * pow(max(dot(normal_, H), 0.0), shiness);

        //float attenuation = 1.0 - clamp(lengthSq / (lightRange * lightRange + 1), 0.0, 1.0);
        float attenuation = 1.0 - lengthSq / (lightRange * lightRange + 1);
        attenuation = 1.0 / (lengthSq + 1.0);

        float intensity = lightIntensity * attenuation;

        if (isSpotLight > 0.0)
        {
            intensity *= smoothstep(lightOuterSpotAngle, lightInnerSpotAngle, dot(L, lightDir));
        }

        ambient *= intensity;
        specular *= intensity;
        diffuse *= intensity * lightColor;

        o_lighting.rgb += ambient + (specular + diffuse);
    }

    return vec4(o_lighting , 1.0);
}

void main()
{
    vec2 screenUv = gl_FragCoord.xy / u_screenParam.xy;
    //screenUv = v2f.texCoord;

    vec3 ws_pos = texture(u_position_map, screenUv).rgb;
    vec3 normal = texture(u_normal_map, screenUv).rgb;
    normal = normalize(normal);
    
    vec4 albedo = texture(u_albedoSpec_map, screenUv).rgba;
    //float shininess = texture(u_albedoSpec_map, screenUv).a;

    o_color = phongLight(ws_pos, normal, albedo) + vec4(directionalLight(ws_pos, normal, albedo).rgb, 0.0);
    //o_color = vec4(normal.rgb, 1.0);
    //o_color = vec4(vec3(ws_pos.z), 1.0);

    // bright for blur
    float brightness = dot(o_color.rgb, vec3(0.2126, 0.7152, 0.0722));
    brightness = step(1.0, brightness);
    o_brightColor = vec4(mix(vec3(0.0), o_color.rgb, vec3(brightness)), 1.0);
}