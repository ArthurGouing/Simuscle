#version 330 core
in vec3 vNormal;
in mat4 view;
out vec4 FragColor;

uniform sampler2D MatcapTexture;

void main()
{
    // FragColor =  vec4(vNormal, 1.0f);
    // vec2 muv = vec2(0.5f, 0.5f) + dot(vNormal, vec3(1.0f, 0.0f, 0.0f));
    highp vec2 muv = vec2(view * vec4(normalize(vNormal), 0))*vec2(0.3, 0.2)+vec2(0.5,0.5);
    FragColor = texture(MatcapTexture, vec2(muv.x, 1.0-muv.y));
} 
