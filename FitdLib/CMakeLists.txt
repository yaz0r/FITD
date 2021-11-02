cmake_minimum_required(VERSION 3.9)

include_directories(
    "."
    "${THIRD_PARTY}/SDL/include"
    "${THIRD_PARTY}/imgui"
    "${THIRD_PARTY}/soloud.cmake/soloud/include"
    "${THIRD_PARTY}/bgfx.cmake/bgfx/include"
    "${THIRD_PARTY}/bgfx.cmake/bx/include"
    "${THIRD_PARTY}/bgfx.cmake/bx/include/compat/msvc"
    "${THIRD_PARTY}/bgfx.cmake/bgfx/3rdparty"
    "${THIRD_PARTY}/bgfx.cmake/bimg/include"
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE SRC_ROOT
    "*.cpp"
    "*.h"
)

set(SOURCES
 ${SOURCES}
 ${SRC_ROOT}
 "${THIRD_PARTY}/imgui/imgui.cpp"
 "${THIRD_PARTY}/imgui/imgui.h"
 "${THIRD_PARTY}/imgui/imgui_draw.cpp"
 "${THIRD_PARTY}/imgui/imgui_tables.cpp"
 "${THIRD_PARTY}/imgui/imgui_widgets.cpp"
 "${THIRD_PARTY}/imgui/backends/imgui_impl_sdl.cpp"
)

function(assign_source_group)
    foreach(_source IN ITEMS ${ARGN})
        if (IS_ABSOLUTE "${_source}")
            file(RELATIVE_PATH _source_rel "${CMAKE_CURRENT_SOURCE_DIR}" "${_source}")
        else()
            set(_source_rel "${_source}")
        endif()
        get_filename_component(_source_path "${_source_rel}" PATH)
        string(REPLACE "/" "\\" _source_path_msvc "${_source_path}")
        source_group("${_source_path_msvc}" FILES "${_source}")
    endforeach()
endfunction(assign_source_group)

macro(addShaderProgram vsname psname varyingname)

set(SOURCES
    ${SOURCES}
    shaders/${vsname}.sc
    shaders/${psname}.sc
    shaders/${varyingname}.sc
)

shaderc(FILE shaders/${vsname}.sc BIN2C ${vsname}_glsl OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/shaders/generated/${vsname}_glsl.h VERTEX WINDOWS VARYINGDEF shaders/${varyingname}.sc INCLUDES ${BGFX_DIR}/src PROFILE 430)
shaderc(FILE shaders/${psname}.sc BIN2C ${psname}_glsl OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/shaders/generated/${psname}_glsl.h FRAGMENT WINDOWS VARYINGDEF shaders/${varyingname}.sc INCLUDES ${BGFX_DIR}/src PROFILE 430)

set(SOURCES
    ${SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/shaders/generated/${vsname}_glsl.h
    ${CMAKE_CURRENT_SOURCE_DIR}/shaders/generated/${psname}_glsl.h
)

IF(${CMAKE_SYSTEM_NAME} MATCHES iOS|tvOS)
    shaderc(FILE shaders/${vsname}.sc OUTPUT ${THIRD_PARTY}/../data/shaders/generated/${vsname}.metal.bin VERTEX IOS VARYINGDEF shaders/${varyingname}.sc INCLUDES ${BGFX_DIR}/src PROFILE metal)
    shaderc(FILE shaders/${psname}.sc OUTPUT ${THIRD_PARTY}/../data/shaders/generated/${psname}.metal.bin FRAGMENT IOS VARYINGDEF shaders/${varyingname}.sc INCLUDES ${BGFX_DIR}/src PROFILE metal)
    set(SOURCES
        ${SOURCES}
        ${THIRD_PARTY}/../data/shaders/generated/${vsname}.metal.bin
        ${THIRD_PARTY}/../data/shaders/generated/${psname}.metal.bin
    )
endif()

IF(${CMAKE_SYSTEM_NAME} MATCHES Darwin)
    shaderc(FILE shaders/${vsname}.sc OUTPUT ${THIRD_PARTY}/../data/shaders/generated/${vsname}.metal.bin VERTEX OSX VARYINGDEF shaders/${varyingname}.sc INCLUDES ${BGFX_DIR}/src PROFILE metal)
    shaderc(FILE shaders/${psname}.sc OUTPUT ${THIRD_PARTY}/../data/shaders/generated/${psname}.metal.bin FRAGMENT OSX VARYINGDEF shaders/${varyingname}.sc INCLUDES ${BGFX_DIR}/src PROFILE metal)
    set(SOURCES
        ${SOURCES}
        ${THIRD_PARTY}/../data/shaders/generated/${vsname}.metal.bin
        ${THIRD_PARTY}/../data/shaders/generated/${psname}.metal.bin
    )
endif()

IF(${CMAKE_SYSTEM_NAME} MATCHES Windows)
    shaderc(FILE shaders/${vsname}.sc BIN2C ${vsname}_dx11 OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/shaders/generated/${vsname}_dx11.h VERTEX WINDOWS VARYINGDEF shaders/${varyingname}.sc INCLUDES ${BGFX_DIR}/src PROFILE vs_5_0)
    shaderc(FILE shaders/${psname}.sc BIN2C ${psname}_dx11 OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/shaders/generated/${psname}_dx11.h FRAGMENT WINDOWS VARYINGDEF shaders/${varyingname}.sc INCLUDES ${BGFX_DIR}/src PROFILE ps_5_0)
    shaderc(FILE shaders/${vsname}.sc BIN2C ${vsname}_dx11_debug OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/shaders/generated/${vsname}_dx11_debug.h DEBUG VERTEX WINDOWS VARYINGDEF shaders/${varyingname}.sc INCLUDES ${BGFX_DIR}/src PROFILE vs_5_0)
    shaderc(FILE shaders/${psname}.sc BIN2C ${psname}_dx11_debug OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/shaders/generated/${psname}_dx11_debug.h DEBUG FRAGMENT WINDOWS VARYINGDEF shaders/${varyingname}.sc INCLUDES ${BGFX_DIR}/src PROFILE ps_5_0)
    set(SOURCES
        ${SOURCES}
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders/generated/${vsname}_dx11.h
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders/generated/${psname}_dx11.h
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders/generated/${vsname}_dx11_debug.h
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders/generated/${psname}_dx11_debug.h
    )

    
    shaderc(FILE shaders/${vsname}.sc BIN2C ${vsname}_spirv OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/shaders/generated/${vsname}_spirv.h VERTEX LINUX VARYINGDEF shaders/${varyingname}.sc INCLUDES ${BGFX_DIR}/src PROFILE spirv)
    shaderc(FILE shaders/${psname}.sc BIN2C ${psname}_spirv OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/shaders/generated/${psname}_spirv.h FRAGMENT LINUX VARYINGDEF shaders/${varyingname}.sc INCLUDES ${BGFX_DIR}/src PROFILE spirv)
    set(SOURCES
        ${SOURCES}
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders/generated/${vsname}_spirv.h
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders/generated/${psname}_spirv.h
    )
endif()

endmacro()

addShaderProgram(background_vs background_ps background.varying.def)
addShaderProgram(maskBackground_vs maskBackground_ps maskBackground.varying.def)
addShaderProgram(flat_vs flat_ps flat.varying.def)
addShaderProgram(noise_vs noise_ps noise.varying.def)
addShaderProgram(ramp_vs ramp_ps ramp.varying.def)

assign_source_group(${SOURCES})

add_library (FitdLib ${SOURCES})
