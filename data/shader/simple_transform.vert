#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 oPos;
out vec3 oNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    oPos = vec3(model * vec4(aPos, 1.0f));
    oNormal = mat3(transpose(inverse(model))) * aNormal;
    //oNormal = aNormal;
    gl_Position = projection * view * vec4(oPos, 1.0);
}
