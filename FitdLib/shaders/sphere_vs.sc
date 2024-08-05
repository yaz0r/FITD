$input a_position, a_texcoord0, a_texcoord1
$output v_texcoord0, v_sphereParams, v_screenSpacePosition

#include "bgfx_shader.sh"

void main()
{
    gl_Position = vec4(a_position.x/160.0-1.0, 1.0-a_position.y/100.0, a_position.z/40960.f, 1.0);
    v_texcoord0 = a_texcoord0;

    vec2 sphereCenter = a_texcoord1.xy;
    float sphereSize = a_texcoord1.z;
    float sphereMaterial = a_texcoord1.w;

    v_screenSpacePosition = a_position.xyz;
    v_sphereParams.xy = sphereCenter.xy;
    v_sphereParams.z = sphereSize;
    v_sphereParams.w = sphereMaterial;
}
