#version 330 core

uniform sampler2D screenTexture;
in vec2 TexCoords;


out vec4 FragCol;
void main()
{
    vec3 col = texture(screenTexture, TexCoords).rgb;
    FragCol = vec4(col, 1.0);
}