#type vertex

#version 330 core
layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Color;

uniform mat4 u_Transform;

out vec3 v_Color;

void main()
{
    v_Color = a_Color;
    gl_Position = u_Transform * vec4(a_Position, 1.0);
}


#type fragment

#version 330 core
out vec4 FragColor;

in vec3 v_Color;

void main()
{
    FragColor = vec4(v_Color, 1.0);
}