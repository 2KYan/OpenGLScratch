#version 330 core

in vec4 oColor;
out vec4 FragColor;

uniform vec4 colorShift;

void main()
{
    FragColor = oColor * colorShift;
}