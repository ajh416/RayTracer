cmake_minimum_required(VERSION 3.5)

project(RayTracer VERSION 0.1
	DESCRIPTION "GPU Ray Tracer"
	LANGUAGES CXX C)

# Enable compile_commands.json for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set the source directory
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Find all .cpp files in the source directory and its subdirectories
file(GLOB_RECURSE SOURCES "${SRC_DIR}/**.cpp" "${SRC_DIR}/**.h")

SET(GLFW_BUILD_X11 ON CACHE BOOL "Build for X11")
SET(GLFW_BUILD_WAYLAND OFF CACHE BOOL "Build for Wayland")

# Don't let glfw build examples/tests/docs or install
SET(GLFW_BUILD_DOCS OFF CACHE BOOL "GLFW lib only")
SET(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "GLFW lib only")
SET(GLFW_BUILD_TESTS OFF CACHE BOOL "GLFW lib only")
SET(GLFW_INSTALL OFF CACHE BOOL "GLFW lib only")

add_subdirectory(libs/glfw)
add_subdirectory(libs/glm)

add_executable(RayTracer ${SOURCES} libs/stb_image/stb_image_write.h libs/stb_image/stb_image.h libs/glad/src/glad.c libs/imgui/imgui.cpp libs/imgui/imgui_draw.cpp libs/imgui/imgui_widgets.cpp libs/imgui/imgui_tables.cpp)

# For imgui to not try and load OpenGL itself
target_compile_definitions(RayTracer PRIVATE IMGUI_IMPL_OPENGL_LOADER_CUSTOM)

# Enable experimental GLM features, used for camera
target_compile_definitions(RayTracer PRIVATE GLM_ENABLE_EXPERIMENTAL)

# Link to glfw
target_link_libraries(RayTracer PRIVATE glfw)

# Include directories
target_include_directories(RayTracer PRIVATE src/ libs/ libs/glad/include/ libs/imgui/ libs/glm/)

# Set C++ standard to 20
target_compile_features(RayTracer PRIVATE cxx_std_20)

# Set the warning level and optimization level, no optimizations leads to VERY slow frames
target_compile_options(RayTracer PRIVATE -Wall -O2)

# Set the operating system specific defines
if (WIN32)
	target_compile_definitions(RayTracer PRIVATE RT_WINDOWS)
else()
	# used by the stl for multithreading on ubuntu (why? linux as a whole?)
	target_link_libraries(RayTracer PRIVATE -ltbb)
	target_compile_definitions(RayTracer PRIVATE RT_LINUX)
endif (WIN32)
