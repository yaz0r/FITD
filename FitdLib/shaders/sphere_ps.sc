$input v_texcoord0, v_sphereParams, v_screenSpacePosition

#include "bgfx_shader.sh"

highp USAMPLER2D(s_paletteTexture, 1);

void main()
{
    vec2 sphereCenter = v_sphereParams.xy;
    float sphereSize = v_sphereParams.z;
    float fDistanceToCenter = length(v_screenSpacePosition - sphereCenter);
    if(fDistanceToCenter > sphereSize)
        discard;

    int color = int(v_texcoord0.x * 15.f);
    int bank = int(v_texcoord0.y * 15.f);

    int colorOffset = (bank << 4) + color;

    gl_FragColor.r = (texelFetch(s_paletteTexture, ivec2(0, colorOffset), 0) / 255.f).r;
    gl_FragColor.g = (texelFetch(s_paletteTexture, ivec2(1, colorOffset), 0) / 255.f).r;
    gl_FragColor.b = (texelFetch(s_paletteTexture, ivec2(2, colorOffset), 0) / 255.f).r;
    gl_FragColor.w = 1;
}
