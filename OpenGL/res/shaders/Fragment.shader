#version 330 core

out vec4 FragColor;
in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float transparency;

void main()
{
    vec2 texture_coord = vec2(-1 * TexCoord.x, TexCoord.y);
    
    FragColor = mix(texture(texture1, TexCoord), texture(texture2, texture_coord), transparency);
};
