USAMPLER2D(s_paletteTexture, 1);

vec4 getColorFromRawOffset(uint colorOffset) {
	vec4 outputColor;
	outputColor.r = float(texelFetch(s_paletteTexture, ivec2(0, colorOffset), 0).r) / 255.f;
	outputColor.g = float(texelFetch(s_paletteTexture, ivec2(1, colorOffset), 0).r) / 255.f;
	outputColor.b = float(texelFetch(s_paletteTexture, ivec2(2, colorOffset), 0).r) / 255.f;
	outputColor.w = 1.f;

	return outputColor;
}

vec4 getColor(int bank, int color) {
	uint colorOffset = uint((bank << 4) + color);
	return getColorFromRawOffset(colorOffset);
}