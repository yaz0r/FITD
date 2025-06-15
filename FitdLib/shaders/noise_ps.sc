$input v_texcoord0, v_screenSpacePosition

#include "bgfx_shader.sh"
#include "palette.sh"

USAMPLER2D(s_noiseTexture, 0);

void main()
{
    int color = int(v_texcoord0.x * 15.f);
    int bank = int(v_texcoord0.y * 15.f);

    uvec2 noiseSampler = uvec2(v_screenSpacePosition.xy * v_screenSpacePosition.z) % uvec2(255, 255);
    float noise = float(texelFetch(s_noiseTexture, ivec2(noiseSampler), 0).r) / 255.f;
    //color = int(noise * 15.f);

    vec4 baseColor = getColor(bank, color);
    gl_FragColor = baseColor * (0.5f + noise/2.f);
}
