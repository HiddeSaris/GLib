#type vertex
#version 330 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec2 a_TexCoord;
//layout (location = 3) in vec4 a_Color;

out vec3 v_FragPos;
out vec3 v_Normal;
out vec2 v_TexCoord;
//out vec4 v_Color;

uniform mat4 u_Projection;
uniform mat4 u_View;
uniform mat4 u_Model;
uniform mat3 u_NormalMatrix;

void main() 
{
    v_FragPos = vec3(u_Model * vec4(a_Position, 1.0));
    v_Normal = u_NormalMatrix * a_Normal;
    v_TexCoord = a_TexCoord;
    // v_Color = a_Color;
    gl_Position = u_Projection * u_View * vec4(v_FragPos, 1.0);
}

#type fragment
#version 330 core

layout (location = 0) out vec4 FragColor;

in vec3 v_FragPos;
in vec3 v_Normal;
in vec2 v_TexCoord;
// in vec4 v_Color;

// material textures
uniform sampler2D u_AlbedoMap;
uniform sampler2D u_NormalMap;
uniform sampler2D u_MetallicMap;
uniform sampler2D u_RoughnessMap;
uniform sampler2D u_AOMap;

uniform vec3 u_CamPos;

struct PointLight {
    vec3 Position;

    vec3 Color;
    float Intensity;
};
uniform PointLight u_PointLights[10];
uniform int u_NumPointLights;

struct DirLight {
    vec3 Direction;

    vec3 Color;
    float Intensity;
};
uniform DirLight u_DirLights[1];
uniform int u_NumDirLights;

struct SpotLight {
    vec3 Position;
    vec3 Direction;
    float Cutoff;
    float OuterCutoff;

    vec3 Color;
    float Intensity;
};
uniform SpotLight u_SpotLights[5];
uniform int u_NumSpotLights;

#define PI 3.14159265359

vec3 GetNormalFromMap()
{
    vec3 tangentNormal = texture(u_NormalMap, v_TexCoord).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(v_FragPos);
    vec3 Q2  = dFdy(v_FragPos);
    vec2 st1 = dFdx(v_TexCoord);
    vec2 st2 = dFdy(v_TexCoord);

    vec3 N   = normalize(v_Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
    float denom = NdotH2 * (a2 - 1.0) + 1.0;
    denom = PI * denom * denom;

    return a2 / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;
    return NdotV / (NdotV * (1.0 - k) + k);
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx1 = GeometrySchlickGGX(NdotV, roughness);
    float ggx2 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 GammaCorrect(vec3 color) {
    color = color / (color + vec3(1.0));
    return pow(color, vec3(1.0/2.2));
}

vec3 CalcLight(vec3 N, vec3 V, vec3 L, vec3 radiance, vec3 F0, float roughness, float metallic, vec3 albedo) {
    vec3 H = normalize(V + L);
    float NdotL = max(dot(N, L), 0.0);
    float HdotV = max(dot(H, V), 0.0);
    float NdotV = max(dot(N, V), 0.0);
    vec3 F = fresnelSchlick(HdotV, F0);
    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);

    vec3 num = NDF * F * G;
    float denom = 4.0 * NdotV * NdotL + 0.0001;
    vec3 specular = num / denom;

    vec3 kD = (vec3(1.0) - F) * (1.0 - metallic);
    return (kD * albedo / PI + specular) * radiance * NdotL;
}

vec3 CalcPointLightRadiance(PointLight light) {
    float distance = length(light.Position - v_FragPos);
    float attenuation = 1.0 / (distance * distance);
    return light.Color * light.Intensity * attenuation;
}

vec3 CalcDirLightRadiance(DirLight light) {
    return light.Color * light.Intensity;
}

vec3 CalcSpotLightRadiance(SpotLight light) {
    float distance = length(light.Position - v_FragPos);
    float attenuation = 1.0 / (distance * distance);

    float theta = dot(normalize(light.Position - v_FragPos), normalize(-light.Direction));
    float epsilon = light.Cutoff - light.OuterCutoff;
    float intensity = clamp((theta - light.OuterCutoff) / epsilon, 0.0, 1.0);
    return light.Color * light.Intensity * attenuation * intensity;
}

void main() {
    vec3 albedo = pow(texture(u_AlbedoMap, v_TexCoord).rgb, vec3(2.2));
    float alpha = texture(u_AlbedoMap, v_TexCoord).a;
    if (alpha == 0)
        discard;
    vec3 normal = GetNormalFromMap();
    float metallic = texture(u_MetallicMap, v_TexCoord).r;
    float roughness = texture(u_RoughnessMap, v_TexCoord).r;
    float ambient_occlusion = texture(u_AOMap, v_TexCoord).r;

    vec3 N = normalize(v_Normal);
    vec3 V = normalize(u_CamPos - v_FragPos);

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    vec3 Lo = vec3(0.0);
    for (int i = 0; i < u_NumPointLights; i++) {
        PointLight light = u_PointLights[i];
        vec3 L = normalize(light.Position - v_FragPos);
        vec3 radiance = CalcPointLightRadiance(light);
        Lo += CalcLight(N, V, L, radiance, F0, roughness, metallic, albedo);
    }
    for (int i = 0; i < u_NumDirLights; i++) {
        DirLight light = u_DirLights[i];
        vec3 L = normalize(-light.Direction);
        vec3 radiance = CalcDirLightRadiance(light);
        Lo += CalcLight(N, V, L, radiance, F0, roughness, metallic, albedo);
    }
    for (int i = 0; i < u_NumSpotLights; i++) {
        SpotLight light = u_SpotLights[i];
        vec3 L = normalize(light.Position - v_FragPos);
        vec3 radiance = CalcSpotLightRadiance(light);
        Lo += CalcLight(N, V, L, radiance, F0, roughness, metallic, albedo);
    }

    vec3 ambient = vec3(0.03) * albedo * ambient_occlusion;
    vec3 color = ambient + Lo;
    color = GammaCorrect(color);

    FragColor = vec4(color, alpha);
    // float z = gl_FragCoord.z * 2.0 - 1.0;
    // float far = 100.0;
    // float near = 0.01;
    // z = (2.0 * near * far) /
    //        (far + near - z * (far - near));
    // FragColor = vec4(vec3(z), 1.0);
}