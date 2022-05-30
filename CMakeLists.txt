# CMakeList.txt : CMake project for HelloSquare, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)
find_package(Git)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MACOSX_RPATH 1)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
cmake_policy(SET CMP0071 NEW)
cmake_policy(SET CMP0076 NEW)

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set(CMAKE_POLICY_DEFAULT_CMP0079 NEW)

project("StarlightEngine")

#TODO: add ability to rely on installed libraries 
#THIS IS NEEDED FOR NOW...
option(BUILD_LIBRARIES "Build external libraries" ON)

option(UPDATE_SUBMODULES "If building external libraries, check for updates and existence of needed submodules" ON)
option(PULL_SUBMODULE "Check and pull updates for git submodules if needed" ON)

find_package(Vulkan REQUIRED FATAL_ERROR)

set(LINK_INCLUDE "")		#list of libs found on system
set(LINK_SUBMODULE "")

find_library(TINY_OBJ_LOADER_LIB NAMES tinyobjloader)
#set(STAR_COMMON_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs/StarlightCommon/include")

add_subdirectory("StarCommon")

add_subdirectory("EngineCore")

add_executable(${PROJECT_NAME} "src/StarlightEngine.cpp")
set(CMAKE_MEDIA_DIR "${CMAKE_CURRENT_BINARY_DIR}/media")
configure_file("Engine.cfg" "Engine.cfg")

set(gtest_force_shared_crt ON)
set(BUILD_GMOCK ON)
add_subdirectory("external/googletest")

