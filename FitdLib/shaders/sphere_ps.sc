$input v_texcoord0, v_sphereParams, v_screenSpacePosition

#include "bgfx_shader.sh"
#include "palette.sh"

void main()
{
    vec2 sphereCenter = v_sphereParams.xy;
    float sphereSize = v_sphereParams.z;
    float fDistanceToCenter = length(v_screenSpacePosition.xy - sphereCenter);
    if(fDistanceToCenter > sphereSize)
        discard;

    int color = int(v_texcoord0.x * 15.f);
    int bank = int(v_texcoord0.y * 15.f);
    int material = int(v_sphereParams.w);

    if(material == 0) { // flat
        gl_FragColor = getColor(bank, color);
    } else if(material == 1) { // TODO!
        gl_FragColor = getColor(bank, color);
     } else if(material == 2) { // transparent
        gl_FragColor = getColor(bank, color);
        gl_FragColor.w = 0.5f;
    } else if(material == 3) { // marbre
        vec2 normalizedPosition = (v_screenSpacePosition.xy - sphereCenter.xy) / sphereSize;
        float angle = asin(normalizedPosition.y);
        float distanceToCircleOnScanline = cos(angle) - normalizedPosition.x; // value is in 0/2 range
        distanceToCircleOnScanline /= 2.f; // remap to 0 / 1
        color = int((1.f - distanceToCircleOnScanline) * 15.f);
        gl_FragColor = getColor(bank, color);
     } else if(material == 4) { // TODO!
        gl_FragColor = getColor(bank, color);
     } else if(material == 6) { // TODO!
        gl_FragColor = getColor(bank, color);
     } else {
        gl_FragColor.r = 1.f;
        gl_FragColor.g = 0.f;
        gl_FragColor.b = 0.f;
        gl_FragColor.w = 1.f;
    }
}
