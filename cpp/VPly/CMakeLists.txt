# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("VPly")

set(CMAKE_CXX_STANDARD 14)

# Include sub-projects.
set(VPLY_ROOT_DIR ${PROJECT_SOURCE_DIR})
set(VPLY_SOURCE_DIR ${PROJECT_SOURCE_DIR})

if (EXISTS "${VPLY_SOURCE_DIR}/test.cpp")
	add_executable(VPlyTest "${VPLY_SOURCE_DIR}/test.cpp")
	target_include_directories(VPlyTest PUBLIC ${VPLY_SOURCE_DIR})
endif()