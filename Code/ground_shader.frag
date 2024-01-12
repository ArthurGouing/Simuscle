#version 330 core
in vec3 Pos;
in vec3 cam_pos;
out vec4 FragColor;

float pattern(vec3 Pos)
{
  float alpha = 0.f;
  float eps = 1.f; // = line width
  vec3 local_d = Pos - floor(Pos);
  float sigma = 200000.f * eps / 20.f; //= line_width
  alpha = max(exp(-sigma * pow(local_d.x-eps/2.f, 2)), exp(-sigma * pow(local_d.y-eps/2.f, 2)));
  return alpha; // alpha = 0 or 1
}

void main()
{
  // vec3 background = 7. * vec3(0.03, 0.07, 0.07); // TODO: le récuếrer depuis le CPU pour qu'il soit commun avec le fondvec3
  // Init colors
  vec3 background = vec3(0.22, 0.28, 0.28);
  // background = pow(background, vec3(1./0.6545));
  vec3 dark = vec3(0.28, 0.28, 0.28);
  vec3 light = vec3(0.75, 0.75, 0.70)-dark;

  // Compute texture
  vec3 color = dark + light * pattern(Pos);
  float d = length(Pos + cam_pos);
  color = mix(color, background, 1.-exp(-0.003*d*d));

  // Gamma correction:
  color = pow(color, vec3(0.8545));
  FragColor = vec4(color, 1.0);
} 
