$input a_position, a_color0
$output v_color0

#include "bgfx_shader.sh"

void main()
{
    gl_Position = vec4(a_position.x/160.0-1.0, 1.0-a_position.y/100.0, a_position.z/40960.f, 1.0);
    v_color0 = a_color0/256.f;
}
