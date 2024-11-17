cmake_minimum_required(VERSION 3.10)

project(RayTracer VERSION 0.1
	DESCRIPTION "CPU Ray Tracer"
        LANGUAGES CXX)

# Set the source directory
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Find all .cpp files in the source directory and its subdirectories
file(GLOB_RECURSE SOURCES "${SRC_DIR}/**.cpp" "${SRC_DIR}/**.h")

add_executable(RayTracer ${SOURCES} libs/stb_image/stb_image_write.h libs/stb_image/stb_image.h)

target_include_directories(RayTracer PRIVATE src libs)
target_compile_features(RayTracer PRIVATE cxx_std_20)

if (WIN32)
	target_compile_definitions(RayTracer PRIVATE RT_WINDOWS)
else()
	# used by the stl for multithreading on ubuntu (linux as a whole?)
	target_link_libraries(RayTracer PRIVATE -ltbb)
	target_compile_definitions(RayTracer PRIVATE RT_LINUX)
endif (WIN32)
