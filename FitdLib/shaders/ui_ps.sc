$input v_texcoord0

#include "bgfx_shader.sh"
#include "palette.sh"

USAMPLER2D(s_backgroundTexture, 0);

void main()
{
    vec2 position = vec2(v_texcoord0.xy) * vec2(320, 200);
    uvec4 rawTexel = texelFetch(s_backgroundTexture, ivec2(position), 0);
    if(rawTexel.r == uint(0))
        discard;
    gl_FragColor = getColorFromRawOffset(rawTexel.r);
}
