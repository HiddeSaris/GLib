#type vertex

#version 330 core
layout (location = 0) in vec3 a_Pos;

uniform mat4 u_Transform;

void main()
{
    gl_Position = u_Transform * vec4(a_Pos, 1.0);
}


#type fragment

#version 330 core
out vec4 FragColor;

void main()
{
    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
}