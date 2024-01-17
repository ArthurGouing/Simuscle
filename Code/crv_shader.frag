#version 330 core
in mat4 ourview;
in vec3 Pos;
in vec3 Col;
out vec4 FragColor;

void main()
{
    FragColor = vec4(Col, 1.0);
}
