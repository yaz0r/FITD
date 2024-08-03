#include "embeddedShadersMacro.h"
#include "config.h"

#if BGFX_PLATFORM_SUPPORTS_SPIRV
#include "shaders/generated/background_vs_spv.h"
#include "shaders/generated/background_ps_spv.h"
#include "shaders/generated/maskBackground_vs_spv.h"
#include "shaders/generated/maskBackground_ps_spv.h"
#include "shaders/generated/ramp_vs_spv.h"
#include "shaders/generated/ramp_ps_spv.h"
#include "shaders/generated/noise_vs_spv.h"
#include "shaders/generated/noise_ps_spv.h"
#include "shaders/generated/flat_vs_spv.h"
#include "shaders/generated/flat_ps_spv.h"
#include "shaders/generated/sphere_vs_spv.h"
#include "shaders/generated/sphere_ps_spv.h"
#endif

#if BGFX_PLATFORM_SUPPORTS_METAL
#include "shaders/generated/background_vs_mtl.h"
#include "shaders/generated/background_ps_mtl.h"
#include "shaders/generated/maskBackground_vs_mtl.h"
#include "shaders/generated/maskBackground_ps_mtl.h"
#include "shaders/generated/ramp_vs_mtl.h"
#include "shaders/generated/ramp_ps_mtl.h"
#include "shaders/generated/noise_vs_mtl.h"
#include "shaders/generated/noise_ps_mtl.h"
#include "shaders/generated/flat_vs_mtl.h"
#include "shaders/generated/flat_ps_mtl.h"
#include "shaders/generated/sphere_vs_mtl.h"
#include "shaders/generated/sphere_ps_mtl.h"
#endif

#if BGFX_PLATFORM_SUPPORTS_GLSL
#include "shaders/generated/background_vs_glsl.h"
#include "shaders/generated/background_ps_glsl.h"
#include "shaders/generated/maskBackground_vs_glsl.h"
#include "shaders/generated/maskBackground_ps_glsl.h"
#include "shaders/generated/ramp_vs_glsl.h"
#include "shaders/generated/ramp_ps_glsl.h"
#include "shaders/generated/noise_vs_glsl.h"
#include "shaders/generated/noise_ps_glsl.h"
#include "shaders/generated/flat_vs_glsl.h"
#include "shaders/generated/flat_ps_glsl.h"
#include "shaders/generated/sphere_vs_glsl.h"
#include "shaders/generated/sphere_ps_glsl.h"
#endif

#if BGFX_PLATFORM_SUPPORTS_DXBC
#include "shaders/generated/background_vs_dx11.h"
#include "shaders/generated/background_ps_dx11.h"
#include "shaders/generated/maskBackground_vs_dx11.h"
#include "shaders/generated/maskBackground_ps_dx11.h"

#include "shaders/generated/ramp_vs_dx11.h"
#include "shaders/generated/ramp_ps_dx11.h"
#include "shaders/generated/noise_vs_dx11.h"
#include "shaders/generated/noise_ps_dx11.h"
#include "shaders/generated/flat_vs_dx11.h"
#include "shaders/generated/flat_ps_dx11.h"
#include "shaders/generated/sphere_vs_dx11.h"
#include "shaders/generated/sphere_ps_dx11.h"
#endif

static const bgfx::EmbeddedShader s_embeddedShaders[] =
{
	BGFX_EMBEDDED_SHADER(background_vs),
	BGFX_EMBEDDED_SHADER(background_ps),
	BGFX_EMBEDDED_SHADER(maskBackground_vs),
	BGFX_EMBEDDED_SHADER(maskBackground_ps),
	BGFX_EMBEDDED_SHADER(flat_vs),
	BGFX_EMBEDDED_SHADER(flat_ps),
	BGFX_EMBEDDED_SHADER(noise_vs),
	BGFX_EMBEDDED_SHADER(noise_ps),
	BGFX_EMBEDDED_SHADER(ramp_vs),
	BGFX_EMBEDDED_SHADER(ramp_ps),
    BGFX_EMBEDDED_SHADER(sphere_vs),
    BGFX_EMBEDDED_SHADER(sphere_ps),

	BGFX_EMBEDDED_SHADER_END()
};

bgfx::ProgramHandle loadBgfxProgram(const std::string& VSFile, const std::string& PSFile)
{
	bgfx::RendererType::Enum type = bgfx::getRendererType();

	bgfx::ProgramHandle ProgramHandle = bgfx::createProgram(
		bgfx::createEmbeddedShader(s_embeddedShaders, type, VSFile.c_str())
		, bgfx::createEmbeddedShader(s_embeddedShaders, type, PSFile.c_str())
		, true
	);
	assert(bgfx::isValid(ProgramHandle));
	return ProgramHandle;
}
