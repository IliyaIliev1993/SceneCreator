#version 330 core
layout(origin_upper_left) in vec4 gl_FragCoord;
out vec4 FragColor;

in vec2 oTexCoord;

uniform sampler2D iTexture;
uniform vec4 vColor;
uniform vec4 vecClip;

void main()
{
    if(gl_FragCoord.x > vecClip.x && gl_FragCoord.x < (vecClip.x + vecClip.w) &&
       gl_FragCoord.y > vecClip.y && gl_FragCoord.y < (vecClip.y + vecClip.z))
    {
        FragColor = vec4(0.0, 0.0, 0.0, 0.0);
        return;
    }

    FragColor = texture(iTexture, oTexCoord) * vec4(vColor.xyz, 1.0);
}
