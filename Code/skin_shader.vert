# version 460
// in
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNor;

uniform mat4 vp_mat;
uniform mat4 view;
// uniform float theta_vert;

// out
out vec3 vNormal;
out mat4 ourview;
out vec3 Pos;
out float theta;

// main
void main()
{
    gl_Position = vp_mat * vec4(aPos.x, aPos.y, aPos.z, 1.0);
    vNormal = aNor; // Normal in model space
    ourview = view;
    Pos = aPos;
    // theta = theta_vert;
}
