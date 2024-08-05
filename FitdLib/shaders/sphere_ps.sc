$input v_texcoord0, v_sphereParams, v_screenSpacePosition

#include "bgfx_shader.sh"

highp USAMPLER2D(s_paletteTexture, 1);

void main()
{
    vec2 sphereCenter = v_sphereParams.xy;
    float sphereSize = v_sphereParams.z;
    float fDistanceToCenter = length(v_screenSpacePosition.xy - sphereCenter);
    if(fDistanceToCenter > sphereSize)
        discard;

    int color = int(v_texcoord0.x * 15.f);
    int bank = int(v_texcoord0.y * 15.f);
    int material = (int)v_sphereParams.w;

    if(material == 0) { // flat
        int colorOffset = (bank << 4) + color;
        gl_FragColor.r = (texelFetch(s_paletteTexture, ivec2(0, colorOffset), 0) / 255.f).r;
        gl_FragColor.g = (texelFetch(s_paletteTexture, ivec2(1, colorOffset), 0) / 255.f).r;
        gl_FragColor.b = (texelFetch(s_paletteTexture, ivec2(2, colorOffset), 0) / 255.f).r;
        gl_FragColor.w = 1;
    } else if(material == 2) { // transparent
        int colorOffset = (bank << 4) + color;
        gl_FragColor.r = (texelFetch(s_paletteTexture, ivec2(0, colorOffset), 0) / 255.f).r;
        gl_FragColor.g = (texelFetch(s_paletteTexture, ivec2(1, colorOffset), 0) / 255.f).r;
        gl_FragColor.b = (texelFetch(s_paletteTexture, ivec2(2, colorOffset), 0) / 255.f).r;
        gl_FragColor.w = 0.5;
    } else if(material == 3) { // marbre
        vec2 normalizedPosition = (v_screenSpacePosition.xy - sphereCenter.xy) / sphereSize;
        float angle = asin(normalizedPosition.y);
        float distanceToCircleOnScanline = cos(angle) - normalizedPosition.x; // value is in 0/2 range
        distanceToCircleOnScanline /= 2.f; // remap to 0 / 1
        color = (1.f - distanceToCircleOnScanline) * 15.f;

        int colorOffset = (bank << 4) + color;
        gl_FragColor.r = (texelFetch(s_paletteTexture, ivec2(0, colorOffset), 0) / 255.f).r;
        gl_FragColor.g = (texelFetch(s_paletteTexture, ivec2(1, colorOffset), 0) / 255.f).r;
        gl_FragColor.b = (texelFetch(s_paletteTexture, ivec2(2, colorOffset), 0) / 255.f).r;
        gl_FragColor.w = 1;
    } else {
        gl_FragColor.r = 1.f;
        gl_FragColor.g = 0;
        gl_FragColor.b = 0;
        gl_FragColor.w = 1;
    }
}
