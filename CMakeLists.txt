cmake_minimum_required(VERSION 3.5)

project(RayTracer VERSION 0.1
	DESCRIPTION "CPU Ray Tracer"
	LANGUAGES CXX C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set the source directory
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Find all .cpp files in the source directory and its subdirectories
file(GLOB_RECURSE SOURCES "${SRC_DIR}/**.cpp" "${SRC_DIR}/**.h")

add_subdirectory(libs/glfw)

SET(GLFW_BUILD_DOCS OFF CACHE BOOL "GLFW lib only")
SET(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "GLFW lib only")
SET(GLFW_BUILD_TESTS OFF CACHE BOOL "GLFW lib only")
SET(GLFW_INSTALL OFF CACHE BOOL "GLFW lib only")

add_executable(RayTracer ${SOURCES} libs/stb_image/stb_image_write.h libs/stb_image/stb_image.h libs/glad/src/glad.c libs/imgui/imgui.cpp libs/imgui/imgui_draw.cpp libs/imgui/imgui_widgets.cpp libs/imgui/imgui_tables.cpp)

target_compile_definitions(RayTracer PRIVATE IMGUI_IMPL_OPENGL_LOADER_CUSTOM)
target_link_libraries(RayTracer PRIVATE glfw)

target_include_directories(RayTracer PRIVATE src/ libs/ libs/glad/include/ libs/imgui/)
target_compile_features(RayTracer PRIVATE cxx_std_20)

if (WIN32)
	target_compile_definitions(RayTracer PRIVATE RT_WINDOWS)
else()
	# used by the stl for multithreading on ubuntu (linux as a whole?)
	target_link_libraries(RayTracer PRIVATE -ltbb)
	target_compile_definitions(RayTracer PRIVATE RT_LINUX)
endif (WIN32)
