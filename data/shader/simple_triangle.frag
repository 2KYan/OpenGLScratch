#version 330 core

in vec4 oColor;
in vec2 oTex;
out vec4 FragColor;

uniform vec4 colorShift;
uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
    FragColor = mix(texture(texture1, oTex), texture(texture2, oTex), 0.8) * vec4(oColor) * colorShift;
}