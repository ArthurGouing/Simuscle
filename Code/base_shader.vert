# version 460
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNor;

uniform mat4 vp_mat;

out vec3 vNormal;
out mat4 view;

void main()
{
    gl_Position = vp_mat * vec4(aPos.x, aPos.y, aPos.z, 1.0);
    vNormal = normalize(aNor); // Normal in model space
    view = vp_mat;
}
