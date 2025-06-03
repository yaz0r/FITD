project(FITD)
cmake_minimum_required(VERSION 3.9)

set(THIRD_PARTY "${CMAKE_SOURCE_DIR}/ThirdParty")

set(BGFX_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BGFX_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory( ${THIRD_PARTY}/bgfx.cmake )

add_subdirectory( ${THIRD_PARTY}/soloud.cmake )

set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)
add_subdirectory( ${THIRD_PARTY}/SDL )

set(SKIP_INSTALL_ALL on)
set(ZLIB_LIBRARY zlibstatic)
# Don't build zlib tests
set(ZLIB_TESTS OFF CACHE BOOL "Build zlib tests")
add_subdirectory(${THIRD_PARTY}/zlib ${CMAKE_BINARY_DIR}/zlib/zlib-build)

add_subdirectory( FitdLib )
add_subdirectory( Fitd )
