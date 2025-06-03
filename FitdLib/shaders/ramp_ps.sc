$input v_texcoord0

#include "bgfx_shader.sh"

USAMPLER2D(s_paletteTexture, 1);

float _mod(float x, float y)
{
  return x - y * floor(x/y);
}

void main()
{
    float colorf = v_texcoord0.x;

    colorf = _mod(colorf, 2.f);

    if(colorf > 1.f) {
        colorf = 1.f - (colorf - 1.f);
    }

    int color = int(colorf * 15.f);
    int bank = int(v_texcoord0.y * 15.f);

    int colorOffset = (bank << 4) + color;

    gl_FragColor.r = float(texelFetch(s_paletteTexture, ivec2(0, colorOffset), 0).r) / 255.f;
    gl_FragColor.g = float(texelFetch(s_paletteTexture, ivec2(1, colorOffset), 0).r) / 255.f;
    gl_FragColor.b = float(texelFetch(s_paletteTexture, ivec2(2, colorOffset), 0).r) / 255.f;
    gl_FragColor.w = 1.f;
}
