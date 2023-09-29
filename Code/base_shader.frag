#version 330 core
in vec3 norcolor;
out vec4 FragColor;

void main()
{
    FragColor = vec4(norcolor, 1.0f);
} 
