uniform vec2 i_resolution;  // Width and height of the shader

uniform vec3 camera_pos;
uniform mat4 rotation;
uniform float camera_dist;
 
// Constants
#define PI 3.1415925359
#define TWO_PI 6.2831852
#define MAX_STEPS 100
#define MAX_DIST 0.
#define SURFACE_DIST 1.
 

void main()
{
  vec3 background = vec3(0.22, 0.28, 0.28);
  background = vec3(0.22, 0.22, 0.22);
   
  // Set the output color
  vec3 color = pow(background, vec3(0.8545));
  gl_FragColor = vec4(color,1.0);
}
