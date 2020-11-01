#version 330 core
out vec4 FragColor;

in vec2 oTexCoord;
uniform sampler2D iTexture;
uniform vec4 vecDatas;
const float PI = 3.1415926535;

void main()
{
  float aperture = 178.0;
  float apertureHalf = (0.5 * aperture * (PI / 180.0));
  float maxFactor = apertureHalf - vecDatas.w;

  vec2 uv;
  vec2 xy = 2.0 * (oTexCoord + (-1 * vecDatas.xy)) - 1.0 ;
  float d = length(xy);
  if (d < (2.0-maxFactor))
  {
    d = length(xy * maxFactor);
    float z = sqrt(1.0 - d * d);
    float r = atan(d, z) / PI;
    float phi = atan(xy.y, xy.x);

    uv.x = r * cos(phi) + (0.5 + vecDatas.x);
    uv.y = r * sin(phi) + (0.5 + vecDatas.y);
  }
  else
  {
    uv = oTexCoord;
  }

  vec4 color = texture(iTexture, uv);
  FragColor = color;
}
