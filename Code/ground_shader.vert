# version 460
// in
layout (location = 0) in vec3 aPos;

uniform mat4 vp_mat;
uniform vec3 camera_pos;

// out
out vec3 Pos;
out vec3 cam_pos;

// main
void main()
{
    gl_Position = vp_mat * vec4(aPos.x, aPos.y, aPos.z, 1.0);
    Pos = aPos;
    cam_pos = camera_pos;
}
