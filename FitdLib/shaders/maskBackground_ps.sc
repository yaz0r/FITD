$input v_texcoord0

#include "bgfx_shader.sh"
#include "palette.sh"

USAMPLER2D(s_maskPaletteTexture, 0);
USAMPLER2D(s_backgroundTexture, 2);

void main()
{
    vec2 position = vec2(v_texcoord0.xy) * vec2(320, 200);
    uvec4 rawMask = texelFetch(s_maskPaletteTexture, ivec2(position), 0);
    if(int(rawMask.r) == 0)
        discard;

    uvec4 rawTexel = texelFetch(s_backgroundTexture, ivec2(position), 0);
    gl_FragColor = getColorFromRawOffset(rawTexel.r);
}
