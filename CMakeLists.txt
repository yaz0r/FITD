project(FITD)
cmake_minimum_required(VERSION 3.9)

MACRO(COPY_BUILD_CONFIG SourceConfigName DestConfigName)
if(CMAKE_CONFIGURATION_TYPES)
	list(APPEND CMAKE_CONFIGURATION_TYPES ${DestConfigName})
	string(TOUPPER ${SourceConfigName} SOURCE_UPCASE)
	string(TOUPPER ${DestConfigName} DEST_UPCASE)

	set(CMAKE_SHARED_LINKER_FLAGS_${DEST_UPCASE} ${CMAKE_SHARED_LINKER_FLAGS_${SOURCE_UPCASE}})
	set(CMAKE_EXE_LINKER_FLAGS_${DEST_UPCASE} ${CMAKE_EXE_LINKER_FLAGS_${SOURCE_UPCASE}})
	set(CMAKE_C_FLAGS_${DEST_UPCASE} ${CMAKE_C_FLAGS_${SOURCE_UPCASE}})
	set(CMAKE_CXX_FLAGS_${DEST_UPCASE} ${CMAKE_CXX_FLAGS_${SOURCE_UPCASE}})
else()
	message("  XXX custom build types are not allowed...")
endif()
ENDMACRO(COPY_BUILD_CONFIG)

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /DBX_CONFIG_DEBUG=0")

#set( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /DBX_CONFIG_DEBUG=1")

IF(MSVC)
    #set( CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /D_MSVC_STL_HARDENING=1")
    #set( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /D_MSVC_STL_HARDENING=1")
ENDIF(MSVC)

COPY_BUILD_CONFIG(Debug Debug_Asan)

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

target_compile_options(
  Fitd PRIVATE "$<$<CONFIG:Debug_Asan>:/fsanitize=address>"
)
target_compile_options(
  FitdLib PRIVATE "$<$<CONFIG:Debug_Asan>:/fsanitize=address>"
)
target_compile_options(
  bimg PRIVATE "$<$<CONFIG:Debug_Asan>:/fsanitize=address>"
)