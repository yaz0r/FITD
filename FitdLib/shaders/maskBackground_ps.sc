$input v_texcoord0

#include "bgfx_shader.sh"

USAMPLER2D(s_maskPaletteTexture, 0);
USAMPLER2D(s_paletteTexture, 1);
USAMPLER2D(s_backgroundTexture, 2);

void main()
{
    vec2 position = vec2(v_texcoord0.xy) * vec2(320, 200);
    uvec4 rawMask = texelFetch(s_maskPaletteTexture, ivec2(position), 0);
    if(int(rawMask.r) == 0)
        discard;

    uvec4 rawTexel = texelFetch(s_backgroundTexture, ivec2(position), 0);
    gl_FragColor.r = float(texelFetch(s_paletteTexture, ivec2(0, rawTexel.r), 0).r) / 255.f;
    gl_FragColor.g = float(texelFetch(s_paletteTexture, ivec2(1, rawTexel.r), 0).r) / 255.f;
    gl_FragColor.b = float(texelFetch(s_paletteTexture, ivec2(2, rawTexel.r), 0).r) / 255.f;
    gl_FragColor.a = 1.f;
}
