cmake_minimum_required(VERSION 3.1)

# Specify compilers
if ("${CMAKE_C_COMPILER}" STREQUAL "")
    set(CMAKE_C_COMPILER clang-3.9 CACHE INTERNAL "")
    message(STATUS "No C compiler was specified. Setting to ${CMAKE_C_COMPILER}")
endif()
if ("${CMAKE_CXX_COMPILER}" STREQUAL "")
    set(CMAKE_CXX_COMPILER clang++-3.9 CACHE INTERNAL "")
    message(STATUS "No C++ compiler was specified. Setting to ${CMAKE_CXX_COMPILER}")
endif()

# Name of the project
project(SceneCreator)

# Specify linker flags - set CMAKE_CURRENT_BINARY_DIR to append the current build directory
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath,'$ORIGIN'" )
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath,'$ORIGIN'" )

message(STATUS "CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "CMAKE_CURRENT_BINARY_DIR: ${CMAKE_CURRENT_BINARY_DIR}")

# Check if the build directory starts with build* prefix
if(NOT ${CMAKE_CURRENT_BINARY_DIR} MATCHES "${CMAKE_CURRENT_SOURCE_DIR}/build*")
    message(FATAL_ERROR "\n CMake needs to be run in a directory with prefix /build*")
endif()

# Output compiler Information and build type
message(STATUS "====== Current compiler is: ${CMAKE_CXX_COMPILER} ======")
message(STATUS "====================================")
if ("${CMAKE_BUILD_TYPE}" STREQUAL "")
    message(STATUS "No selected build type")
    message(STATUS "Setting to Debug")
    set(CMAKE_BUILD_TYPE "Debug")
else()
    message(STATUS "Selected build type is ${CMAKE_BUILD_TYPE}")
endif()
message(STATUS "====================================")

# Get all source files and headers
file(GLOB SOURCES
    main_app/*.h
    main_app/*.cpp
    main_app/debug/*.h
    main_app/debug/*.cpp
    main_app/event_handler/*.h
    main_app/event_handler/*.cpp
    main_app/renderer/*.h
    main_app/renderer/*.cpp
    main_app/texture/*.h
    main_app/texture/*.cpp
    main_app/timer_mgr/*.cpp
    main.cpp)

# Create executable PROJECT_NAME
add_executable(${PROJECT_NAME} ${SOURCES})

# Specify C++14 as the standard
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 14)

# Add include directories
set(SCENE_CREATOR_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(GL_INCLUDES ${SCENE_CREATOR_ROOT_DIR}/gl_includes)
set(INTERNAL_DIRS
    ${SCENE_CREATOR_ROOT_DIR}
    ${SCENE_CREATOR_ROOT_DIR}/main_app
    ${SCENE_CREATOR_ROOT_DIR}/main_app/debug
    ${SCENE_CREATOR_ROOT_DIR}/main_app/event_handler
    ${SCENE_CREATOR_ROOT_DIR}/main_app/renderer
    ${SCENE_CREATOR_ROOT_DIR}/main_app/texture
    ${SCENE_CREATOR_ROOT_DIR}/main_app/timer_mgr)
set(GL_INCLUDE_DIRS
    ${GL_INCLUDES}
    ${GL_INCLUDES}/glad
    ${GL_INCLUDES}/glm
    ${GL_INCLUDES}/imgui
    ${GL_INCLUDES}/KHR
    ${GL_INCLUDEDS}/rapidjson
    ${GL_INCLUDES}/shader
    ${GL_INCLUDES}/spdlog
    ${GL_INCLUDES}/stb_image)
include_directories(${INTERNAL_DIRS})
include_directories(${GL_INCLUDE_DIRS})

#find required libraries
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

#add library glad
add_library(Glad ${GL_INCLUDES}/glad/glad.h ${GL_INCLUDES}/glad.c)

#add library imgui
file(GLOB IMGUI_SOURCES ${GL_INCLUDES}/imgui/*.h ${GL_INCLUDES}/imgui/*.cpp)
add_library(ImGui ${IMGUI_SOURCES})

#link all libraries to executable
target_link_libraries(${PROJECT_NAME} glfw GL Glad ImGui X11 pthread Xrandr
                        Xi ${CMAKE_DL_LIBS} stdc++fs)

