#version 330 core
out vec4 FragColor;

in vec2 oTexCoord;

uniform sampler2D iTexture;
uniform vec4 vColor;

void main()
{
    FragColor = texture(iTexture, oTexCoord) * vColor;
}
