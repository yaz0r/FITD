$input v_texcoord0

#include "bgfx_shader.sh"

USAMPLER2D(s_backgroundTexture, 0);
USAMPLER2D(s_paletteTexture, 1);

void main()
{
    vec2 position = vec2(v_texcoord0.xy) * vec2(320, 200);
    uvec4 rawTexel = texelFetch(s_backgroundTexture, ivec2(position), 0);
    ivec2 base = ivec2(0, rawTexel.r);
    gl_FragColor.r = float(texelFetch(s_paletteTexture, base + ivec2(0, 0), 0).r) / 255.f;
    gl_FragColor.g = float(texelFetch(s_paletteTexture, base + ivec2(1, 0), 0).r) / 255.f;
    gl_FragColor.b = float(texelFetch(s_paletteTexture, base + ivec2(2, 0), 0).r) / 255.f;
    gl_FragColor.a = 1.f;
}
