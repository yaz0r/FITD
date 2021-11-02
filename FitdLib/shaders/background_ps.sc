$input v_texcoord0

#include "bgfx_shader.sh"

highp USAMPLER2D(s_backgroundTexture, 0);
highp USAMPLER2D(s_paletteTexture, 1);

void main()
{
    uvec4 rawTexel = texelFetch(s_backgroundTexture, ivec2(v_texcoord0.x * 320, v_texcoord0.y * 200), 0);
    gl_FragColor.r = (texelFetch(s_paletteTexture, ivec2(0, rawTexel.r), 0) / 255.f).r;
    gl_FragColor.g = (texelFetch(s_paletteTexture, ivec2(1, rawTexel.r), 0) / 255.f).r;
    gl_FragColor.b = (texelFetch(s_paletteTexture, ivec2(2, rawTexel.r), 0) / 255.f).r;
    gl_FragColor.a = 1.f;
}
