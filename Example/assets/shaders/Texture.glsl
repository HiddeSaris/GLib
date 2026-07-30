#type vertex

#version 330 core
layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec2 a_TexCoord;
layout (location = 3) in vec4 a_Color;

uniform mat4 u_Model;
uniform mat4 u_ViewProjection;

out vec3 v_FragPos;
out vec3 v_Normal;
out vec2 v_TexCoord;
out vec4 v_Color;

void main() 
{
    v_FragPos = vec3(u_Model * vec4(a_Position, 1.0));
    v_Normal = mat3(transpose(inverse(u_Model))) * a_Normal;
    v_TexCoord = a_TexCoord;
    v_Color = a_Color;
    gl_Position = u_ViewProjection * u_Model * vec4(a_Position, 1.0);
}


#type fragment

#version 330 core

layout (location = 0) out vec4 Color;

in vec3 v_FragPos;
in vec3 v_Normal;
in vec2 v_TexCoord;
in vec4 v_Color;

uniform vec3 u_ViewPos;

uniform struct {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
} u_Light;

// uniform struct { 
//     //vec3 ambient;
//     //vec4 diffuse;
//     //vec4 emission;
//     float specular;
//     float shininess;
// } u_Material;

uniform float u_MaterialShininess;

uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;

void main()
{
    vec3 norm = normalize(v_Normal);
    vec3 lightDir = normalize(u_Light.position - v_FragPos);

    // diffuse
    float diffuseStrength = max(dot(norm, lightDir), 0.0);
    vec4 diffuseTex = texture(texture_diffuse, v_TexCoord);
    vec3 diffuse = diffuseStrength * u_Light.diffuse * vec3(diffuseTex);

    // specular
    vec3 viewDir = normalize(u_ViewPos - v_FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float specularStrength = pow(max(dot(viewDir, reflectDir), 0.0), u_MaterialShininess);
    vec4 specularTex = texture(texture_specular, v_TexCoord);
    vec3 specular = specularStrength * u_Light.specular * vec3(specularTex);

    // ambient
    vec3 ambient = u_Light.ambient * vec3(diffuseTex);

    vec4 result = vec4(diffuse + specular + ambient, diffuseTex.a);
    Color = result * v_Color;
    //Color = vec4(vec3(gl_FragCoord.z), 1.0);
}