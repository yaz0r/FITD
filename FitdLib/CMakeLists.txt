cmake_minimum_required(VERSION 3.9)

include_directories(
    "."
    "${THIRD_PARTY}/SDL/include/SDL3"
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
 "${THIRD_PARTY}/imgui/backends/imgui_impl_sdl3.cpp"
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

macro(addShaderProgramPermutation vsname psname varyingname)
    bgfx_compile_shaders(SHADERS shaders/${vsname}.sc AS_HEADERS OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/shaders/generated TYPE VERTEX VARYING_DEF ${CMAKE_CURRENT_SOURCE_DIR}/shaders/${varyingname}.sc INCLUDE_DIRS ${BGFX_DIR}/src AS_HEADERS)
    bgfx_compile_shaders(SHADERS shaders/${psname}.sc AS_HEADERS OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/shaders/generated TYPE FRAGMENT VARYING_DEF ${CMAKE_CURRENT_SOURCE_DIR}/shaders/${varyingname}.sc INCLUDE_DIRS ${BGFX_DIR}/src AS_HEADERS)
    #shaderc(FILE shaders/${vsname}.sc BIN2C ${vsname}_${api} OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/shaders/generated/${vsname}_${api}.debug.h DEBUG VERTEX ${platform} VARYINGDEF shaders/${varyingname}.sc INCLUDES ${BGFX_DIR}/src PROFILE ${profileVS})
    #shaderc(FILE shaders/${psname}.sc BIN2C ${psname}_${api} OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/shaders/generated/${psname}_${api}.debug.h DEBUG FRAGMENT ${platform} VARYINGDEF shaders/${varyingname}.sc INCLUDES ${BGFX_DIR}/src PROFILE ${profilePS})
    #set(SOURCES
     #   ${SOURCES}
    #    ${CMAKE_CURRENT_SOURCE_DIR}/shaders/generated/${vsname}_${api}.h
     #   ${CMAKE_CURRENT_SOURCE_DIR}/shaders/generated/${psname}_${api}.h
        #${CMAKE_CURRENT_SOURCE_DIR}/shaders/generated/${vsname}_${api}.debug.h
        #${CMAKE_CURRENT_SOURCE_DIR}/shaders/generated/${psname}_${api}.debug.h
    #)
endmacro()

macro(addShaderProgram vsname psname varyingname)

    set(SOURCES
        ${SOURCES}
        shaders/${vsname}.sc
        shaders/${psname}.sc
        shaders/${varyingname}.sc
    )

    addShaderProgramPermutation(${vsname} ${psname} ${varyingname})
    addShaderProgramPermutation(${vsname} ${psname} ${varyingname})

endmacro()

addShaderProgram(ui_vs ui_ps ui.varying.def)
addShaderProgram(background_vs background_ps background.varying.def)
addShaderProgram(maskBackground_vs maskBackground_ps maskBackground.varying.def)
addShaderProgram(flat_vs flat_ps flat.varying.def)
addShaderProgram(noise_vs noise_ps noise.varying.def)
addShaderProgram(ramp_vs ramp_ps ramp.varying.def)
addShaderProgram(sphere_vs sphere_ps sphere.varying.def)

assign_source_group(${SOURCES})

if(MSVC)
    add_compile_options(/Zc:__cplusplus)
    add_compile_options(/Zc:preprocessor)
endif()

add_library (FitdLib ${SOURCES})



#set( CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /DBX_CONFIG_DEBUG=1")
#set( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /DBX_CONFIG_DEBUG=1")

set_property(TARGET FitdLib PROPERTY CXX_STANDARD 20)
