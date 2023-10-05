#version 330 core
in vec3 vNormal;
in mat4 ourview;
in vec3 Pos;
out vec4 FragColor;

uniform sampler2D MatcapTexture;

void main()
{
    bool my_matcap = true;
    
    if (my_matcap)
    {
      vec3 i_0 = vec3(0.0f, 0.0f, -1.0f);
      vec3 n_0 = normalize(i_0-normalize(vNormal));
      float theta = 90 * 6.2832/360;
      vec2 ruv = vec2(0.5, 0.5) + 0.5 * vec2(cos(theta)*n_0.x + sin(theta)*n_0.y, cos(theta)*n_0.y + sin(theta)*n_0.x);
      FragColor = texture(MatcapTexture, ruv);
    }
    else
    {
      highp vec2 muv = vec2(0.5) + 0.5 * vec2(ourview * vec4(normalize(vNormal), 0));
      FragColor = texture(MatcapTexture, vec2(muv.x, 1.0-muv.y));
    }
} 
