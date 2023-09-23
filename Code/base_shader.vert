# version 460
layout (location = 0) in vec3 aPos;

uniform mat4 vp_mat;

void main()
{
    gl_Position = vp_mat * vec4(aPos.x, aPos.y, aPos.z, 1.0);
}
