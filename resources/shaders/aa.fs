#version 330 core

uniform sampler2DMS msaa_texture;
in vec2 TexCoords;

uniform int windowWidth;
uniform int windowHeight;
uniform int sampleNum;

out vec4 FragCol;
void main()
{
    ivec2 viewPortDim = ivec2(windowWidth, windowHeight);
    ivec2 coord = ivec2(viewPortDim * TexCoords);
    vec3 col = vec3(0.0);

    for(int i = 0; i < sampleNum; i++){
        col += texelFetch(msaa_texture, coord, i).rgb;
    }

    col *= 1.0/sampleNum;
    FragCol = vec4(col, 1.0);
}