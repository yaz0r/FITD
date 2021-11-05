cmake_minimum_required(VERSION 3.9)

include_directories(
    "."
    "${THIRD_PARTY}/SDL/include"
    "${THIRD_PARTY}/imgui"
    "${THIRD_PARTY}/soloud.cmake/soloud/include"
    "${THIRD_PARTY}/bgfx.cmake/bgfx/include"
    "${THIRD_PARTY}/bgfx.cmake/bx/include"
    "${THIRD_PARTY}/bgfx.cmake/bgfx/3rdparty"
    "${THIRD_PARTY}/bgfx.cmake/bimg/include"
)

IF(${MSVC})
    include_directories("${THIRD_PARTY}/bgfx.cmake/bx/include/compat/msvc")
endif()

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

macro(addShaderProgramPermutation vsname psname varyingname platform api profileVS profilePS)
    shaderc(FILE shaders/${vsname}.sc BIN2C ${vsname}_${api} OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/shaders/generated/${vsname}_${api}.h VERTEX ${platform} VARYINGDEF shaders/${varyingname}.sc INCLUDES ${BGFX_DIR}/src PROFILE ${profileVS})
    shaderc(FILE shaders/${psname}.sc BIN2C ${psname}_${api} OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/shaders/generated/${psname}_${api}.h FRAGMENT ${platform} VARYINGDEF shaders/${varyingname}.sc INCLUDES ${BGFX_DIR}/src PROFILE ${profilePS})
    #shaderc(FILE shaders/${vsname}.sc BIN2C ${vsname}_${api} OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/shaders/generated/${vsname}_${api}.debug.h DEBUG VERTEX ${platform} VARYINGDEF shaders/${varyingname}.sc INCLUDES ${BGFX_DIR}/src PROFILE ${profileVS})
    #shaderc(FILE shaders/${psname}.sc BIN2C ${psname}_${api} OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/shaders/generated/${psname}_${api}.debug.h DEBUG FRAGMENT ${platform} VARYINGDEF shaders/${varyingname}.sc INCLUDES ${BGFX_DIR}/src PROFILE ${profilePS})
    set(SOURCES
        ${SOURCES}
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders/generated/${vsname}_${api}.h
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders/generated/${psname}_${api}.h
        #${CMAKE_CURRENT_SOURCE_DIR}/shaders/generated/${vsname}_${api}.debug.h
        #${CMAKE_CURRENT_SOURCE_DIR}/shaders/generated/${psname}_${api}.debug.h
    )
endmacro()

macro(addShaderProgram vsname psname varyingname)

set(SOURCES
    ${SOURCES}
    shaders/${vsname}.sc
    shaders/${psname}.sc
    shaders/${varyingname}.sc
)

IF(${CMAKE_SYSTEM_NAME} MATCHES iOS|tvOS)
    addShaderProgramPermutation(${vsname} ${psname} ${varyingname} IOS glsl 430 430)
    addShaderProgramPermutation(${vsname} ${psname} ${varyingname} IOS metal metal metal)
endif()

IF(${CMAKE_SYSTEM_NAME} MATCHES Darwin)
    addShaderProgramPermutation(${vsname} ${psname} ${varyingname} OSX glsl 430 430)
    addShaderProgramPermutation(${vsname} ${psname} ${varyingname} OSX mtl metal metal)
endif()

IF(${CMAKE_SYSTEM_NAME} MATCHES Windows)
    addShaderProgramPermutation(${vsname} ${psname} ${varyingname} WINDOWS dx11 vs_5_0 ps_5_0)
    addShaderProgramPermutation(${vsname} ${psname} ${varyingname} WINDOWS glsl 430 430)
    addShaderProgramPermutation(${vsname} ${psname} ${varyingname} WINDOWS spv spirv spirv)
endif()

IF(${CMAKE_SYSTEM_NAME} MATCHES Linux)
    addShaderProgramPermutation(${vsname} ${psname} ${varyingname} WINDOWS glsl 430 430)
    addShaderProgramPermutation(${vsname} ${psname} ${varyingname} WINDOWS spv spirv spirv)
endif()

endmacro()

addShaderProgram(background_vs background_ps background.varying.def)
addShaderProgram(maskBackground_vs maskBackground_ps maskBackground.varying.def)
addShaderProgram(flat_vs flat_ps flat.varying.def)
addShaderProgram(noise_vs noise_ps noise.varying.def)
addShaderProgram(ramp_vs ramp_ps ramp.varying.def)

assign_source_group(${SOURCES})

add_library (FitdLib ${SOURCES})