set(STAR_COMMON_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs/StarlightCommon")
set(STAR_COMMON_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs/StarlightCommon/include")
set(STAR_COMMON_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR})
message("star build - ${STAR_COMMON_BUILD_DIR}")

target_include_directories(${PROJECT_NAME}
	PRIVATE 
		${CMAKE_CURRENT_BINARY_DIR}
		${StarlightCommon_INCLUDE_DIR}
)

target_link_libraries(${PROJECT_NAME}
	PUBLIC 
		StarlightEngineCore
	PRIVATE 
		StarlightCommon::StarlightCommon
)

#TODO: NEED BETTER SOLUTION FOR MEDIA FILES
file(COPY "${CMAKE_SOURCE_DIR}/media/images/texture.png" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/media/images")
file(COPY "${CMAKE_SOURCE_DIR}/media/shaders/fragShader.frag" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/media/shaders")
file(COPY "${CMAKE_SOURCE_DIR}/media/shaders/vertShader.vert" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/media/shaders")
file(COPY "${CMAKE_SOURCE_DIR}/media/models/lion-statue/source/rapid.obj" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/media/models/lion-statue/source")
file(COPY "${CMAKE_SOURCE_DIR}/media/models/lion-statue/source/material0_basecolor.png" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/media/models/lion-statue/source")
file(COPY "${CMAKE_SOURCE_DIR}/media/models/lion-statue/vertShader.vert" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/media/models/lion-statue")
file(COPY "${CMAKE_SOURCE_DIR}/media/models/lion-statue/fragShader.frag" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/media/models/lion-statue")

#set(gtest_force_shared_crt on)
#add_subdirectory("external/googletest")
#set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
#[[
if(BUILD_LIBRARIES)
	# Suppress all warnings from external projects.
	#set_property(DIRECTORY APPEND PROPERTY COMPILE_OPTIONS -w)


	set(gtest_force_shared_crt ON)
	set(BUILD_GMOCK ON)
	add_subdirectory("external/googletest")

	set(SPIRV_HEADERS_SKIP_EXAMPLES ON CACHE BOOL "" FORCE)
	add_subdirectory("external/spirvheaders")

	set(GLFW_STANDALONE OFF CACHE BOOL "" FORCE)
	add_subdirectory("external/glfw")

	add_subdirectory("external/glm")

	#prevent spirv from building tests 
	set(SPIRV_SKIP_EXECUTABLES ON CACHE BOOL "" FORCE)
	set(SPIRV_SKIP_TESTS ON CACHE BOOL "" FORCE)
	add_subdirectory("external/spirvtools")

	set(ENABLE_CTEST OFF CACHE BOOL "" FORCE)
	set(ENABLE_GLSLANG_BINARIES OFF CACHE BOOL "" FORCE)
	add_subdirectory("external/glslang")

	set(SHADERC_SKIP_EXAMPLES ON CACHE BOOL "" FORCE)
	set(SHADERC_SKIP_TESTS ON CACHE BOOL "" FORCE)
	set(SHADERC_ENABLE_SHARED_CRT ON CACHE BOOL "" FORCE)
	set(SHADERC_SKIP_COPYRIGHT_CHECK ON CACHE BOOL "" FORCE)
	add_subdirectory("external/shaderc")

	add_subdirectory("external/tinyobjloader")

endif()
]]

#[[Keeping these for now, as of now -- reliant on submodules for build
find_library(glslang_LIBRARY NAMES glslang HINTS "$ENV{VULKAN_SDK}/lib")
find_library(glslang_LIBRARY_DEBUG NAMES glslangd HINTS "$ENV{VULKAN_SDK}/lib")
find_library(glslang_HLSL NAMES hlsl HINTS "$ENV{VULKAN_SDK}/lib")
find_library(glslang_HLSL_DEBUG NAMES hlsld HINTS "$ENV{VULKAN_SDK}/lib")
find_library(glslang_OSDEPENDENT NAMES osdependent HINTS "$ENV{VULKAN_SDK}/lib")
find_library(glslang_OSDEPENDENT_DEBUG NAMES osdependentd HINTS "$ENV{VULKAN_SDK}/lib")
find_library(glslang_OGLCOMPILER NAMES oglcompiler HINTS "$ENV{VULKAN_SDK}/lib")
find_library(glslang_OGLCOMPILER_DEBUG NAMES oglcompilerd HINTS "$ENV{VULKAN_SDK}/lib")
find_library(glslang_SPIRV NAMES spirv HINTS "$ENV{VULKAN_SDK}/lib")
find_library(glslang_SPIRV_DEBUG NAMES spirvd HINTS "$ENV{VULKAN_SDK}/lib")
find_library(glslang_machineind NAME machineindependent HINTS "$ENV{VULKAN_SDK}/lib")
find_library(glslang_machineind_DEBUG NAME machineindependentd HINTS "$ENV{VULKAN_SDK}/lib")
find_library(SPIRV_TOOLS_STATIC NAME spirv-tools-static HINTS "$ENV{VULKAN_SDK}/lib")
find_library(SPIRV_TOOLS_STATIC NAME spirv-tools-staticd HINTS "$ENV{VULKAN_SDK}/lib")]]
#[[
link_directories(HelloSquareTextured "$ENV{VULKAN_SDK}/lib")
link_directories(HelloSquareTextured "external")

set(LIBRARIES
		glslang 
		HLSL
		OSDependent
		OGLCompiler
		spirv
		glslang-default-resource-limits
)
set(LIBRARIES_DEBUG
		glslangd 
		HLSLd
		OSDependentd
		OGLCompilerd
		spirvd
		glslang-default-resource-limitsd
)
#set(GLSLANG_LINK_LIBS_DEBUG glslang ${glslang_OGLCOMPILER_DEBUG} SPIRV ${glslang_HLSL_DEBUG} ${glslang_OSDEPENDENT_DEBUG} MachineIndependentd GenericCodeGend SPIRV-Tools-optd SPIRV-Toolsd SPVRemapperd )

# Add source to this project's executableo.
add_executable (HelloSquareTextured "src/HelloSquare.cpp" "src/HelloSquare.h" "src/HelloSquareApplication.cpp" "src/HelloSquareApplication.h" "src/VertexColor.h" "src/VertexColor.cpp" "src/Vertex.h" "src/VulkanObject.h" "src/VulkanObject.cpp" "src/Shader.h" "src/Shader.cpp" "src/GLSLShader.h" "src/GLSLShader.cpp" "src/Enums.h" "src/GLSlangHelper.h"  "src/Constants.h" "src/ShadercHelper.h"  )

set_property(TARGET HelloSquareTextured PROPERTY CXX_STANDARD 17)

#configure_file("${cmake_source_dir}/shaders/vertshader_7.vert" "${cmake_current_binary_dir}" copyonly)
#configure_file("${cmake_source_dir}/shaders/fragShader_3.frag" "${cmake_current_binary_dir}" copyonly)

file(GLOB_RECURSE GLSL_SOURCE_FILES
    "media/shaders/*.frag"
    "media/shaders/*.vert"
)

message("Is vulkan found? ${VULKAN_FOUND}")
message("Where is it ${Vulkan_LIBRARY}")
message("test: ${glslang_SPIRV_DEBUG}")
target_include_directories(HelloSquareTextured PRIVATE "${PROJECT_BINARY_DIR}/"
                                                "external"
												"external/glfw/include"
												"external/glm"
												"external/stb"
												"external/shaderc/libshaderc/include"
												"external/shaderc/libshaderrc_util/include"
												"external/tinyobjloader"
												"${Vulkan_INCLUDE_DIRS}")


target_link_libraries(HelloSquareTextured ${Vulkan_LIBRARIES}
						glfw ${GLFW_LIBRARIES}
						#shaderc
						#tinyobjloader
)


if(WIN32)
    #libs needed for glfw
else()
	#macos and linux
endif()

#if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin" OR )
	#macos 
	#target_link_libraries(HelloSquareTextured
	#	spirv-cross
	#)
    
#endif()
#pass needed directories to application

# TODO: Add tests and install targets if needed.
]]