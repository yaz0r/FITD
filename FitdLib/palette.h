#pragma once

#include <array>
typedef std::array<std::array<unsigned char, 3>, 256> palette_t;
void paletteFill(palette_t& palette, unsigned char r, unsigned char g, unsigned b);
void setPalette(palette_t& palette);
void copyPalette(void* source, palette_t& dest);
void copyPalette(palette_t& source, palette_t& dest);
void convertPaletteIfRequired(palette_t& lpalette);
void computePalette(palette_t& inPalette, palette_t& outPalette, int coef);

extern palette_t currentGamePalette;
