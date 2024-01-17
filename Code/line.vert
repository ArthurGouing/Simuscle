# version 460
// in
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aCol;

uniform mat4 vp_mat;

// out
out vec3 Color;

// main
void main()
{
    gl_Position = vp_mat * vec4(aPos.x, aPos.y, aPos.z, 1.0);
    Color = aCol;
}
