$input v_texcoord0

#include "bgfx_shader.sh"

highp USAMPLER2D(s_paletteTexture, 1);

void main()
{
    float colorf = v_texcoord0.x;

    colorf %= 2.f;

    if(colorf > 1.f) {
        colorf = 1.f - (colorf - 1.f);
    }

    int color = int(colorf * 15.f);
    int bank = int(v_texcoord0.y * 15.f);

    int colorOffset = (bank << 4) + color;

    gl_FragColor.r = (texelFetch(s_paletteTexture, ivec2(0, colorOffset), 0) / 255.f).r;
    gl_FragColor.g = (texelFetch(s_paletteTexture, ivec2(1, colorOffset), 0) / 255.f).r;
    gl_FragColor.b = (texelFetch(s_paletteTexture, ivec2(2, colorOffset), 0) / 255.f).r;
    gl_FragColor.w = 1;
}
