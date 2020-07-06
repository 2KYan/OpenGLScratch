#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;

void main()
{    
    vec4 texColor = texture(texture_diffuse1, TexCoords);
    //FragColor = vec4(TexCoords.r, TexCoords.g, 0.0,1.0);
    if (texColor.a < 0.1) {
        discard;
    }
    FragColor = texColor;
    
}
