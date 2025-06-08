$input v_texcoord0

#include "bgfx_shader.sh"
#include "palette.sh"

float _mod(float x, float y)
{
  return x - y * floor(x/y);
}

void main()
{
    float colorf = v_texcoord0.x;

    colorf = _mod(colorf, 2.f);

    if(colorf > 1.f) {
        colorf = 1.f - (colorf - 1.f);
    }

    int color = int(colorf * 15.f);
    int bank = int(v_texcoord0.y * 15.f);
    gl_FragColor = getColor(bank, color);
}
