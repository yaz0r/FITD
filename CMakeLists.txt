project(FITD)
cmake_minimum_required(VERSION 3.9)

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DBX_CONFIG_DEBUG=0")
#set( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /DBX_CONFIG_DEBUG=1")

IF(MSVC)
    #set( CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /D_MSVC_STL_HARDENING=1")
    #set( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /D_MSVC_STL_HARDENING=1")
ENDIF(MSVC)

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

set(USE_SANITIZER ON)
if(USE_SANITIZER)
	set(SANITIZER_TYPE "address,undefined,leak")
	target_link_options(Fitd PRIVATE "-fsanitize=${SANITIZER_TYPE}")
	target_compile_options(Fitd PRIVATE "-fsanitize=${SANITIZER_TYPE}")
	target_compile_options(FitdLib PRIVATE "-fsanitize=${SANITIZER_TYPE}")
	target_compile_options(bimg PRIVATE "-fsanitize=${SANITIZER_TYPE}")
	target_compile_options(bgfx PRIVATE "-fsanitize=${SANITIZER_TYPE}")
	target_compile_options(soloud PRIVATE "-fsanitize=${SANITIZER_TYPE}")
endif()
