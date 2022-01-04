#version 330 core

uniform sampler2D planeTexture;

in vec2 TexCoord;
out vec4 FragColor;
void main()
{
    FragColor = texture(planeTexture, TexCoord);
}