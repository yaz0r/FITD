cmake_minimum_required(VERSION 3.0.0)
project(soloud C)

file(GLOB_RECURSE SOLOUD_HEADERS ${soloud_SOURCE_DIR}/soloud/include/*.h)
file(GLOB_RECURSE SOLOUD_SOURCES
    ${soloud_SOURCE_DIR}/soloud/src/core/*.cpp
    ${soloud_SOURCE_DIR}/soloud/src/audiosource/wav/*.c*
    ${soloud_SOURCE_DIR}/soloud/src/backend/sdl3_static/*.cpp
    ${soloud_SOURCE_DIR}/soloud/src/backend/null/*.cpp
)

include_directories(${soloud_SOURCE_DIR}/soloud/include "${THIRD_PARTY}/SDL/include" "${THIRD_PARTY}/SDL/include/SDL3")

set(SOLOUD_SOURCES
${SOLOUD_HEADERS}
${SOLOUD_SOURCES}
)

add_library(soloud STATIC ${SOLOUD_SOURCES})
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

target_compile_definitions(soloud PRIVATE WITH_SDL3_STATIC=1)
