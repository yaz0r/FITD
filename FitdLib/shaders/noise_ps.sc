$input v_texcoord0

#include "bgfx_shader.sh"
#include "palette.sh"

void main()
{
    int color = int(v_texcoord0.x * 15.f);
    int bank = int(v_texcoord0.y * 15.f);
    gl_FragColor = getColor(bank, color);
}
