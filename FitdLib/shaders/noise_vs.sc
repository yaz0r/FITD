$input a_position, a_texcoord0
$output v_texcoord0, v_screenSpacePosition

#include "bgfx_shader.sh"

void main()
{
    gl_Position = vec4(a_position.x/160.0-1.0, 1.0-a_position.y/100.0, a_position.z/40960.f, 1.0);
    v_texcoord0 = a_texcoord0;
    v_screenSpacePosition = a_position.xyz;
}
