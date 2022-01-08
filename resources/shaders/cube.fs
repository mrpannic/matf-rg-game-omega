#version 330 core

uniform sampler2D cubeTexture;

in vec2 TexCoord;
out vec4 FragColor;
void main()
{
    FragColor = vec4(0.33f,0.1f,0.5f,1.0f);
}