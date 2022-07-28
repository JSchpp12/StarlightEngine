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

add_subdirectory("external/glm")

set(GLFW_STANDALONE OFF CACHE BOOL "" FORCE)
add_subdirectory("external/glfw")

#prepare vars for child projects
set(STB_DIRECTORY ${PROJECT_SOURCE_DIR}/external/stb)
if (NOT IS_DIRECTORY ${STB_DIRECTORY})
	message(FATAL_ERROR "Stb is needed for compilation. Pull copy into external directory.")
endif()

#add child projects
add_subdirectory("StarlightCommon")
add_subdirectory("StarlightCore")

set(${PROJECT_NAME}_SOURCE 
	"src/StarlightEngine.cpp"
	"src/Callback.cpp"
	"src/CameraController.cpp"
	"src/OptionsController.cpp"
	"src/InteractionSystem.cpp"
	"src/SceneBuilder.cpp"
	#applications
	"src/Applications/ObjectApp.cpp"
	"src/Applications/MultipleObjectApp.cpp"
	"src/Applications/LightApp.cpp"
	"src/Applications/TextureApp.cpp"
	"src/Applications/LightTypeApp.cpp"
)

set(${PROJECT_NAME}_HEADERS 
	"src/StarlightEngine.h"
	"src/Callback.h"
	"src/CameraController.h" 
	"src/OptionsController.h"
	"src/InteractionSystem.h"
	"src/SceneBuilder.hpp"
	#applications
	"src/Applications/ObjectApp.h"
	"src/Applications/MultipleObjectApp.h"
	"src/Applications/LightApp.h"
	"src/Applications/TextureApp.h"
	"src/Applications/LightTypeApp.h"
)

add_executable(${PROJECT_NAME} "${${PROJECT_NAME}_SOURCE};${${PROJECT_NAME}_HEADERS}")
#set(CMAKE_MEDIA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/media")
#configure_file("Engine.cfg" "Engine.cfg")

set(gtest_force_shared_crt ON)
set(BUILD_GMOCK ON)
add_subdirectory("external/googletest")

set(STAR_COMMON_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs/StarlightCommon")
set(STAR_COMMON_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs/StarlightCommon/include")
set(STAR_COMMON_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR})

target_include_directories(${PROJECT_NAME}
	PRIVATE 
		${CMAKE_CURRENT_SOURCE_DIR}/src/Applications
		${CMAKE_CURRENT_SOURCE_DIR}/src
		${STB_DIRECTORY}
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
set(PREP_MEDIA_CMD "${CMAKE_CURRENT_SOURCE_DIR}/PrepareMedia.py")
add_custom_target(
	always_run_target ALL 
	DEPENDS ${${PROJECT_NAME}_SOURCE}
)
add_custom_command(
	TARGET
		always_run_target
	DEPENDS
		${${PROJECT_NAME}_SOURCE}
	COMMAND 
		python3 ${PREP_MEDIA_CMD} buildDir=${CMAKE_CURRENT_BINARY_DIR} mediaDir=${CMAKE_CURRENT_SOURCE_DIR}/media configFile=${CMAKE_CURRENT_SOURCE_DIR}/Engine.cfg
)


#[[
include(FindPythonInterp REQUIRED)
add_custom_command(
	OUTPUT 
		${CMAKE_CURRENT_BINARY_DIR}/media
	COMMAND 
		"${PYTHON_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/PrepareMedia.py" --argument
	DEPENDS 
		some_target
)

add_custom_target(prepare_media ALL 
	DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/PrepareMedia.py"
)]]

#[[
set(PREP_MEDIA_CMD "PrepareMedia.py ${CMAKE_BINARY_DIR}")
add_custom_target(
	always_run_target ALL 
	DEPENDS ${${PROJECT_NAME}_SOURCE}
	COMMAND python ${py_cmd}
)]]

#install 
install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/media" DESTINATION media)
install(TARGETS ${PROJECT_NAME} DESTINATION bin )
install(TARGETS StarlightEngineCore DESTINATION lib)

set(INSTALL_DIR ${CMAKE_INSTALL_PREFIX})

#if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin" OR )
	#macos 
	#target_link_libraries(HelloSquareTextured
	#	spirv-cross
	#)
    
#endif()
#pass needed directories to application

# TODO: Add tests and install targets if needed.