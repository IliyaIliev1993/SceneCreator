#version 330 core
layout(origin_upper_left) in vec4 gl_FragCoord;
out vec4 FragColor;

in vec2 oTexCoord;

uniform sampler2D iTexture;
uniform vec4 vColor;
uniform vec4 vecCrop;

void main()
{
    if(gl_FragCoord.x > vecCrop.x && gl_FragCoord.x < (vecCrop.x + vecCrop.w) &&
       gl_FragCoord.y > vecCrop.y && gl_FragCoord.y < (vecCrop.y + vecCrop.z))
    {
        FragColor = texture(iTexture, oTexCoord) * vec4(vColor.xyz, 1.0);
        return;
    }

    FragColor = vec4(0.0, 0.0, 0.0, 0.0);
}
